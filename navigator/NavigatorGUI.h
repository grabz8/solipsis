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

    struct NaviEntry {
        std::string name;
        NaviState state;
    };

    enum Navis {
        NAVI_LOGIN = 0,
        NAVI_OPTIONS,
        NAVI_CHAT,
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
    ~NavigatorGUI() {}

    // Start up GUI
    void startup();

    // Interfaces
    void login();
    void inWorld();

    // Mouse
    void SetMouseVisibility(bool visible);

    // Handlers
    void loginPageRefresh(const NaviData& naviData);
    void connect(const NaviData& naviData);
    void options(const NaviData& naviData);
    void quit(const NaviData& naviData);
    void optionsPageRefresh(const NaviData& naviData);
    void optionsOk(const NaviData& naviData);
    void optionsBack(const NaviData& naviData);
    void chatPageRefresh(const NaviData& naviData);
    void sendMessage(const NaviData& naviData);

#ifdef UIDEBUG
    void switchDebug();
    void debugPageRefresh(const NaviData& naviData);
    void debugCommand(const NaviData& naviData);
#endif

protected:
    void hidePreviousNavi();
};

#endif // #ifndef __NavigatorGUI_h__