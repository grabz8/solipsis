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

// navigatorActiveXCtrl.h : Declaration of the CnavigatorActiveXCtrl
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include <map>

#include "NavigatorModule.h"
#include "IApplication.h"
#include "IInstance.h"
#include "IWindow.h"
#include "Event.h"

// InavigatorActiveXCtrl
[
	object,
	uuid(9C4CFCE7-3F34-41DC-97FD-2D0E65D14490),
	dual,
	helpstring("InavigatorActiveXCtrl Interface"),
	pointer_default(unique)
]
__interface InavigatorActiveXCtrl : public IDispatch
{
};


// CnavigatorActiveXCtrl
[
	coclass,
	threading("apartment"),
	vi_progid("navigatorActiveX.navigatorActiveXCtrl"),
	progid("navigatorActiveX.navigatorActiveXCtrl.1"),
	version(1.0),
	uuid("76107200-CA5D-45EF-8099-DF1750A560D5"),
	helpstring("navigatorActiveXCtrl Class"),
	support_error_info(InavigatorActiveXCtrl),
	registration_script("control.rgs")
]
class ATL_NO_VTABLE CnavigatorActiveXCtrl : 
	public InavigatorActiveXCtrl,
	public IPersistStreamInitImpl<CnavigatorActiveXCtrl>,
	public IOleControlImpl<CnavigatorActiveXCtrl>,
	public IOleObjectImpl<CnavigatorActiveXCtrl>,
	public IOleInPlaceActiveObjectImpl<CnavigatorActiveXCtrl>,
	public IViewObjectExImpl<CnavigatorActiveXCtrl>,
	public IOleInPlaceObjectWindowlessImpl<CnavigatorActiveXCtrl>,
	public IPersistStorageImpl<CnavigatorActiveXCtrl>,
	public ISpecifyPropertyPagesImpl<CnavigatorActiveXCtrl>,
	public IQuickActivateImpl<CnavigatorActiveXCtrl>,
	public IDataObjectImpl<CnavigatorActiveXCtrl>,
	public IProvideClassInfo2Impl<&__uuidof(CnavigatorActiveXCtrl), NULL>,
	public CComControl<CnavigatorActiveXCtrl>,
    public Solipsis::IWindow
{
public:

    // overridden from NavigatorModule::IWindow
    virtual void * getHandle() { return mhWnd; }
    virtual unsigned int getWidth() { return mWidth; }
    virtual unsigned int getHeight() { return mHeight; }
    // end overridden

    CnavigatorActiveXCtrl();

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE | 
	OLEMISC_CANTLINKINSIDE | 
	OLEMISC_INSIDEOUT | 
	OLEMISC_ACTIVATEWHENVISIBLE | 
	OLEMISC_SETCLIENTSITEFIRST
)


BEGIN_PROP_MAP(CnavigatorActiveXCtrl)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(CnavigatorActiveXCtrl)
	CHAIN_MSG_MAP(CComControl<CnavigatorActiveXCtrl>)
	DEFAULT_REFLECTION_HANDLER()
	MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
    MESSAGE_HANDLER(WM_SIZE, OnSize)
    MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMouseLButtonDblClk)
    MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMouseLButtonDown)
    MESSAGE_HANDLER(WM_LBUTTONUP, OnMouseLButtonUp)
    MESSAGE_HANDLER(WM_RBUTTONDOWN, OnMouseRButtonDown)
    MESSAGE_HANDLER(WM_RBUTTONUP, OnMouseRButtonUp)
    MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
    MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
    MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// InavigatorActiveXCtrl
public:
    HRESULT OnDraw(ATL_DRAWINFO& di);

	// Additional messages mapping
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnMouseLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease();

protected:
    // keyboard hook function
	static LRESULT CALLBACK fnHookKeyboard(int code, WPARAM wParam, LPARAM lParam);

private:

    bool mInitialized;
    HWND mhWnd;

    // instances
    static std::map<HWND, Solipsis::IInstance*> mInstances;

    // keyboard hook
    HHOOK mKeyboardHook;
    bool mFirstPersonMode;

    // last mouse evt
    Solipsis::MouseEvt lastMouseEvt;

    // navigator instance
    Solipsis::IInstance* mNavigatorInstance;

    // window attributes
    unsigned int mWidth;
    unsigned int mHeight;

//IWebBrowser2* browser;
};

