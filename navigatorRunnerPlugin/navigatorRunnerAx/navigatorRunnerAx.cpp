// navigatorRunnerAx.cpp : Implementation of DLL Exports.
//
// Note: COM+ 1.0 Information:
//      Please remember to run Microsoft Transaction Explorer to install the component(s).
//      Registration is not done by default. 

#include "stdafx.h"
#include "resource.h"
#include "compreg.h"

// The module attribute causes DllMain, DllRegisterServer and DllUnregisterServer to be automatically implemented for you
[ module(dll, uuid = "{941F56E4-2BAB-4740-A08E-FFFB1EF02C12}", 
		 name = "navigatorRunnerAx", 
		 helpstring = "navigatorRunnerAx 1.0 Type Library",
		 resource_name = "IDR_NAVIGATORRUNNERAX", 
		 custom = { "a817e7a1-43fa-11d0-9e44-00aa00b6770a", "{9EB988C4-6619-45F0-B40B-C32830EE56C0}"}) ]
class CnavigatorRunnerAxModule
{
public:
// Override CAtlDllModuleT members
};
		 
