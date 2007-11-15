// navigatorRunAxCtrl.cpp : Implementation of CnavigatorRunAxCtrl
#include "stdafx.h"
#include "navigatorRunAxCtrl.h"
#include <stdio.h>
#include <process.h>


// last location of navigator
char _szLastNavigatorLocation[1024] = "";

// navigator related defines
#define SZ_NAVIGATORRUNNERAXDLL      "navigatorRunnerAx.dll"
#define SZ_NAVIGATOREXE              "Navigator.exe"

// CnavigatorRunAxCtrl

CnavigatorRunAxCtrl::CnavigatorRunAxCtrl()
{
    mUrl2go = L"";
}

HRESULT CnavigatorRunAxCtrl::OnDraw(ATL_DRAWINFO& di)
{
	RECT& rc = *(RECT*)di.prcBounds;
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
//	LPCTSTR pszText = _T("ATL 7.0 : navigatorRunAxCtrl");
	LPCTSTR pszText = _T("Click Me !");
	TextOut(di.hdcDraw, 
		(rc.left + rc.right) / 2, 
		(rc.top + rc.bottom) / 2, 
		pszText, 
		lstrlen(pszText));

	if (bSelectOldRgn)
		SelectClipRgn(di.hdcDraw, hRgnOld);

	return S_OK;
}

LRESULT CnavigatorRunAxCtrl::OnMouseLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return S_OK;
}

LRESULT CnavigatorRunAxCtrl::OnMouseLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    _runNavigatorApp();
    return S_OK;
}

STDMETHODIMP CnavigatorRunAxCtrl::get_URL2GO(BSTR* pVal)
{
	pVal = &mUrl2go;
	return S_OK;
}

STDMETHODIMP CnavigatorRunAxCtrl::put_URL2GO(BSTR newVal)
{
	USES_CONVERSION;
	if (!IsValidURL(NULL, newVal, 0))
        return S_FALSE;
    mUrl2go = newVal;
    mUrl2goStr = OLE2T(mUrl2go);
    return S_OK;
}

void CnavigatorRunAxCtrl::_getPluginPath(const char* dllname, char* p, int size)
{
    // get where plugin dll is
    int len = GetModuleFileName(GetModuleHandle(dllname), p, size);
    for ( int i = len - 1;  i >= 0; i--)
        if ( '\\' == p[i] )
        {
            p[i + 1] = 0;
            break;
        }
}

bool CnavigatorRunAxCtrl::_runNavigatorApp()
{
    bool error = false;

    // find navigator dll
    char location[1024];
    location[0] = '\0';
    // First check if navigator runner ActiveX dll is loaded and return its path
    // Else, hModule will be NULL, in which case, the following will return the application's path
    _getPluginPath(SZ_NAVIGATORRUNNERAXDLL, &location[0], 1024);

    // if found, remember the location
    if (strlen(location) > 0)
        strcpy(_szLastNavigatorLocation, location);
    else
    {
        error = true;
        // some error message to the user...
        ::MessageBox(NULL, SZ_NAVIGATOREXE" could not be found ! Please re-install plugin ...", "Error running "SZ_NAVIGATOREXE" executable", MB_OK);
    }

    if (!error)
    {
        SetCurrentDirectory(_szLastNavigatorLocation);
        std::string executable = std::string(_szLastNavigatorLocation) + std::string(SZ_NAVIGATOREXE);
        char *args[3];
        args[0] = (char*)executable.c_str();
        args[1] = (char*)mUrl2goStr.c_str();
        args[2] = 0;
        ::spawnv(_P_DETACH, executable.c_str(), args);
    }

    return !error;
}
