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
        void avatarPropPageLoaded(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropPageClosed(Navi* caller, const Awesomium::JSArguments& args);
        // Avatar animation page callbaks
        void avatarPropAnimPlayPause(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropAnimStop(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropAnimNext(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropAnimPrev(Navi* caller, const Awesomium::JSArguments& args);
        // Avatar bodyParts page callbacks
        void avatarPropHeight(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropBonePrev(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropBoneNext(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropBPPrev(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropBPNext(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropBPMPrev(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropBPMNext(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropBPMEdit(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropBPMRemove(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropAttPrev(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropAttNext(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropAttMPrev(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropAttMNext(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropAttMEdit(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropAttMRemove(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropSliders(Navi* caller, const Awesomium::JSArguments& args);		// multi callback !
        void avatarPropReset(Navi* caller, const Awesomium::JSArguments& args);
        // Avatar material page callbacks
        void avatarColorAmbient(Navi* caller, const Awesomium::JSArguments& args);
        void avatarColorDiffuse(Navi* caller, const Awesomium::JSArguments& args);
        void avatarColorSpecular(Navi* caller, const Awesomium::JSArguments& args);
        void avatarColorLockAmbientDiffuse(Navi* caller, const Awesomium::JSArguments& args);
        void avatarDoubleSide(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropShininess(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropTransparency(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropScrollU(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropScrollV(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropScaleU(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropScaleV(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropRotateU(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropTextureAdd(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropTextureRemove(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropTexturePrev(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropTextureNext(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropResetColour(Navi* caller, const Awesomium::JSArguments& args);
        void avatarPropSound(Navi* caller, const Awesomium::JSArguments& args);

        void avatarTabberLoad(unsigned pTab);
        // Avatar properties updates
        void avatarTabberSave();
        void avatarTabberChange(Navi* caller, const Awesomium::JSArguments& args);

        // Update the list of loaded textures
        void avatarUpdateTextures(ModifiableMaterialObject* pObject);
        // Update the sliders modifiers
        void avatarUpdateSliders(Vector3 pos, Vector3 ori, Vector3 scale);

        Navigator * mNavigator;
        bool mLockAmbientDiffuse;
   };

} // namespace Solipsis

#endif // #ifndef __GUI_Avatar_h__