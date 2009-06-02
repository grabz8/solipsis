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

#include "GUI_Commands.h"
#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>

using namespace Solipsis;
using namespace CommonTools;
//-------------------------------------------------------------------------------------

GUI_Commands * GUI_Commands::stGUI_Commands = NULL;

GUI_Commands::GUI_Commands() : GUI_Panel("uicommands")
{
    stGUI_Commands = this;
}

void GUI_Commands::showHide()
{
    if (!stGUI_Commands)
    {
        new GUI_Commands();
    }

    if (stGUI_Commands->isVisible())
        stGUI_Commands->hide();
   else
       stGUI_Commands->show();
}

bool GUI_Commands::show()
{
    // Create Navi UI Chat panel bar
    // Lua
    if (m_curState == NSNotCreated)
    {  
        createNavi("local://uicommands.html", Center, 512, 256);
        mNavi = NavigatorGUI::getNavi(mPanelName);
        mNavi->hide();
        mNavi->setMovable(true);
        mNavi->setOpacity(0.75);

        m_curState = NSCreated;

        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_Commands::onPageLoaded));
        mNavi->bind("pageClosed", NaviDelegate(this, &GUI_Commands::onPageClosed));
    }
    else
        mNavi->show();

    return true;
}

void GUI_Commands::onPageLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    mNavi->show();
}

void GUI_Commands::onPageClosed(Navi* caller, const Awesomium::JSArguments& args)
{
    mNavi->hide();
}
