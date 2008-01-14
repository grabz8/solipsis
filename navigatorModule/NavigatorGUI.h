#ifndef __NavigatorGUI_h__
#define __NavigatorGUI_h__

#include "NaviManager.h"

using namespace NaviLibrary;

namespace Solipsis {

class Navigator;

/** This class manages all Graphical User Interfaces of the Navigator.
 */
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
    NaviManager* mNaviMgr;
    static const std::string mNavisNames[NAVI_COUNT];
    NaviState mNavisStates[NAVI_COUNT];
    int mCurrentNavi;
#ifdef UIDEBUG
    bool mTreeDirty;
#endif

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
    void setTreeDirty(bool dirty) { mTreeDirty = dirty; }
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
    void modelerMainFileOpen(const NaviData& naviData);
    void modelerMainFileSave(const NaviData& naviData);
    void modelerMainFileExit(const NaviData& naviData);

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


#ifdef UIDEBUG
    // Debug callbacks
    void debugCommand(const NaviData& naviData);
    void debugPageLoaded(const NaviData& naviData);
    void debugRefreshTree(const NaviData& naviData);
#endif

    // Helpers
    NaviPanel getNaviPanel(const std::string& naviName);
    void naviToShowPageLoaded(const NaviData& naviData);
    void hidePreviousNavi();
};

} // namespace Solipsis

#endif // #ifndef __NavigatorGUI_h__