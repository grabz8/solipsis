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

#ifndef __GUI_AvatarProperties_h__
#define __GUI_AvatarProperties_h__

#include "NavigatorGui.h"
#include "MainApplication/Navigator.h"
#include "GUI_Panel.h"

using namespace NaviLibrary;

namespace Solipsis 
{
    class Navigator;

    class GUI_AvatarProperties : public GUI_Panel
    {
    public:
        static bool createAndShowPanel();
        static void unload();
        static void hidePanel();
        static bool isPanelVisible();

        virtual bool show();

    protected:
        GUI_AvatarProperties();
        static GUI_AvatarProperties * stGUI_AvatarProperties;


        // Avatar properties page callbacks
        void avatarPropPageLoaded(const NaviData& naviData);
        void avatarPropPageClosed(const NaviData& naviData);
        // Avatar animation page callbaks
        void avatarPropAnimPlayPause(const NaviData& naviData);
        void avatarPropAnimStop(const NaviData& naviData);
        void avatarPropAnimNext(const NaviData& naviData);
        void avatarPropAnimPrev(const NaviData& naviData);
        // Avatar bodyParts page callbacks
        void avatarPropHeight(const NaviData& naviData);
        void avatarPropBonePrev(const NaviData& naviData);
        void avatarPropBoneNext(const NaviData& naviData);
        void avatarPropBPPrev(const NaviData& naviData);
        void avatarPropBPNext(const NaviData& naviData);
        void avatarPropBPMPrev(const NaviData& naviData);
        void avatarPropBPMNext(const NaviData& naviData);
        void avatarPropBPMEdit(const NaviData& naviData);
        void avatarPropBPMRemove(const NaviData& naviData);
        void avatarPropAttPrev(const NaviData& naviData);
        void avatarPropAttNext(const NaviData& naviData);
        void avatarPropAttMPrev(const NaviData& naviData);
        void avatarPropAttMNext(const NaviData& naviData);
        void avatarPropAttMEdit(const NaviData& naviData);
        void avatarPropAttMRemove(const NaviData& naviData);
        void avatarPropSliders(const NaviData& naviData);		// multi callback !
        void avatarPropReset(const NaviData& naviData);
        // Avatar material page callbacks
        void avatarColorAmbient(const NaviData& naviData);
        void avatarColorDiffuse(const NaviData& naviData);
        void avatarColorSpecular(const NaviData& naviData);
        void avatarColorLockAmbientDiffuse(const NaviData& naviData);
        void avatarDoubleSide(const NaviData& naviData);
        void avatarPropShininess(const NaviData& naviData);
        void avatarPropTransparency(const NaviData& naviData);
        void avatarPropScrollU(const NaviData& naviData);
        void avatarPropScrollV(const NaviData& naviData);
        void avatarPropScaleU(const NaviData& naviData);
        void avatarPropScaleV(const NaviData& naviData);
        void avatarPropRotateU(const NaviData& naviData);
        void avatarPropTextureAdd(const NaviData& naviData);
        void avatarPropTextureRemove(const NaviData& naviData);
        void avatarPropTexturePrev(const NaviData& naviData);
        void avatarPropTextureNext(const NaviData& naviData);
        void avatarPropResetColour(const NaviData& naviData);
        void avatarPropSound(const NaviData& naviData);

        void avatarTabberLoad(unsigned pTab);
        // Avatar properties updates
        void avatarTabberSave();
        void avatarTabberChange(const NaviData& naviData);

        // Update the list of loaded textures
        void avatarUpdateTextures(ModifiableMaterialObject* pObject);
        // Update the sliders modifiers
        void avatarUpdateSliders(Vector3 pos, Vector3 ori, Vector3 scale);

        Navigator * mNavigator;
        bool mLockAmbientDiffuse;
   };

} // namespace Solipsis

#endif // #ifndef __GUI_Avatar_h__