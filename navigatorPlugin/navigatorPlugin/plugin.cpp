/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */


#include <windows.h>
#include <windowsx.h>
#include <sys/stat.h>

#include "plugin.h"
#include <string>
#include <map>
#include "scriptablePluginObject.h"

using namespace Solipsis;

// ==============================
// key map
// ==============================
static KeyCode sKeyMap[0xFF];
void initKeyMap()
{
    int i;

    /* Map the VK keysyms */
    for ( i=0; i<0xFF; ++i )
        sKeyMap[i] = KC_UNASSIGNED;

	sKeyMap['0'] = KC_0;
	sKeyMap['1'] = KC_1;
	sKeyMap['2'] = KC_2;
	sKeyMap['3'] = KC_3;
	sKeyMap['4'] = KC_4;
	sKeyMap['5'] = KC_5;
	sKeyMap['6'] = KC_6;
	sKeyMap['7'] = KC_7;
	sKeyMap['8'] = KC_8;
	sKeyMap['9'] = KC_9;

	sKeyMap['A'] = KC_A;
	sKeyMap['B'] = KC_B;
	sKeyMap['C'] = KC_C;
	sKeyMap['D'] = KC_D;
	sKeyMap['E'] = KC_E;
	sKeyMap['F'] = KC_F;
	sKeyMap['G'] = KC_G;
	sKeyMap['H'] = KC_H;
	sKeyMap['I'] = KC_I;
	sKeyMap['J'] = KC_J;
	sKeyMap['K'] = KC_K;
	sKeyMap['L'] = KC_L;
	sKeyMap['M'] = KC_M;
	sKeyMap['N'] = KC_N;
	sKeyMap['O'] = KC_O;
	sKeyMap['P'] = KC_P;
	sKeyMap['Q'] = KC_Q;
	sKeyMap['R'] = KC_R;
	sKeyMap['S'] = KC_S;
	sKeyMap['T'] = KC_T;
	sKeyMap['U'] = KC_U;
	sKeyMap['V'] = KC_V;
	sKeyMap['W'] = KC_W;
	sKeyMap['X'] = KC_X;
	sKeyMap['Y'] = KC_Y;
	sKeyMap['Z'] = KC_Z;

	sKeyMap[VK_DELETE] = KC_DELETE;

    sKeyMap[VK_UP] = KC_UP;
    sKeyMap[VK_DOWN] = KC_DOWN;
    sKeyMap[VK_RIGHT] = KC_RIGHT;
    sKeyMap[VK_LEFT] = KC_LEFT;
    sKeyMap[VK_INSERT] = KC_INSERT;
    sKeyMap[VK_HOME] = KC_HOME;
    sKeyMap[VK_END] = KC_END;
    sKeyMap[VK_PRIOR] = KC_PGUP;
    sKeyMap[VK_NEXT] = KC_PGDOWN;

    sKeyMap[VK_F1] = KC_F1;
    sKeyMap[VK_F2] = KC_F2;
    sKeyMap[VK_F3] = KC_F3;
    sKeyMap[VK_F4] = KC_F4;
    sKeyMap[VK_F5] = KC_F5;
    sKeyMap[VK_F6] = KC_F6;
    sKeyMap[VK_F7] = KC_F7;
    sKeyMap[VK_F8] = KC_F8;
    sKeyMap[VK_F9] = KC_F9;
    sKeyMap[VK_F10] = KC_F10;
    sKeyMap[VK_F11] = KC_F11;
    sKeyMap[VK_F12] = KC_F12;
}

// ==============================
// navigator DLL loading
// ==============================

// navaigator application
IApplication* application = 0;

// last location of navigator DLL
static char _szLastNavigatorDllLocation[1024] = "";

// navigator related defines
#define SZ_NPSOLNAVDLL              "npsolnav.dll"
#define SZ_NAVIGATORSUBDIR          "solipsis_navigator"
#define SZ_NAVIGATORREGKEY          "SOFTWARE\\Solipsis\\Navigator" // reg key in HKEY_CURRENT_USER
#define SZ_NAVIGATORINSTALLDIR      "Install Directory"
#define SZ_NAVIGATORDLL             "Navigator.dll"
#define SZ_CREATENAVIGATORAPPFUN    "createApplication"

