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

#include <NaviManager.h>
#include <Navi.h>
#include <OgreTimer.h>
#include <OgreExternalTextureSourceManager.h>
#include "ExternalTextureSourceEx.h"
#include "NavigatorFrameListener.h"
#include "OgreHelpers.h"
#include "DebugHelpers.h"

using namespace NaviLibrary;
using namespace Solipsis;

#define MOUSE_WHEEL_FACTOR (1.0f/120.0f)*0.25f
#define ESCAPE_HITS_CANCEL_FOCUS_DURATION 1000
#define ESCAPE_HITS_CANCEL_FOCUS 2

//-------------------------------------------------------------------------------------
NavigatorFrameListener::NavigatorFrameListener(Navigator* navigator) :
    OgreFrameListener(navigator->getRenderWindowPtr(),navigator->getCameraPtr(),navigator->getSceneMgrPtr()),
    mNavigator(navigator),
    mBoundingBoxesShows(false),
    mCameraMode(CMDetached),
    mSavedCameraMode(CMDetached),
    mEscapeHitsB4CancellingFocus(0),
    mLastEscapeHitTimer(0)
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

        // Send events
        mNavigator->sendEvents();
    }

    // Updating sound
    if (mNavigator->getNavigatorSound() != 0)
        mNavigator->getNavigatorSound()->update();

    return OgreFrameListener::frameStarted(evt);
}

