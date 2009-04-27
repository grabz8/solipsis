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

 

    class GUI_DeclarativeModeler : public GUI_Panel
    {
    public:
        static bool createAndShowPanel();
        static bool unload();

    protected:
        GUI_DeclarativeModeler();
        static GUI_DeclarativeModeler * stGUI_DeclarativeModeler; 
    };


    class GUI_Modeler : public GUI_Panel
    {
    public:
        enum ModelerError {
            ME_NOOBJECTSELECTED,    // No object3D selected
            ME_TEXTUREALREADYOPEN,  // Texture already open
            ME_FILENOTFOUND,
#ifdef DECLARATIVE_MODELER
            ME_DECLARATIVEMODELINGERROR, // decl. mod. error
#endif
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
        void onPageLoaded(const NaviData& naviData);

       // Modeler page callbacks
        void modelerMainFileImport(const NaviData& naviData);
        void modelerMainFileSave(const NaviData& naviData);
        void modelerMainFileExit(const NaviData& naviData);
        // Modeler object creation callbacks
        void modelerMainCreatePlane(const NaviData& naviData);
        void modelerMainCreateBox(const NaviData& naviData);
        void modelerMainCreateCorner(const NaviData& naviData);
        void modelerMainCreatePyramid(const NaviData& naviData);
        void modelerMainCreatePrism(const NaviData& naviData);
        void modelerMainCreateCylinder(const NaviData& naviData);
        void modelerMainCreateHalfCylinder(const NaviData& naviData);
        void modelerMainCreateCone(const NaviData& naviData);
        void modelerMainCreateHalfCone(const NaviData& naviData);
        void modelerMainCreateSphere(const NaviData& naviData);
        void modelerMainCreateHalfSphere(const NaviData& naviData);
        void modelerMainCreateTorus(const NaviData& naviData);
        void modelerMainCreateTube(const NaviData& naviData);
        void modelerMainCreateRing(const NaviData& naviData);
#ifdef DECLARATIVE_MODELER
        void modelerMainCreateSceneFromText(const NaviData& naviData);
#endif

        void modelerActionDelete(const NaviData& naviData);
        void modelerActionMove(const NaviData& naviData);
        void modelerActionRotate(const NaviData& naviData);
        void modelerActionScale(const NaviData& naviData);
        void modelerActionLink(const NaviData& naviData);
        void modelerActionProperties(const NaviData& naviData);
        void modelerActionUndo(const NaviData& naviData);
        void modelerActionSave(const NaviData& naviData);

    };
} // namespace Solipsis

#endif // #ifndef __GUI_Modeler_h__