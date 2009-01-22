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

#ifndef __NavigatorGUI_h__
#define __NavigatorGUI_h__

#include <Facebook.h>
#include "NaviManager.h"
#include "Modeler.h"
#include "ModifiableMaterialObject.h"

using namespace NaviLibrary;

namespace Solipsis {

class Navigator;
class AvatarEditor;

/** This class manages all Graphical User Interfaces of the Navigator.
 */
class NavigatorGUI : public WindowEventListener
{
public:
    enum NaviState {
        NSNotCreated,   // Navi page not created
        NSCreated       // Navi page created and not visible
    };

    enum MsgBoxButtons {
        MBB_OK = 1,
        MBB_CANCEL = MBB_OK >> 1,
        MBB_YESNO = MBB_CANCEL >> 1
    };
    enum MsgBoxIcon {
        MBB_INFO = 0,
        MBB_QUESTION,
        MBB_ERROR,
        MBB_EXCLAMATION
    };
    enum MsgBoxDisplayed {
        MBD_NONE = 0,
        MBD_WORLDSSERVERERROR,
        MBD_WORLDSSERVERCOMPATIBILITYERROR,
        MBD_AUTHENTFBERROR,
        MBD_AUTHENTWSERROR
    };

    enum NaviPanel {
        NAVI_MSGBOX = 0,
        NAVI_LOGIN,
        NAVI_WORLDS,
        NAVI_INFOWS,
        NAVI_OPTIONS,
        NAVI_AUTHENTFB,
        NAVI_AUTHENTWS,
        NAVI_MAINMENU,
        NAVI_STATUSBAR,
        NAVI_CHAT,
        NAVI_ABOUT,
        NAVI_COMMANDS,
        NAVI_CTXTAVATAR,
        NAVI_CTXTWWW,
        NAVI_CTXTSWF,
        NAVI_CTXTVLC,
        NAVI_CTXTVNC,
        NAVI_MODELERMAIN,
        NAVI_MODELERPROP,
        NAVI_AVATARMAIN,
        NAVI_AVATARPROP,
#ifdef UIDEBUG
        NAVI_DEBUG,
#endif
        NAVI_COUNT
    };

    enum ModelerError {
        ME_NOOBJECTSELECTED,    // No object3D selected
        ME_TEXTUREALREADYOPEN,  // Texture already open
        ME_FILENOTFOUND
    };
    static const std::string ms_ModelerErrors[];

protected:
    Navigator* mNavigator;
    NaviManager* mNaviMgr;
    static const std::string ms_NavisNames[NAVI_COUNT];
    NaviState mNavisStates[NAVI_COUNT];
    int mCurrentNavi;
    int mCurrentCtxtPanel;
    unsigned long mCurrentNaviCreationDate;
    unsigned long mStatusBarDisplayDate;
    std::string mLoginInfosText;
    MsgBoxDisplayed mMsgBoxDisplayed;
    std::string mMsgBoxTitleText;
    std::string mMsgBoxMsgText;
    MsgBoxButtons mMsgBoxButtons;
    MsgBoxIcon mMsgBoxIcon;
    Facebook *mFacebook;
#ifdef UIDEBUG
    bool mTreeDirty;
#endif

public:
    NavigatorGUI(Navigator* navigator);
    ~NavigatorGUI();

    // Start up GUI
    bool startup();
    // Update
    void update();

    /** See Ogre::WindowEventListener. */
    virtual void windowResized(RenderWindow* rw);

    // Mouse
    void SetMouseVisibility(bool visible);
    bool isMouseVisible();

    // Show a message box
    void showMessageBox(const std::string& titleText, const std::string& msgText, MsgBoxButtons buttons, MsgBoxIcon icon);
    void hideMessageBox();
    bool isMessageBoxVisible();

    // Interfaces
    void login();
    void inWorld();

    // Apply informations of login panel
    void applyLoginDatas();

    // Set text in status bar + display it for a while
    void setStatusBarText(const std::string& statusText);

    // Main menu panel
    void mainmenuSwitch();

    // Chat panel
    void addChatText(const std::wstring& message);

    // Contextual panel
    void contextShow(int x, int y, NaviPanel ctxtPanel, const String& params);
    bool isContextVisible();
    bool isContextFocused();
    void contextHide();
    void contextDestroy();

    // Main modeler panel
    void modelerMainShow();
    bool isModelerMainVisible();
    void modelerMainHide();
    void modelerMainUnload();

    // Properties modeler panel
    void modelerPropShow();
    bool isModelerPropVisible();
    void modelerPropHide();
    void modelerPropUnload();

    // Avatar panel
    void avatarMainShow();
    bool isAvatarMainVisible();
    void avatarMainHide();
    void avatarMainUnload();
    // Properties avatar panel
    void avatarPropShow();
    bool isAvatarPropVisible();
    void avatarPropHide();
    void avatarPropUnload();

#ifdef UIDEBUG
    void switchDebug();
    void setTreeDirty(bool dirty) { mTreeDirty = dirty; }
    void debugRefreshUrl();
    void debugRefreshDemoVoiceTalkButtonName();
#endif

protected:
    // Handlers
    void messageBoxPageLoaded(const NaviData& naviData);
    void messageBoxResponse(const NaviData& naviData);

