#include "NaviManager.h"
#include "Navi.h"
#include "NavigatorFrameListener.h"
#include "OgreHelpers.h"
#include "DebugHelpers.h"

using namespace NaviLibrary;
using namespace Solipsis;

#define MOUSE_WHEEL_FACTOR (1.0f/120.0f)*0.25f

//-------------------------------------------------------------------------------------
NavigatorFrameListener::NavigatorFrameListener(Navigator* navigator) :
    OgreFrameListener(navigator->getRenderWindowPtr(),navigator->getCameraPtr(),navigator->getSceneMgrPtr()),
    mNavigator(navigator),
    mBoundingBoxesShows(false),
    mCameraMode(CMDetached),
    mSavedCameraMode(CMDetached)
{
    mStandardOverlay = OverlayManager::getSingleton().getByName("Solipsis/StandardOverlay");
    if (mStandardOverlay != 0)
        mStandardOverlay->show();
}

//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::frameStarted(const FrameEvent& evt)
{
#ifdef UIDEBUG
    DebugHelpers::frameStarted(evt, mNavigator, mSceneMgr);
#endif

    // Updating Navi
    if (mNavigator->isNaviSupported())
        NaviManager::Get().Update();

    if (mNavigator->isConnected()) {
        // Process received events
        mNavigator->processEvents();

        // Update peers
        mNavigator->getOgrePeerManager()->frameStarted(evt);
    }

    return OgreFrameListener::frameStarted(evt);
}

//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::keyPressed(const KeyboardEvt& evt)
{ 
    NavigatorGUI* navigatorGUI = mNavigator->getNavigatorGUI();

	// Updating Navi with the key pressed
    if (mNavigator->isNaviSupported() && NaviManager::Get().isAnyNaviFocused()) return true;

	if (mNavigator->getState() == Navigator::SModeling)
	{
		if (evt.mKey == KC_F9)
			navigatorGUI->modelerMainUnload();
		return OgreFrameListener::keyPressed(evt);
	}

    // In world ?
    if (mNavigator->getState() != Navigator::SInWorld)
		return OgreFrameListener::keyPressed(evt);

    if ((navigatorGUI != 0) && navigatorGUI->isContextVisible())
        navigatorGUI->contextHide();

#ifdef PHYSICS
    OgreOde::World* physicsWorld = mNavigator->getOgrePeerManager()->getPhysicsWorld();
#elif PHYSX
    NxScene* physicsScene = mNavigator->getOgrePeerManager()->getPhysicsScene();
#elif PHYSICSPLUGINS
    IPhysicsScene* physicsScene = mNavigator->getOgrePeerManager()->getPhysicsScene();
#endif

    switch (evt.mKey)
    {
#ifdef UIDEBUG
    case KC_PAUSE: // Show/Hide debug panel
        if (navigatorGUI != 0)
            navigatorGUI->switchDebug();
        break;
#endif

    case KC_1: // Switch to 1st person camera
        setCameraMode(CM1stPerson);
        break;
    case KC_2: // Switch to 1st person camera with mouse
        setCameraMode(CM1stPersonWithMouse);
        break;
    case KC_3: // Switch to 3rd person camera
        setCameraMode(CM3rdPerson);
        break;

    case KC_UP:
    case KC_W:
        mNavigator->getUserAvatar()->movementKeyPressed(KC_UP);
        break;

    case KC_DOWN:
    case KC_S:
        mNavigator->getUserAvatar()->movementKeyPressed(KC_DOWN);
        break;

    case KC_LEFT:
    case KC_A:
        mNavigator->getUserAvatar()->movementKeyPressed(KC_LEFT);
        break;

    case KC_RIGHT:
    case KC_D:
        mNavigator->getUserAvatar()->movementKeyPressed(KC_RIGHT);
        break;

    case KC_PGUP:
    case KC_E:
        mNavigator->getUserAvatar()->movementKeyPressed(KC_PGUP);
        break;

    case KC_PGDOWN:
    case KC_C:
        mNavigator->getUserAvatar()->movementKeyPressed(KC_PGDOWN);
        break;

    case KC_F1:
        mNavigator->fakeSurroundingArea(1);
        break;
    case KC_F2:
        mNavigator->fakeSurroundingArea(2);
        break;
    case KC_F3:
        mNavigator->fakeSurroundingArea(3);
        break;
    case KC_F4:
        mNavigator->fakeSurroundingArea(4);
        break;
    case KC_F5:
        mNavigator->fakeSurroundingArea(5);
        break;
    case KC_F6:
        mNavigator->fakeSurroundingArea(0);
        break;

    case KC_F9:
        if (navigatorGUI != 0)
            if (!navigatorGUI->isModelerMainVisible())
                navigatorGUI->modelerMainShow();
            else
                navigatorGUI->modelerMainUnload();
        break;

#ifdef PHYSICS
    case KC_F10:
        if (physicsWorld != 0)
            physicsWorld->setShowDebugContact(!physicsWorld->getShowDebugContact());
        break;
    case KC_F11:
        if (physicsWorld != 0)
            physicsWorld->setShowDebugGeometries(!physicsWorld->getShowDebugGeometries());
        break;
    case KC_END:
        if (physicsWorld != 0)
            mNavigator->getUserAvatar()->movementKeyPressed(KC_END);
        break;
#elif PHYSX
    case KC_END:
        if (physicsScene != 0)
            mNavigator->getUserAvatar()->movementKeyPressed(KC_END);
        break;
#elif PHYSICSPLUGINS
    case KC_F10:
        if (physicsScene != 0)
            physicsScene->setDebugFlags((IPhysicsScene::DebugFlag)(physicsScene->getDebugFlags() ^ IPhysicsScene::DF_CONTACTS));
        break;
    case KC_F11:
        if (physicsScene != 0)
            physicsScene->setDebugFlags((IPhysicsScene::DebugFlag)(physicsScene->getDebugFlags() ^ IPhysicsScene::DF_GEOMETRIES));
        break;
    case KC_END:
        if (physicsScene != 0)
            mNavigator->getUserAvatar()->movementKeyPressed(KC_END);
        break;
#endif
    case KC_F12:
        mBoundingBoxesShows = !mBoundingBoxesShows;
        mSceneMgr->showBoundingBoxes(mBoundingBoxesShows);
        break;
    }
    return OgreFrameListener::keyPressed(evt);
}

