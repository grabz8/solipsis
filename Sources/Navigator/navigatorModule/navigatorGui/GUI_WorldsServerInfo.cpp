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

#include "GUI_WorldsServerInfo.h"

// #include "MainApplication/Navigator.h"
// #include "MainApplication/NavigatorFrameListener.h"
// #include "Tools/DebugHelpers.h"
#include <OgreTimer.h>
#include <CTLog.h>
#include <CTStringHelpers.h>
#include <Navi.h>

// #include "GUI_login.h"
// #include "GUI_MessageBox.h"

using namespace Solipsis;
using namespace CommonTools;

GUI_WorldsServerInfo * GUI_WorldsServerInfo::stGUI_WorldsServerInfo = NULL;

GUI_WorldsServerInfo::GUI_WorldsServerInfo() : GUI_Panel("uiinfows")
{
    stGUI_WorldsServerInfo = this;
    mNavigator = Navigator::getSingletonPtr();
    mCurrentNaviCreationDate = 0;
}

bool GUI_WorldsServerInfo::createAndShowPanel()
{
    if (!stGUI_WorldsServerInfo)
    {
        new GUI_WorldsServerInfo();
    }

    return stGUI_WorldsServerInfo->show();
}

bool GUI_WorldsServerInfo::show()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldsServerInfo()");

    // Hide previous Navi UI
    NavigatorGUI::destroyCurrentPanel();

    if (m_curState == NSNotCreated)
    {
        // Create Navi UI worlds server info
        // Prepare the url to the world server uiinfows.html page
        std::string uiinfowsUrl = "http://" + mNavigator->getWorldsServerAddress() + "/uiinfows.html";
        uiinfowsUrl += "?navVersion=" + StringHelpers::toHexString(mNavigator->getVersion());
        createNavi("uiinfows", "", NaviPosition(Center), 256, 256);
        mNavi->setMovable(false);
        mNavi->hide();
        mNavi->setOpacity(0.75f);
        mNavi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::naviToShowPageLoaded));
        mNavi->bind("ok", NaviDelegate(this, &NavigatorGUI::worldsServerInfoOk));
        // Add 1 event listener to detect network errors
        mNavi->addEventListener(&mWorldsServerEventListener);
        mNavi->navigateTo(uiinfowsUrl);
        m_curState = NSCreated;
    }

    // Set next Navi UI
    NavigatorGUI::setCurrentPanel(this);
    // set Timer
    mCurrentNaviCreationDate = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
    return true;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::onOkPressed(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldsServerInfoOk()");

    // Return to Navi UI login
    GUI_Login::createAndShowPanel();
}


//-------------------------------------------------------------------------------------
void GUI_WorldsServerInfo::onCancelPressed(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldCancel()");

    // Return to Navi UI login
    GUI_Login::createAndShowPanel();
}

//-------------------------------------------------------------------------------------
void GUI_WorldsServerInfo::onNavigateComplete(Navi *caller, const std::string &url, int responseCode)
{
    if (responseCode >= 400 && responseCode < 600)
    {
        if (responseCode == 409)
            worldsServerCompatibilityError();
        else
            worldsServerError();
    }
}



//-------------------------------------------------------------------------------------
void GUI_WorldsServerInfo::worldsServerCompatibilityError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldsServerCompatibilityError()");

    worldsServerInfo();

    GUI_MessageBox::show(
        "Compatibility error", 
        "Your Navigator (version " + StringHelpers::getVersionString(mNavigator->getVersion()) + ") is not compatible<br/>with this Worlds Server !<br/><br/>Upgrade your Navigator and connect again.", 
        GUI_MessageBox::MBB_OK, 
        GUI_MessageBox::MBB_ERROR);

    mCurrentNaviCreationDate = 0; 
}
//-------------------------------------------------

//-------------------------------------------------------------------------------------
void GUI_WorldsServerInfo::worldsServerError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldsServerError()");

    GUI_Login::createAndShowPanel();

    std::string wsHost, wsPort;
    CommonTools::StringHelpers::getURLHostPort(mNavigator->getWorldsServerAddress(), wsHost, wsPort);
    GUI_MessageBox::getMsgBox()->show(
        "Network error", "Unable to connect to the Worlds Server !<br/>Check your Internet connection and configure your firewall<br/>(TCP port " + wsPort + ").", 
        GUI_MessageBox::MBB_OK, 
        GUI_MessageBox::MBB_ERROR);

    mCurrentNaviCreationDate = 0;
}

void GUI_WorldsServerInfo::update()
{    
    unsigned long now = Ogre::Root::getSingleton().getTimer()->getMilliseconds();

    // Worlds server page loaded ?
    if ((mCurrentNaviCreationDate != 0) &&
        (now - mCurrentNaviCreationDate > (unsigned long)mNavigator->getWorldsServerTimeout()*1000))
    {
        worldsServerError();
    }
}


