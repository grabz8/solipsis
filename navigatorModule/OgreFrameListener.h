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

#include <stack>
#include "Ogre.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include "Event.h"

using namespace Ogre;

namespace Solipsis {

/** This class listen/manages frame, window, keyboard, mouse events of a general Ogre application.
 */
class OgreFrameListener : public FrameListener, public KeyboardEventListener, public MouseEventListener
{
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

    // events callbacks stacks
    KeyboardEventListener* mKeyboardListener;
    MouseEventListener* mMouseListener;
    std::stack<KeyboardEventListener*> mKeyListenersStack;
    std::stack<MouseEventListener*> mMouseListenersStack;

    Real mRotate;          // The rotate constant
    Real mMove;            // The movement constant

    SceneManager *mSceneMgr;   // The current SceneManager
    SceneNode *mCamNode;   // The SceneNode the camera is currently attached to

    bool mContinue;        // Whether to continue rendering or not
    Vector3 mDirection;     // Value to move in the correct direction

public:
    OgreFrameListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr);
    virtual ~OgreFrameListener();

    /** See Ogre::FrameListener. */
    virtual bool frameStarted(const FrameEvent& evt);
    /** See Ogre::FrameListener. */
    virtual bool frameEnded(const FrameEvent& evt);

    /** See KeyboardEventListener. */
    virtual bool keyPressed(const KeyboardEvt& evt);
    /** See KeyboardEventListener. */
    virtual bool keyReleased(const KeyboardEvt& evt);

    /** See MouseEventListener. */
    virtual bool mouseMoved(const MouseEvt& evt);
    /** See MouseEventListener. */
    virtual bool mousePressed(const MouseEvt& evt);
    /** See MouseEventListener. */
    virtual bool mouseReleased(const MouseEvt& evt);

    // push 1 new listeners or pop them
    void pushListeners(KeyboardEventListener* keyListener, MouseEventListener* mouseListener);
    void popListeners();
    void setCamera(Camera* Camera) { mCamera = Camera; }

    void showDebugOverlay(bool show);
    void requestShutDown();
    void updateStats(void);
};

} // namespace Solipsis

#endif
