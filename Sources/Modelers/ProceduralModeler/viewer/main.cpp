/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author HUIBAN Vincent

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

#include "ExampleApplication.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "utils/Utils.h"

#include <OIS/OIS.h>

#include <OgreAxisAlignedBox.h>

using namespace std;


class TutorialApplication : public ExampleApplication
{
protected:
	std::vector< std::string > m_meshFiles;
public:
	TutorialApplication( const std::vector< std::string >& meshFiles )
		: m_meshFiles( meshFiles )
	{}

    ~TutorialApplication() 
    {}

protected:
    virtual void createCamera(void)
    {
        mCamera = mSceneMgr->createCamera("PlayerCam");
        mCamera->setPosition(Vector3(900,50,200));
        mCamera->lookAt(Vector3(90,50,400));
        mCamera->setNearClipDistance(5);
    }

    virtual void createViewports(void)
    {
        Viewport* vp = mWindow->addViewport(mCamera);
        vp->setBackgroundColour(ColourValue(0.5,0.5,0.5));
        mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));   
    }

    void createScene(void)
    {
        //mSceneMgr->setAmbientLight( ColourValue( 1.0, 1.0, 1.0 ) );
        Light *light;
        light = mSceneMgr->createLight("Light1");

        /*light->setType(Light::LT_SPOTLIGHT);
        light->setPosition(Vector3(0, 500, 0));

		light->setDirection(Vector3( 0, -1, 0 ));

		light->setSpotlightRange(Degree(35), Degree(170));

        light->setDiffuseColour(0.75, 0.75, 0.75);
        light->setSpecularColour(0.75, 0.75, 0.75);

        light = mSceneMgr->createLight("Light2");
        light->setType(Light::LT_SPOTLIGHT);
        light->setPosition(Vector3(250, 500, 1200));

		light->setDirection(Vector3( 0, -1, 0 ));

		light->setSpotlightRange(Degree(35), Degree(170));

        light->setDiffuseColour(0.75, 0.75, 0.75);
        light->setSpecularColour(0.75, 0.75, 0.75);*/

		light = mSceneMgr->createLight("Light3");
		light->setType(Light::LT_DIRECTIONAL);

		light->setDiffuseColour(0.7, 0.7, 0.7);
		light->setSpecularColour(0.7, 0.7, 0.7);

		light->setDirection(0.25, -1, 0.25);
        
		AxisAlignedBox aabox;
		for( int i = 0; i < m_meshFiles.size(); i++ ) {
			std::string meshRad = pm_utils::split( m_meshFiles[i], "." )[0];
			Entity* entity = mSceneMgr->createEntity( meshRad.c_str() , m_meshFiles[i].c_str() );
			aabox.merge( entity->getBoundingBox() );
			mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject( entity );
		}

        Camera* camera = mSceneMgr->getCamera( "PlayerCam" );
		Vector3 center = ( aabox.getMaximum() + aabox.getMinimum() )/2;
		camera->setPosition( center + Vector3( 0, 50, -200 ) );
	    camera->lookAt( center );


  //      fstream file;
  //      file.open("meshes.txt");
  //      string name;
  //      int firstMesh, lastMesh;
  //      file >> name >> firstMesh >> lastMesh;
		//if(firstMesh == -1){
		//	ostringstream filename;
		//	filename << name << ".mesh" ;
		//	ent = mSceneMgr->createEntity(name.c_str(), filename.str().c_str());
		//	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
		//} else {
		//	int i;
		//	for(i = firstMesh ; i <= lastMesh ; ++i){
  //      		ostringstream entityname, filename;
  //      		entityname << name << i;
  //      		filename << entityname.str() << ".mesh";
		//		ent = mSceneMgr->createEntity(entityname.str().c_str(), filename.str().c_str());
		//		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);

		//	}
		//}
    }
};

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT ) {
	std::vector< std::string > files = pm_utils::split( std::string( strCmdLine ), " " );
#else
int main(int argc, char **argv) {
	std::vector< std::string > files;
	for( int i = 1; i < argc; i++ )
		files.push_back( argv[i] );
#endif
    // Create application object
	TutorialApplication app( files );

    try {
        app.go();
    } catch(Exception& e) {
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        fprintf(stderr, "An exception has occurred: %s\n",
            e.getFullDescription().c_str());
#endif
    }
    return 0;
}
