#include "NaviManager.h"
#include "NavigatorFrameListener.h"
#include "DebugHelpers.h"

using namespace NaviLibrary;

NavigatorFrameListener::NavigatorFrameListener(Navigator* navigator) :
    OgreFrameListener(navigator->getRenderWindowPtr(),navigator->getCameraPtr(),navigator->getSceneMgrPtr()),
    mNavigator(navigator),
    mBoundingBoxesShows(false),
    mCameraMode(CM3rdPerson)
{
    mStandardOverlay = OverlayManager::getSingleton().getByName("Solipsis/StandardOverlay");
    if (mStandardOverlay != 0)
        mStandardOverlay->show();
}

bool NavigatorFrameListener::frameStarted(const FrameEvent& evt)
{
#ifdef UIDEBUG
    DebugHelpers::frameStarted(evt, mNavigator, mSceneMgr);
#endif

    // Updating Navi
    NaviManager::Get().Update();

    // Process received events
    mNavigator->processEvents();

#ifdef PHYSICS
    // Step physics
    OgreOde::StepHandler* stepHandler = mNavigator->getPhysicsStepHandler();
    if (stepHandler != 0)
        stepHandler->step(evt.timeSinceLastFrame);
    OgreOde::World* physicsWorld = mNavigator->getPhysicsWorld();
    if (physicsWorld != 0)
        physicsWorld->synchronise();
#endif

    // Animate
    if (mNavigator->isConnected()) {
        // Peers' avatars
        for (std::map<String,OgrePeer*>::iterator it = mNavigator->getOgrePeersIteratorBegin();it != mNavigator->getOgrePeersIteratorEnd();++it)
            it->second->update(evt.timeSinceLastFrame);
    }

    return OgreFrameListener::frameStarted(evt);
}

void NavigatorFrameListener::setCameraMode(CameraMode mode)
{
    if (mode == mCameraMode) return;

    if (mCamera->getParentSceneNode() != 0)
        mCamera->getParentSceneNode()->detachObject(mCamera);

    switch (mode)
    {
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
    mNavigator->getNavigatorGUI()->SetMouseVisibility(mode != CM1stPerson);
    mCameraMode = mode;
}

NavigatorFrameListener::CameraMode NavigatorFrameListener::getCameraMode()
{
    return mCameraMode;
}


//-----------------------------------------------------------------------------//
//--                                                                         --//
//--                           OIS::MouseListener                            --//
//--                                                                         --//
//-----------------------------------------------------------------------------// 


//-------------------------------------------------------------------------------------

bool NavigatorFrameListener::mouseMoved(const OIS::MouseEvent &e)
{
    // Updating Navi with the mouse motion
    // 3D picking of Navi panels if any NaviMaterial focused
    if (NaviManager::Get().isAnyNaviFocused() && NaviManager::Get().naviFocusedIsMaterialOnly()
        && (mCameraMode != CM1stPerson))
    {
        std::string focusedNavi = NaviManager::Get().getFocusedNaviName();
        if (e.state.Z.rel != 0) NaviManager::Get().injectNaviMouseWheel(focusedNavi, e.state.Z.rel);
        // normalize (x, y) on 0..1 and get the ray emitted from the camera
        Ray mouseRay = mCamera->getCameraToViewportRay((Real)e.state.X.abs/(Real)mCamera->getViewport()->getActualWidth(), (Real)e.state.Y.abs/(Real)mCamera->getViewport()->getActualHeight());
        Real closestDistance = -1.0f;
        Vector2 closestUV;
        Vector2 closestTriUV0, closestTriUV1, closestTriUV2;
        int naviX, naviY;
        if (mNavigator->isNaviHitByMouse(mouseRay, mNavigator->getNaviEntity(focusedNavi),
                                         closestDistance,
                                         closestUV,
                                         closestTriUV0, closestTriUV1, closestTriUV2))
        {
            // compute texture coordinates of the hit
            mNavigator->computeNaviHit(focusedNavi,
                                       closestUV,
                                       closestTriUV0, closestTriUV1, closestTriUV2,
                                       naviX, naviY);
            NaviManager::Get().injectNaviMouseMove(focusedNavi, naviX, naviY);
        }
    }
    else
        if (e.state.Z.rel != 0) NaviManager::Get().injectMouseWheel(e.state.Z.rel);

    // Here we call also the 2D version of injectMouseMove because it will refresh the mouse cursor !
    NaviManager::Get().injectMouseMove(e.state.X.abs, e.state.Y.abs);

    // if 1 NaviMaterial got focus then mouse wheel is not applied on camera 
    if (NaviManager::Get().isAnyNaviFocused() && NaviManager::Get().naviFocusedIsMaterialOnly())
        return true;

    Real mouseWheel = e.state.Z.rel;

    if (!Ogre::Math::RealEqual(mouseWheel, 0))
    {
        if (getCameraMode() == CM1stPerson)
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
        mCamNode->translate(Vector3(mouseWheel/(6*scale),0,0));


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
        mNavigator->getUserAvatar()->getSceneNode()->yaw(Degree(-mRotate*e.state.X.rel));
        mCamNode->getChild(0)->pitch(Degree(-mRotate*e.state.Y.rel));
    }

    return true;
}

//-------------------------------------------------------------------------------------

bool NavigatorFrameListener::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    if (NaviManager::Get().getMouse()->isVisible())
    {
        // Updating Navi with the mouse pressed
        NaviManager::Get().injectMouseDown(id);

        // 3D picking of Navi panels if no 2D panel focused
        if (!NaviManager::Get().isAnyNaviFocused())
        {
            // normalize (x, y) on 0..1 and get the ray emitted from the camera
            Ray mouseRay = mCamera->getCameraToViewportRay((Real)e.state.X.abs/(Real)mCamera->getViewport()->getActualWidth(), (Real)e.state.Y.abs/(Real)mCamera->getViewport()->getActualHeight());
            String naviName;
            int naviX, naviY;
            if (mNavigator->is1NaviHitByMouse(mouseRay, naviName, naviX, naviY))
            {
                NaviManager::Get().focusNavi(naviName);
                NaviManager::Get().injectNaviMouseDown(naviName, id, naviX, naviY);
            }
        }
    }

    return OgreFrameListener::mousePressed(e,id);
}

