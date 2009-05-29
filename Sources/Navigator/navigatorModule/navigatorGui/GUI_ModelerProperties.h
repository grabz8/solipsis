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
        void modelerActionUndo(const Awesomium::JSArguments& args);
        // Modeler properties page callbacks
        void modelerPropPageLoaded(const Awesomium::JSArguments& args);
        void modelerPropPageClosed(const Awesomium::JSArguments& args);
        // Modeler properties callbacks
        void modelerPropObjectName(const Awesomium::JSArguments& args);
        void modelerPropCreator(const Awesomium::JSArguments& args);
        void modelerPropOwner(const Awesomium::JSArguments& args);
        void modelerPropGroup(const Awesomium::JSArguments& args);
        void modelerPropDescription(const Awesomium::JSArguments& args);
        void modelerPropTags(const Awesomium::JSArguments& args);
        void modelerPropModification(const Awesomium::JSArguments& args);
        void modelerPropCopy(const Awesomium::JSArguments& args);
        // Modeler model callbacks
        void modelerPropTaperX(const Awesomium::JSArguments& args);
        void modelerPropTaperY(const Awesomium::JSArguments& args);
        void modelerPropTopShearX(const Awesomium::JSArguments& args);
        void modelerPropTopShearY(const Awesomium::JSArguments& args);
        void modelerPropTwistBegin(const Awesomium::JSArguments& args);
        void modelerPropTwistEnd(const Awesomium::JSArguments& args);
        void modelerPropDimpleBegin(const Awesomium::JSArguments& args);
        void modelerPropDimpleEnd(const Awesomium::JSArguments& args);
        void modelerPropPathCutBegin(const Awesomium::JSArguments& args);
        void modelerPropPathCutEnd(const Awesomium::JSArguments& args);
        void modelerPropHoleSizeX(const Awesomium::JSArguments& args);
        void modelerPropHoleSizeY(const Awesomium::JSArguments& args);
        void modelerPropHollowShape(const Awesomium::JSArguments& args);
        void modelerPropSkew(const Awesomium::JSArguments& args);
        void modelerPropRevolution(const Awesomium::JSArguments& args);
        void modelerPropRadiusDelta(const Awesomium::JSArguments& args);
        // Modeler material callbacks
        void modelerColorAmbient(const Awesomium::JSArguments& args);
        void modelerColorDiffuse(const Awesomium::JSArguments& args);
        void modelerColorSpecular(const Awesomium::JSArguments& args);
        void modelerColorLockAmbientDiffuse(const Awesomium::JSArguments& args);
        void modelerDoubleSide(const Awesomium::JSArguments& args);
        void modelerPropShininess(const Awesomium::JSArguments& args);
        void modelerPropTransparency(const Awesomium::JSArguments& args);
        void modelerPropScrollU(const Awesomium::JSArguments& args);
        void modelerPropScrollV(const Awesomium::JSArguments& args);
        void modelerPropScaleU(const Awesomium::JSArguments& args);
        void modelerPropScaleV(const Awesomium::JSArguments& args);
        void modelerPropRotateU(const Awesomium::JSArguments& args);
        void modelerPropTextureAdd(const Awesomium::JSArguments& args);
        void modelerPropTextureRemove(const Awesomium::JSArguments& args);
        void modelerPropTextureApply(const Awesomium::JSArguments& args);
        void modelerPropTexturePrev(const Awesomium::JSArguments& args);
        void modelerPropTextureNext(const Awesomium::JSArguments& args);
        void modelerPropWWWTextureApply(const Awesomium::JSArguments& args);
        void modelerPropSWFTextureApply(const Awesomium::JSArguments& args);
        void modelerPropSWFMrlBrowse(const Awesomium::JSArguments& args);
        void modelerPropVLCTextureApply(const Awesomium::JSArguments& args);
        void modelerPropVLCMrlBrowse(const Awesomium::JSArguments& args);
        void modelerPropVNCTextureApply(const Awesomium::JSArguments& args);
        // Modeler 3D callbacks
        void modelerPropPositionX(const Awesomium::JSArguments& args);
        void modelerPropPositionY(const Awesomium::JSArguments& args);
        void modelerPropPositionZ(const Awesomium::JSArguments& args);
        void modelerPropOrientationX(const Awesomium::JSArguments& args);
        void modelerPropOrientationY(const Awesomium::JSArguments& args);
        void modelerPropOrientationZ(const Awesomium::JSArguments& args);
        void modelerPropScaleX(const Awesomium::JSArguments& args);
        void modelerPropScaleY(const Awesomium::JSArguments& args);
        void modelerPropScaleZ(const Awesomium::JSArguments& args);
        void modelerPropCollision(const Awesomium::JSArguments& args);
        void modelerPropGravity(const Awesomium::JSArguments& args);

        //	void modelerProperties(const Awesomium::JSArguments& args);
        // Modeler properties updates
        void modelerTabberLoad(unsigned pTab);
        void modelerTabberSave();
        void modelerTabberChange(const Awesomium::JSArguments& args);

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