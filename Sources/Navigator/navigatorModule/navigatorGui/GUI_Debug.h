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

#ifndef __GUI_Debug_h__
#define __GUI_Debug_h__

// #include <Facebook.h>
#include "NavigatorGui.h"
#include "MainApplication/Navigator.h"
#include "GUI_Panel.h"
// #include "World/Modeler.h"
// #include "ModifiableMaterialObject.h"

using namespace NaviLibrary;
#ifdef UIDEBUG
namespace Solipsis 
{
    class Navigator;

    /** This class manages all Graphical User Interfaces of the Navigator.
    */
    class GUI_Debug : public GUI_Panel
    {
    public:
        static void switchPanel();
        static void refreshUrl();
        static void refreshDemoVoiceTalkButtonName();
        static void setTreeDirty(bool dirty);

       // Show the page
        virtual bool show();

    protected:
        GUI_Debug();

        // Debug callbacks
        void debugCommand(const Awesomium::JSArguments& args);
        void navCommand(const Awesomium::JSArguments& args);
        void onPageLoaded(const Awesomium::JSArguments& args);
        void onPageClosed(const Awesomium::JSArguments& args);
        void refreshTree(const Awesomium::JSArguments& args);


        static GUI_Debug * stGUI_Debug;

       bool mTreeDirty;
       NaviManager* mNaviMgr;
       Navigator * mNavigator;

   };

} // namespace Solipsis
#endif


#endif // #ifndef __GUI_Debug_h__