//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::keyPressed(const KeyboardEvt& evt)
{ 
    NavigatorGUI* navigatorGUI = mNavigator->getNavigatorGUI();
    Modeler* modeler = mNavigator->mModeler;
	AvatarEditor* avatarEditor = mNavigator->getAvatarEditor();

    // Escape hits count to cancel focus Navi/VNC/...
    if (evt.mKey == KC_ESCAPE)
    {
        unsigned long now = Root::getSingleton().getTimer()->getMilliseconds();
        if (mEscapeHitsB4CancellingFocus == 0)
            mLastEscapeHitTimer = now;
        if (now - mLastEscapeHitTimer < ESCAPE_HITS_CANCEL_FOCUS_DURATION)
            mEscapeHitsB4CancellingFocus++;
        else
            mEscapeHitsB4CancellingFocus = 1;
        mLastEscapeHitTimer = now;
    }

    // Updating Navi with the key pressed
    if (mNavigator->isNaviSupported() && NaviManager::Get().isAnyNaviFocused())
    {
        if (mEscapeHitsB4CancellingFocus >= ESCAPE_HITS_CANCEL_FOCUS)
        {
            mEscapeHitsB4CancellingFocus = 0;
            mNavigator->resetMousePicking();
            NaviManager::Get().deFocusAllNavis();
        }
        return true;
    }

	// is modeling ?
    if (mNavigator->getState() == Navigator::SModeling && modeler != 0)
    {
        if (modeler->isOnGizmo())
        {
            switch (evt.mKey)
            {
            case KC_F9:
	            modeler->lockGizmo(false);
	            if (navigatorGUI != 0)
                    navigatorGUI->modelerMainUnload();
	            return OgreFrameListener::keyPressed(evt);

            case KC_UP:
            case KC_W:
	            if (mNavigator->isOnLeftCTRL)
                {
                    //mNavigator->undo();
                    if( !modeler->isSelectionEmpty() )
                        modeler->getSelected()->undo();
                }
                else
                    mNavigator->MdlrModifGizmo(Vector3(.1,0,0));
	            return OgreFrameListener::keyPressed(evt);

            case KC_DOWN:
            case KC_S:
	            mNavigator->MdlrModifGizmo(Vector3(-.1,0,0));
	            return OgreFrameListener::keyPressed(evt);

            case KC_LEFT:
            case KC_A:
	            mNavigator->MdlrModifGizmo(Vector3(0,0,-.1));
	            return OgreFrameListener::keyPressed(evt);

            case KC_RIGHT:
            case KC_D:
	            mNavigator->MdlrModifGizmo(Vector3(0,0,.1));
	            return OgreFrameListener::keyPressed(evt);

            case KC_PGUP:
            case KC_E:
	            mNavigator->MdlrModifGizmo(Vector3(0,.1,0));
	            return OgreFrameListener::keyPressed(evt);

            case KC_PGDOWN:
            case KC_C:
	            mNavigator->MdlrModifGizmo(Vector3(0,-.1,0));
	            return OgreFrameListener::keyPressed(evt);
            }
        }


        switch (evt.mKey)
        {
        case KC_F9:
            if (modeler->isSelectionLocked() && !navigatorGUI->isModelerMainVisible())
            {
                navigatorGUI->modelerPropHide();
                navigatorGUI->modelerMainShow();
            }
            else 
                navigatorGUI->modelerMainUnload();
            return OgreFrameListener::keyPressed(evt);

        case KC_LCONTROL:
            modeler->getSelection()->set_lock( true );
            mNavigator->isOnLeftCTRL = true;
            return OgreFrameListener::keyPressed(evt);

        case KC_DELETE:
            //mNavigator->suppr();
            if( !modeler->isSelectionEmpty() )
            {
                // remove the current selection
                modeler->removeSelection();

                // hide the gizmos axes
                modeler->getSelection()->mTransformation->showGizmosMove(false);
                modeler->getSelection()->mTransformation->showGizmosRotate(false);
                modeler->getSelection()->mTransformation->showGizmosScale(false);
            }
            if (modeler->isSelectionLocked())
            {
                navigatorGUI->modelerPropUnload();
                navigatorGUI->modelerMainShow();
            }    

            return OgreFrameListener::keyPressed(evt);

        case KC_W:
            if (mNavigator->isOnLeftCTRL) 
            {
                //mNavigator->undo();
                if( !modeler->isSelectionEmpty() )
                    modeler->getSelected()->undo();
            }
            return OgreFrameListener::keyPressed(evt);
        }
    }

	// is editing the avatar ?
	else if (mNavigator->getState() == Navigator::SAvatarEdit && avatarEditor != 0)
	{
		switch (evt.mKey)
		{
		case KC_F8:
			if (/*modeler->isSelectionLocked() &&*/ !navigatorGUI->isAvatarMainVisible())
			{
				navigatorGUI->avatarPropHide();
				navigatorGUI->avatarMainShow();
			}
			else 
				navigatorGUI->avatarMainUnload();
			return OgreFrameListener::keyPressed(evt);
		}
	}

    if ((navigatorGUI != 0) && navigatorGUI->isContextVisible())
        navigatorGUI->contextHide();

    // VNC panel ?
    if (mNavigator->getPickedMovable() && (mNavigator->getPickedMovable()->getQueryFlags() & Navigator::QFVNCPanel))
    {
        if (mEscapeHitsB4CancellingFocus >= ESCAPE_HITS_CANCEL_FOCUS)
        {
            mEscapeHitsB4CancellingFocus = 0;
            mNavigator->resetMousePicking();
            return true;
        }
        MovableObject* vncMovableObj = mNavigator->getPickedMovable();
        Entity* pickedEntity = static_cast<Entity*>(vncMovableObj->getParentSceneNode()->getAttachedObject(0));
        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
        ExternalTextureSourceEx* vncExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc"));
        Evt vncEvt;
        vncEvt.mKeyboard = evt;
        vncExtTextSrc->handleEvt(mtlName, Event(0, &vncEvt));
        return true;
    }

    switch (evt.mKey)
    {
#ifdef UIDEBUG
    case KC_PAUSE: // Show/Hide debug panel
        if (navigatorGUI != 0)
            navigatorGUI->switchDebug();
        break;
#endif

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

    case KC_F8:
        if (navigatorGUI != 0)
			if(mNavigator->getState() == Navigator::SInWorld)
				//if (!navigatorGUI->isAvatarMainVisible())
				navigatorGUI->avatarMainShow();
			else if(mNavigator->getState() == Navigator::SAvatarEdit)
				navigatorGUI->avatarMainUnload();
        break;

    case KC_F9:
        if (navigatorGUI != 0)
			if(mNavigator->getState() == Navigator::SInWorld)
				//if (!navigatorGUI->isModelerMainVisible())
                navigatorGUI->modelerMainShow();
            else if(mNavigator->getState() == Navigator::SModeling)
                navigatorGUI->modelerMainUnload();
        break;

    case KC_F12:
        mBoundingBoxesShows = !mBoundingBoxesShows;
        mSceneMgr->showBoundingBoxes(mBoundingBoxesShows);
        break;
    }

	Avatar* userAvatar = mNavigator->getUserAvatar();
	if (userAvatar != 0)
	{
		switch (evt.mKey)
		{
		case KC_1: // Switch to 1st person camera
			setCameraMode(CM1stPerson);
			break;
		case KC_2: // Switch to 1st person camera with mouse
			setCameraMode(CM1stPersonWithMouse);
			break;
		case KC_3: // Switch to 3rd person camera
			setCameraMode(CM3rdPerson);
			break;
		case KC_4: // Switch to TrunAround person camera
			setCameraMode(CMAroundPerson);
			break;

		case KC_UP:
		case KC_W:
			userAvatar->movementKeyPressed(KC_UP);
			break;

		case KC_DOWN:
		case KC_S:
			userAvatar->movementKeyPressed(KC_DOWN);
			break;

		case KC_LEFT:
		case KC_A:
			userAvatar->movementKeyPressed(KC_LEFT);
			break;

		case KC_RIGHT:
		case KC_D:
			userAvatar->movementKeyPressed(KC_RIGHT);
			break;

		case KC_PGUP:
		case KC_E:
			userAvatar->movementKeyPressed(KC_PGUP);
			break;

		case KC_PGDOWN:
		case KC_C:
			userAvatar->movementKeyPressed(KC_PGDOWN);
			break;

		case KC_END:
			userAvatar->movementKeyPressed(KC_END);
			break;
		}
	}

    return OgreFrameListener::keyPressed(evt);
}

