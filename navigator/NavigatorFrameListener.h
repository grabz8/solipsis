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
        CM3rdPerson
    };

protected:
    Navigator* mNavigator;
    Overlay* mStandardOverlay;
    bool mBoundingBoxesShows;
    CameraMode mCameraMode;
    CameraMode mSavedCameraMode;

    Real time;

    // OIS::MouseListener
    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    // OIS::KeyListener
    bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);

public:
    NavigatorFrameListener(Navigator* navigator);
    virtual bool frameStarted(const FrameEvent& evt);

    void setCameraMode(CameraMode mode);
    CameraMode getCameraMode();
    void detachCamera();
    void attachCamera();
};

} // namespace Solipsis

#endif // #ifndef __NavigatorFrameListener_h__