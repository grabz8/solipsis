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

#ifndef __GUI_Avatar_h__
#define __GUI_Avatar_h__

#include "NavigatorGui.h"
#include "MainApplication/Navigator.h"
#include "GUI_Panel.h"

using namespace NaviLibrary;

namespace Solipsis 
{
    class Navigator;
 
    class GUI_Avatar : public GUI_Panel
    {
    public:
        static bool createAndShowPanel();
        static void unload();
        static bool isPanelVisible();

        virtual bool show();
        virtual void destroy();
        virtual void hide();

        bool isVisible();

    protected:
        GUI_Avatar();
        static GUI_Avatar * stGUI_Avatar;

        void avatarMainPageLoaded(const NaviData& naviData);
        void avatarMainFileOpen(const NaviData& naviData);
        void avatarMainFileEdit(const NaviData& naviData);
        void avatarMainFileSave(const NaviData& naviData);
        void avatarMainFileSaveAs(const NaviData& naviData);
        void avatarMainFileExit(const NaviData& naviData);
        //    void avatarMainSelectPrev(const NaviData& naviData);
        //	void avatarMainSelectNext(const NaviData& naviData);
        void avatarMainSelected(const NaviData& naviData);


        Navigator * mNavigator;
    };
} // namespace Solipsis

#endif // #ifndef __GUI_Avatar_h__