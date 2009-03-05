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

#include "Prerequisites.h"
#include "AutoCreatedWindow.h"
#include "NavigatorModule.h"
#include "Instance.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
AutoCreatedWindow::AutoCreatedWindow(Instance* instance) :
    mInstance(instance),
    mInputManager(0), mMouse(0), mKeyboard(0), mJoy(0),
    mMouseExclusive(false)
{
}

//-------------------------------------------------------------------------------------
AutoCreatedWindow::~AutoCreatedWindow()
{
    // Remove ourself as a Window listener
    WindowEventUtilities::removeWindowEventListener(mInstance->getRenderWindowPtr(), this);
    windowClosed(mInstance->getRenderWindowPtr());
}

//-------------------------------------------------------------------------------------
void AutoCreatedWindow::setMouseExclusive(bool exclusive)
{
    if (exclusive == mMouseExclusive) return;
    // Mouse is always exclusive in fullscreen mode
    if (isFullscreen()) return;

    mMouseExclusive = exclusive;

    // Re-initialize OIS (input context, devices, listeners)
    finalizeOIS();
    initializeOIS();
}

//-------------------------------------------------------------------------------------
bool AutoCreatedWindow::frameStarted(const FrameEvent& evt)
{
    if (mMouse)
        mMouse->capture();
    if (mKeyboard)
        mKeyboard->capture();

    return true;
}

//-------------------------------------------------------------------------------------
void AutoCreatedWindow::windowResized(RenderWindow* rw)
{
    //Adjust mouse clipping area
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//-------------------------------------------------------------------------------------
void AutoCreatedWindow::windowClosed(RenderWindow* rw)
{
    mInstance->requestTerminate();

    //Unattach OIS before window shutdown (very important under Linux)
    //Only close for window that created OIS (the main window in these demos)
    if (rw == mInstance->getRenderWindowPtr())
        finalizeOIS();
}

//-------------------------------------------------------------------------------------
bool AutoCreatedWindow::keyPressed(const OIS::KeyEvent &e)
{
    Evt keyboardEvt;
    keyboardEvt.mType = ETKeyPressed;
    keyboardEvt.mKeyboard.mKey = (KeyCode)e.key;
    mInstance->processEvent(Event(0, &keyboardEvt));

    return true;
}

//-------------------------------------------------------------------------------------
bool AutoCreatedWindow::keyReleased(const OIS::KeyEvent &e)
{
    Evt keyboardEvt;
    keyboardEvt.mType = ETKeyReleased;
    keyboardEvt.mKeyboard.mKey = (KeyCode)e.key;
    mInstance->processEvent(Event(0, &keyboardEvt));

    return true;
}

//-------------------------------------------------------------------------------------
bool AutoCreatedWindow::mouseMoved(const OIS::MouseEvent &e)
{
    Evt mouseEvt;
    mouseEvt.mType = ETMouseMoved;
    mouseEvt.mMouse.mState.mX = e.state.X.abs;
    mouseEvt.mMouse.mState.mY = e.state.Y.abs;
    mouseEvt.mMouse.mState.mZ = e.state.Z.abs;
    mouseEvt.mMouse.mState.mXrel = e.state.X.rel;
    mouseEvt.mMouse.mState.mYrel = e.state.Y.rel;
    mouseEvt.mMouse.mState.mZrel = e.state.Z.rel;
    mouseEvt.mMouse.mState.mButtons = MBNone;
    mInstance->processEvent(Event(0, &mouseEvt));

    return true;
}

//-------------------------------------------------------------------------------------
bool AutoCreatedWindow::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    Evt mouseEvt;
    mouseEvt.mType = ETMousePressed;
    mouseEvt.mMouse.mState.mX = e.state.X.abs;
    mouseEvt.mMouse.mState.mY = e.state.Y.abs;
    mouseEvt.mMouse.mState.mZ = e.state.Z.abs;
    mouseEvt.mMouse.mState.mXrel = e.state.X.rel;
    mouseEvt.mMouse.mState.mYrel = e.state.Y.rel;
    mouseEvt.mMouse.mState.mZrel = e.state.Z.rel;
    mouseEvt.mMouse.mState.mButtons = MBNone;
    if (id == OIS::MB_Left) mouseEvt.mMouse.mState.mButtons = MBLeft;
    if (id == OIS::MB_Right) mouseEvt.mMouse.mState.mButtons = MBRight;
    if (id == OIS::MB_Middle) mouseEvt.mMouse.mState.mButtons = MBMiddle;
    mInstance->processEvent(Event(0, &mouseEvt));

    return true;
}

