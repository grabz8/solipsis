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

#ifndef __GUI_ModelerProperties_h__
#define __GUI_ModelerProperties_h__

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

    class GUI_ModelerProperties : public GUI_Panel
    {
    public:
        static bool createAndShowPanel();
        static void hidePanel();
        static void unloadPanel();
        static bool isPanelVisible();

        virtual bool show();
        virtual void destroy();

    protected:
        GUI_ModelerProperties();

        // callback
        void modelerActionUndo(Navi* caller, const Awesomium::JSArguments& args);
        // Modeler properties page callbacks
        void modelerPropPageLoaded(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropPageClosed(Navi* caller, const Awesomium::JSArguments& args);
        // Modeler properties callbacks
        void modelerPropObjectName(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropCreator(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropOwner(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropGroup(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropDescription(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropTags(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropModification(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropCopy(Navi* caller, const Awesomium::JSArguments& args);
        // Modeler model callbacks
        void modelerPropTaperX(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropTaperY(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropTopShearX(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropTopShearY(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropTwistBegin(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropTwistEnd(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropDimpleBegin(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropDimpleEnd(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropPathCutBegin(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropPathCutEnd(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropHoleSizeX(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropHoleSizeY(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropHollowShape(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropSkew(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropRevolution(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropRadiusDelta(Navi* caller, const Awesomium::JSArguments& args);
        // Modeler material callbacks
        void modelerColorAmbient(Navi* caller, const Awesomium::JSArguments& args);
        void modelerColorDiffuse(Navi* caller, const Awesomium::JSArguments& args);
        void modelerColorSpecular(Navi* caller, const Awesomium::JSArguments& args);
        void modelerColorLockAmbientDiffuse(Navi* caller, const Awesomium::JSArguments& args);
        void modelerDoubleSide(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropShininess(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropTransparency(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropScrollU(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropScrollV(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropScaleU(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropScaleV(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropRotateU(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropTextureAdd(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropTextureRemove(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropTextureApply(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropTexturePrev(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropTextureNext(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropWWWTextureApply(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropSWFTextureApply(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropSWFMrlBrowse(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropVLCTextureApply(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropVLCMrlBrowse(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropVNCTextureApply(Navi* caller, const Awesomium::JSArguments& args);
        // Modeler 3D callbacks
        void modelerPropPositionX(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropPositionY(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropPositionZ(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropOrientationX(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropOrientationY(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropOrientationZ(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropScaleX(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropScaleY(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropScaleZ(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropCollision(Navi* caller, const Awesomium::JSArguments& args);
        void modelerPropGravity(Navi* caller, const Awesomium::JSArguments& args);

        //	void modelerProperties(Navi* caller, const Awesomium::JSArguments& args);
        // Modeler properties updates
        void modelerTabberLoad(unsigned pTab);
        void modelerTabberSave();
        void modelerTabberChange(Navi* caller, const Awesomium::JSArguments& args);

        static GUI_ModelerProperties * stGUI_ModelerProperties; 
        Navigator * mNavigator;

        // Update the command -> backup if the command is different from the last used
        void modelerUpdateCommand(Object3D::Command pCommand, Object3D* pObject);
        // Reset all deformation's sliders to the initial position
        void modelerUpdateDeformationSliders();
        // Add a button representing a deformation to the list of deformations in the properties panel
        void modelerAddNewDeformation(Object3D::Command pCommand);
        // Update the list of loaded textures
        void modelerUpdateTextures();

        // Start mode link when the user click on the Menu PopUp on 'link'
        bool mModeLink;
        bool mLockAmbientDiffuse;
        // ...
        std::vector<std::string>	mDeformButton;
   };

} // namespace Solipsis

#endif // #ifndef __GUI_ModelerProperties_h__