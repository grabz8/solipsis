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
    std::map<Ogre::String,Ogre::String>::iterator dbgCmd;
    // Launch demoNavi1 ?
    dbgCmd = DebugHelpers::debugCommands.find("demoNavi1");
    if (dbgCmd != DebugHelpers::debugCommands.end())
    {
        DebugHelpers::debugCommands.erase(dbgCmd);
        mNavigator->demoNavi1();
    }
    // Launch demoNavi2 ?
    dbgCmd = DebugHelpers::debugCommands.find("demoNavi2");
    if (dbgCmd != DebugHelpers::debugCommands.end())
    {
        DebugHelpers::debugCommands.erase(dbgCmd);
        mNavigator->demoNavi2();
    }
    // Rotate sun light ?
    static bool rotateSunLight = false;
    dbgCmd = DebugHelpers::debugCommands.find("rotateSunLight");
    if (dbgCmd != DebugHelpers::debugCommands.end())
    {
        DebugHelpers::debugCommands.erase(dbgCmd);
        rotateSunLight = !rotateSunLight;
    }
    if (rotateSunLight)
    {
        Light* light = mSceneMgr->getLight("SunLight");
        if (light != 0) {
            Matrix4 r(Quaternion(Radian(evt.timeSinceLastFrame), Vector3::UNIT_Y));
            Vector3 dir = r.transformAffine(light->getDirection());
            light->setDirection(dir);
        }
    }
#endif

    // Updating Navi
    NaviManager::Get().Update();

    // Process received events
    mNavigator->processEvents();

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
    mNavigator->getUserAvatar()->getSceneNode()->setVisible(mode != CM1stPerson, false);
    mNavigator->getUserAvatar()->setNameVisibility(mode != CM1stPerson);
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
    if (e.state.Z.rel != 0) NaviManager::Get().injectMouseWheel(e.state.Z.rel);
    NaviManager::Get().injectMouseMove(e.state.X.abs, e.state.Y.abs);

    Real mouseWheel = e.state.Z.rel;

    if (!Ogre::Math::RealEqual(mouseWheel, 0))
    {
        if (getCameraMode() == CM1stPerson)
        {
            setCameraMode(CM3rdPerson);
            mouseWheel *= 6; //To be sure to go away from the avatar
        }

        Vector3 pos = mNavigator->getUserAvatar()->getSceneNode()->getPosition();
        Vector3 size = mNavigator->getUserAvatar()->getEntity()->getBoundingBox().getSize();

        size.x /=2;
        size.y *=-1;
        size.z = 0;
        //move 3rd person camera toward avatar
        mCamera->lookAt(pos - (mNavigator->getUserAvatar()->getSceneNode()->getOrientation()*size)); 
        mCamNode->translate(Vector3(mouseWheel/6,0,0));


        //Switch to 1st person camera if close to avatar
        Vector3 posAbs = mNavigator->getUserAvatar()->getSceneNode()->getPosition() - (mNavigator->getUserAvatar()->getSceneNode()->getOrientation() * size);
        //Vector3 posAbs = mNavigator->getUserAvatar()->getSceneNode()->getWorldPosition() - (mNavigator->getUserAvatar()->getSceneNode()->getWorldOrientation() * size);
        Vector3 camAbs = mCamera->getPosition();
        //Vector3 camAbs = mCamera->getWorldPosition();
        if (posAbs.squaredDistance(camAbs)<(size.x)*(size.x))
        {
            if (getCameraMode()==CM3rdPerson)
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
    // Updating Navi with the mouse pressed
    NaviManager::Get().injectMouseDown(id);

    return OgreFrameListener::mousePressed(e,id);
}

//-------------------------------------------------------------------------------------

bool NavigatorFrameListener::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{ 
    // Updating Navi with the mouse pressed
    NaviManager::Get().injectMouseUp(id);

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

    case KC_1: //Changement de camera
        setCameraMode(CM1stPerson);
        break;

    case KC_2: //Changement de camera
        setCameraMode(CM3rdPerson);
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
    case KC_F12:
        mBoundingBoxesShows = !mBoundingBoxesShows;
        mSceneMgr->showBoundingBoxes(mBoundingBoxesShows);
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

    case KC_PGDOWN:
    case KC_E:
        mNavigator->getUserAvatar()->movementKeyPressed(KC_PGDOWN);
        break;

    case KC_PGUP:
    case KC_Q:
        mNavigator->getUserAvatar()->movementKeyPressed(KC_PGUP);
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

    case KC_PGDOWN:
    case KC_E:
        mNavigator->getUserAvatar()->movementKeyReleased(KC_PGDOWN);
        break;

    case KC_PGUP:
    case KC_Q:
        mNavigator->getUserAvatar()->movementKeyReleased(KC_PGUP);
        break;
    }
    return true;
}
