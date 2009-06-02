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

#include "GUI_AuthentWorldServer.h"
#include <CTStringHelpers.h>
#include <Navi.h>
#include "GUI_Login.h"


using namespace Solipsis;
using namespace CommonTools;
//-------------------------------------------------------------------------------------


GUI_AuthentWorldServer * GUI_AuthentWorldServer::stGUI_AuthentWorldServer = NULL;

GUI_AuthentWorldServer::GUI_AuthentWorldServer() : GUI_FromServer("uiauthentws")
{
    stGUI_AuthentWorldServer = this;
}

bool GUI_AuthentWorldServer::createAndShowPanel(const std::string& pwd)
{
    if (!stGUI_AuthentWorldServer)
    {
        new GUI_AuthentWorldServer();
    }

    return stGUI_AuthentWorldServer->show(pwd);
}

bool GUI_AuthentWorldServer::show(const std::string& pwd)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentWorldsServer()");

    // Hide previous Navi UI
    NavigatorGUI::destroyCurrentPanel();

    if (m_curState == NSNotCreated)
    {
        // Create Navi UI authentication on Worlds server
        // Prepare the url to the world server uiauthentws.html page
        std::string uiauthentwsUrl = "http://" + mNavigator->getWorldsServerAddress() + "/uiauthentws.html";
        uiauthentwsUrl += "?navVersion=" + StringHelpers::toHexString(mNavigator->getVersion());
        uiauthentwsUrl += "&login=" + mNavigator->getLogin() + "&pwd=" + pwd;
        createNavi( "", NaviPosition(Center), 256, 128);
        mNavi->setMovable(false);
        mNavi->hide();
        mNavi->setOpacity(0.75f);
        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_Panel::onPanelLoaded));
        mNavi->bind("ok", NaviDelegate(this, &GUI_AuthentWorldServer::onOk));
        // Add 1 event listener to detect network errors
        mNavi->addEventListener(this);
        mNavi->loadURL(uiauthentwsUrl);
        m_curState = NSCreated;
    }

    NavigatorGUI::setCurrentPanel(this);

    // set Timer
    mCurrentNaviCreationDate = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
    return true;
}

// jamais appelé
// //-------------------------------------------------------------------------------------
// void NavigatorGUI::authentWorldsServerError()
// {
//     LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentWorldsServerError()");
// 
//     std::string wsHost, wsPort;
//     CommonTools::StringHelpers::getURLHostPort(mNavigator->getWorldsServerAddress(), wsHost, wsPort);
//     GUI_MessageBox::getMsgBox()->show("Authentication error", "Authentication failed !", 
//         GUI_MessageBox::MBB_OK, GUI_MessageBox::MBB_ERROR);
// 
//     login();
// }

//-------------------------------------------------------------------------------------
void GUI_AuthentWorldServer::onOk(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentWorldsServerOk()");

    std::string result = args.at(0).toString();
    NodeId nodeId = args.at(1).toString();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentWorldsServerOk() result=%s, nodeId=%s", result.c_str(), nodeId.c_str());
    if (nodeId.empty())
    {
        GUI_Login::createAndShowPanel();;
        return;
    }

    mNavigator->setNodeId(XmlHelpers::convertAuthentTypeToRepr(ATSolipsis) + nodeId);
    // Call connect
    bool connected = mNavigator->connect();
}