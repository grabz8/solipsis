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

#include "GUI_ChooseWorld.h"

// #include "MainApplication/Navigator.h"
// #include "MainApplication/NavigatorFrameListener.h"
// #include "Tools/DebugHelpers.h"
#include <OgreTimer.h>
#include <CTLog.h>
#include <CTStringHelpers.h>
#include <Navi.h>

#include "GUI_login.h"
#include "GUI_MessageBox.h"


using namespace Solipsis;
using namespace CommonTools;

////////////////////////////////////////////////////////////////////////

GUI_ChooseWorld * GUI_ChooseWorld::stGUI_ChooseWorld = NULL;

GUI_ChooseWorld::GUI_ChooseWorld() : GUI_FromServer("uiworlds")
{
    stGUI_ChooseWorld = this;
}

bool GUI_ChooseWorld::createAndShowPanel()
{
    if (!stGUI_ChooseWorld)
    {
        new GUI_ChooseWorld();
    }

    return stGUI_ChooseWorld->show();
}

//-------------------------------------------------------------------------------------
bool GUI_ChooseWorld::show()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::world()");
    NavigatorGUI::destroyCurrentPanel();

    if (m_curState == NSNotCreated)
    {
        // Create Navi UI worlds
        // Prepare the url to the world server uiworlds.html page
        std::string uiworldsUrl = "http://" + mNavigator->getWorldsServerAddress() + "/uiworlds.html";
        uiworldsUrl += "?navVersion=" + StringHelpers::toHexString(mNavigator->getVersion());
        std::string localWorldHost = mNavigator->getLocalWorldAddress();
        // Add the local world ?
        if (!mNavigator->getLocalWorldAddress().empty())
            uiworldsUrl += "&localWorld=" + mNavigator->getLocalWorldAddress();
        createNavi( "", NaviPosition(Center), 256, 256);
        
        mNavi->setMovable(false);
        mNavi->hide();
        mNavi->setOpacity(0.75f);

        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_Panel::onPanelLoaded));
        mNavi->bind("ok", NaviDelegate(this, &GUI_ChooseWorld::onOkPressed));
        mNavi->bind("cancel", NaviDelegate(this, &GUI_ChooseWorld::onCancelPressed));

        // Add 1 event listener to detect network errors
        mNavi->addEventListener(this);
        mNavi->loadURL(uiworldsUrl);
        m_curState = NSCreated;
    }

    NavigatorGUI::setCurrentPanel(this);

    // set Timer
    mCurrentNaviCreationDate = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
    return true;
}

//-------------------------------------------------------------------------------------
void GUI_ChooseWorld::onOkPressed(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldOk()");

    std::string world = args[0].toString();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldOk() world=%s", world.c_str());
    std::string worldHost, worldPort;
    CommonTools::StringHelpers::getURLHostPort(world, worldHost, worldPort);
    mNavigator->setWorldAddress(world);

    // extended datas associated to the world server : voice IP server, VNC server, VLC server, ...
    std::string voipServer = args[1].toString();
    if (voipServer.size())
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldOk() voipServer=%s", voipServer.c_str());
        mNavigator->setVoIPServerAddress(voipServer);
    }
    std::string vncServer = args[2].toString();
    if (vncServer.size())
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldOk() vncServer=%s", vncServer.c_str());
    }
    std::string vlcServer = args[2].toString();
    if (vlcServer.size())
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldOk() vlcServer=%s", vlcServer.c_str());
    }

    // Return to Navi UI login
    GUI_Login::createAndShowPanel();
}

//-------------------------------------------------------------------------------------
void GUI_ChooseWorld::onCancelPressed(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldCancel()");

    // Return to Navi UI login
    GUI_Login::createAndShowPanel();
}

