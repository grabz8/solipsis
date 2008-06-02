#ifndef __NavigatorFrameListener_h__
#define __NavigatorFrameListener_h__

#include "OgreFrameListener.h"
#include "Navigator.h"

namespace Solipsis {

/** This class is the dedicated frame listener of the Navigator application.
 */
class NavigatorFrameListener : public OgreFrameListener
{
public:
    enum CameraMode {
        CMDetached,
        CM1stPerson,
        CM1stPersonWithMouse,
        CM3rdPerson,
        CMAroundPerson
    };

protected:
    Navigator* mNavigator;
    Overlay* mStandardOverlay;
    bool mBoundingBoxesShows;
    CameraMode mCameraMode;
    CameraMode mSavedCameraMode;
    int mEscapeHitsB4CancellingFocus;
    unsigned long mLastEscapeHitTimer;

public:
    NavigatorFrameListener(Navigator* navigator);

    /** See OgreFrameListener. */
    virtual bool frameStarted(const FrameEvent& evt);

    /** See OgreFrameListener. */
    virtual bool keyPressed(const KeyboardEvt& evt);
    /** See OgreFrameListener. */
    virtual bool keyReleased(const KeyboardEvt& evt);

    /** See OgreFrameListener. */
    virtual bool mouseMoved(const MouseEvt& evt);
    /** See OgreFrameListener. */
    virtual bool mousePressed(const MouseEvt& evt);
    /** See OgreFrameListener. */
    virtual bool mouseReleased(const MouseEvt& evt);

    void setCameraMode(CameraMode mode);
    CameraMode getCameraMode();
    void detachCamera();
    void attachCamera();
};

} // namespace Solipsis

#endif // #ifndef __NavigatorFrameListener_h__