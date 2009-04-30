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

#include "GUI_About.h"
#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>

using namespace Solipsis;
using namespace CommonTools;
//-------------------------------------------------------------------------------------

GUI_About * GUI_About::stGUI_About = NULL;

GUI_About::GUI_About() : GUI_Panel("uiabout")
{
    stGUI_About = this;
}

void GUI_About::showHide()
{
    if (!stGUI_About)
    {
        new GUI_About();
    }

    if (stGUI_About->isVisible())
        stGUI_About->hide();
   else
       stGUI_About->show();
}

bool GUI_About::show()
{
    // Create Navi UI Chat panel bar
    // Lua
    if (m_curState == NSNotCreated)
    {
//         if guiName == "uiabout" then
//             -- Create Navi UI about
//             x, y, w, h = fitNaviOnScreen(512, 512)
//             naviMgrCreateNavi("uiabout", "http://www.solipsis.org", "Center", 0, 16, w, h, true, true)
//             naviSetOpacity("uiabout", 0.75)
//             return true
//         else

//         -- screen fitting computation
//             function fitNaviOnScreen(w, h)
//             local scrWidth, scrHeight = navigator:getRenderWinMetrics()
//             while w > scrWidth or h > scrHeight do
//         if w > scrWidth then w = w/2 end
//             if h > scrHeight then h = h/2 end
//                 end
//                 local x = scrWidth/2 - w/2
//                 local y = scrHeight/2 - h/2
//                 return x, y, w, h
//                 end

        int w = 1024, h = 1024;  
        fitOnScreen(w, h);
        createNavi("local://uiabout.html", Center, w, h);

        mNavi = NavigatorGUI::getNavi(mPanelName);
        mNavi->show();
        mNavi->setMovable(true);
        mNavi->setIgnoreBounds(true);
        mNavi->setOpacity(0.9);

        mNavi->bind("pageClosed", NaviDelegate(this, &GUI_About::onClose));

        m_curState = NSCreated;
    }
    else
         mNavi->show();

    return true;
}

void GUI_About::onClose(const NaviData& naviData)
{
    destroy();
}