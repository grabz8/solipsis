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

#include "GUI_StatusBar.h"
#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>

using namespace Solipsis;
using namespace CommonTools;
//-------------------------------------------------------------------------------------

GUI_StatusBar * GUI_StatusBar::stGUI_StatusBar = NULL;

GUI_StatusBar::GUI_StatusBar() : GUI_Panel("uistatusbar")
{
    stGUI_StatusBar = this;
}

bool GUI_StatusBar::createAndShowPanel()
{
    if (!stGUI_StatusBar)
    {
        new GUI_StatusBar();
    }

    return stGUI_StatusBar->show();
}


bool GUI_StatusBar::show()
{
    // Create Navi UI status bar
    // Lua
    if (m_curState == NSNotCreated)
    {
//         if guiName == "uistatusbar" then
//             -- Create Navi UI status bar
//             naviMgrCreateNavi("uistatusbar", "local://uistatusbar.html", "BottomLeft", 0, 0, 512, 16, false, false)
//             naviSetMask("uistatusbar", "alphafade512x16.png")
//             naviSetIgnoreBounds("uistatusbar", true)
//             return true
//         else
        
        createNavi("local://uistatusbar.html", BottomLeft, 512, 16);

        mNavi = NavigatorGUI::getNavi(mPanelName);
        mNavi->setMask("alphafade512x16.png");
        mNavi->hide();
        mNavi->setMovable(false);
        mNavi->setIgnoreBounds(true);

        m_curState = NSCreated;
    }

    return true;
}

void GUI_StatusBar::updateBar()
{
    if (!stGUI_StatusBar)
        return;

    stGUI_StatusBar->update();

}

void GUI_StatusBar::update()
{
    if (!stGUI_StatusBar)
        return;
    unsigned long now = Ogre::Root::getSingleton().getTimer()->getMilliseconds();

    // Status bar update
    if ((stGUI_StatusBar->mStatusBarDisplayDate != 0) && (now - mStatusBarDisplayDate > 8*1000))
    {
        if (stGUI_StatusBar->mNavi == 0) return;
        if (stGUI_StatusBar->mNavi->getVisibility())
            stGUI_StatusBar->mNavi->hide(true);

        stGUI_StatusBar->mStatusBarDisplayDate = 0;
    }
}

//-------------------------------------------------------------------------------------
void GUI_StatusBar::setStatusBarText(const std::string& statusText)
{
    if (!stGUI_StatusBar)
        return;

    stGUI_StatusBar->mNavi->evaluateJS("$('statusbarText').innerHTML = '" + statusText + "'");
    stGUI_StatusBar->mStatusBarDisplayDate = Ogre::Root::getSingleton().getTimer()->getMilliseconds();

    if (!stGUI_StatusBar->mNavi->getVisibility())
        stGUI_StatusBar->mNavi->show(true);
}


void GUI_StatusBar::windowResized(RenderWindow* rw)
{
    if (mNavi)
    {
        //Adjust mouse clipping area
        unsigned int width, height, depth;
        int left, top;
        rw->getMetrics(width, height, depth, left, top);
        mNavi->setPosition(NaviPosition(0,height-16));

    }

}