typedef void (CALLBACK* LPFNSETDLLDIRECTORY)(LPCTSTR);

bool _callSetDllDirectory(LPCTSTR pathName)
{
    bool retValue = false;
    HMODULE hmod;

    if ((hmod = GetModuleHandle("kernel32.dll"))) 
    {
        LPFNSETDLLDIRECTORY pSetDllDirectory = NULL;
        pSetDllDirectory = (LPFNSETDLLDIRECTORY)GetProcAddress(hmod, "SetDllDirectoryA");

        if (pSetDllDirectory) 
        {
            pSetDllDirectory(pathName);
            retValue = true;
        }
    }
    return retValue;
}

void _getPluginPath(const char* dllname, char* p, int size)
{
    // get where plugin dll is
    int len = GetModuleFileName(GetModuleHandle(dllname), p, size);
    assert(len>0);
    for ( int i = len - 1;  i >= 0; i--)
        if ( '\\' == p[i] )
        {
            p[i + 1] = 0;
            break;
        }
}

void _readRegistryKey(const char* regKey, const char* regName, char* value, int sizeIn)
{
    DWORD ret;
    DWORD size = sizeIn;
    HKEY hKey = NULL;
    
    ret = RegOpenKeyEx(HKEY_CURRENT_USER, regKey, 0, KEY_READ, &hKey);
    
    if (hKey)
    {
        ret = RegQueryValueEx(hKey, regName, NULL, NULL, (LPBYTE)value, &size);
        value[size - 1] = 0;
        RegCloseKey(hKey);
    }
}

HMODULE _loadNavigatorDll(char* location)
{
    HMODULE h = NULL;
    char pluginPath[1024];

    // 1 ) look in the same folder as this module
    {
        _getPluginPath(SZ_NPSOLNAVDLL, &pluginPath[0], 1024);
        strcpy(location,pluginPath);
        _callSetDllDirectory(pluginPath);
        h = LoadLibrary(SZ_NAVIGATORDLL);

        // on a reload, even though dll may have been loaded from c:\program files,
        // since it's already in memory, we may be confused to think it came from here
        // so use the last know good location if it's not null
        if ((h != NULL) && (strlen(_szLastNavigatorDllLocation) > 0))
            strcpy(location, _szLastNavigatorDllLocation);
    }

    // 2 ) if not, look in the subdirectory
    if (h == NULL)
    {
        char subPath[1024];
        strcpy(subPath, pluginPath);
        strcat(subPath, SZ_NAVIGATORSUBDIR);

        // if this directory exists, then add it to the list of possible places for this dll to exist
        struct _stat buf;
        if (_stat(subPath, &buf) == 0)
        {
            // if directory exists
            strcpy(location, subPath);

            if (_callSetDllDirectory(subPath))
            {
                // win xp
                h = LoadLibrary(SZ_NAVIGATORDLL);
            }
            else
            {
                // win2K, etc.
            }
            if (h != NULL)
            {
                // if loaded, remember the location
                strcpy(_szLastNavigatorDllLocation, subPath);
            }
        }
    }

    // 3 ) if not, look in the registry, load it from there
    if (h == NULL)
    {
        char registryPath[1024] = "";

        _readRegistryKey(SZ_NAVIGATORREGKEY, SZ_NAVIGATORINSTALLDIR, registryPath, 1024);

        if (strlen(registryPath) != 0)
        {

            // if there is a backslash at the end, remove it
            if (registryPath[ strlen(registryPath) - 1 ] == '\\')
                registryPath[ strlen(registryPath) - 1 ] = 0;

            struct _stat buf;
            if (_stat(registryPath, &buf) == 0)
            {
                strcpy(location, registryPath);
                // if directory exists
                if (_callSetDllDirectory(registryPath))
                {
                    // win xp
                    h = LoadLibrary(SZ_NAVIGATORDLL);
                }
                else
                {
                    // win2K, etc.
                }
                if (h != NULL)
                {
                    // if loaded, remember the location
                    strcpy(_szLastNavigatorDllLocation, registryPath);
                }
            }
        }
    }

    return h;
}

