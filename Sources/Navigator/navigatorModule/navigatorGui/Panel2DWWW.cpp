/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Prerequisites.h"

#include "Panel2DWWW.h"
#include "navigatorGui/GUI_ContextMenu.h"
#include <CTLog.h>

using namespace Ogre;
using namespace NaviLibrary;
using namespace CommonTools;

namespace Solipsis {

const String Panel2DWWWFactory::ms_Type = "www";

//-------------------------------------------------------------------------------------
Panel2DWWW::Panel2DWWW(const String& type, const String& name) :
    Panel2D(type, name),
    mNavi(0),
    mMouseCaptured(false)
{
}

//-------------------------------------------------------------------------------------
Panel2DWWW::~Panel2DWWW()
{
}

//-------------------------------------------------------------------------------------
bool Panel2DWWW::create(ushort zOrder)
{
    mNavi = NaviManager::Get().getNavi(mName);
    if (mNavi == 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Panel2DWWW::create() Navi %s not found !", mName.c_str());
        return false;
    }
    mMatName = mNavi->getMaterialName();

    if (!Panel2D::create(zOrder))
        return false;

    // Set title
    if (mTitle != 0)
        mTitle->setCaption("www");

    return true;
}

//-------------------------------------------------------------------------------------
void Panel2DWWW::restoreFocus()
{
    NaviManager::Get().focusNavi(mNavi);
}

//-------------------------------------------------------------------------------------
bool Panel2DWWW::keyPressed(const KeyboardEvt& evt)
{
    return (NaviManager::Get().getFocusedNavi() == mNavi);
}

//-------------------------------------------------------------------------------------
bool Panel2DWWW::keyReleased(const KeyboardEvt& evt)
{
    return (NaviManager::Get().getFocusedNavi() == mNavi);
}

//-------------------------------------------------------------------------------------
bool Panel2DWWW::mouseMoved(const MouseEvt& evt)
{
    bool eventHandled = Panel2D::mouseMoved(evt);

    if ((mPartOverMouse != PClient) && !mMouseCaptured)
        return eventHandled;
    if (NaviManager::Get().getFocusedNavi() != mNavi)
        return eventHandled;
    eventHandled = true;
    int textureX, textureY;
    getTextureRelativePoint(evt.mState.mX, evt.mState.mY, textureX, textureY);
    mNavi->injectMouseMove(textureX, textureY);
    return eventHandled;
}

//-------------------------------------------------------------------------------------
bool Panel2DWWW::mousePressed(const MouseEvt& evt)
{
    bool eventHandled = Panel2D::mousePressed(evt);

    if (mPartOverMouse != PClient)
        return eventHandled;
    if (NaviManager::Get().getFocusedNavi() != mNavi)
        return eventHandled;
    eventHandled = true;
    mMouseCaptured = true;
    if (evt.mState.mButtons & MBLeft)
    {
        int textureX, textureY;
        getTextureRelativePoint(evt.mState.mX, evt.mState.mY, textureX, textureY);
        mNavi->injectMouseDown(textureX, textureY);
    }
    else if ((evt.mState.mButtons & MBRight) && !GUI_ContextMenu::isContextVisible())
    {
        GUI_ContextMenu::createAndShowPanel(evt.mState.mX, evt.mState.mY, GUI_ContextMenu::NAVI_CTXTWWW, mName);
        Panel2DMgr::getSingleton().defocus();
    }
    return eventHandled;
}

//-------------------------------------------------------------------------------------
bool Panel2DWWW::mouseReleased(const MouseEvt& evt)
{
    bool eventHandled = Panel2D::mouseReleased(evt);

    if (NaviManager::Get().getFocusedNavi() != mNavi)
        return eventHandled;
    eventHandled = true;
    mMouseCaptured = false;
    int textureX, textureY;
    getTextureRelativePoint(evt.mState.mX, evt.mState.mY, textureX, textureY);
    mNavi->injectMouseUp(textureX, textureY);
    return eventHandled;
}

//-------------------------------------------------------------------------------------
void Panel2DWWW::getTextureSize(int& textureWidth, int& textureHeight)
{
    unsigned short w, h;
    mNavi->getExtents(w, h);
    textureWidth = (int)w;
    textureHeight = (int)h;
}

//-------------------------------------------------------------------------------------
void Panel2DWWW::onFocus(bool isFocused)
{
    if (!isFocused)
        NaviManager::Get().deFocusAllNavis();
    else
        NaviManager::Get().focusNavi(mNavi);
}

//-------------------------------------------------------------------------------------
void Panel2DWWW::onResized()
{
    Panel2D::onResized();

    // Ratio 1/1 enabled ?
    if (mState != SOriginal)
    {
        ButtonsMap::const_iterator btnIt = mButtons.find("ratio11");
        if (btnIt != mButtons.end())
            if (!btnIt->second->getEventState())
                return;
    }

    int clientWidth, clientHeight;
    getClientExtents(clientWidth, clientHeight);
    mNavi->resizeNavi(clientWidth, clientHeight);
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
