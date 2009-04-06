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

#include "GUI_MessageBox.h"
// #include "MainApplication/Navigator.h"
// #include "MainApplication/NavigatorFrameListener.h"
// #include "Tools/DebugHelpers.h"
// #include <OgreTimer.h>
#include <CTLog.h>
// #include <CTIO.h>
#include <CTStringHelpers.h>
// #include <CTNetSocket.h>
// #include <CTSystem.h>
#include <Navi.h>

// #include "World/Modeler.h"
// #include <AvatarEditor.h>
// #include <CharacterManager.h>
// #include <Character.h>
// #include <CharacterInstance.h>
// #include <VoiceEngineManager.h>
// #include "World/Avatar.h"

#ifdef _MSC_VER
 #pragma warning (disable:4355)
#endif // _MSC_VER

using namespace Solipsis;
using namespace CommonTools;

GUI_MessageBox * GUI_MessageBox::st_GUI_MessageBox = NULL;

//-------------------------------------------------------------------------------------
void GUI_Panel::switchLuaNavi(bool createDestroy)
{
    if (m_curState == NavigatorGUI::NSNotCreated)
    {
        // Create Navi panel
        // Lua
        if (!Navigator::getSingletonPtr()->getNavigatorLua()->call("createGUI", "%s", mPanelName.c_str()))
        {
            LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorGUI::switchLuaNavi() Unable to create GUI called %s", mPanelName.c_str());
            return;
        }
        m_curState = NavigatorGUI::NSCreated;
        // the navi panel
        mNavi = NavigatorGUI::getNavi(mPanelName);
    }
    else
    {
        mNavi = NavigatorGUI::getNavi(mPanelName);
        if (!mNavi->getVisibility())
        {
            mNavi->show(true);
        }
        else
        {
            if (!createDestroy)
            {
                mNavi->hide(true);
                NaviManager::Get().deFocusAllNavis();
            }
            else
            {
                NavigatorGUI::destroyNavi(mNavi);
                m_curState = NavigatorGUI::NSNotCreated;
                mNavi = NULL;
            }
        }
    }
}

//-------------------------------------------------------------------------------------
void GUI_MessageBox::show(const std::string& titleText, 
                            const std::string& msgText, 
                            MsgBoxButtons buttons, 
                            MsgBoxIcon icon,
                            MsgBoxDisplayed msgBoxDisplayed)
{
    GUI_MessageBox * instance = getMsgBox();
    if (instance->m_curState != NavigatorGUI::NSCreated)
        hide();

    instance->switchLuaNavi(true);

    NaviLibrary::Navi* navi = NavigatorGUI::getNavi(mPanelName);
    navi->setModal(true);

    mMsgBoxTitleText = titleText;
    mMsgBoxMsgText = msgText;
    mMsgBoxButtons = buttons;
    mMsgBoxIcon = icon;

    navi->bind("pageLoaded", NaviDelegate(this, &GUI_MessageBox::messageBoxPageLoaded));
    navi->bind("response", NaviDelegate(this, &GUI_MessageBox::messageBoxResponse));
}

//-------------------------------------------------------------------------------------
void GUI_MessageBox::hide()
{
    if (m_curState != NavigatorGUI::NSCreated) 
        return;

    switchLuaNavi( true);
}

//-------------------------------------------------------------------------------------
bool GUI_MessageBox::isVisible()
{
    if (m_curState != NavigatorGUI::NSCreated) 
        return false;

    return ((mNavi != 0) && mNavi->getVisibility());
}

//-------------------------------------------------------------------------------------
void GUI_MessageBox::messageBoxPageLoaded(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::messageBoxPageLoaded()");

    mNavi->evaluateJS("$('titleText').innerHTML = '" + mMsgBoxTitleText + "'");
    mNavi->evaluateJS("$('msgText').innerHTML = '" + mMsgBoxMsgText + "'");
    mNavi->evaluateJS("setButtons(" + StringHelpers::toString(mMsgBoxButtons) + ")");
    mNavi->evaluateJS("setIcon(" + StringHelpers::toString(mMsgBoxIcon) + ")");

    // Show Navi UI message box
    if (m_curState == NavigatorGUI::NSCreated)
        mNavi->show(true);
}

//-------------------------------------------------------------------------------------
void GUI_MessageBox::messageBoxResponse(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::messageBoxResponse()");

    hide();

    switch (mMsgBoxDisplayed)
    {
    case MBD_WORLDSSERVERERROR:
    case MBD_AUTHENTFBERROR:
    case MBD_AUTHENTWSERROR:
        // Return to Navi UI login
    //    login();
        break;
    case MBD_WORLDSSERVERCOMPATIBILITYERROR:
        // Display the Worlds Server info page
    //    worldsServerInfo();
        break;
    case MBD_CONNECTIONERROR:
        Navigator::getSingletonPtr()->disconnect();
        break;
    case MBD_CONNECTIONLOSTERROR:
        break;
    }
    mMsgBoxDisplayed = MBD_NONE;
}