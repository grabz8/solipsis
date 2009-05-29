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
        void onPageLoaded(const Awesomium::JSArguments& args);

       // Modeler page callbacks
        void modelerMainFileImport(const Awesomium::JSArguments& args);
        void modelerMainFileSave(const Awesomium::JSArguments& args);
        void modelerMainFileExit(const Awesomium::JSArguments& args);
        void modelerMainFileSaveAs(const Awesomium::JSArguments& args);

        // Scene From Text Modeler Panel
        void modelerMainCreateTerrain(const Awesomium::JSArguments& args);

        // Modeler object creation callbacks
        void modelerMainCreatePlane(const Awesomium::JSArguments& args);
        void modelerMainCreateBox(const Awesomium::JSArguments& args);
        void modelerMainCreateCorner(const Awesomium::JSArguments& args);
        void modelerMainCreatePyramid(const Awesomium::JSArguments& args);
        void modelerMainCreatePrism(const Awesomium::JSArguments& args);
        void modelerMainCreateCylinder(const Awesomium::JSArguments& args);
        void modelerMainCreateHalfCylinder(const Awesomium::JSArguments& args);
        void modelerMainCreateCone(const Awesomium::JSArguments& args);
        void modelerMainCreateHalfCone(const Awesomium::JSArguments& args);
        void modelerMainCreateSphere(const Awesomium::JSArguments& args);
        void modelerMainCreateHalfSphere(const Awesomium::JSArguments& args);
        void modelerMainCreateTorus(const Awesomium::JSArguments& args);
        void modelerMainCreateTube(const Awesomium::JSArguments& args);
        void modelerMainCreateRing(const Awesomium::JSArguments& args);
        void modelerMainCreateSceneFromText(const Awesomium::JSArguments& args);
        void modelerActionDelete(const Awesomium::JSArguments& args);
        void modelerActionMove(const Awesomium::JSArguments& args);
        void modelerActionRotate(const Awesomium::JSArguments& args);
        void modelerActionScale(const Awesomium::JSArguments& args);
        void modelerActionLink(const Awesomium::JSArguments& args);
        void modelerActionProperties(const Awesomium::JSArguments& args);
        void modelerActionUndo(const Awesomium::JSArguments& args);
        void modelerActionSave(const Awesomium::JSArguments& args);

    };
} // namespace Solipsis

#endif // #ifndef __GUI_Modeler_h__