IApplication* _createApplication()
{
    IApplication* application = 0;
    bool error = true;

    // find navigator dll
    char location[1024];
    HMODULE h = _loadNavigatorDll(location);
    if (h)
    {
        IApplication* (*createApplicationFunction)(const char*, bool, const char*) = reinterpret_cast<IApplication* (*)(const char*, bool, const char*)>(GetProcAddress(h, SZ_CREATENAVIGATORAPPFUN));
        if (createApplicationFunction != NULL)
        {
            assert(strlen(location)>0);
            // make sure it closes with a backslash
            if (location[strlen(location) - 1] != '\\')
            {
                strcat(location,"\\");
            }
            SetCurrentDirectory(location);
            application = createApplicationFunction(location, false, 0);
            if (application != 0)
                error = false;
        }
    }

    if (error)
    {
        // some error message to the user...
        MessageBox(NULL, SZ_NAVIGATORDLL" could not be found ! Please re-install plugin ...", "Error loading "SZ_NAVIGATORDLL" plugin", MB_OK);
    }

    return application;
}

//////////////////////////////////////
//
// general initialization and shutdown
//
NPError NS_PluginInitialize()
{
    std::string envVar;
    envVar = CONTAINER_NAME_ENV"=mozilla";
    _putenv(envVar.c_str());
    envVar = NAVI_SUPPORT_ENV"=no";
    _putenv(envVar.c_str());

    // create the navigator application
    application = _createApplication();

    initKeyMap();

  return NPERR_NO_ERROR;
}

void NS_PluginShutdown()
{
    // destroy the navigator application
    if (application)
	{
        application->destroy();
		application = 0;
	}
}

/////////////////////////////////////////////////////////////
//
// construction and destruction of our plugin instance object
//

nsPluginInstanceBase * NS_NewPluginInstance(nsPluginCreateData * aCreateDataStruct)
{
  if (!aCreateDataStruct)
    return NULL;

  nsPluginInstance * plugin = new nsPluginInstance(aCreateDataStruct);
  return plugin;
}

void NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin)
{
  if (aPlugin)
    delete (nsPluginInstance *)aPlugin;
}

////////////////////////////////////////
//
// nsPluginInstance class implementation
//

std::map<HWND, IInstance*> mInstances;

nsPluginInstance::nsPluginInstance(nsPluginCreateData * aCreateDataStruct) : nsPluginInstanceBase(),
  mInstance(aCreateDataStruct->instance),
  mInitialized(FALSE),
  mhWnd(NULL),
  mOldProc(NULL),
  mKeyboardHook(NULL),
  mNavigatorInstance(0),
  mWidth(0),
  mHeight(0),
  mScriptableObject(NULL)
{
    // check type
    assert(!strcmp(aCreateDataStruct->type, "application/x-solnav"));

    // keep the plugin resident to avoid destroy/re-creation of application
    NPN_SetValue(mInstance, NPPVpluginKeepLibraryInMemory, (void*)TRUE);
    // TODO: remove this line in order to destroy completely Ogre (like in Ax IE)
    // and resolve the pb of RenderSystem_Direct3D9_d dll loading pb (specified module not found exception)

    // parameters
    assert(aCreateDataStruct->argc > 0);
    for ( int i = 0; i < aCreateDataStruct->argc; i++)
    {
        if (!_stricmp(aCreateDataStruct->argn[i], "width"))
        {
            sscanf(aCreateDataStruct->argv[i], "%d", &mWidth);
        }
        else if (!_stricmp(aCreateDataStruct->argn[i], "height"))
        {
            sscanf(aCreateDataStruct->argv[i], "%d", &mHeight);
        }
    }

    lastMouseEvt.mType = ETNone;

    strcpy(mPaintString, "");
}

nsPluginInstance::~nsPluginInstance()
{
    if (mScriptableObject)
        NPN_ReleaseObject(mScriptableObject);
}

static LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);

NPBool nsPluginInstance::init(NPWindow* aWindow)
{
  if (aWindow == NULL)
    return FALSE;

  mhWnd = (HWND)aWindow->window;
  if (mhWnd == NULL)
    return FALSE;

    // get window extents
    RECT rc;
    if (GetClientRect(mhWnd, &rc))
    {
	    mWidth = rc.right - rc.left;
	    mHeight = rc.bottom - rc.top;
    }
    // create the navigator instance and set this window
    _createInstance();
    assert(mNavigatorInstance);
    bool ret = mNavigatorInstance->setWindow(this);
    assert(ret);

    // subclass window so we can intercept window messages and
    // do our drawing to it
    mOldProc = SubclassWindow(mhWnd, (WNDPROC)PluginWinProc);

  // associate window with our nsPluginInstance object so we can access 
  // it in the window procedure
  SetWindowLong(mhWnd, GWL_USERDATA, (LONG)this);

    // get keyboard events on this thread
    mKeyboardHook = ::SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)kbHookProc, NULL, GetCurrentThreadId());
    assert(mKeyboardHook);

  mInitialized = TRUE;
  return TRUE;
}