//-------------------------------------------------------------------------------------
bool AutoCreatedWindow::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    Evt mouseEvt;
    mouseEvt.mType = ETMouseReleased;
    mouseEvt.mMouse.mState.mX = e.state.X.abs;
    mouseEvt.mMouse.mState.mY = e.state.Y.abs;
    mouseEvt.mMouse.mState.mZ = e.state.Z.abs;
    mouseEvt.mMouse.mState.mXrel = e.state.X.rel;
    mouseEvt.mMouse.mState.mYrel = e.state.Y.rel;
    mouseEvt.mMouse.mState.mZrel = e.state.Z.rel;
    mouseEvt.mMouse.mState.mButtons = MBNone;
    if (id == OIS::MB_Left) mouseEvt.mMouse.mState.mButtons = MBLeft;
    if (id == OIS::MB_Right) mouseEvt.mMouse.mState.mButtons = MBRight;
    if (id == OIS::MB_Middle) mouseEvt.mMouse.mState.mButtons = MBMiddle;
    mInstance->processEvent(Event(0, &mouseEvt));

    return true;
}

//-------------------------------------------------------------------------------------
void AutoCreatedWindow::initialize()
{
#ifdef WIN32
    // Set application icon
    HWND hwnd = (HWND)getHandle();
    HINSTANCE hinstance = GetModuleHandle(NULL);
    HICON icon = LoadIcon(hinstance, MAKEINTRESOURCE(AUTOCREATEDWINDOW_ICON_INDEX));
    SendMessage(hwnd, WM_SETICON, ICON_BIG, LPARAM(icon));
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, LPARAM(icon));
#endif

    // Mouse is always exclusive in fullscreen mode
    mMouseExclusive = isFullscreen();

    // Initialize OIS (input context, devices, listeners)
    initializeOIS();

    // Register as a Window listener
    WindowEventUtilities::addWindowEventListener(mInstance->getRenderWindowPtr(), this);

    // Register as an Ogre Frame listener
    Root::getSingletonPtr()->addFrameListener(this);
}

//-------------------------------------------------------------------------------------
void AutoCreatedWindow::initializeOIS()
{
    using namespace OIS;

    if (mInputManager != 0) return;

    // Create the input context
    ParamList pl;
    std::ostringstream windowHndStr;
    windowHndStr << (size_t)getHandle();
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    if (!mMouseExclusive)
    {
#ifdef WIN32
        pl.insert(make_pair(std::string("w32_mouse"), "DISCL_FOREGROUND"));
        pl.insert(make_pair(std::string("w32_mouse"), "DISCL_NONEXCLUSIVE"));
#endif
    }
    mInputManager = InputManager::createInputSystem(pl);

    // Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
    mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject(OISKeyboard, true));
    mMouse = static_cast<Mouse*>(mInputManager->createInputObject(OISMouse, true));
    try {
        mJoy = static_cast<JoyStick*>(mInputManager->createInputObject(OISJoyStick, true));
    }
    catch(...) {
        mJoy = 0;
    }

    // Set initial mouse clipping size
    windowResized(mInstance->getRenderWindowPtr());

    // OIS Listeners
    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);
}

//-------------------------------------------------------------------------------------
void AutoCreatedWindow::finalizeOIS()
{
    using namespace OIS;

    if (mInputManager == 0) return;

    // Release all keys before removing keyboard listener
    char keys[256];
    mKeyboard->copyKeyStates(keys);
    for (int k = 0; k < 256; ++k)
        if (keys[k])
            keyReleased(KeyEvent(mKeyboard, (OIS::KeyCode)k, (unsigned int)k));

    mInputManager->destroyInputObject(mMouse);
    mMouse = 0;
    mInputManager->destroyInputObject(mKeyboard);
    mKeyboard = 0;
    mInputManager->destroyInputObject(mJoy);
    mJoy = 0;

    InputManager::destroyInputSystem(mInputManager);
    mInputManager = 0;
}

//-------------------------------------------------------------------------------------
