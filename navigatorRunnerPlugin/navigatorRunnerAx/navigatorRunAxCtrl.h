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

// navigatorRunAxCtrl.h : Declaration of the CnavigatorRunAxCtrl
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include <string>

// InavigatorRunAxCtrl
[
	object,
	uuid(843F493E-9E7F-4FE7-AE36-AD6D7C3F2952),
	dual,
	helpstring("InavigatorRunAxCtrl Interface"),
	pointer_default(unique)
]
__interface InavigatorRunAxCtrl : public IDispatch
{
	[propget, bindable, requestedit, id(1)]				  HRESULT URL2GO([out, retval] BSTR* pVal);
	[propput, bindable, requestedit, id(1)]				  HRESULT URL2GO([in] BSTR newVal);
};


// CnavigatorRunAxCtrl
[
	coclass,
	threading("apartment"),
	vi_progid("navigatorRunnerAx.navigatorRunAxCtrl"),
	progid("navigatorRunnerAx.navigatorRunAxCtrl.1"),
	version(1.0),
	uuid("AED76CAE-AABF-4E40-8BFE-BCD6F328061C"),
	helpstring("navigatorRunAxCtrl Class"),
	support_error_info(InavigatorRunAxCtrl),
	registration_script("control.rgs")
]
class ATL_NO_VTABLE CnavigatorRunAxCtrl : 
	public CStockPropImpl<CnavigatorRunAxCtrl, InavigatorRunAxCtrl>,
	public IPersistStreamInitImpl<CnavigatorRunAxCtrl>,
	public IOleControlImpl<CnavigatorRunAxCtrl>,
	public IOleObjectImpl<CnavigatorRunAxCtrl>,
	public IOleInPlaceActiveObjectImpl<CnavigatorRunAxCtrl>,
	public IViewObjectExImpl<CnavigatorRunAxCtrl>,
	public IOleInPlaceObjectWindowlessImpl<CnavigatorRunAxCtrl>,
	public IPersistStorageImpl<CnavigatorRunAxCtrl>,
	public ISpecifyPropertyPagesImpl<CnavigatorRunAxCtrl>,
	public IQuickActivateImpl<CnavigatorRunAxCtrl>,
	public IDataObjectImpl<CnavigatorRunAxCtrl>,
	public IProvideClassInfo2Impl<&__uuidof(CnavigatorRunAxCtrl), NULL>,
	public CComControl<CnavigatorRunAxCtrl>,
	public IObjectSafetyImpl<CnavigatorRunAxCtrl,INTERFACESAFE_FOR_UNTRUSTED_CALLER|INTERFACESAFE_FOR_UNTRUSTED_DATA>,
	public IPersistPropertyBagImpl<CnavigatorRunAxCtrl>
{
public:

	CnavigatorRunAxCtrl();

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE | 
	OLEMISC_CANTLINKINSIDE | 
	OLEMISC_INSIDEOUT | 
	OLEMISC_ACTIVATEWHENVISIBLE | 
	OLEMISC_SETCLIENTSITEFIRST
)


BEGIN_PROP_MAP(CnavigatorRunAxCtrl)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	PROP_ENTRY("URL2GO", 1, CLSID_NULL)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

// mark activex as safe for a web page
BEGIN_CATEGORY_MAP(CnavigatorRunAxCtrl)
	IMPLEMENTED_CATEGORY( CATID_SafeForScripting )
	IMPLEMENTED_CATEGORY( CATID_SafeForInitializing )
END_CATEGORY_MAP()

BEGIN_MSG_MAP(CnavigatorRunAxCtrl)
	CHAIN_MSG_MAP(CComControl<CnavigatorRunAxCtrl>)
    MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMouseLButtonDown)
    MESSAGE_HANDLER(WM_LBUTTONUP, OnMouseLButtonUp)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// InavigatorRunAxCtrl
public:
    HRESULT OnDraw(ATL_DRAWINFO& di);

	// Additional messages mapping
	LRESULT OnMouseLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    // Additional properties
	STDMETHOD(get_URL2GO)(BSTR* pVal);
	STDMETHOD(put_URL2GO)(BSTR newVal);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

private:
    void _getPluginPath(const char* dllname, char* p, int size);
    bool _runNavigatorApp();

private:
    // URL where navigator will go on startup
    BSTR mUrl2go;
    std::string mUrl2goStr;
};

