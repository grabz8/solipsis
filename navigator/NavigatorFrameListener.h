#ifndef __NavigatorFrameListener_h__
#define __NavigatorFrameListener_h__

#include "OgreFrameListener.h"
#include "Navigator.h"

class NavigatorFrameListener : public OgreFrameListener
{
public:
    enum CameraMode {
        CM1stPerson,
        CM3rdPerson
    };

protected:
    Navigator* mNavigator;
    Overlay* mStandardOverlay;
    bool mBoundingBoxesShows;
    CameraMode mCameraMode;

    Real time;

public:
    NavigatorFrameListener(Navigator* navigator);
    virtual bool frameStarted(const FrameEvent& evt);

    void setCameraMode(CameraMode mode);
    CameraMode getCameraMode();

    //OIS::MouseListener
    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    //OIS::KeyListener
    bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);
};

#endif // #ifndef __NavigatorFrameListener_h__