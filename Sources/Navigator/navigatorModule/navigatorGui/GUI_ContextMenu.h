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

        // avatar callbacks
       void onAvatarSelect(const NaviData& naviData);
        void onPanelLoaded(const NaviData& naviData);

        // WWW callbacks 
       void onWWWPanelLoaded(const NaviData& naviData);
       void onWWWCommand(const NaviData& naviData);

       // SWF callbacks 
       void onSWFPanelLoaded(const NaviData& naviData);
       void onSWFCommand(const NaviData& naviData);

       // VLC callbacks 
       void onVLCPanelLoaded(const NaviData& naviData);
       void onVLCCommand(const NaviData& naviData);

       // VNC callbacks 
       void onVNCPanelLoaded(const NaviData& naviData);
       void onVNCCommand(const NaviData& naviData);

        static GUI_ContextMenu * stGUI_ContextMenu;
        static const std::string ms_NavisContexts[NAVI_CTXTCOUNT];

        NaviContext m_curContext;

    };
} // namespace Solipsis

#endif // #ifndef __GUI_ContextMenu_h__