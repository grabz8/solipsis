// compreg.h : Declaration of the CCompReg

#pragma once

#include "resource.h"       // main symbols

// IComponentRegistrar
[
	object,
	uuid(a817e7a2-43fa-11d0-9e44-00aa00b6770a),
	dual,
	helpstring("IComponentRegistrar Interface"),
	pointer_default(unique)
]
__interface IComponentRegistrar : IDispatch
{
	[id(1)]	HRESULT Attach([in] BSTR bstrPath);
	[id(2)]	HRESULT RegisterAll();
	[id(3)]	HRESULT UnregisterAll();
	[id(4)]	HRESULT GetComponents([out, satype(BSTR)] SAFEARRAY** pbstrCLSIDs, [out, satype(BSTR)] SAFEARRAY** pbstrDescriptions);
	[id(5)]	HRESULT RegisterComponent([in] BSTR bstrCLSID);
	[id(6)] HRESULT UnregisterComponent([in] BSTR bstrCLSID);
};


// CCompReg
[
	coclass,
	threading("single"),
	uuid(6131CB44-7B69-428A-84BF-78E2DC102B77),
	helpstring("ComponentRegistrar Class")
]
class ATL_NO_VTABLE CCompReg : 
	public IComponentRegistrar
{
public:
	CCompReg()
	{
	}

// IComponentRegistrar
public:
    STDMETHOD(Attach)(BSTR bstrPath)
	{
		return S_OK;
	}
	STDMETHOD(RegisterAll)()
	{
		return _AtlComModule.RegisterServer(TRUE);
	}
	STDMETHOD(UnregisterAll)()    
	{
		_AtlComModule.UnregisterServer(TRUE);
		return S_OK;
	}
	STDMETHOD(GetComponents)(SAFEARRAY **ppCLSIDs, SAFEARRAY **ppDescriptions)
	{
		if( ppCLSIDs == NULL || ppDescriptions == NULL )
			return E_POINTER;
		int nComponents = 0;
		for (_ATL_OBJMAP_ENTRY** ppEntry = _AtlComModule.m_ppAutoObjMapFirst; ppEntry < _AtlComModule.m_ppAutoObjMapLast; ppEntry++)
		{
			if (*ppEntry != NULL)
			{
				_ATL_OBJMAP_ENTRY* pEntry = *ppEntry;
				if (pEntry->pclsid != NULL)
				{
					LPCTSTR pszDescription = pEntry->pfnGetObjectDescription();
					if (pszDescription)
						nComponents++;
				}
			}
		}
		SAFEARRAYBOUND rgBound[1];
		rgBound[0].lLbound = 0;
		rgBound[0].cElements = nComponents;
		*ppCLSIDs = SafeArrayCreate(VT_BSTR, 1, rgBound);
		if( *ppCLSIDs == NULL )
			return AtlHresultFromLastError();
		*ppDescriptions = SafeArrayCreate(VT_BSTR, 1, rgBound);
		if( *ppDescriptions == NULL )
			return AtlHresultFromLastError();
		LONG i = 0;
		for (_ATL_OBJMAP_ENTRY** ppEntry = _AtlComModule.m_ppAutoObjMapFirst; ppEntry < _AtlComModule.m_ppAutoObjMapLast; ppEntry++)
		{
			if (*ppEntry != NULL)
			{
				_ATL_OBJMAP_ENTRY* pEntry = *ppEntry;
				if (pEntry->pclsid != NULL)
				{
					LPCTSTR pszDescription = pEntry->pfnGetObjectDescription();
					if (pszDescription)
					{
						LPOLESTR pszCLSID;
						StringFromCLSID(*pEntry->pclsid, &pszCLSID);
						BSTR pBSTR = OLE2BSTR(pszCLSID);
						if( pBSTR == NULL )
						{
							CoTaskMemFree(pszCLSID);
							return E_OUTOFMEMORY;
						}
						HRESULT hResult = SafeArrayPutElement(*ppCLSIDs, &i, pBSTR);
						CoTaskMemFree(pszCLSID);
						if( FAILED(hResult) )
							return hResult;
						pBSTR = T2BSTR_EX(pszDescription);
						if( pBSTR == NULL )
						{
							return E_OUTOFMEMORY;
						}
						hResult = SafeArrayPutElement(*ppDescriptions, &i, pBSTR);
						if( FAILED(hResult) )
							return hResult;
						i++;
					}
				}
			}
		}

		return S_OK;
	}
	STDMETHOD(RegisterComponent)(BSTR bstrCLSID)
	{
		CLSID clsid;
		CLSIDFromString(bstrCLSID, &clsid);
		_AtlComModule.RegisterServer(TRUE, &clsid);
		return S_OK;
	}
	STDMETHOD(UnregisterComponent)(BSTR bstrCLSID)
	{
		CLSID clsid;
		CLSIDFromString(bstrCLSID, &clsid);
		_AtlComModule.UnregisterServer(FALSE, &clsid);
		return S_OK;
	}
};

