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

#ifndef __GUI_ChooseWorld_h__
#define __GUI_ChooseWorld_h__

// #include <Facebook.h>
#include "NavigatorGui.h"
#include "MainApplication/Navigator.h"
#include "GUI_Panel.h"
// #include "World/Modeler.h"
// #include "ModifiableMaterialObject.h"

using namespace NaviLibrary;

namespace Solipsis 
{
    class Navigator;

    /** This class manages all Graphical User Interfaces of the Navigator.
    */
    class GUI_ChooseWorld : public GUI_Panel
    {
    public:
        static bool createAndShowPanel();

        // Show the page
        virtual bool show();
        virtual void update();

        // ok is pressed
        void onOkPressed(const NaviData& naviData);
        void onCancelPressed(const NaviData& naviData);

         
    protected:
        GUI_ChooseWorld();

        void worldsServerCompatibilityError();
        void worldsServerError();
 
        ////// NaviEventListener Interface /////////////
        virtual void onNaviDataEvent(Navi *caller, const NaviData &naviData) {}
        virtual void onLinkClicked(Navi *caller, const std::string &linkHref) {}
        virtual void onLocationChange(Navi *caller, const std::string &url) {}
        virtual void onNavigateComplete(Navi *caller, const std::string &url, int responseCode);

        static GUI_ChooseWorld * stGUI_ChooseWorld;

        unsigned long mCurrentNaviCreationDate;


        Navigator * mNavigator;
    };

} // namespace Solipsis

#endif // #ifndef __GUI_ChooseWorld_h__