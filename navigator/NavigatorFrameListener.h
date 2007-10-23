#ifndef __NavigatorFrameListener_h__
#define __NavigatorFrameListener_h__

#include "OgreFrameListener.h"
#include "Navigator.h"

#ifdef PHYSICS
#include "OgreOde_Core.h"
#endif

class NavigatorFrameListener :
    public OgreFrameListener
#ifdef PHYSICS
    ,
	public OgreOde::StepListener, 
	public OgreOde::TriangleMeshRayListener,
    public OgreOde::CollisionListener
#endif
{
public:
    enum CameraMode {
        CM1stPerson,
        CM1stPersonWithMouse,
        CM3rdPerson
    };

protected:
    Navigator* mNavigator;
    Overlay* mStandardOverlay;
    bool mBoundingBoxesShows;
    CameraMode mCameraMode;

    Real time;

    // OIS::MouseListener
    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    // OIS::KeyListener
    bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);

#ifdef PHYSICS
    // OgreOde::CollisionListener
    bool collision(OgreOde::Contact* contact);
#endif

public:
    NavigatorFrameListener(Navigator* navigator);
    virtual bool frameStarted(const FrameEvent& evt);

    void setCameraMode(CameraMode mode);
    CameraMode getCameraMode();
};

#endif // #ifndef __NavigatorFrameListener_h__