//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::keyReleased(const KeyboardEvt& evt)
{ 
    NavigatorGUI* navigatorGUI = mNavigator->getNavigatorGUI();

    // Updating Navi with the key released
    if (mNavigator->isNaviSupported() && NaviManager::Get().isAnyNaviFocused() && mNavigator->getState() != Navigator::SAvatarEdit) 
		return true;

    // In modeler ?
    if (mNavigator->getState() == Navigator::SModeling)
    {
        switch (evt.mKey) 
        {
            case KC_LCONTROL:
                mNavigator->isOnLeftCTRL = false;
                mNavigator->mModeler->getSelection()->set_lock( false );
                //return OgreFrameListener::keyReleased(evt);
                break;
        }
    }

    // VNC panel ?
    if (mNavigator->getPickedMovable() && (mNavigator->getPickedMovable()->getQueryFlags() & Navigator::QFVNCPanel))
    {
        MovableObject* vncMovableObj = mNavigator->getPickedMovable();
        Entity* pickedEntity = static_cast<Entity*>(vncMovableObj->getParentSceneNode()->getAttachedObject(0));
        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
        ExternalTextureSourceEx* vncExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc"));
        Evt vncEvt;
        vncEvt.mKeyboard = evt;
        vncExtTextSrc->handleEvt(mtlName, Event(0, &vncEvt));
        return true;
    }

    Avatar* userAvatar = mNavigator->getUserAvatar();
    if (userAvatar != 0)
    {
        switch (evt.mKey)
        {
        case KC_UP:
        case KC_W:
            userAvatar->movementKeyReleased(KC_UP);
            break;

        case KC_DOWN:
        case KC_S:
            userAvatar->movementKeyReleased(KC_DOWN);
            break;

        case KC_LEFT:
        case KC_A:
            userAvatar->movementKeyReleased(KC_LEFT);
            break;

        case KC_RIGHT:
        case KC_D:
            userAvatar->movementKeyReleased(KC_RIGHT);
            break;

        case KC_PGUP:
        case KC_E:
            userAvatar->movementKeyReleased(KC_PGUP);
            break;

        case KC_PGDOWN:
        case KC_C:
            userAvatar->movementKeyReleased(KC_PGDOWN);
            break;
        }
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

    // VNC panel ?
    if (mNavigator->getPickedMovable() && (mNavigator->getPickedMovable()->getQueryFlags() & Navigator::QFVNCPanel))
    {
        MovableObject* vncMovableObj = mNavigator->getPickedMovable();
        Entity* pickedEntity = static_cast<Entity*>(vncMovableObj->getParentSceneNode()->getAttachedObject(0));
        // normalize (x, y) on 0..1 and get the ray emitted from the camera
        Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
        // Compute VNC panel mouse location
        Real closestDistance = -1.0f;
        Vector2 closestUV;
        Vector2 closestTriUV0, closestTriUV1, closestTriUV2;
        Vector2 vncXY;
        if (OgreHelpers::isEntityHitByMouse(mouseRay, pickedEntity,
                                            closestDistance,
                                            closestUV,
                                            closestTriUV0, closestTriUV1, closestTriUV2))
        {
            // compute texture coordinates of the hit
            mNavigator->computeVncHit(closestUV,
                                      closestTriUV0, closestTriUV1, closestTriUV2,
                                      vncXY);
            String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
            ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
            ExternalTextureSourceEx* vncExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc"));
            Evt vncEvt;
            vncEvt.mType = evt.mType;
            vncEvt.mMouse.mState = evt.mState;
            vncEvt.mMouse.mState.mXreal = vncXY.x;
            vncEvt.mMouse.mState.mYreal = vncXY.y;
            vncExtTextSrc->handleEvt(mtlName, Event(0, &vncEvt));
        }
    }

    if (mNavigator->getState() == Navigator::SModeling &&
	    !NaviManager::Get().isAnyNaviFocused())
    {
        if (getCameraMode() == CM1stPerson)
        {
            mNavigator->getUserAvatar()->getSceneNode()->yaw(Degree(-mRotate*evt.mState.mXrel));
            mCamNode->getChild(0)->pitch(Degree(mRotate*evt.mState.mYrel));
        }

        //TODO : move / rotate / scale
        //if (mNavigator->isOnGizmo) mNavigator->onMouseMoved(evt);
        return true;
    }

	if (mNavigator->getState() != Navigator::SInWorld && mNavigator->getState() != Navigator::SAvatarEdit)
        return true;

    // if 1 NaviMaterial got focus then mouse wheel is not applied on camera 
    if (mNavigator->isNaviSupported() && NaviManager::Get().isAnyNaviFocused() && NaviManager::Get().getFocusedNavi()->isMaterialOnly())
        return true;

    Real mouseWheel = evt.mState.mZrel;

    if (!Ogre::Math::RealEqual(mouseWheel, 0))
    {
// GILLES begin

		if (getCameraMode() == CM1stPerson&&mouseWheel < 0.0f)
		{
			setCameraMode(CM3rdPerson);
			mCamNode->translate(Vector3(mouseWheel*MOUSE_WHEEL_FACTOR,0,0));//To be sure to go away from the avatar
			
		}
        if (getCameraMode() != CMAroundPerson&&getCameraMode() != CM1stPerson)
        {
            Vector3 pos = mNavigator->getUserAvatar()->getSceneNode()->getPosition();
            Real scale = mNavigator->getUserAvatar()->getSceneNode()->getScale().y;
            Vector3 size = mNavigator->getUserAvatar()->getEntity()->getBoundingBox().getSize();
		
            size.x /=2;
            size.y *=-1;
            size.z = 0;
            //move 3rd person camera toward avatar
            mCamera->lookAt(pos - (mNavigator->getUserAvatar()->getSceneNode()->getOrientation() * size)); 
            mCamNode->translate(Vector3(mouseWheel*MOUSE_WHEEL_FACTOR,0,0));

            //Switch to 1st person camera if close to avatar
 
			Vector3 posAbs = mNavigator->getUserAvatar()->getSceneNode()->getWorldPosition() - (mNavigator->getUserAvatar()->getSceneNode()->getWorldOrientation() * size);
			//Vector3 posAbs = mNavigator->getUserAvatar()->getSceneNode()->getPosition() - (mNavigator->getUserAvatar()->getSceneNode()->getWorldOrientation() * size);
            Vector3 camAbs = mCamera->getWorldPosition();
			//Vector3 camAbs = mCamera->getPosition();  Attention:getWorldPosition() instead of getPosition() to use the same coordinate. 
            if (posAbs.squaredDistance(camAbs) < (size.x)*(size.x))
            {
				if (getCameraMode() == CM3rdPerson)
				{
					mCamNode->translate(Vector3(-1*mouseWheel*MOUSE_WHEEL_FACTOR,0,0));// Revise the position of 3rd person camera
					setCameraMode(CM1stPerson);
				}
            }
        }
				
// GILLES end
    }

    if (getCameraMode() == CM1stPerson)
    {
        mNavigator->getUserAvatar()->getSceneNode()->yaw(Degree(-mRotate*evt.mState.mXrel));
        mCamNode->getChild(0)->pitch(Degree(-mRotate*evt.mState.mYrel));
    }
// GILLES begin
    else if (getCameraMode() == CMAroundPerson)
    {
        SceneNode* camPitchNode = mSceneMgr->getSceneNode("TurnAroundPersonCamPitchNode");
        Vector3 pos = mNavigator->getUserAvatar()->getSceneNode()->getPosition();
        Real scale = mNavigator->getUserAvatar()->getSceneNode()->getScale().y;
        Vector3 size = mNavigator->getUserAvatar()->getEntity()->getBoundingBox().getSize();

        size.x /=2;
        size.y *=-1;
        size.z = 0;
        //move 3rd person camera toward avatar
        mCamera->lookAt(pos - (mNavigator->getUserAvatar()->getSceneNode()->getOrientation()*size)*Vector3(0,1.1,0)); 
        //take care to not go behin the avatar with the wheel up
        camPitchNode->translate(Vector3(mouseWheel*MOUSE_WHEEL_FACTOR,0,0));

        //apply the rotation around the avatar
        static Real yaw = 0;
        static Real pitch = 0;
        yaw = -mRotate*evt.mState.mXrel;
        pitch += -mRotate*evt.mState.mYrel;

        mCamNode->yaw(Degree(yaw));
        mCamNode->getChild(0)->roll(Degree(pitch));
    }
// GILLES end

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
            MovableObject* vncMovableObj = 0;
            Vector2 vncXY;
            if (navigatorGUI->isContextVisible())
                navigatorGUI->contextHide();
            else if ((evt.mState.mButtons & MBRight) && !navigatorGUI->isContextVisible())
            {
                MovableObject* vlcMovableObj = 0;
                if (mNavigator->is1AvatarHitByMouse(avatar))
                    navigatorGUI->contextShow(evt.mState.mX, evt.mState.mY, NavigatorGUI::NAVI_CTXTAVATAR, "look#talk#cancel");
                else if (mNavigator->is1NaviHitByMouse(naviName, naviX, naviY))
                    navigatorGUI->contextShow(evt.mState.mX, evt.mState.mY, NavigatorGUI::NAVI_CTXTWWW, naviName);
                else if (mNavigator->is1VLCHitByMouse(vlcMovableObj))
                {
                    Entity* pickedEntity = static_cast<Entity*>(vlcMovableObj->getParentSceneNode()->getAttachedObject(0));
                    String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
                    navigatorGUI->contextShow(evt.mState.mX, evt.mState.mY, NavigatorGUI::NAVI_CTXTVLC, mtlName);
                }
            }
            else if (mNavigator->is1NaviHitByMouse(naviName, naviX, naviY))
            {
                NaviLibrary::Navi* navi = NaviManager::Get().getNavi(naviName);
                NaviManager::Get().focusNavi(navi);
                navi->injectMouseDown(naviX, naviY);
            }
            // VNC panel ?
            else if (mNavigator->is1VNCHitByMouse(vncMovableObj, vncXY))
            {
                Entity* pickedEntity = static_cast<Entity*>(vncMovableObj->getParentSceneNode()->getAttachedObject(0));
                String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
                ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
                ExternalTextureSourceEx* vncExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc"));
                Evt vncEvt;
                vncEvt.mType = evt.mType;
                vncEvt.mMouse.mState = evt.mState;
                vncEvt.mMouse.mState.mXreal = vncXY.x;
                vncEvt.mMouse.mState.mYreal = vncXY.y;
                vncExtTextSrc->handleEvt(mtlName, Event(0, &vncEvt));
            }
        }
        else if ((mNavigator->getState() == Navigator::SModeling) &&
                !NaviManager::Get().isAnyNaviFocused())
        {
            // the mouse is out of a naviPanel

            //TODO : move / rotate / scale
            //if (mNavigator->isOnGizmo) mNavigator->onMousePressed(evt);
            //else
            {
                // normalize (x, y) on 0..1 and get the ray emitted from the camera
                Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
                // compute the picking
                mNavigator->computeMousePicking(mouseRay);
            }
        }
        else
        {
            // the mouse is on a naviPanel
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
        // VNC panel ?
        else if (mNavigator->getPickedMovable() && (mNavigator->getPickedMovable()->getQueryFlags() & Navigator::QFVNCPanel))
        {
            MovableObject* vncMovableObj = mNavigator->getPickedMovable();
            Entity* pickedEntity = static_cast<Entity*>(vncMovableObj->getParentSceneNode()->getAttachedObject(0));
            // normalize (x, y) on 0..1 and get the ray emitted from the camera
            Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
            // Compute VNC panel mouse location
            Real closestDistance = -1.0f;
            Vector2 closestUV;
            Vector2 closestTriUV0, closestTriUV1, closestTriUV2;
            Vector2 vncXY;
            if (OgreHelpers::isEntityHitByMouse(mouseRay, pickedEntity,
                                                closestDistance,
                                                closestUV,
                                                closestTriUV0, closestTriUV1, closestTriUV2))
            {
                // compute texture coordinates of the hit
                mNavigator->computeVncHit(closestUV,
                                          closestTriUV0, closestTriUV1, closestTriUV2,
                                          vncXY);
                String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
                ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
                ExternalTextureSourceEx* vncExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc"));
                Evt vncEvt;
                vncEvt.mType = evt.mType;
                vncEvt.mMouse.mState = evt.mState;
                vncEvt.mMouse.mState.mXreal = vncXY.x;
                vncEvt.mMouse.mState.mYreal = vncXY.y;
                vncExtTextSrc->handleEvt(mtlName, Event(0, &vncEvt));
            }
        }
        else if ((mNavigator->getState() == Navigator::SModeling) &&
                !NaviManager::Get().isAnyNaviFocused())
        {
            //TODO : move / rotate / scale
            //if (mNavigator->mModeler->isOnGizmo()) mNavigator->onMouseReleased(evt);
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

    Avatar* userAvatar = mNavigator->getUserAvatar();

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
        userAvatar->setMvtType(Avatar::MT1stPerson);
        break;
    case CM3rdPerson:
        mCamNode = mSceneMgr->getSceneNode("ThirdPersonCamNode");
        mSceneMgr->getSceneNode("ThirdPersonCamPitchNode")->attachObject(mCamera);
        userAvatar->setMvtType(Avatar::MT3rdPerson);
        break;
// GILLES begin
    case CMAroundPerson:
        mCamNode = mSceneMgr->getSceneNode("TurnAroundPersonCamNode");
        //mCamera->yaw(Radian(Ogre::Math::HALF_PI));
        mSceneMgr->getSceneNode("TurnAroundPersonCamPitchNode")->attachObject(mCamera);
        userAvatar->setMvtType(Avatar::MTArountPerson);
        break;
// GILLES end
    }
// GILLES begin
    //userAvatar->getSceneNode()->setVisible(mode == CM3rdPerson, false);
    //userAvatar->getSceneNode()->setVisible(mode == CM3rdPerson || mode == CMAroundPerson, false);
	userAvatar->getEntity()->setVisible(mode == CM3rdPerson || mode == CMAroundPerson);
    //userAvatar->setNameVisibility(mode == CM3rdPerson);
    userAvatar->setNameVisibility(mode == CM3rdPerson || mode == CMAroundPerson);
    NavigatorGUI* navigatorGUI = mNavigator->getNavigatorGUI();
    if (navigatorGUI != 0)
        //navigatorGUI->SetMouseVisibility(mode != CM1stPerson);
        navigatorGUI->SetMouseVisibility(mode != CM1stPerson && mode != CMAroundPerson);
// GILLES end
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
