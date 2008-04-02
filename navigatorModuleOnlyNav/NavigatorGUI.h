#ifndef __NavigatorGUI_h__
#define __NavigatorGUI_h__

#include "NaviManager.h"
#include "Modeler.h"

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
        NAVI_MODELERPROP,
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

    // Properties modeler panel
    void modelerPropShow();
    bool isModelerPropVisible();
    void modelerPropHide();
    void modelerPropUnload();

#ifdef UIDEBUG
    void switchDebug();
    void setTreeDirty(bool dirty) { mTreeDirty = dirty; }
    void debugRefreshUrl();
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

    // Modeler page callbacks
    void modelerMainFileOpen(const NaviData& naviData);
    void modelerMainFileSave(const NaviData& naviData);
    void modelerMainFileExit(const NaviData& naviData);
    // Modeler object creation callbacks
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

//    void modelerProperties(const NaviData& naviData);
    void modelerTabberChange(const NaviData& naviData);
public:
    // Modeler properties updates
    void modelerTabberLoad(unsigned pTab);
    void modelerTabberSave();	
protected:
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

#ifdef UIDEBUG
    // Debug callbacks
    void debugCommand(const NaviData& naviData);
    void navCommand(const NaviData& naviData);
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