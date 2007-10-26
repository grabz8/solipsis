#ifndef __NavigatorGUI_h__
#define __NavigatorGUI_h__

#include "NaviManager.h"

using namespace NaviLibrary;

class Navigator;

class NavigatorGUI
{
protected:
    enum NaviState {
        NSNotCreated,   // Navi page not created
        NSLoaded,       // Navi page created, loaded and not visible
        NSVisible       // Navi page visible
    };

    enum NaviPanel {
        NAVI_LOGIN = 0,
        NAVI_OPTIONS,
        NAVI_CHAT,
        NAVI_MODELERMAIN,
#ifdef UIDEBUG
        NAVI_DEBUG,
#endif
        NAVI_COUNT
    };

protected:
    Navigator* mNavigator;
    NaviManager& mNaviMgr;
    static const std::string mNavisNames[NAVI_COUNT];
    NaviState mNavisStates[NAVI_COUNT];
    int mCurrentNavi;

public:
    NavigatorGUI(Navigator* navigator);
    ~NavigatorGUI();

    // Start up GUI
    void startup();

    // Mouse
    void SetMouseVisibility(bool visible);
    bool isMouseVisible();

    // Interfaces
    void login();
    void inWorld();

#ifdef UIDEBUG
    void switchDebug();
#endif

    void modelerMainShow();
    bool isModelerMainVisible();
    void modelerMainHide();
    void modelerMainUnload();

protected:
    // Handlers
    // Login/Options/InWorld callbacks
    void loginPageRefresh(const NaviData& naviData);
    void connect(const NaviData& naviData);
    void options(const NaviData& naviData);
    void optionsPageRefresh(const NaviData& naviData);
    void quit(const NaviData& naviData);
    void optionsOk(const NaviData& naviData);
    void optionsBack(const NaviData& naviData);
    void chatPageRefresh(const NaviData& naviData);
    void sendMessage(const NaviData& naviData);

#ifdef UIDEBUG
    void debugCommand(const NaviData& naviData);
#endif

    // Modeler callbacks
    void modelerMainFileExit(const NaviData& naviData);

    // Helpers
    NaviPanel getNaviPanel(const std::string& naviName);
    void naviToShowPageRefresh(const NaviData& naviData);
    void hidePreviousNavi();
};

#endif // #ifndef __NavigatorGUI_h__