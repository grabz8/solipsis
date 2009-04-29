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
        void modelerActionUndo(const NaviData& naviData);
        // Modeler properties page callbacks
        void modelerPropPageLoaded(const NaviData& naviData);
        void modelerPropPageClosed(const NaviData& naviData);
        // Modeler properties callbacks
        void modelerPropObjectName(const NaviData& naviData);
        void modelerPropCreator(const NaviData& naviData);
        void modelerPropOwner(const NaviData& naviData);
        void modelerPropGroup(const NaviData& naviData);
        void modelerPropDescription(const NaviData& naviData);
        void modelerPropTags(const NaviData& naviData);
        void modelerPropModification(const NaviData& naviData);
        void modelerPropCopy(const NaviData& naviData);
        // Modeler model callbacks
        void modelerPropTaperX(const NaviData& naviData);
        void modelerPropTaperY(const NaviData& naviData);
        void modelerPropTopShearX(const NaviData& naviData);
        void modelerPropTopShearY(const NaviData& naviData);
        void modelerPropTwistBegin(const NaviData& naviData);
        void modelerPropTwistEnd(const NaviData& naviData);
        void modelerPropDimpleBegin(const NaviData& naviData);
        void modelerPropDimpleEnd(const NaviData& naviData);
        void modelerPropPathCutBegin(const NaviData& naviData);
        void modelerPropPathCutEnd(const NaviData& naviData);
        void modelerPropHoleSizeX(const NaviData& naviData);
        void modelerPropHoleSizeY(const NaviData& naviData);
        void modelerPropHollowShape(const NaviData& naviData);
        void modelerPropSkew(const NaviData& naviData);
        void modelerPropRevolution(const NaviData& naviData);
        void modelerPropRadiusDelta(const NaviData& naviData);
        // Modeler material callbacks
        void modelerColorAmbient(const NaviData& naviData);
        void modelerColorDiffuse(const NaviData& naviData);
        void modelerColorSpecular(const NaviData& naviData);
        void modelerColorLockAmbientDiffuse(const NaviData& naviData);
        void modelerDoubleSide(const NaviData& naviData);
        void modelerPropShininess(const NaviData& naviData);
        void modelerPropTransparency(const NaviData& naviData);
        void modelerPropScrollU(const NaviData& naviData);
        void modelerPropScrollV(const NaviData& naviData);
        void modelerPropScaleU(const NaviData& naviData);
        void modelerPropScaleV(const NaviData& naviData);
        void modelerPropRotateU(const NaviData& naviData);
        void modelerPropTextureAdd(const NaviData& naviData);
        void modelerPropTextureRemove(const NaviData& naviData);
        void modelerPropTextureApply(const NaviData& naviData);
        void modelerPropTexturePrev(const NaviData& naviData);
        void modelerPropTextureNext(const NaviData& naviData);
        void modelerPropWWWTextureApply(const NaviData& naviData);
        void modelerPropSWFTextureApply(const NaviData& naviData);
        void modelerPropSWFMrlBrowse(const NaviData& naviData);
        void modelerPropVLCTextureApply(const NaviData& naviData);
        void modelerPropVLCMrlBrowse(const NaviData& naviData);
        void modelerPropVNCTextureApply(const NaviData& naviData);
        // Modeler 3D callbacks
        void modelerPropPositionX(const NaviData& naviData);
        void modelerPropPositionY(const NaviData& naviData);
        void modelerPropPositionZ(const NaviData& naviData);
        void modelerPropOrientationX(const NaviData& naviData);
        void modelerPropOrientationY(const NaviData& naviData);
        void modelerPropOrientationZ(const NaviData& naviData);
        void modelerPropScaleX(const NaviData& naviData);
        void modelerPropScaleY(const NaviData& naviData);
        void modelerPropScaleZ(const NaviData& naviData);
        void modelerPropCollision(const NaviData& naviData);
        void modelerPropGravity(const NaviData& naviData);

        //	void modelerProperties(const NaviData& naviData);
        // Modeler properties updates
        void modelerTabberLoad(unsigned pTab);
        void modelerTabberSave();
        void modelerTabberChange(const NaviData& naviData);

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