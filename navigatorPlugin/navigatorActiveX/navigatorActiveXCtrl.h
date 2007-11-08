// navigatorActiveXCtrl.h : Declaration of the CnavigatorActiveXCtrl
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>

#include "NavigatorModule.h"
#include "INavigatorApp.h"
#include "IInstance.h"
#include "IWindow.h"

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
    public NavigatorModule::IWindow
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


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease();

protected:
    // create the navigator instance
    void _createInstance();

private:

    bool mInitialized;
    HWND mhWnd;

    // keyboard hook
    HHOOK mKeyboardHook;

    // navigator instance
    NavigatorModule::IInstance* mNavigatorInstance;

    // window attributes
    unsigned int mWidth;
    unsigned int mHeight;
};

