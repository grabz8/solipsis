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
#include "World/Modeler.h"
#include "ModifiableMaterialObject.h"

using namespace NaviLibrary;

namespace Solipsis {

class Navigator;
class AvatarEditor;
class GUI_Panel;

/** This class manages all Graphical User Interfaces of the Navigator.
 */
class NavigatorGUI : public WindowEventListener
{
public:
    enum NaviState {
        NSNotCreated,   // Navi page not created
        NSCreated       // Navi page created and not visible
    };
    
    enum NaviPanel {
        NAVI_CHAT,
        NAVI_ABOUT,
        NAVI_COMMANDS,

        NAVI_MODELERMAIN,
        NAVI_MODELERPROP,

#ifdef DECLARATIVE_MODELER
		NAVI_MODELERSCENEFROMTEXT,
#endif

        NAVI_AVATARMAIN,
        NAVI_AVATARPROP,

#ifdef UIDEBUG
        NAVI_DEBUG,
#endif

        NAVI_COUNT
    };

 

protected:
    Navigator* mNavigator;
    NaviManager* mNaviMgr;
    static const std::string ms_NavisNames[NAVI_COUNT];

    NaviState mNavisStates[NAVI_COUNT];
    int mCurrentNavi;
    int mCurrentCtxtPanel;
    unsigned long mCurrentNaviCreationDate;
    std::string mLoginInfosText;

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

    // Interfaces
//     void login();
    void inWorld();

    // Apply informations of login panel
//     void applyLoginDatas();

    // Main menu panel
    void mainmenuSwitch();

    // Chat panel
    void addChatText(const std::wstring& message);

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

#ifdef DECLARATIVE_MODELER
	// Scene From Text Modeler Panel
	void modelerSceneFromTextShow();
    bool isModelerSceneFromTextVisible();
    void modelerSceneFromTextHide();
    void modelerSceneFromTextUnload();
#endif
#ifdef UIDEBUG
    void switchDebug();
    void setTreeDirty(bool dirty) { mTreeDirty = dirty; }
    void debugRefreshUrl();
    void debugRefreshDemoVoiceTalkButtonName();
#endif

    void connectionLostError();
    void connectionServerError();

protected:
    // Login/Options/InWorld callbacks
//     void loginPageLoaded(const NaviData& naviData);
//     void world(const NaviData& naviData);
//     void connect(const NaviData& naviData);
//     void options(const NaviData& naviData);
//     void quit(const NaviData& naviData);

    
//     void worldOk(const NaviData& naviData);
//     void worldCancel(const NaviData& naviData);
//     void worldsServerInfo();
//     void worldsServerInfoOk(const NaviData& naviData);
// 
//     void optionsPageLoaded(const NaviData& naviData);
//     void optionsOk(const NaviData& naviData);
//     void optionsBack(const NaviData& naviData);
// 
//     void authentFacebook();
//     void authentFacebookError();
//     void authentFacebookPageLoaded(const NaviData& naviData);
//     void authentFacebookOk(const NaviData& naviData);
//     void authentFacebookCancel(const NaviData& naviData);
// 
//     void authentWorldsServer(const std::string& pwd);
//     void authentWorldsServerError();
//     void authentWorldsServerOk(const NaviData& naviData);


 


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

#ifdef DECLARATIVE_MODELER
	// Modeler Scene From Text Setup properties page callbacks
    void modelerSceneFromTextPageLoaded(const NaviData& naviData);
	void modelerSceneFromTextExec(const NaviData& naviData);	
	void modelerSceneFromTextCancelled(const NaviData& naviData);
#endif

public:

	// Avatar properties updates
	void avatarTabberLoad(unsigned pTab);
    void avatarTabberSave();

protected:
    // Update the list of loaded textures
    void avatarUpdateTextures(ModifiableMaterialObject* pObject);
    // Update the sliders modifiers
    void avatarUpdateSliders(Vector3 pos, Vector3 ori, Vector3 scale);


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
    bool setNaviVisibility(const std::string& naviName, bool show);

    ///////////////////// à supprimer à terme
    // à supprimer ? oui à terme
    void switchLuaNavi(NaviPanel naviPanel, bool createDestroy = false);
    const std::string& getNaviName(NaviPanel naviPanel);

   
//     static void destroyNavi(NaviLibrary::Navi *pNavi)
//     {
//         mNaviGui->mNaviMgr->destroyNavi(pNavi);
//     }

    static NaviLibrary::Navi * getNavi(const std::string& naviName)
    {   
        return mNaviGui->mNaviMgr->getNavi(naviName);
    }


    static NaviManager* getNaviMgr()
    {   
        return mNaviGui->mNaviMgr;
    }
    
    ///////////////////////////////////////////////

    static NavigatorGUI * mNaviGui;

////////////////////////// nouvelle implémentation ////////////////////
    // a a panel to the list
    static void registerGuiPanel(GUI_Panel *pPanel);
    static void unregisterGuiPanel(GUI_Panel *pPanel);
    static void destroyAllRegisteredPanels();

    static void setCurrentPanel(GUI_Panel * pCurrentPanel)
    {
        mNaviGui->m_pCurrentPanel = pCurrentPanel;
        if (pCurrentPanel == NULL)
        {
            mNaviGui->mCurrentNaviCreationDate = 0;
        }
    }

    static void destroyCurrentPanel();

    static GUI_Panel * getCurrentPanel()
    {
        return mNaviGui->m_pCurrentPanel;
    }

    std::map<std::string, GUI_Panel *> m_panels;
    GUI_Panel * m_pCurrentPanel;
};

} // namespace Solipsis

#endif // #ifndef __NavigatorGUI_h__