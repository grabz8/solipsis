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

#ifndef __GUI_Modeler_h__
#define __GUI_Modeler_h__

#include "NavigatorGui.h"
#include "MainApplication/Navigator.h"
#include "GUI_Panel.h"

using namespace NaviLibrary;

namespace Solipsis 
{
    class Navigator;



    class GUI_Modeler : public GUI_Panel
    {
    public:
        enum ModelerError {
            ME_NOOBJECTSELECTED,    // No object3D selected
            ME_TEXTUREALREADYOPEN,  // Texture already open
            ME_FILENOTFOUND,
            ME_DECLARATIVEMODELINGERROR, // decl. mod. error
            ME_TERRAINMODELINGERROR, // terrain mod. error
        };
        static const std::string ms_ModelerErrors[];

        static bool createAndShowPanel();
        static void unload();
        static bool isPanelVisible();

        virtual bool show();
        virtual void destroy();

    protected:
        GUI_Modeler();
        static GUI_Modeler * stGUI_Modeler;
        Navigator * mNavigator;

        // Modeler fake right click callbacks
        void onPageLoaded(Navi* caller, const Awesomium::JSArguments& args);

       // Modeler page callbacks
        void modelerMainFileImport(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainFileSave(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainFileExit(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainFileSaveAs(Navi* caller, const Awesomium::JSArguments& args);

        // Scene From Text Modeler Panel
        void modelerMainCreateTerrain(Navi* caller, const Awesomium::JSArguments& args);

        // Modeler object creation callbacks
        void modelerMainCreatePlane(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreateBox(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreateCorner(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreatePyramid(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreatePrism(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreateCylinder(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreateHalfCylinder(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreateCone(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreateHalfCone(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreateSphere(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreateHalfSphere(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreateTorus(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreateTube(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreateRing(Navi* caller, const Awesomium::JSArguments& args);
        void modelerMainCreateSceneFromText(Navi* caller, const Awesomium::JSArguments& args);
        void modelerActionDelete(Navi* caller, const Awesomium::JSArguments& args);
        void modelerActionMove(Navi* caller, const Awesomium::JSArguments& args);
        void modelerActionRotate(Navi* caller, const Awesomium::JSArguments& args);
        void modelerActionScale(Navi* caller, const Awesomium::JSArguments& args);
        void modelerActionLink(Navi* caller, const Awesomium::JSArguments& args);
        void modelerActionProperties(Navi* caller, const Awesomium::JSArguments& args);
        void modelerActionUndo(Navi* caller, const Awesomium::JSArguments& args);
        void modelerActionSave(Navi* caller, const Awesomium::JSArguments& args);

    };
} // namespace Solipsis

#endif // #ifndef __GUI_Modeler_h__