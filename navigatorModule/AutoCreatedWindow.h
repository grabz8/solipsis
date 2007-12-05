#ifndef __AUTOCREATEDWINDOW_H__
#define __AUTOCREATEDWINDOW_H__

#include "Ogre.h"
#include "IWindow.h"
#include "Instance.h"

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
    virtual unsigned int getWidth() { return mInstance->getRenderWindowPtr()->getWidth(); }
    /** See IWindow. */
    virtual unsigned int getHeight() { return mInstance->getRenderWindowPtr()->getHeight(); }

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

    /** Initialize bindings to manage this auto-created window.
    */
    void initialize();
};

} // end namespace

#endif // __AUTOCREATEDWINDOW_H__