void nsPluginInstance::shut()
{
    if (application)
    {
        if (mNavigatorInstance)
        {
            // subclass it back
            SubclassWindow(mhWnd, mOldProc);

	        // unregister the keyboard hook now
	        // so we stop sending keyboard messages
            ::UnhookWindowsHookEx(mKeyboardHook);

            // remove/destroy this instance
            mInstances.erase(mhWnd);
            application->destroyInstance(mNavigatorInstance);
            mNavigatorInstance = 0;
        }
    }

    mhWnd = NULL;
    mInitialized = FALSE;
    ::ShowCursor(TRUE);
}

NPBool nsPluginInstance::isInitialized()
{
  return mInitialized;
}

NPError nsPluginInstance::NewStream(NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)         
{ 
    if (stream->notifyData == NULL)
    {
    }
    return NPERR_NO_ERROR; 
}

NPError nsPluginInstance::DestroyStream (NPStream *stream, NPError reason)
{
    return NPERR_NO_ERROR; 
}

// ==============================
// ! Scriptability related code !
// ==============================
//
// here the plugin is asked by Mozilla to tell if it is scriptable
// we should return a valid interface id and a pointer to 
// nsScriptablePeer interface which we should have implemented
// and which should be defined in the corressponding *.xpt file
// in the bin/components folder
NPError nsPluginInstance::GetValue(NPPVariable variable, void *value)
{
    NPError rv = NPERR_NO_ERROR;

    switch (variable) {
    case NPPVpluginNameString:
        *((char **)value) = "npsolnav";
        break;
    case NPPVpluginDescriptionString:
        *((char **)value) = "Solipsis Navigator Plugin for Mozilla";
        break;
    case NPPVpluginScriptableNPObject:
        *(NPObject **)value = getScriptableObject();
        break;
    default:
        rv = NPERR_GENERIC_ERROR;
        break;
    }

    return rv;
}

const char * nsPluginInstance::getVersion()
{
  return NPN_UserAgent(mInstance);
}

void nsPluginInstance::_createInstance()
{
    assert(mNavigatorInstance == 0);

    if (application)
    {
        mNavigatorInstance = application->createInstance();
        assert(mNavigatorInstance);
        mInstances[mhWnd] = mNavigatorInstance;
    }
}

LRESULT nsPluginInstance::OnMouseLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//    ::SetCapture(mhWnd);
    if (mNavigatorInstance != 0)
    {
        Evt mouseEvt;
        mouseEvt.mType = ETMousePressed;
        mouseEvt.mMouse.mState.mX = (int)GET_X_LPARAM(lParam);
        mouseEvt.mMouse.mState.mY = (int)GET_Y_LPARAM(lParam);
        mouseEvt.mMouse.mState.mZ = 0;
        mouseEvt.mMouse.mState.mXrel = 0;
        mouseEvt.mMouse.mState.mYrel = 0;
        mouseEvt.mMouse.mState.mZrel = 0;
        mouseEvt.mMouse.mState.mButtons = MBLeft;
        mNavigatorInstance->processEvent(Event(0, &mouseEvt));
    }
    return S_OK;
}

LRESULT nsPluginInstance::OnMouseLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (mNavigatorInstance != 0)
    {
        Evt mouseEvt;
        mouseEvt.mType = ETMouseReleased;
        mouseEvt.mMouse.mState.mX = (int)GET_X_LPARAM(lParam);
        mouseEvt.mMouse.mState.mY = (int)GET_Y_LPARAM(lParam);
        mouseEvt.mMouse.mState.mZ = 0;
        mouseEvt.mMouse.mState.mXrel = 0;
        mouseEvt.mMouse.mState.mYrel = 0;
        mouseEvt.mMouse.mState.mZrel = 0;
        mouseEvt.mMouse.mState.mButtons = MBLeft;
        mNavigatorInstance->processEvent(Event(0, &mouseEvt));
    }
    return S_OK;
}

