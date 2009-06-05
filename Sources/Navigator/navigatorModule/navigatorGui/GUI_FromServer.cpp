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

#include "GUI_FromServer.h"
#include <CTLog.h>
#include <CTStringHelpers.h>
#include <Navi.h>

#include "GUI_Login.h"
#include "GUI_MessageBox.h"
#include "GUI_WorldsServerInfo.h"

using namespace Solipsis;
using namespace CommonTools;

/////////////////

GUI_FromServer::GUI_FromServer(const std::string & panelName) : GUI_Panel(panelName)
{
    mNavigator = Navigator::getSingletonPtr();
    mCurrentNaviCreationDate = 0;
}


//-------------------------------------------------------------------------------------
void GUI_FromServer::onNavigateComplete(Navi *caller, const std::string &url, int responseCode)
{
    if (responseCode >= 400 && responseCode < 600)
    {
        if (responseCode == 409)
            serverCompatibilityError();
        else
            serverError();
    }
}


//-------------------------------------------------------------------------------------
void GUI_FromServer::serverError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldsServerError()");

    GUI_Login::createAndShowPanel();

    std::string wsHost, wsPort;
    CommonTools::StringHelpers::getURLHostPort(mNavigator->getWorldsServerAddress(), wsHost, wsPort);
    GUI_MessageBox::getMsgBox()->show(
        "Network error", "Unable to connect to the Server !<br/>Check your Internet connection and configure your firewall<br/>(TCP port " + wsPort + ").", 
        GUI_MessageBox::MBB_OK, 
        GUI_MessageBox::MBB_ERROR);

    mCurrentNaviCreationDate = 0;
}

//-------------------------------------------------------------------------------------
void GUI_FromServer::serverCompatibilityError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldsServerCompatibilityError()");

    GUI_WorldsServerInfo::createAndShowPanel();

    GUI_MessageBox::show(
        "Compatibility error", 
        "Your Navigator (version " + StringHelpers::getVersionString(mNavigator->getVersion()) + ") is not compatible<br/>with this Worlds Server !<br/><br/>Upgrade your Navigator and connect again.", 
        GUI_MessageBox::MBB_OK, 
        GUI_MessageBox::MBB_ERROR);

    mCurrentNaviCreationDate = 0; 
}

//-------------------------------------------------

void GUI_FromServer::update()
{    
    unsigned long now = Ogre::Root::getSingleton().getTimer()->getMilliseconds();

    // Worlds server page loaded ?
    if ((mCurrentNaviCreationDate != 0) &&
        (now - mCurrentNaviCreationDate > (unsigned long)mNavigator->getWorldsServerTimeout()*1000))
    {
        serverError();
    }
}
