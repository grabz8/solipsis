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

#ifndef __OgreFrameListener_h__
#define __OgreFrameListener_h__

#include <stack>
#include "Ogre.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include "Tools/Event.h"

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

    bool mFocused;        // Whether rendering window is focused or not
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

#endif // #ifndef __OgreFrameListener_h__
