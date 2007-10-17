/**
	\file 
		main.cpp
	\brief
		Imperativ Intuitiv Modeler : an application to create / edit some 3D primitives / OGRE meshes
	\author
		ARTEFACTO - Gilles GAUDIN
*/

#include "OgreCEGUIRenderer.h"
#include "OgreCEGUIResourceProvider.h"

#include "MiiApp.h"

/* TODO
- namespace solipsis
- entete : ARTEFACTO GPL ...
- Doxygen : /// \brief ...
- m : membre
- p : param func
- l : local

*/


//----------------------------------------------------------------//
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
{
	// Clean the solipsis log 
	SOLdeleteFile("SOLlog.log");

	SOLIPSISINFO("Trying to start the miiapp....");
	// Create application object
	MiiApp* app = new MiiApp();
	app->init("resourcesMII.cfg");

	try {
		app->go();
	} 
	catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		delete app;
		return 1; // return an error code
#else
		std::cerr << "An exception has occured: " <<
			e.getFullDescription().c_str() << std::endl;
		return 1; // return an error code
#endif
	}
	catch( OIS::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox( NULL, e.eText, "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		delete app;
		return 1; // return an error code
#else
		std::cerr << "An exception has occured: " <<
			e.eText << std::endl;
		return 1; // return an error code
#endif
	}

	SOLIPSISINFO("Shutting down the app....");

	// Prepare the reste of the config 
	bool willReset = app->mustResetConfig();
	// Quit the app
    delete app;
	app = NULL;
	if (willReset)
	{
		SOLIPSISINFO("Will reset the config file ....");
        SOLdeleteFile("Ogre.cfg");
	}
	SOLIPSISINFO("Shutting down success -- GoodBye....");
	return 0;
}