//-------------------------------------------------------------------------------------

bool NavigatorFrameListener::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    if (NaviManager::Get().getMouse()->isVisible())
    {
        // Updating Navi with the mouse pressed
        // 3D picking of Navi panels if any NaviMaterial focused
        if (NaviManager::Get().isAnyNaviFocused() && NaviManager::Get().naviFocusedIsMaterialOnly())
        {
            // normalize (x, y) on 0..1 and get the ray emitted from the camera
            Ray mouseRay = mCamera->getCameraToViewportRay((Real)e.state.X.abs/(Real)mCamera->getViewport()->getActualWidth(), (Real)e.state.Y.abs/(Real)mCamera->getViewport()->getActualHeight());
            String naviName;
            int naviX, naviY;
            if (mNavigator->is1NaviHitByMouse(mouseRay, naviName, naviX, naviY))
                NaviManager::Get().injectNaviMouseUp(naviName, id, naviX, naviY);
        }
        else
            NaviManager::Get().injectMouseUp(id);
    }

    return OgreFrameListener::mouseReleased(e,id);
}

//-------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------//
//--                                                                         --//
//--                           OIS::KeyListener                              --//
//--                                                                         --//
//-----------------------------------------------------------------------------// 


//-------------------------------------------------------------------------------------

bool NavigatorFrameListener::keyPressed(const OIS::KeyEvent &e)
{ 
#ifdef PHYSICS
    OgreOde::World* physicsWorld = mNavigator->getPhysicsWorld();
#endif

    // Updating Navi with the key pressed
    if (NaviManager::Get().isAnyNaviFocused()) return true;

    using namespace OIS;
    switch (e.key)
    {
#ifdef UIDEBUG
    case KC_PAUSE: // Show/Hide debug panel
        mNavigator->getNavigatorGUI()->switchDebug();
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

#ifdef PHYSICS
    case KC_F10:
        if (physicsWorld != 0)
            physicsWorld->setShowDebugContact(!physicsWorld->getShowDebugContact());
        break;
    case KC_F11:
        if (physicsWorld != 0)
            physicsWorld->setShowDebugGeometries(!physicsWorld->getShowDebugGeometries());
        break;
#ifdef FEET
    case KC_END:
        if (physicsWorld != 0)
            mNavigator->getUserAvatar()->movementKeyPressed(KC_END);
        break;
#endif
#endif
    case KC_F12:
        mBoundingBoxesShows = !mBoundingBoxesShows;
        mSceneMgr->showBoundingBoxes(mBoundingBoxesShows);
        break;
    }
    return OgreFrameListener::keyPressed(e);
}

//-------------------------------------------------------------------------------------

bool NavigatorFrameListener::keyReleased(const OIS::KeyEvent &e)
{
    using namespace OIS;
    switch (e.key) 
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
    return true;
}

#ifdef PHYSICS
//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::collision(OgreOde::Contact* contact)
{
    contact->setCoulombFriction(0.9);
    contact->setBouncyness(0.2);
    contact->setSoftness(0.8, 10e-5);

    return true;
}
#endif