//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::keyReleased(const KeyboardEvt& evt)
{ 
    NavigatorGUI* navigatorGUI = mNavigator->getNavigatorGUI();

    // Updating Navi with the key pressed
    if (mNavigator->isNaviSupported() && NaviManager::Get().isAnyNaviFocused()) return true;

	if (mNavigator->getState() == Navigator::SModeling)
	{
		return OgreFrameListener::keyReleased(evt);
	}

    // In world ?
    if (mNavigator->getState() != Navigator::SInWorld) 
		return OgreFrameListener::keyReleased(evt);

    switch (evt.mKey)
    {
    case KC_UP:
    case KC_W:
        mNavigator->getUserAvatar()->movementKeyReleased(KC_UP);
        break;

    case KC_DOWN:
    case KC_S:
        mNavigator->getUserAvatar()->movementKeyReleased(KC_DOWN);
        break;

    case KC_LEFT:
    case KC_A:
        mNavigator->getUserAvatar()->movementKeyReleased(KC_LEFT);
        break;

    case KC_RIGHT:
    case KC_D:
        mNavigator->getUserAvatar()->movementKeyReleased(KC_RIGHT);
        break;

    case KC_PGUP:
    case KC_E:
        mNavigator->getUserAvatar()->movementKeyReleased(KC_PGUP);
        break;

    case KC_PGDOWN:
    case KC_C:
        mNavigator->getUserAvatar()->movementKeyReleased(KC_PGDOWN);
        break;
    }
    return OgreFrameListener::keyReleased(evt);
}