    // Login/Options/InWorld callbacks
    void loginPageLoaded(const NaviData& naviData);
    void world(const NaviData& naviData);
    void connect(const NaviData& naviData);
    void options(const NaviData& naviData);
    void quit(const NaviData& naviData);

    class WorldsServerEventListener : public NaviEventListener
    {
    private:
        NavigatorGUI *mNavigatorGUI;
	public:
        WorldsServerEventListener(NavigatorGUI *navigatorGUI) : mNavigatorGUI(navigatorGUI) {}
        virtual void onNaviDataEvent(Navi *caller, const NaviData &naviData) {}
		virtual void onLinkClicked(Navi *caller, const std::string &linkHref) {}
        virtual void onLocationChange(Navi *caller, const std::string &url) {}
		virtual void onNavigateComplete(Navi *caller, const std::string &url, int responseCode);
    };
    WorldsServerEventListener mWorldsServerEventListener;
    void worldsServerCompatibilityError();
    void worldsServerError();
    void worldOk(const NaviData& naviData);
    void worldCancel(const NaviData& naviData);
    void worldsServerInfo();
    void worldsServerInfoOk(const NaviData& naviData);

    void optionsPageLoaded(const NaviData& naviData);
    void optionsOk(const NaviData& naviData);
    void optionsBack(const NaviData& naviData);

    void authentFacebook();
    void authentFacebookError();
    void authentFacebookPageLoaded(const NaviData& naviData);
    void authentFacebookOk(const NaviData& naviData);
    void authentFacebookCancel(const NaviData& naviData);

    void authentWorldsServer(const std::string& pwd);
    void authentWorldsServerError();
    void authentWorldsServerOk(const NaviData& naviData);

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
    // Modeler fake right click callbacks
    void modelerActionDelete(const NaviData& naviData);
    void modelerActionMove(const NaviData& naviData);
    void modelerActionRotate(const NaviData& naviData);
    void modelerActionScale(const NaviData& naviData);
    void modelerActionLink(const NaviData& naviData);
    void modelerActionProperties(const NaviData& naviData);
    void modelerActionUndo(const NaviData& naviData);
    void modelerActionSave(const NaviData& naviData);

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
    void modelerTabberChange(const NaviData& naviData);

    // Avatar page callbacks
	void avatarTabberChange(const NaviData& naviData);
    void avatarMainPageLoaded(const NaviData& naviData);
    void avatarMainFileOpen(const NaviData& naviData);
    void avatarMainFileEdit(const NaviData& naviData);
    void avatarMainFileSave(const NaviData& naviData);
    void avatarMainFileSaveAs(const NaviData& naviData);
    void avatarMainFileExit(const NaviData& naviData);
//    void avatarMainSelectPrev(const NaviData& naviData);
//	void avatarMainSelectNext(const NaviData& naviData);
	void avatarMainSelected(const NaviData& naviData);
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

public:
    // Modeler properties updates
    void modelerTabberLoad(unsigned pTab);
    void modelerTabberSave();
	// Avatar properties updates
	void avatarTabberLoad(unsigned pTab);
    void avatarTabberSave();

protected:
    // Update the command -> backup if the command is different from the last used
    void modelerUpdateCommand(Object3D::Command pCommand, Object3D* pObject);
    // Reset all deformation's sliders to the initial position
    void modelerUpdateDeformationSliders();
    // Add a button representing a deformation to the list of deformations in the properties panel
    void modelerAddNewDeformation(Object3D::Command pCommand);
    // Update the list of loaded textures
    void modelerUpdateTextures();
	// Update the list of loaded textures
	void avatarUpdateTextures(ModifiableMaterialObject* pObject);
	// Update the sliders modifiers
	void avatarUpdateSliders(Vector3 pos, Vector3 ori, Vector3 scale);

    // Start mode link when the user click on the Menu PopUp on 'link'
    bool mModeLink;
    bool mLockAmbientDiffuse;
    // ...
    std::vector<std::string>	mDeformButton;

#ifdef UIDEBUG
    // Debug callbacks
    void debugCommand(const NaviData& naviData);
    void navCommand(const NaviData& naviData);
    void debugPageLoaded(const NaviData& naviData);
    void debugPageClosed(const NaviData& naviData);
    void debugRefreshTree(const NaviData& naviData);
#endif

    // Helpers
    NaviPanel getNaviPanel(const std::string& naviName);
    void naviToShowPageLoaded(const NaviData& naviData);
    void hidePreviousNavi();
    void destroyNavi(NaviPanel naviPanel);

public:
    const std::string& getNaviName(NaviPanel naviPanel);
    bool setNaviVisibility(const std::string& naviName, bool show);
    void switchLuaNavi(NaviPanel naviPanel, bool createDestroy = false);
};

} // namespace Solipsis

#endif // #ifndef __NavigatorGUI_h__