/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Filename:    ExampleFrameListener.h
Description: Defines an example frame listener which responds to frame events.
This frame listener just moves a specified camera around based on
keyboard and mouse movements.
Mouse:    Freelook
W or Up:  Forward
S or Down:Backward
A:        Step left
D:        Step right
             PgUp:     Move upwards
             PgDown:   Move downwards
             F:        Toggle frame rate stats on/off
             R:        Render mode
             T:        Cycle texture filtering
                       Bilinear, Trilinear, Anisotropic(8)
             P:        Toggle on/off display of camera position / orientation
-----------------------------------------------------------------------------
*/

#ifndef __OgreFrameListener_H__
#define __OgreFrameListener_H__

#include "Ogre.h"
#include "OgreStringConverter.h"
#include "OgreException.h"

#ifndef NAVIGATORMODULE_SELF_BUILD // to replace by 1 interface classes
//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>
#else
#include "Event.h"
#endif

using namespace Ogre;
using namespace NavigatorModule;

#ifndef NAVIGATORMODULE_SELF_BUILD // to replace by 1 interface classes
class OgreFrameListener: public FrameListener, public WindowEventListener, public OIS::KeyListener, public OIS::MouseListener
#else
class OgreFrameListener: public FrameListener, public KeyboardEventListener, public MouseEventListener
#endif
{
public:
    OgreFrameListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr);
    virtual ~OgreFrameListener();

#ifndef NAVIGATORMODULE_SELF_BUILD // to replace by 1 interface classes
    //WindowEventListener
    virtual void windowResized(RenderWindow* rw);
    virtual void windowClosed(RenderWindow* rw);
#endif

    //FrameListener
    bool frameStarted(const FrameEvent& evt);
    bool frameEnded(const FrameEvent& evt);

#ifndef NAVIGATORMODULE_SELF_BUILD // to replace by 1 interface classes
   //OIS::MouseListener
   virtual bool mouseMoved(const OIS::MouseEvent &e);
   virtual bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
   virtual bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

   //OIS::KeyListener
   virtual bool keyPressed(const OIS::KeyEvent &e);
   virtual bool keyReleased(const OIS::KeyEvent &e);
#endif
   virtual bool keyPressed(const KeyboardEvt& evt);
   virtual bool keyReleased(const KeyboardEvt& evt);

   virtual bool mouseMoved(const MouseEvt& evt);
   virtual bool mousePressed(const MouseEvt& evt);
   virtual bool mouseReleased(const MouseEvt& evt);

   void showDebugOverlay(bool show);
   void requestShutDown();
   void updateStats(void);

protected:
    Camera* mCamera;

    Vector3 mTranslateVector;
    RenderWindow* mWindow;
    bool mStatsOn;

    std::string mDebugText;

    unsigned int mNumScreenShots;
    float mMoveScale;
    Degree mRotScale;
    // just to stop toggles flipping too fast
    Real mTimeUntilNextToggle ;
    Radian mRotX, mRotY;
    TextureFilterOptions mFiltering;
    int mAniso;

    int mSceneDetailIndex ;
    Real mMoveSpeed;
    Degree mRotateSpeed;
    Overlay* mDebugOverlay;

#ifndef NAVIGATORMODULE_SELF_BUILD // to replace by 1 interface classes
    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;
    OIS::JoyStick* mJoy;
#endif

    Real mRotate;          // The rotate constant
    Real mMove;            // The movement constant

    SceneManager *mSceneMgr;   // The current SceneManager
    SceneNode *mCamNode;   // The SceneNode the camera is currently attached to

    bool mContinue;        // Whether to continue rendering or not
    Vector3 mDirection;     // Value to move in the correct direction
};

#endif