//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::mouseMoved(const MouseEvt& evt)
{
    // Updating Navi with the mouse motion
    // 3D picking of Navi panels if any NaviMaterial focused
    if (mNavigator->isNaviSupported())
    {
        if ((mNavigator->getState() == Navigator::SInWorld) &&
            NaviManager::Get().isAnyNaviFocused() && NaviManager::Get().getFocusedNavi()->isMaterialOnly()
            && (mCameraMode != CM1stPerson))
        {
            std::string focusedNavi = NaviManager::Get().getFocusedNavi()->getName();
            if (evt.mState.mZrel != 0) NaviManager::Get().getFocusedNavi()->injectMouseWheel(evt.mState.mZrel);
            // normalize (x, y) on 0..1 and get the ray emitted from the camera
            Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
            // Compute Navi panel mouse location
            Real closestDistance = -1.0f;
            Vector2 closestUV;
            Vector2 closestTriUV0, closestTriUV1, closestTriUV2;
            int naviX, naviY;
            if (OgreHelpers::isEntityHitByMouse(mouseRay, mNavigator->getNaviEntity(focusedNavi),
                                                closestDistance,
                                                closestUV,
                                                closestTriUV0, closestTriUV1, closestTriUV2))
            {
                // compute texture coordinates of the hit
                mNavigator->computeNaviHit(focusedNavi,
                                           closestUV,
                                           closestTriUV0, closestTriUV1, closestTriUV2,
                                           naviX, naviY);
                NaviManager::Get().getFocusedNavi()->injectMouseMove(naviX, naviY);
            }
        }
        else
            if (evt.mState.mZrel != 0) NaviManager::Get().injectMouseWheel(evt.mState.mZrel);

        // Here we call also the 2D version of injectMouseMove because it will refresh the mouse cursor !
        NaviManager::Get().injectMouseMove(evt.mState.mX, evt.mState.mY);
    }

    if (mNavigator->getState() != Navigator::SInWorld)
        return true;

    // if 1 NaviMaterial got focus then mouse wheel is not applied on camera 
    if (mNavigator->isNaviSupported() && NaviManager::Get().isAnyNaviFocused() && NaviManager::Get().getFocusedNavi()->isMaterialOnly())
        return true;

    Real mouseWheel = evt.mState.mZrel;

    if (!Ogre::Math::RealEqual(mouseWheel, 0))
    {
        if (getCameraMode() != CM3rdPerson)
        {
            setCameraMode(CM3rdPerson);
            mouseWheel *= 6; //To be sure to go away from the avatar
        }

        Vector3 pos = mNavigator->getUserAvatar()->getSceneNode()->getPosition();
        Real scale = mNavigator->getUserAvatar()->getSceneNode()->getScale().y;
        Vector3 size = mNavigator->getUserAvatar()->getEntity()->getBoundingBox().getSize();

        size.x /=2;
        size.y *=-1;
        size.z = 0;
        //move 3rd person camera toward avatar
        mCamera->lookAt(pos - (mNavigator->getUserAvatar()->getSceneNode()->getOrientation()*size)); 
        mCamNode->translate(Vector3(mouseWheel*MOUSE_WHEEL_FACTOR,0,0));

        //Switch to 1st person camera if close to avatar
        Vector3 posAbs = mNavigator->getUserAvatar()->getSceneNode()->getPosition() - (mNavigator->getUserAvatar()->getSceneNode()->getOrientation() * size);
        //Vector3 posAbs = mNavigator->getUserAvatar()->getSceneNode()->getWorldPosition() - (mNavigator->getUserAvatar()->getSceneNode()->getWorldOrientation() * size);
        Vector3 camAbs = mCamera->getPosition();
        //Vector3 camAbs = mCamera->getWorldPosition();
        if (posAbs.squaredDistance(camAbs)<(size.x)*(size.x))
        {
            if (getCameraMode() == CM3rdPerson)
                setCameraMode(CM1stPerson);
        }
    }

    if (getCameraMode() == CM1stPerson)
    {
        mNavigator->getUserAvatar()->getSceneNode()->yaw(Degree(-mRotate*evt.mState.mXrel));
        mCamNode->getChild(0)->pitch(Degree(-mRotate*evt.mState.mYrel));
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::mousePressed(const MouseEvt& evt)
{
    NavigatorGUI* navigatorGUI = mNavigator->getNavigatorGUI();
    if ((navigatorGUI != 0) && NaviLibrary::NaviMouse::Get().isVisible())
    {
        int buttonsId = (evt.mState.mButtons & MBLeft) ? LeftMouseButton : ((evt.mState.mButtons & MBRight) ? RightMouseButton : MiddleMouseButton);

        // Updating Navi with the mouse pressed
        NaviManager::Get().injectMouseDown(buttonsId);

        // 3D picking of Navi panels if no 2D panel focused
        mNavigator->resetMousePicking();
        if ((mNavigator->getState() == Navigator::SInWorld) &&
            !NaviManager::Get().isAnyNaviFocused())
        {
            // normalize (x, y) on 0..1 and get the ray emitted from the camera
            Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
            // compute the picking
            mNavigator->computeMousePicking(mouseRay);
            // Navi panel ?
            String naviName;
            int naviX, naviY;
            Avatar* avatar;
            if (navigatorGUI->isContextVisible())
                navigatorGUI->contextHide();
            else if (mNavigator->is1AvatarHitByMouse(avatar) && !navigatorGUI->isContextVisible())
                navigatorGUI->contextShow(evt.mState.mX, evt.mState.mY, "look#talk#cancel");
            else if (mNavigator->is1NaviHitByMouse(naviName, naviX, naviY))
            {
                NaviLibrary::Navi* navi = NaviManager::Get().getNavi(naviName);
                NaviManager::Get().focusNavi(navi);
                navi->injectMouseDown(naviX, naviY);
            }
        }
    }

    return OgreFrameListener::mousePressed(evt);
}

//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::mouseReleased(const MouseEvt& evt)
{
    NavigatorGUI* navigatorGUI = mNavigator->getNavigatorGUI();
    if ((navigatorGUI != 0) && NaviLibrary::NaviMouse::Get().isVisible())
    {
        int buttonsId = (evt.mState.mButtons & MBLeft) ? LeftMouseButton : ((evt.mState.mButtons & MBRight) ? RightMouseButton : MiddleMouseButton);

        // Updating Navi with the mouse released
        // 3D picking of Navi panels if any NaviMaterial focused
        if ((mNavigator->getState() == Navigator::SInWorld) &&
            NaviManager::Get().isAnyNaviFocused() && NaviManager::Get().getFocusedNavi()->isMaterialOnly())
        {
            std::string focusedNavi = NaviManager::Get().getFocusedNavi()->getName();
            // normalize (x, y) on 0..1 and get the ray emitted from the camera
            Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
            // Compute Navi panel mouse location
            Real closestDistance = -1.0f;
            Vector2 closestUV;
            Vector2 closestTriUV0, closestTriUV1, closestTriUV2;
            int naviX = 0, naviY = 0;
            if (OgreHelpers::isEntityHitByMouse(mouseRay, mNavigator->getNaviEntity(focusedNavi),
                                                closestDistance,
                                                closestUV,
                                                closestTriUV0, closestTriUV1, closestTriUV2))
            {
                // compute texture coordinates of the hit
                mNavigator->computeNaviHit(focusedNavi,
                                           closestUV,
                                           closestTriUV0, closestTriUV1, closestTriUV2,
                                           naviX, naviY);
            }
            NaviManager::Get().getFocusedNavi()->injectMouseUp(naviX, naviY);
        }
        else
            NaviManager::Get().injectMouseUp(buttonsId);
    }

    return OgreFrameListener::mouseReleased(evt);
}

//-------------------------------------------------------------------------------------
void NavigatorFrameListener::setCameraMode(CameraMode mode)
{
    if (mode == mCameraMode) return;

    if (mCamera->getParentSceneNode() != 0)
        mCamera->getParentSceneNode()->detachObject(mCamera);

    switch (mode)
    {
    case CMDetached:
        break;
    case CM1stPerson:
    case CM1stPersonWithMouse:
        mCamNode = mSceneMgr->getSceneNode("FirstPersonCamNode");
        mCamNode->setOrientation(Quaternion::IDENTITY);
        mCamNode->yaw(Radian(-Ogre::Math::HALF_PI));
        mSceneMgr->getSceneNode("FirstPersonCamPitchNode")->attachObject(mCamera);
        mNavigator->getUserAvatar()->setMvtType(Avatar::MT1stPerson);
        break;
    case CM3rdPerson:
        mCamNode = mSceneMgr->getSceneNode("ThirdPersonCamNode");
        mSceneMgr->getSceneNode("ThirdPersonCamPitchNode")->attachObject(mCamera);
        mNavigator->getUserAvatar()->setMvtType(Avatar::MT3rdPerson);
        break;
    }
    mNavigator->getUserAvatar()->getSceneNode()->setVisible(mode == CM3rdPerson, false);
    mNavigator->getUserAvatar()->setNameVisibility(mode == CM3rdPerson);
    NavigatorGUI* navigatorGUI = mNavigator->getNavigatorGUI();
    if (navigatorGUI != 0)
        navigatorGUI->SetMouseVisibility(mode != CM1stPerson);
    mCameraMode = mode;
}

//-------------------------------------------------------------------------------------
NavigatorFrameListener::CameraMode NavigatorFrameListener::getCameraMode()
{
    return mCameraMode;
}

//-------------------------------------------------------------------------------------
void NavigatorFrameListener::detachCamera()
{
    if (mSavedCameraMode != CMDetached)
        return;
    mSavedCameraMode = getCameraMode();
    setCameraMode(CMDetached);
}

//-------------------------------------------------------------------------------------
void NavigatorFrameListener::attachCamera()
{
    if (mSavedCameraMode == CMDetached)
        return;
    setCameraMode(mSavedCameraMode);
    mSavedCameraMode = CMDetached;
}

//-------------------------------------------------------------------------------------
