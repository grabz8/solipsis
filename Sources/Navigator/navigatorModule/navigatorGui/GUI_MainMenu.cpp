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
#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>

#include "Tools/DebugHelpers.h"

using namespace Solipsis;
using namespace CommonTools;
//-------------------------------------------------------------------------------------

GUI_MainMenu * GUI_MainMenu::stGUI_MainMenu = NULL;

GUI_MainMenu::GUI_MainMenu() : GUI_Panel("uimainmenu")
{
    stGUI_MainMenu = this;
}

bool GUI_MainMenu::createAndShowPanel()
{
    if (!stGUI_MainMenu)
    {
        new GUI_MainMenu();
    }

    return stGUI_MainMenu->show();
}


bool GUI_MainMenu::show()
{
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

