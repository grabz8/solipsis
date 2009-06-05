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
#include <CTStringHelpers.h>
#include <Navi.h>
#include "GUI_Login.h"


using namespace Solipsis;
using namespace CommonTools;
//-------------------------------------------------------------------------------------


GUI_WorldsServerInfo * GUI_WorldsServerInfo::stGUI_WorldsServerInfo = NULL;

GUI_WorldsServerInfo::GUI_WorldsServerInfo() : GUI_FromServer("uiinfows")
{
    stGUI_WorldsServerInfo = this;
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
        createNavi( Center, 256, 256);
        mNavi->setMovable(false);
        mNavi->hide();
        mNavi->setOpacity(0.75f);
        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_Panel::onPanelLoaded));
        mNavi->bind("ok", NaviDelegate(this, &GUI_WorldsServerInfo::onOkPressed));
        // Add 1 event listener to detect network errors
        mNavi->addEventListener(this);
        mNavi->loadURL(uiinfowsUrl);
        m_curState = NSCreated;
    }

    NavigatorGUI::setCurrentPanel(this);

    // set Timer
    mCurrentNaviCreationDate = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
    return true;
}

//-------------------------------------------------------------------------------------
void GUI_WorldsServerInfo::onOkPressed(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldsServerInfoOk()");

    // Return to Navi UI login
    GUI_Login::createAndShowPanel();
}
