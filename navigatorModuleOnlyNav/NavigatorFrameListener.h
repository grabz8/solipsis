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
    CameraMode mLastCameraMode;
    CameraMode mSavedCameraMode;
    int mEscapeHitsB4CancellingFocus;
    unsigned long mLastEscapeHitTimer;
	bool mMouseMiddlePressed;

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
    void saveLastCameraMode() { mLastCameraMode = mCameraMode; }
    CameraMode getLastCameraMode() { return mLastCameraMode; }
    void detachCamera();
    void attachCamera();
};

} // namespace Solipsis

#endif // #ifndef __NavigatorFrameListener_h__