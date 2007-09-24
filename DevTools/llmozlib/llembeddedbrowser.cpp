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
 * The Original Code is Linden Lab Inc. (http://lindenlab.com) code.
 *
 * The Initial Developer of the Original Code is:
 *   Callum Prentice (callum@ubrowser.com)
 *
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Callum Prentice (callum@ubrowser.com)
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

// Windows specific switches
#ifdef WIN32
	// appears to be required by LibXUL/Mozilla code to avoid crashes in debug versions of their code (undef'd at end of this file)
	#ifdef _DEBUG
		#define DEBUG 1
	#endif
#endif	// WIN32

#include "llembeddedbrowser.h"
#include "llembeddedbrowserwindow.h"

#ifdef WIN32
	#pragma warning( disable : 4265 )	// "class has virtual functions, but destructor is not virtual"
	#pragma warning( disable : 4291 )	// (no matching operator delete found; memory will not be freed if initialization throws an exception)
#endif	// WIN32

#include "nsBuildID.h"
#include "nsICacheService.h"
#include "nsIPref.h"
#include "nsNetCID.h"
#include "nsProfileDirServiceProvider.h"
#include "nsXULAppAPI.h"

// singleton pattern - initialization
LLEmbeddedBrowser* LLEmbeddedBrowser::sInstance = 0;

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowser::LLEmbeddedBrowser() :
	mErrorNum( 0 )
{
}

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowser::~LLEmbeddedBrowser()
{
}

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowser* LLEmbeddedBrowser::getInstance()
{
    if ( ! sInstance )
    {
        sInstance = new LLEmbeddedBrowser;
    };

	return sInstance;
};

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowser::setLastError( int errorNumIn )
{
	mErrorNum = errorNumIn;
}

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowser::clearLastError()
{
	mErrorNum = 0x0000;
}

////////////////////////////////////////////////////////////////////////////////
//
int LLEmbeddedBrowser::getLastError()
{
	return mErrorNum;
}

////////////////////////////////////////////////////////////////////////////////
//
std::string LLEmbeddedBrowser::getGREVersion()
{
	// take the string directly from Mozilla
	return std::string( GRE_BUILD_ID );
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowser::init( std::string appBaseDir, std::string profileDirName )
{
	std::string appRuntimeDir( appBaseDir );
	std::string xulRuntimeDir( appBaseDir );

	nsCOMPtr< nsILocalFile > xulDir;
	nsresult result = NS_NewNativeLocalFile( nsCString( xulRuntimeDir.c_str() ), PR_FALSE, getter_AddRefs( xulDir ) );
	if ( NS_FAILED( result ) )
	{
		setLastError( 0x1000 );
		return false;
	};

	nsCOMPtr< nsILocalFile > appDir;
	result = NS_NewNativeLocalFile( nsCString( appRuntimeDir.c_str() ), PR_FALSE, getter_AddRefs( appDir ) );
	if ( NS_FAILED( result ) )
	{
		setLastError( 0x1001 );
		return false;
	};

	result = XRE_InitEmbedding( xulDir, appDir, nsnull, nsnull, 0 );
	if ( NS_FAILED( result ) )
	{
		setLastError( 0x1002 );
		return false;
	};

	nsCOMPtr< nsILocalFile > appDataDir;
	std::string profileDir( appBaseDir );
	NS_NewNativeLocalFile( nsDependentCString( profileDir.c_str() ), PR_TRUE, getter_AddRefs( appDataDir ) );
	appDataDir->Append( NS_ConvertUTF8toUCS2( profileDirName.c_str() ) );

	nsCOMPtr< nsILocalFile > localAppDataDir( do_QueryInterface( appDataDir ) );

	nsCOMPtr< nsProfileDirServiceProvider > locProvider;
	NS_NewProfileDirServiceProvider( PR_TRUE, getter_AddRefs( locProvider ) );
	if ( ! locProvider )
	{
		setLastError( 0x1003 );
		XRE_TermEmbedding();
		return PR_FALSE;
	};

	result = locProvider->Register();
	if ( NS_FAILED( result ) )
	{
		setLastError( 0x1004 );
		XRE_TermEmbedding();
		return PR_FALSE;
	};

	result = locProvider->SetProfileDir( localAppDataDir );
	if ( NS_FAILED( result ) )
	{
		setLastError( 0x1005 );
		XRE_TermEmbedding();
		return PR_FALSE;
	};

	// TODO: temporary fix - need to do this in the absence of the relevant dialogs
	nsCOMPtr<nsIPref> pref = do_CreateInstance( NS_PREF_CONTRACTID );
	if ( pref )
	{
		pref->SetBoolPref( "security.warn_entering_secure", PR_FALSE );
		pref->SetBoolPref( "security.warn_entering_weak", PR_FALSE );
		pref->SetBoolPref( "security.warn_leaving_secure", PR_FALSE );
		pref->SetBoolPref( "security.warn_submit_insecure", PR_FALSE );
	}
	else
	{
		setLastError( 0x1006 );
		return false;
	};

	clearLastError();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowser::reset()
{
	XRE_TermEmbedding();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowser::clearCache()
{
	nsCOMPtr< nsICacheService > cacheService = do_GetService( NS_CACHESERVICE_CONTRACTID );
	if (! cacheService)
		return false;

	cacheService->EvictEntries( nsICache::STORE_ANYWHERE );

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowser::setBrowserAgentId( std::string idIn )
{
	nsCOMPtr<nsIPref> pref = do_CreateInstance( NS_PREF_CONTRACTID );
	if ( pref )
	{
		pref->SetCharPref( "general.useragent.extra.* ", idIn.c_str() );
	};
}

// BEGIN GREG Proxy config addon
////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowser::getProxyConfig( int &proxyType, std::string &proxyHttp, int &proxyHttpPort, std::string &autoConfigURL )
{
	nsCOMPtr<nsIPrefBranch> prefBranch;
	nsCOMPtr<nsIPrefService> prefService(do_GetService(NS_PREFSERVICE_CONTRACTID));
	nsresult rv;
	if (prefService)
	{
		prefService->GetBranch("", getter_AddRefs(prefBranch));
		if (prefBranch)
		{
			nsXPIDLCString tmpstr;
			PRInt32 tmpint;
			rv = prefBranch->GetIntPref("network.proxy.type", &tmpint);
			if (NS_SUCCEEDED(rv))
				proxyType = tmpint;
			rv = prefBranch->GetCharPref("network.proxy.http", getter_Copies(tmpstr));
			if (NS_SUCCEEDED(rv))
				proxyHttp = PromiseFlatCString( tmpstr ).get();
            rv = prefBranch->GetIntPref("network.proxy.http_port", &tmpint);
            if (NS_SUCCEEDED(rv))
                proxyHttpPort = tmpint;
			rv = prefBranch->GetCharPref("network.proxy.autoconfig_url", getter_Copies(tmpstr));
			if (NS_SUCCEEDED(rv))
				autoConfigURL = PromiseFlatCString( tmpstr ).get();
			return true;
		}
	}

	return false;
}

void LLEmbeddedBrowser::setProxyConfig( int proxyType, const std::string &proxyHttp, int proxyHttpPort, const std::string autoConfigURL )
{
	// Update user prefs and save them
	nsCOMPtr<nsIPrefBranch> prefBranch;
	nsCOMPtr<nsIPrefService> prefService(do_GetService(NS_PREFSERVICE_CONTRACTID));
	if (prefService)
	{
		prefService->GetBranch("", getter_AddRefs(prefBranch));
		if (prefBranch)
		{
			prefBranch->SetIntPref("network.proxy.type", proxyType);
			prefBranch->SetCharPref("network.proxy.http", proxyHttp.c_str());
			prefBranch->SetIntPref("network.proxy.http_port", proxyHttpPort);
			prefBranch->SetCharPref("network.proxy.autoconfig_url", autoConfigURL.c_str());
			prefService->SavePrefFile(nsnull);
		}
	}

	// Apply immediately changes
	nsCOMPtr<nsIPref> pref(do_GetService(NS_PREF_CONTRACTID));
	if (pref)
	{
		pref->SetIntPref("network.proxy.type", proxyType);
		pref->SetCharPref("network.proxy.http", proxyHttp.c_str());
		pref->SetIntPref("network.proxy.http_port", proxyHttpPort);
		pref->SetCharPref("network.proxy.autoconfig_url", autoConfigURL.c_str());
	}
}
////////////////////////////////////////////////////////////////////////////////
// END GREG Proxy config addon

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowserWindow* createWindow(  void* nativeWindowHandleIn, int widthIn, int heightIn, PRUint32 aChromeFlags, nsIWebBrowserChrome* aParent, nsIWebBrowserChrome** aNewWindow )
{
	LLEmbeddedBrowserWindow* chrome = new LLEmbeddedBrowserWindow();
	if ( ! chrome )
	{
		return 0;
	};

	CallQueryInterface( NS_STATIC_CAST( nsIWebBrowserChrome*, chrome ), aNewWindow );

	NS_ADDREF( *aNewWindow );

	chrome->SetChromeFlags( aChromeFlags );

	nsCOMPtr< nsIWebBrowser > newBrowser;

	chrome->createBrowser( nativeWindowHandleIn, widthIn, heightIn, getter_AddRefs( newBrowser ) );
	if ( ! newBrowser )
	{
		return 0;
	};

	return chrome;
}

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowserWindow* LLEmbeddedBrowser::createBrowserWindow( void* nativeWindowHandleIn, int browserWidthIn, int browserHeightIn )
{
    nsCOMPtr< nsIWebBrowserChrome > chrome;
    LLEmbeddedBrowserWindow* newWin = createWindow( nativeWindowHandleIn, browserWidthIn, browserHeightIn, nsIWebBrowserChrome::CHROME_ALL, nsnull, getter_AddRefs( chrome ) );
    if ( newWin && chrome )
    {
		newWin->setParent( this );
        nsCOMPtr< nsIWebBrowser > newBrowser;
        chrome->GetWebBrowser( getter_AddRefs( newBrowser ) );
        nsCOMPtr< nsIWebNavigation > webNav( do_QueryInterface ( newBrowser ) );
		webNav->LoadURI( NS_ConvertUTF8toUTF16( "about:blank" ).get(), nsIWebNavigation::LOAD_FLAGS_NONE, nsnull, nsnull, nsnull );

		clearLastError();

		return newWin;
    };

	setLastError( 0x2001 );

	return 0;
};

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowser::destroyBrowserWindow( LLEmbeddedBrowserWindow* browserWindowIn )
{
	nsCOMPtr< nsIWebBrowser > webBrowser;
	nsCOMPtr< nsIWebNavigation > webNavigation;

	browserWindowIn->GetWebBrowser( getter_AddRefs( webBrowser ) );
	webNavigation = do_QueryInterface( webBrowser );
	if ( webNavigation )
	{
		webNavigation->Stop( nsIWebNavigation::STOP_ALL );
	};

	nsCOMPtr< nsIWebBrowser > browser = nsnull;
	browserWindowIn->GetWebBrowser( getter_AddRefs( browser ) );
	nsCOMPtr< nsIBaseWindow > browserAsWin = do_QueryInterface( browser );
	if ( browserAsWin )
	{
		browserAsWin->Destroy();
	};

	browserWindowIn->SetWebBrowser( nsnull );

	NS_RELEASE( browserWindowIn );

	delete browserWindowIn;

	clearLastError();

	return true;
}

// Windows specific switches
#ifdef WIN32
	#pragma warning( 3 : 4291 ) // (no matching operator delete found; memory will not be freed if initialization throws an exception)
	#pragma warning( 3 : 4265 )	// "class has virtual functions, but destructor is not virtual"

	// #define required by this file for LibXUL/Mozilla code to avoid crashes in their debug code
	#ifdef _DEBUG
		#undef DEBUG
	#endif

#endif	// WIN32