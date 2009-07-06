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

#ifndef __GUI_MainMenu_h__
#define __GUI_MainMenu_h__

#include "NavigatorGui.h"
#include "MainApplication/Navigator.h"
#include "GUI_Panel.h"

using namespace NaviLibrary;

namespace Solipsis 
{
    class Navigator;

    class GUI_MainMenu : public GUI_Panel, public NaviEventListener
    {
    public:
        static bool createAndShowPanel();
        static void showHide(bool bShow);
        virtual bool show();

        ////// NaviEventListener Interface /////////////
        virtual void onNaviDataEvent(Navi *caller, const NaviData &naviData);
        virtual void onLinkClicked(Navi *caller, const std::string &linkHref) {}
        virtual void onLocationChange(Navi *caller, const std::string &url) {}
        virtual void onNavigateComplete(Navi *caller, const std::string &url, int responseCode) {}

#ifdef UIDEBUG       
        void GUI_MainMenu::debugCommand(const NaviData& naviData);
#endif

    protected:
        void onClick(const String& item);

        GUI_MainMenu();
        static GUI_MainMenu * stGUI_MainMenu;
        Navigator * mNavigator;


    };
} // namespace Solipsis

#endif // #ifndef __GUI_MainMenu_h__