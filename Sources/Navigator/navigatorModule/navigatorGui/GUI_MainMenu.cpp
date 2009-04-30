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

#include "GUI_MainMenu.h"
#include "GUI_Chat.h"
#include "GUI_Avatar.h"
#include "GUI_Modeler.h"
#include "GUI_Commands.h"
#include "GUI_About.h"

#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>

#include "tools/trace.h"

#include "Tools/DebugHelpers.h"

using namespace Solipsis;
using namespace CommonTools;
//-------------------------------------------------------------------------------------

GUI_MainMenu * GUI_MainMenu::stGUI_MainMenu = NULL;

GUI_MainMenu::GUI_MainMenu() : GUI_Panel("uimainmenu")
{
    stGUI_MainMenu = this;
    mNavigator = Navigator::getSingletonPtr();
}

bool GUI_MainMenu::createAndShowPanel()
{
    if (!stGUI_MainMenu)
    {
        new GUI_MainMenu();
    }

    return stGUI_MainMenu->show();
}

void GUI_MainMenu::showHide(bool bShow)
{
    if (!stGUI_MainMenu)
    {
        return; 
    }

    if (bShow)
        stGUI_MainMenu->show();
    else
        stGUI_MainMenu->hide();
}



bool GUI_MainMenu::show()
{
    if (m_curState == NSCreated)
        return true;

    if (m_curState == NavigatorGUI::NSNotCreated)
    {
        // Create Navi panel
        // Lua
        createNavi("local://uimainmenu.html", TopLeft, 512, 16);
        mNavi = NavigatorGUI::getNavi(mPanelName);
        mNavi->setMask("alphafade512x16.png");
        mNavi->hide();
        mNavi->setMovable(false);
        mNavi->setIgnoreBounds(true);

        mNavi->addEventListener(this);

        m_curState = NSCreated;
    }

    if (!GUI_Panel::show())
        return false;

#ifdef UIDEBUG
    mNavi->bind("debugCommand", NaviDelegate(this, &GUI_MainMenu::debugCommand));
#endif

    return true;
}

#ifdef UIDEBUG
//-------------------------------------------------------------------------------------
void GUI_MainMenu::debugCommand(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::debugCommand()");

    // Get message to send
    std::string cmd;
    std::string params;
    cmd = naviData["cmd"].str();
    params = naviData["params"].str();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "cmd=%s, params=%s", cmd.c_str(), params.c_str());

    // Push debug command
    DebugHelpers::debugCommands[String(cmd)] = String(params);
}
#endif

void GUI_MainMenu::onNaviDataEvent(Navi *caller, const NaviData &naviData)
{
    m_curState = NSCreated;
   // OutputDebugTrace("GUI_MainMenu::onNaviDataEvent name %s\n" , naviData.getName().c_str());
    if (naviData.getName() == "menuClick" && naviData.size())
    {
     //   OutputDebugTrace("GUI_MainMenu::onNaviDataEvent data %s\n" , naviData["item"].str().c_str());
        onClick(naviData["item"].str());

    }
    //  
}

void GUI_MainMenu::onLinkClicked(Navi *caller, const std::string &linkHref)
{
    m_curState = NSCreated;
}

//-------------------------------------------------------------------------------------
void GUI_MainMenu::onClick(const String& item)
{
    // Perform action associated to item selected
    // Submenu File
    if (item == "Exit")
        mNavigator->disconnect();
    // Submenu Action
    else if (item == "Talk")
        mNavigator->toggleVoIP();
    // Submenu View
    else if (item == "1stPerson")
        mNavigator->setCameraMode(Navigator::CM1stPerson);
    else if (item == "1stPersonMouse")
        mNavigator->setCameraMode(Navigator::CM1stPersonWithMouse);
    else if (item == "3rdPerson")
        mNavigator->setCameraMode(Navigator::CM3rdPerson);
    else if (item == "Orbit")
        mNavigator->setCameraMode(Navigator::CMAroundPerson);
    // Submenu Panels
    else if (item == "Chat")
        GUI_Chat::showHide();

    else if (item == "Avatar")
    {
        if (mNavigator->getState() == Navigator::SInWorld)
        {
            mNavigator->setCameraMode(Navigator::CMAroundPerson);
            GUI_Avatar::createAndShowPanel();
        }
        else if (mNavigator->getState() == Navigator::SAvatarEdit)
        {
            GUI_Avatar::unload();
            mNavigator->setCameraMode(mNavigator->getLastCameraMode());
        }
    }
    else if (item == "Modeler")
    {
        if (mNavigator->getState() == Navigator::SInWorld)
        {
            mNavigator->setCameraMode(Navigator::CMModeling);
            GUI_Modeler::createAndShowPanel();
        }
        else if (mNavigator->getState() == Navigator::SModeling)
        {
            GUI_Modeler::unload();
            mNavigator->setCameraMode(mNavigator->getLastCameraMode());
        }
    }
    // Submenu Help
    else if (item == "About")
        GUI_About::showHide();
    else if (item == "Commands")
        GUI_Commands::showHide();
}
