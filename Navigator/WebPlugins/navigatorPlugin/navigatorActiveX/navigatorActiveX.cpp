// navigatorActiveX.cpp : Implementation of DLL Exports.
//
// Note: COM+ 1.0 Information:
//      Please remember to run Microsoft Transaction Explorer to install the component(s).
//      Registration is not done by default. 

#include "stdafx.h"
#include "resource.h"
#include "compreg.h"

// The module attribute causes DllMain, DllRegisterServer and DllUnregisterServer to be automatically implemented for you
[ module(dll, uuid = "{658D3BE1-A5CD-4C3B-BDA1-2B5D42451C65}", 
		 name = "navigatorActiveX", 
		 helpstring = "navigatorActiveX 1.0 Type Library",
		 resource_name = "IDR_NAVIGATORACTIVEX", 
		 custom = { "a817e7a1-43fa-11d0-9e44-00aa00b6770a", "{6131CB44-7B69-428A-84BF-78E2DC102B77}"}) ]
class CnavigatorActiveXModule
{
public:
// Override CAtlDllModuleT members
};
		 
