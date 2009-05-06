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

void clampNaviOnScreen(int &x, int & y, int w, int h )
{
    int cx = x - w/2;
    int cy = y - h/2;

    unsigned int scrWidth, scrHeight, colourDepth;
    int left, top;
    Navigator::getSingletonPtr()->getRenderWindowPtr()->getMetrics(scrWidth, scrHeight, colourDepth, left, top);

    if (cx > ((int) scrWidth - w)) 
        cx = ((int) scrWidth - w);
    else if (cx < 0)
        cx = 0;

    if (cy > ((int) scrHeight - h)) 
        cy = ((int) scrHeight - h);
    else if (cy < 0)
        cy = 0;
}





bool GUI_ContextMenu::show(int x, int y, NaviContext ctxtPanel, const String& params)
{
    if (m_curState != NSNotCreated)
        destroy();

    m_curContext = ctxtPanel;
    mPanelName = ms_NavisContexts[m_curContext];

    switch (m_curContext)
    {
  /*  case NAVI_CTXTAVATAR:
        {
            int naviW = 256, naviH = 256;
              // set navi position
            clampNaviOnScreen(x, y, naviW, naviH);
    //        CreateNavi() "", x, y, naviW, naviH, false, false);


            createNavi("", x, y, naviW, naviH);         
            mNavi->setMovable(false);
            mNavi->show();

            mNavi->setColorKey( "#010203", 0, "#000000");
            mNavi->setOpacity( 0.75);

            NaviLibrary::NaviData naviData("uictxtavatarDatas");
            naviData["items"] = params;
            naviData["itemWidth"] = "50";
            naviData["itemHeight"] = "50";

                //                 naviDatas["items"] = items
                //                 naviDatas["itemWidth"] = itemW
                //                 naviDatas["itemHeight"] = itemH
                //                 naviNavigateTo(guiName, "local://" .. guiName .. ".html", naviDatas)            mNavi->navigateTo()
        }
       


        break;
//         if guiName == "uictxtavatar" then
//             -- Create Navi UI context about avatar
//             local args = { ... }
//         local x, y, items = args[1], args[2], args[3]
//         logMessage(string.format("x, y, items = %d, %d, %s", x, y, items))
//             local itemW, itemH = 50, 50
//             local naviW, naviH = 256, 256
//             x, y = clampNaviOnScreen(x, y, naviW, naviH)
//             if not naviMgrIsNaviExists(guiName) then
//                 naviMgrCreateNavi(guiName, "", x, y, naviW, naviH, false, false)
//                 naviSetColorKey(guiName, "#010203", 0, "#000000")
//                 naviSetOpacity(guiName, 0.75)
//                 naviAddEventListener(guiName, guiName .. "Listener")
//             else
//             naviSetPosition(guiName, x, y)
//             end

//             local naviDatas = {}
//             naviDatas["naviDataName"] = guiName .. "Datas"
//                 naviDatas["items"] = items
//                 naviDatas["itemWidth"] = itemW
//                 naviDatas["itemHeight"] = itemH
//                 naviNavigateTo(guiName, "local://" .. guiName .. ".html", naviDatas)
//                 return true
//         else



        break;*/

    default:
        // Create Navi UI context
        // Lua
        if (!Navigator::getSingletonPtr()->getNavigatorLua()->call("createGUI", "%s%d%d%s", mPanelName.c_str(), x, y, params.c_str()))
        {
            LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorGUI::contextShow() Unable to create GUI called %s", mPanelName.c_str());
            return false;
        }
        mNavi = NavigatorGUI::getNavi(mPanelName);
   }

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
