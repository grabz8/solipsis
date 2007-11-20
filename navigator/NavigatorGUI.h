#ifndef __NavigatorGUI_h__
#define __NavigatorGUI_h__

#include "NaviManager.h"

using namespace NaviLibrary;

namespace Solipsis {

class Navigator;

class NavigatorGUI
{
public:
    enum NaviState {
        NSNotCreated,   // Navi page not created
        NSCreated       // Navi page created and not visible
    };

    enum NaviPanel {
        NAVI_LOGIN = 0,
        NAVI_OPTIONS,
        NAVI_CHAT,
        NAVI_CONTEXT,
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
    bool startup();

    // Mouse
    void SetMouseVisibility(bool visible);
    bool isMouseVisible();

    // Interfaces
    void login();
    void inWorld();

    // Contextual panel
    void contextShow(int x, int y, const std::string& items);
    bool isContextVisible();
    void contextHide();

    // Main modeler panel
    void modelerMainShow();
    bool isModelerMainVisible();
    void modelerMainHide();
    void modelerMainUnload();

#ifdef UIDEBUG
    void switchDebug();
#endif

protected:
    // Handlers
    // Login/Options/InWorld callbacks
    void loginPageLoaded(const NaviData& naviData);
    void connect(const NaviData& naviData);
    void options(const NaviData& naviData);
    void optionsPageLoaded(const NaviData& naviData);
    void quit(const NaviData& naviData);
    void optionsOk(const NaviData& naviData);
    void optionsBack(const NaviData& naviData);
    void chatPageLoaded(const NaviData& naviData);

    // Modeler callbacks
    void modelerMainFileExit(const NaviData& naviData);

#ifdef UIDEBUG
    // Debug callbacks
    void debugCommand(const NaviData& naviData);
#endif

    // Helpers
    NaviPanel getNaviPanel(const std::string& naviName);
    void naviToShowPageLoaded(const NaviData& naviData);
    void hidePreviousNavi();
};

} // namespace Solipsis

#endif // #ifndef __NavigatorGUI_h__