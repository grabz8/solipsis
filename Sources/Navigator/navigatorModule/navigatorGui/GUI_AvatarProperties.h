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
        void avatarPropPageLoaded(const Awesomium::JSArguments& args);
        void avatarPropPageClosed(const Awesomium::JSArguments& args);
        // Avatar animation page callbaks
        void avatarPropAnimPlayPause(const Awesomium::JSArguments& args);
        void avatarPropAnimStop(const Awesomium::JSArguments& args);
        void avatarPropAnimNext(const Awesomium::JSArguments& args);
        void avatarPropAnimPrev(const Awesomium::JSArguments& args);
        // Avatar bodyParts page callbacks
        void avatarPropHeight(const Awesomium::JSArguments& args);
        void avatarPropBonePrev(const Awesomium::JSArguments& args);
        void avatarPropBoneNext(const Awesomium::JSArguments& args);
        void avatarPropBPPrev(const Awesomium::JSArguments& args);
        void avatarPropBPNext(const Awesomium::JSArguments& args);
        void avatarPropBPMPrev(const Awesomium::JSArguments& args);
        void avatarPropBPMNext(const Awesomium::JSArguments& args);
        void avatarPropBPMEdit(const Awesomium::JSArguments& args);
        void avatarPropBPMRemove(const Awesomium::JSArguments& args);
        void avatarPropAttPrev(const Awesomium::JSArguments& args);
        void avatarPropAttNext(const Awesomium::JSArguments& args);
        void avatarPropAttMPrev(const Awesomium::JSArguments& args);
        void avatarPropAttMNext(const Awesomium::JSArguments& args);
        void avatarPropAttMEdit(const Awesomium::JSArguments& args);
        void avatarPropAttMRemove(const Awesomium::JSArguments& args);
        void avatarPropSliders(const Awesomium::JSArguments& args);		// multi callback !
        void avatarPropReset(const Awesomium::JSArguments& args);
        // Avatar material page callbacks
        void avatarColorAmbient(const Awesomium::JSArguments& args);
        void avatarColorDiffuse(const Awesomium::JSArguments& args);
        void avatarColorSpecular(const Awesomium::JSArguments& args);
        void avatarColorLockAmbientDiffuse(const Awesomium::JSArguments& args);
        void avatarDoubleSide(const Awesomium::JSArguments& args);
        void avatarPropShininess(const Awesomium::JSArguments& args);
        void avatarPropTransparency(const Awesomium::JSArguments& args);
        void avatarPropScrollU(const Awesomium::JSArguments& args);
        void avatarPropScrollV(const Awesomium::JSArguments& args);
        void avatarPropScaleU(const Awesomium::JSArguments& args);
        void avatarPropScaleV(const Awesomium::JSArguments& args);
        void avatarPropRotateU(const Awesomium::JSArguments& args);
        void avatarPropTextureAdd(const Awesomium::JSArguments& args);
        void avatarPropTextureRemove(const Awesomium::JSArguments& args);
        void avatarPropTexturePrev(const Awesomium::JSArguments& args);
        void avatarPropTextureNext(const Awesomium::JSArguments& args);
        void avatarPropResetColour(const Awesomium::JSArguments& args);
        void avatarPropSound(const Awesomium::JSArguments& args);

        void avatarTabberLoad(unsigned pTab);
        // Avatar properties updates
        void avatarTabberSave();
        void avatarTabberChange(const Awesomium::JSArguments& args);

        // Update the list of loaded textures
        void avatarUpdateTextures(ModifiableMaterialObject* pObject);
        // Update the sliders modifiers
        void avatarUpdateSliders(Vector3 pos, Vector3 ori, Vector3 scale);

        Navigator * mNavigator;
        bool mLockAmbientDiffuse;
   };

} // namespace Solipsis

#endif // #ifndef __GUI_Avatar_h__