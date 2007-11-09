// navigatorActiveXCtrl.cpp : Implementation of CnavigatorActiveXCtrl
#include "stdafx.h"
#include "navigatorActiveXCtrl.h"


// ==============================
// navigator DLL loading
// ==============================

using namespace NavigatorModule;

// navaigator application
INavigatorApp* navigatorApp = 0;

// last location of navigator DLL
char _szLastNavigatorDllLocation[1024] = "";

// navigator related defines
#define SZ_NAVIGATORACTIVEXDLL      "navigatorActiveX.dll"
#define SZ_NAVIGATORDLL             "Navigator.dll"

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

INavigatorApp* _createNavigatorApp()
{
    INavigatorApp* navigatorApp = 0;
    bool error = true;

    // find navigator dll
    char location[1024];
    location[0] = '\0';
    // First check if navigator ActiveX dll is loaded and return its path
    // Else, hModule will be NULL, in which case, the following will return the application's path
    _getPluginPath(SZ_NAVIGATORACTIVEXDLL, &location[0], 1024);

    // if loaded, remember the location
    if (strlen(location) > 0)
        strcpy(_szLastNavigatorDllLocation, location);

    navigatorApp = INavigatorApp::createNavigatorApp(location);
    if (navigatorApp != 0)
        error = false;

    if (error)
    {
        // some error message to the user...
        MessageBox(NULL, SZ_NAVIGATORDLL" could not be found ! Please re-install plugin ...", "Error loading "SZ_NAVIGATORDLL" plugin", MB_OK);
    }

    return navigatorApp;
}

// CnavigatorActiveXCtrl

CnavigatorActiveXCtrl::CnavigatorActiveXCtrl() :
  mInitialized(false),
  mhWnd(NULL),
  mKeyboardHook(NULL),
  mNavigatorInstance(0),
  mWidth(0),
  mHeight(0)
{
	// important, ATL by default creates windowless controls
	m_bWindowOnly = true;
}

HRESULT CnavigatorActiveXCtrl::OnDraw(ATL_DRAWINFO& di)
{
	if (!mInitialized)
    {
		IOleInPlaceActiveObjectImpl<CnavigatorActiveXCtrl>::GetWindow(&mhWnd);
		RECT& rc = *(RECT*)di.prcBounds;
		mWidth = rc.right - rc.left;
		mHeight = rc.bottom - rc.top;

        // create the navigator application
        if (navigatorApp == 0)
            navigatorApp = _createNavigatorApp();

        SetCurrentDirectory(_szLastNavigatorDllLocation);

        // create the navigator instance and set this window
        _createInstance();
        assert(mNavigatorInstance);
        bool ret = mNavigatorInstance->setWindow(this);
        assert(ret);

        mInitialized = true;
    }

/*    RECT& rc = *(RECT*)di.prcBounds;
    // Set Clip region to the rectangle specified by di.prcBounds
    HRGN hRgnOld = NULL;
    if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
    hRgnOld = NULL;
    bool bSelectOldRgn = false;

    HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);

    if (hRgnNew != NULL)
    {
    bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
    }

    Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
    SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
    LPCTSTR pszText = _T("ATL 7.0 : navigatorActiveXCtrl");
    TextOut(di.hdcDraw, 
    (rc.left + rc.right) / 2, 
    (rc.top + rc.bottom) / 2, 
    pszText, 
    lstrlen(pszText));

    if (bSelectOldRgn)
    SelectClipRgn(di.hdcDraw, hRgnOld);
*/
    return S_OK;
}

void CnavigatorActiveXCtrl::FinalRelease()
{
    if (navigatorApp)
    {
        if (mNavigatorInstance)
        {
            navigatorApp->destroyInstance(mNavigatorInstance);
            mNavigatorInstance = 0;
        }
    }

    mhWnd = NULL;
    mInitialized = false;

    // destroy the navigator application
    if (navigatorApp)
	{
        navigatorApp->destroy();
		navigatorApp = 0;
	}
}

void CnavigatorActiveXCtrl::_createInstance()
{
    assert(mNavigatorInstance == 0);

    if (navigatorApp)
    {
        ::putenv("$CONTAINER_NAME=activex");

        mNavigatorInstance = navigatorApp->createInstance();
        assert(mNavigatorInstance);
    }
}
