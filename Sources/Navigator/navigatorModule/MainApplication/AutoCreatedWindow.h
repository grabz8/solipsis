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

#ifndef __AutoCreatedWindow_h__
#define __AutoCreatedWindow_h__

#include "Ogre.h"
#include "IWindow.h"
#include "MainApplication/Instance.h"

//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

using namespace Ogre;

namespace Solipsis {

/** This class manages the window auto-created by Ogre.
    This class is usefull in standalone configuration when the application does not want
    to create/manage its own window, keyboard, mouse.
 */
class AutoCreatedWindow : public IWindow, public FrameListener, public WindowEventListener, public OIS::KeyListener, public OIS::MouseListener
{
protected:
    /// Instance associated to this window
    Instance* mInstance;

    /// OIS Input manager
    OIS::InputManager* mInputManager;
    /// OIS Mouse device
    OIS::Mouse* mMouse;
    /// OIS Keyboard device
    OIS::Keyboard* mKeyboard;
    /// OIS Joystick device
    OIS::JoyStick* mJoy;

    /// True if mouse is exclusive to this window
    bool mMouseExclusive;

public:
    /** Constructor, not to be used directly (use Instance::setWindow()).
    */
    AutoCreatedWindow(Instance* instance);
    ~AutoCreatedWindow();

    /** See IWindow. */
    virtual void* getHandle()
    {
        size_t windowHnd = 0;
        mInstance->getRenderWindowPtr()->getCustomAttribute("WINDOW", &windowHnd);
        return (void*)windowHnd;
    }
    /** See IWindow. */
    virtual bool isFullscreen() { return mInstance->getRenderWindowPtr()->isFullScreen(); }
    /** See IWindow. */
    virtual unsigned int getWidth() { return mInstance->getRenderWindowPtr()->getWidth(); }
    /** See IWindow. */
    virtual unsigned int getHeight() { return mInstance->getRenderWindowPtr()->getHeight(); }
    /** See IWindow. */
    virtual void setMouseExclusive(bool exclusive);

    /** See Ogre::FrameListener. */
    virtual bool frameStarted(const FrameEvent& evt);

    /** See Ogre::WindowEventListener. */
    virtual void windowResized(RenderWindow* rw);
    /** See Ogre::WindowEventListener. */
    virtual void windowClosed(RenderWindow* rw);

    /** See OIS::KeyListener. */
    virtual bool keyPressed(const OIS::KeyEvent &e);
    /** See OIS::KeyListener. */
    virtual bool keyReleased(const OIS::KeyEvent &e);

    /** See OIS::MouseListener. */
    virtual bool mouseMoved(const OIS::MouseEvent &e);
    /** See OIS::MouseListener. */
    virtual bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    /** See OIS::MouseListener. */
    virtual bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    /** Initialize bindings to manage this auto-created window. */
    void initialize();

protected:
    /** Initialize OIS (input context, devices, listeners). */
    void initializeOIS();
    /** Finalize OIS. */
    void finalizeOIS();
};

} // namespace Solipsis

#endif // #ifndef __AutoCreatedWindow_h__
