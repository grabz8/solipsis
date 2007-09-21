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

#include "llmozlib.h"

#include <windows.h>
#include <iostream>
#include <sstream>

#include <stdlib.h>

class screenGrab :
	public LLEmbeddedBrowserWindowObserver
{
	public:
		screenGrab( std::string profileBaseDirIn,
					std::string profileNameIn,
					int browserWindowWidthIn, int browserWindowHeightIn,
					std::string urlIn ) :

			mBrowserWindowWidth( browserWindowWidthIn ),
			mBrowserWindowHeight( browserWindowHeightIn ),
			mUrl( urlIn ),
			mWindow( 0 ),
			mKeepGoing( true ),
			mDocCount( 0 )
		{
			if ( ! LLMozLib::getInstance()->init( profileBaseDirIn, profileNameIn ) )
			{
				std::cout << "Unable to initialize browser control." << std::endl;

				mKeepGoing = false;
			}
			else
			{
				std::cout << "Browser Initialized." << std::endl;

				mWindow = LLMozLib::getInstance()->createBrowserWindow( (void*)GetDesktopWindow(), mBrowserWindowWidth, mBrowserWindowHeight );

				LLMozLib::getInstance()->addObserver( mWindow, this );

				// append details to agent string
				LLMozLib::getInstance()->setBrowserAgentId( "[ screenGrabber]" );

				LLMozLib::getInstance()->navigateTo( mWindow, urlIn );
			};
		};

		void eventLoop()
		{
			MSG msg;
			HANDLE hFakeEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
			while ( mKeepGoing )
			{
				while ( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
				{
					::GetMessage( &msg, NULL, 0, 0 );
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				};

				::MsgWaitForMultipleObjects ( 1, &hFakeEvent, FALSE, 100, QS_ALLEVENTS );
			};

			::CloseHandle( hFakeEvent );
		};

		void onNavigateBegin( const EventType& eventIn )
		{
			if ( eventIn.getEventWindowId() == mWindow )
			{
				if ( mDocCount == 0 )
					std::cout << "Starting to load " << mUrl << std::endl;

				++mDocCount;
			};
		};

		// targa files seem to be the easiest way to write 32bit RGBA images
		void writeTarga( std::string filenameIn, int widthIn, int heightIn, const unsigned char* pixelsIn )
		{
			FILE* fd = fopen( filenameIn.c_str(), "wb" );

			short int val = 0;
			fwrite( &val, 1, 1, fd );

			val = 0;
			fwrite( &val, 1, 1, fd );

			val = 2;
			fwrite( &val, 1, 1, fd );

			val = 0;
			fwrite( &val, 1, 1, fd );
			fwrite( &val, 1, 1, fd );
			fwrite( &val, 1, 1, fd );
			fwrite( &val, 1, 1, fd );
			fwrite( &val, 1, 1, fd );

			val = 0;
			fwrite( &val, 2, 1, fd );
			fwrite( &val, 2, 1, fd  );
			fwrite( &widthIn, 2, 1, fd  );
			fwrite( &heightIn, 2, 1, fd  );

			val = 32;
			fwrite( &val, 1, 1, fd  );

			val = 8;
			fwrite( &val, 1, 1, fd  );

			for ( int y = heightIn - 1; y > - 1; --y )
			{
				fwrite( pixelsIn + y * widthIn * 4, 1, widthIn * 4, fd );
			};

			fclose( fd );
		};

		void writeFile( int windowIdIn )
		{
			const unsigned char* pixels = LLMozLib::getInstance()->grabBrowserWindow( windowIdIn );

			int rowSpan = LLMozLib::getInstance()->getBrowserRowSpan( windowIdIn );
			int actualWidth = LLMozLib::getInstance()->getBrowserRowSpan( windowIdIn ) / LLMozLib::getInstance()->getBrowserDepth( windowIdIn );
			int height = LLMozLib::getInstance()->getBrowserHeight( windowIdIn);
			int depth = LLMozLib::getInstance()->getBrowserDepth( windowIdIn );
			int numBytes =  LLMozLib::getInstance()->getBrowserRowSpan( windowIdIn ) * height;

			std::string escUrl( "" );

			for( unsigned int i = 0; i < mUrl.length(); ++i )
			{
				if ( mUrl.substr( i, 1 ) == "\\" ||
					 mUrl.substr( i, 1 ) == "/" ||
					 mUrl.substr( i, 1 ) == ":" ||
					 mUrl.substr( i, 1 ) == "*" ||
					 mUrl.substr( i, 1 ) == "?" ||
					 mUrl.substr( i, 1 ) == "\"" ||
					 mUrl.substr( i, 1 ) == "<" ||
					 mUrl.substr( i, 1 ) == ">" ||
					 mUrl.substr( i, 1 ) == "|" )
				{
					escUrl += "-";
				}
				else
				{
					escUrl += mUrl.substr( i, 1 );
				};
			};

			std::ostringstream codec;
			codec << escUrl;
			codec << "_";
			codec << actualWidth;
			codec << "x";
			codec << height;
			codec << ".tga";

			std::cout << "Saving page as ";
			std::cout << codec.str();
			std::cout << " (";
			std::cout << actualWidth;
			std::cout << " x ";
			std::cout << height;
			std::cout << " x ";
			std::cout << depth;
			std::cout << " Targa image)" << std::endl;

			writeTarga( codec.str(), actualWidth, height, pixels );

			std::cout << "Finished saving.\n";
		};

		void onNavigateComplete( const EventType& eventIn )
		{
			if ( eventIn.getEventWindowId() == mWindow )
			{
				--mDocCount;
				if ( mDocCount == 0 )
				{
					std::cout << "Completed loading " << mUrl << std::endl;
					writeFile( mWindow );
					mKeepGoing = false;
				};
			};
		};

		~screenGrab()
		{
			if ( mWindow )
			{
				LLMozLib::getInstance()->remObserver( mWindow, this );
				LLMozLib::getInstance()->destroyBrowserWindow( mWindow );
			};

			LLMozLib::getInstance()->reset();
		};

	private:
		int mBrowserWindowWidth;
		int mBrowserWindowHeight;
		int mWindow;
		int mDocCount;
		std::string mUrl;
		bool mKeepGoing;
};

int main( int argc, char* argv[] )
{
	if ( argc != 4 )
	{
		std::cout << std::endl;
		std::cout << "screengrab: Grabs a page and from a URL and saves it to a Targa file." << std::endl << std::endl;
		std::cout << "usage: screengrab <url> <width> <height>" << std::endl << std::endl;
		std::cout << "example: screengrab http://news.google.com 2000 2000" << std::endl;
		exit( 1 );
	};

	char path [ 1024 ];
	_fullpath( path, 0, 1024 );

	std::string url = std::string( argv[ 1 ] );
	int width = atoi( argv[ 2 ] );
	int height = atoi( argv[ 3 ] );

	screenGrab* instance = new screenGrab( std::string( path ), "screenGrab", width, height, url );

	if ( instance )
	{
		instance->eventLoop();

		delete instance;
	};
}