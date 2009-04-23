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

#include "GUI_ContextMenu.h"
#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>

#include "Tools/DebugHelpers.h"

using namespace Solipsis;
using namespace CommonTools;
//-------------------------------------------------------------------------------------

const std::string GUI_ContextMenu::ms_NavisContexts[] = 
{
    "uictxtavatar",
    "uictxtwww",
    "uictxtswf",
    "uictxtvlc",
    "uictxtvnc"
};

GUI_ContextMenu * GUI_ContextMenu::stGUI_ContextMenu = NULL;

GUI_ContextMenu::GUI_ContextMenu() : GUI_Panel("noContext")
{
    stGUI_ContextMenu = this;
}

bool GUI_ContextMenu::createAndShowPanel(int x, int y, NaviContext ctxtPanel, const String& params)
{
    if (!stGUI_ContextMenu)
    {
        new GUI_ContextMenu();
    }

    return stGUI_ContextMenu->show(x, y, ctxtPanel, params);
}

void GUI_ContextMenu::hideMenu()
{
    if (!stGUI_ContextMenu) return;

    stGUI_ContextMenu->hide();
}

void GUI_ContextMenu::hide()
{
    if (m_curState == NSNotCreated || !mNavi) return;

    // Hide Navi UI context
    mNavi->hide();
    m_curState = NSNotCreated;
}

bool GUI_ContextMenu::show(int x, int y, NaviContext ctxtPanel, const String& params)
{
    if (m_curState != NSNotCreated)
        destroy();

    m_curContext = ctxtPanel;
    mPanelName = ms_NavisContexts[m_curContext];

    // Create Navi UI context
    // Lua
    if (!Navigator::getSingletonPtr()->getNavigatorLua()->call("createGUI", "%s%d%d%s", mPanelName.c_str(), x, y, params.c_str()))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorGUI::contextShow() Unable to create GUI called %s", mPanelName.c_str());
        return false;
    }
    mNavi = NavigatorGUI::getNavi(mPanelName);

    m_curState = NSCreated;
    return true;
}

//-------------------------------------------------------------------------------------
bool GUI_ContextMenu::isContextVisible()
{
    if (!stGUI_ContextMenu || !stGUI_ContextMenu->mNavi) return false;

    return (stGUI_ContextMenu->mNavi->getVisibility());
}

//-------------------------------------------------------------------------------------
bool GUI_ContextMenu::isContextFocused()
{
    if (!stGUI_ContextMenu || !stGUI_ContextMenu->mNavi) return false;
    return (stGUI_ContextMenu->mNavi == NaviManager::Get().getFocusedNavi());
}

//-------------------------------------------------------------------------------------
void GUI_ContextMenu::destroy()
{
    if (m_curState == NSNotCreated || !mNavi) return;

    // Destroy Navi UI context
    NaviManager::Get().destroyNavi(mNavi);
    m_curState = NSNotCreated;
    mNavi = NULL;
}