LRESULT nsPluginInstance::OnMouseRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//    ::ReleaseCapture();
    if (mNavigatorInstance != 0)
    {
        Evt mouseEvt;
        mouseEvt.mType = ETMousePressed;
        mouseEvt.mMouse.mState.mX = (int)GET_X_LPARAM(lParam);
        mouseEvt.mMouse.mState.mY = (int)GET_Y_LPARAM(lParam);
        mouseEvt.mMouse.mState.mZ = 0;
        mouseEvt.mMouse.mState.mXrel = 0;
        mouseEvt.mMouse.mState.mYrel = 0;
        mouseEvt.mMouse.mState.mZrel = 0;
        mouseEvt.mMouse.mState.mButtons = MBRight;
        mNavigatorInstance->processEvent(Event(0, &mouseEvt));
    }
    return S_OK;
}

LRESULT nsPluginInstance::OnMouseRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (mNavigatorInstance != 0)
    {
        Evt mouseEvt;
        mouseEvt.mType = ETMouseReleased;
        mouseEvt.mMouse.mState.mX = (int)GET_X_LPARAM(lParam);
        mouseEvt.mMouse.mState.mY = (int)GET_Y_LPARAM(lParam);
        mouseEvt.mMouse.mState.mZ = 0;
        mouseEvt.mMouse.mState.mXrel = 0;
        mouseEvt.mMouse.mState.mYrel = 0;
        mouseEvt.mMouse.mState.mZrel = 0;
        mouseEvt.mMouse.mState.mButtons = MBRight;
        mNavigatorInstance->processEvent(Event(0, &mouseEvt));
    }
    return S_OK;
}

LRESULT nsPluginInstance::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (mNavigatorInstance != 0)
    {
        Evt mouseEvt;
        mouseEvt.mType = ETMouseMoved;
        mouseEvt.mMouse.mState.mX = (int)GET_X_LPARAM(lParam);
        mouseEvt.mMouse.mState.mY = (int)GET_Y_LPARAM(lParam);
        mouseEvt.mMouse.mState.mZ = 0;
        if (lastMouseEvt.mType == ETNone)
        {
            mouseEvt.mMouse.mState.mXrel = 0;
            mouseEvt.mMouse.mState.mYrel = 0;
            mouseEvt.mMouse.mState.mZrel = 0;
        }
        else
        {
            mouseEvt.mMouse.mState.mXrel = mouseEvt.mMouse.mState.mX - lastMouseEvt.mState.mX;
            mouseEvt.mMouse.mState.mYrel = mouseEvt.mMouse.mState.mY - lastMouseEvt.mState.mY;
            mouseEvt.mMouse.mState.mZrel = mouseEvt.mMouse.mState.mZ - lastMouseEvt.mState.mZ;
        }
        lastMouseEvt = mouseEvt.mMouse;
        mouseEvt.mMouse.mState.mButtons = MBNone;
        if (wParam & MK_LBUTTON) mouseEvt.mMouse.mState.mButtons = (MouseButton)((int)mouseEvt.mMouse.mState.mButtons | MBLeft);
        if (wParam & MK_MBUTTON) mouseEvt.mMouse.mState.mButtons = (MouseButton)((int)mouseEvt.mMouse.mState.mButtons | MBMiddle);
        if (wParam & MK_RBUTTON) mouseEvt.mMouse.mState.mButtons = (MouseButton)((int)mouseEvt.mMouse.mState.mButtons | MBRight);
        mNavigatorInstance->processEvent(Event(0, &mouseEvt));
    }
    return S_OK;
}

LRESULT nsPluginInstance::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (mNavigatorInstance != 0)
    {
        Evt keyboardEvt;
        keyboardEvt.mType = ETKeyPressed;
        keyboardEvt.mKeyboard.mKey = sKeyMap[wParam];
        mNavigatorInstance->processEvent(Event(0, &keyboardEvt));
    }
    return S_OK;
}

LRESULT nsPluginInstance::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (mNavigatorInstance != 0)
    {
        Evt keyboardEvt;
        keyboardEvt.mType = ETKeyReleased;
        keyboardEvt.mKeyboard.mKey = sKeyMap[wParam];
        mNavigatorInstance->processEvent(Event(0, &keyboardEvt));
    }
    return S_OK;
}

