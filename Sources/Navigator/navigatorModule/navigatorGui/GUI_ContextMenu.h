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

#ifndef __GUI_ContextMenu_h__
#define __GUI_ContextMenu_h__

#include "NavigatorGui.h"
#include "MainApplication/Navigator.h"
#include "GUI_Panel.h"

using namespace NaviLibrary;

namespace Solipsis 
{
    class Navigator;

 

    class GUI_ContextMenu : public GUI_Panel
    {
    public:
        enum NaviContext
        {
            NAVI_CTXTAVATAR,
            NAVI_CTXTWWW,
            NAVI_CTXTVLC,
            NAVI_CTXTVNC,
            NAVI_CTXTSWF,

            NAVI_CTXTCOUNT
        };

        static bool createAndShowPanel(int x, int y, NaviContext ctxtPanel, const String& params);
        static void destroyMenu();
        static bool isContextVisible();
        static bool isContextFocused();

    protected:
        GUI_ContextMenu();
        virtual bool show(int x, int y, NaviContext ctxtPanel, const String& params);
        //virtual void hide();
        virtual void destroy();

        // clamp navi extents into screen
        void clampNaviOnScreen(int& x, int& y, int w, int h);

        // avatar callbacks
        void onAvatarSelect(Navi* caller, const Awesomium::JSArguments& args);
        void onPanelLoaded(Navi* caller, const Awesomium::JSArguments& args);

        // WWW callbacks 
        void onWWWPanelLoaded(Navi* caller, const Awesomium::JSArguments& args);
        void onWWWCommand(Navi* caller, const Awesomium::JSArguments& args);

        // SWF callbacks 
        void onSWFPanelLoaded(Navi* caller, const Awesomium::JSArguments& args);
        void onSWFCommand(Navi* caller, const Awesomium::JSArguments& args);

        // VLC callbacks 
        void onVLCPanelLoaded(Navi* caller, const Awesomium::JSArguments& args);
        void onVLCCommand(Navi* caller, const Awesomium::JSArguments& args);

        // VNC callbacks 
        void onVNCPanelLoaded(Navi* caller, const Awesomium::JSArguments& args);
        void onVNCCommand(Navi* caller, const Awesomium::JSArguments& args);

        static GUI_ContextMenu * stGUI_ContextMenu;
        static const std::string ms_NavisContexts[NAVI_CTXTCOUNT];

        NaviContext m_curContext;

    };
} // namespace Solipsis

#endif // #ifndef __GUI_ContextMenu_h__