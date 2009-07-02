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

#include "Panel2DExtTextSrcEx.h"
#include "navigatorGui/GUI_ContextMenu.h"
#include <CTLog.h>

using namespace Ogre;
using namespace CommonTools;

namespace Solipsis {

const String Panel2DVLCFactory::ms_Type = "vlc";
const String Panel2DVNCFactory::ms_Type = "vnc";
const String Panel2DSWFFactory::ms_Type = "swf";

//-------------------------------------------------------------------------------------
Panel2DExtTextSrcEx::Panel2DExtTextSrcEx(const Ogre::String& type, const String& name) :
    Panel2D(type, name),
    mExtTextSrcEx(0),
    mMouseCaptured(false),
    mGotFocus(false)
{
}

//-------------------------------------------------------------------------------------
Panel2DExtTextSrcEx::~Panel2DExtTextSrcEx()
{
}

//-------------------------------------------------------------------------------------
bool Panel2DExtTextSrcEx::create(ushort zOrder)
{
    mMatName = mName;
    MaterialPtr material = MaterialManager::getSingleton().getByName(mMatName);
    if (material.isNull())
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Panel2DExtTextSrcEx::create() Material %s not found !", mMatName.c_str());
        return false;
    }

    String plugin = mType;
    if ((plugin != "vlc") && (plugin != "vnc") && (plugin != "swf"))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Panel2DExtTextSrcEx::create() Unknown plugin type %d !", mType);
        return false;
    }
    ExternalTextureSourceManager::getSingleton().setCurrentPlugIn(plugin);
    mExtTextSrcEx = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource(plugin));

    if (!Panel2D::create(zOrder))
        return false;

    // Set title
    if (mTitle != 0)
        mTitle->setCaption(plugin);

    return true;
}

//-------------------------------------------------------------------------------------
bool Panel2DExtTextSrcEx::keyPressed(const KeyboardEvt& evt)
{
    Evt kbdEvt;
    kbdEvt.mType = evt.mType;
    kbdEvt.mKeyboard = evt;
    mExtTextSrcEx->handleEvt(mMatName, Event(0, &kbdEvt));
    return true;
}

//-------------------------------------------------------------------------------------
bool Panel2DExtTextSrcEx::keyReleased(const KeyboardEvt& evt)
{
    Evt kbdEvt;
    kbdEvt.mType = evt.mType;
    kbdEvt.mKeyboard = evt;
    mExtTextSrcEx->handleEvt(mMatName, Event(0, &kbdEvt));
    return true;
}

//-------------------------------------------------------------------------------------
bool Panel2DExtTextSrcEx::mouseMoved(const MouseEvt& evt)
{
    bool eventHandled = Panel2D::mouseMoved(evt);

    if ((mPartOverMouse != PClient) && !mMouseCaptured)
        return eventHandled;
    eventHandled = true;
    Evt mouseEvt;
    mouseEvt.mType = evt.mType;
    mouseEvt.mMouse.mState = evt.mState;
    getTextureUVRelativePoint(evt.mState.mX, evt.mState.mY, mouseEvt.mMouse.mState.mXreal, mouseEvt.mMouse.mState.mYreal);
    mExtTextSrcEx->handleEvt(mMatName, Event(0, &mouseEvt));
    return eventHandled;
}

//-------------------------------------------------------------------------------------
bool Panel2DExtTextSrcEx::mousePressed(const MouseEvt& evt)
{
    bool eventHandled = Panel2D::mousePressed(evt);

    if (mPartOverMouse != PClient)
        return eventHandled;
    eventHandled = true;
    mMouseCaptured = true;
    String plugin = mType;
    bool showContextMenu = ((evt.mState.mButtons & MBRight) && !GUI_ContextMenu::isContextVisible());
    if (showContextMenu && ((plugin == "vnc") || (plugin == "swf")) && !mGotFocus)
        showContextMenu = false;
    if (showContextMenu)
    {
        if (plugin == "vlc")
            GUI_ContextMenu::createAndShowPanel(evt.mState.mX, evt.mState.mY, GUI_ContextMenu::NAVI_CTXTVLC, mName);
        else if (plugin == "vnc")
            GUI_ContextMenu::createAndShowPanel(evt.mState.mX, evt.mState.mY, GUI_ContextMenu::NAVI_CTXTVNC, mName);
        else if (plugin == "swf")
            GUI_ContextMenu::createAndShowPanel(evt.mState.mX, evt.mState.mY, GUI_ContextMenu::NAVI_CTXTSWF, mName);
        Panel2DMgr::getSingleton().defocus();
    }
    else
    {
        Evt mouseEvt;
        mouseEvt.mType = evt.mType;
        mouseEvt.mMouse.mState = evt.mState;
        getTextureUVRelativePoint(evt.mState.mX, evt.mState.mY, mouseEvt.mMouse.mState.mXreal, mouseEvt.mMouse.mState.mYreal);
        mExtTextSrcEx->handleEvt(mMatName, Event(0, &mouseEvt));
    }
    return eventHandled;
}

//-------------------------------------------------------------------------------------
bool Panel2DExtTextSrcEx::mouseReleased(const MouseEvt& evt)
{
    bool eventHandled = Panel2D::mouseReleased(evt);

    eventHandled = true;
    mMouseCaptured = false;
    mGotFocus = false;
    Evt mouseEvt;
    mouseEvt.mType = evt.mType;
    mouseEvt.mMouse.mState = evt.mState;
    getTextureUVRelativePoint(evt.mState.mX, evt.mState.mY, mouseEvt.mMouse.mState.mXreal, mouseEvt.mMouse.mState.mYreal);
    mExtTextSrcEx->handleEvt(mMatName, Event(0, &mouseEvt));
    return eventHandled;
}

//-------------------------------------------------------------------------------------
void Panel2DExtTextSrcEx::onFocus(bool isFocused)
{
    mGotFocus = true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