LRESULT CALLBACK nsPluginInstance::PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    nsPluginInstance* p = (nsPluginInstance*)(GetWindowLong(hWnd, GWL_USERDATA));
    BOOL handled;

    switch (msg)
    {
    case WM_SETCURSOR:
        {
        }
        break;
    case WM_SIZE:
        {
        }
        break;
    case WM_KEYDOWN:
        {
            if (p)
                p->OnKeyDown(msg, wParam, lParam, handled);
        }
        break;
    case WM_KEYUP:
        {
            if (p)
                p->OnKeyUp(msg, wParam, lParam, handled);
        }
        break;
    case WM_LBUTTONDOWN:
        {
            if (p)
                p->OnMouseLButtonDown(msg, wParam, lParam, handled);
        }
        break;
    case WM_LBUTTONUP:
        {
            if (p)
                p->OnMouseLButtonUp(msg, wParam, lParam, handled);
        }
        break;
    case WM_RBUTTONDOWN:
        {
            if (p)
                p->OnMouseRButtonDown(msg, wParam, lParam, handled);
        }
        break;
    case WM_RBUTTONUP:
        {
            if (p)
                p->OnMouseRButtonUp(msg, wParam, lParam, handled);
        }
        break;
    case WM_MBUTTONDOWN:
        {
        }
        break;
    case WM_MBUTTONUP:
        {
        }
        break;
    case WM_MOUSEMOVE:
        {
            if (p)
                p->OnMouseMove(msg, wParam, lParam, handled);
        }
        break;
    case WM_PAINT:
        {
            // draw a frame and display the string
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rc;
            GetClientRect(hWnd, &rc);
            FrameRect(hdc, &rc, GetStockBrush(BLACK_BRUSH));

            // get our plugin instance object and ask it for the version string
            nsPluginInstance *plugin = (nsPluginInstance *)GetWindowLong(hWnd, GWL_USERDATA);
            if (plugin) {
                const char * string = plugin->getPaintString();
                DrawText(hdc, string, strlen(string), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            }
            else {
                char string[] = "Error occured";
                DrawText(hdc, string, strlen(string), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            }

            EndPaint(hWnd, &ps);
        }
        break;
    default:
        break;
    }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK nsPluginInstance::kbHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    if (code == HC_NOREMOVE)
        return 0;

    IInstance* instance = 0;
    HWND hWnd = ::GetFocus();
    std::map<HWND, IInstance*>::const_iterator it;
    it = mInstances.find(hWnd);
    if (it != mInstances.end())
        instance = it->second;
    else
        instance = mInstances.begin()->second;
    if (instance)
    {
        Evt keyboardEvt;
        keyboardEvt.mType = (HIWORD(lParam) & KF_UP) ? ETKeyReleased : ETKeyPressed;
        keyboardEvt.mKeyboard.mKey = sKeyMap[wParam];
        instance->processEvent(Event(0, &keyboardEvt));
        ::SetFocus(hWnd);
    }

    return 1;
}

// return instance of the scriptable object
NPObject* nsPluginInstance::getScriptableObject()
{
    if (!mScriptableObject) {
        mScriptableObject = NPN_CreateObject(getInstance(), &ScriptablePluginObject::sScriptablePluginObject_NPClass);
        ((ScriptablePluginObject*) mScriptableObject)->setPlugin(this);
    }

    if (mScriptableObject)
        NPN_RetainObject(mScriptableObject);

    return mScriptableObject;
}

// return the string to display on WM_PAINT
const char* nsPluginInstance::getPaintString()
{
    return mPaintString;
}

// ==============================
// javascript calls for tests only
// ==============================

// assign the version string to display it on WM_PAINT in the plugin window
void nsPluginInstance::showVersion()
{
    strncpy(mPaintString, getVersion(), sizeof(mPaintString) - 1);
    InvalidateRect(mhWnd, NULL, TRUE);
    UpdateWindow(mhWnd);
}

// clean the string to display on WM_PAINT in the plugin window
void nsPluginInstance::clear()
{
    strncpy(mPaintString, "", sizeof(mPaintString) - 1);
    InvalidateRect(mhWnd, NULL, TRUE);
    UpdateWindow(mhWnd);
}
