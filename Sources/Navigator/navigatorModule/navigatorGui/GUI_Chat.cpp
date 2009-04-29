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

#include "GUI_Chat.h"
#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>

using namespace Solipsis;
using namespace CommonTools;
//-------------------------------------------------------------------------------------

GUI_Chat * GUI_Chat::stGUI_Chat = NULL;

GUI_Chat::GUI_Chat() : GUI_Panel("uichat")
{
    stGUI_Chat = this;
}

void GUI_Chat::showHide()
{
    if (!stGUI_Chat)
    {
        new GUI_Chat();
    }

    if (stGUI_Chat->isVisible())
        stGUI_Chat->hide();
   else
       stGUI_Chat->show();
}

bool GUI_Chat::show()
{
    // Create Navi UI Chat panel bar
    // Lua
    if (m_curState == NSNotCreated)
    {
        if (!Navigator::getSingletonPtr()->getNavigatorLua()->call("createGUI", "%s", mPanelName.c_str()))
            throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create GUI called " + mPanelName, "NavigatorGUI::inWorld()"); 

        m_curState = NSCreated;
        mNavi = NavigatorGUI::getNavi(mPanelName);
    }

    return true;
}

//-------------------------------------------------------------------------------------
void GUI_Chat::addText(const std::wstring& message)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::addChatText()");

    // Navi MultiValue will encode the wstring in URI encoded string and add 1 call to decodeURIComponent on it
    stGUI_Chat->mNavi->evaluateJS("$('textChat').value += ?", NaviLibrary::NaviUtilities::Args(message));
    stGUI_Chat->mNavi->evaluateJS("$('textChat').value += '\\n'");
    stGUI_Chat->mNavi->evaluateJS("$('textChat').scrollTop = $('textChat').scrollHeight;");
}