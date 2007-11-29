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

public:
    NavigatorFrameListener(Navigator* navigator);

    /** These methods implement FrameListener
    */
    virtual bool frameStarted(const FrameEvent& evt);

    /** These methods implement KeyListener
    */
    virtual bool keyPressed(const OIS::KeyEvent &e);
    virtual bool keyReleased(const OIS::KeyEvent &e);

    /** These methods implement MouseListener
    */
    virtual bool mouseMoved(const OIS::MouseEvent &e);
    virtual bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    void setCameraMode(CameraMode mode);
    CameraMode getCameraMode();
    void detachCamera();
    void attachCamera();

protected:
    Navigator* mNavigator;
    Overlay* mStandardOverlay;
    bool mBoundingBoxesShows;
    CameraMode mCameraMode;
    CameraMode mSavedCameraMode;

    Real time;
};

} // namespace Solipsis

#endif // #ifndef __NavigatorFrameListener_h__