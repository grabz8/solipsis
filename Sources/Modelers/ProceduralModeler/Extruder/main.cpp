#include "ExampleApplication.h"
#include "gle.h"
#include "OgreExtrusion.h"

#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

class TutorialApplication : public ExampleApplication
{
protected:
public:
    TutorialApplication()
    {
    }

    ~TutorialApplication()
    {
    }
protected:
    virtual void createCamera(void)
    {
    	//We creayte the camera, supplying its name, its position, the point it's looking at
    	//and the minimum distance too see an entity
        mCamera = mSceneMgr->createCamera("PlayerCam");
        mCamera->setPosition(Vector3(0,10,500));
        mCamera->lookAt(Vector3(0,0,0));
        mCamera->setNearClipDistance(5);
    }

    virtual void createViewports(void)
    {
    	//we create the viewport to attach the camera
        Viewport* vp = mWindow->addViewport(mCamera);
        vp->setBackgroundColour(ColourValue(0,0,0));
        mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
        
        //we don't show the ogre information window
        vp->setOverlaysEnabled(false);
    }

    void createScene(void)
    {

    	//We set the ambient light and a point light in order to see the normals of the created volume 
        mSceneMgr->setAmbientLight(ColourValue(0.8f,0.8f,0.8f));
        Light *light = mSceneMgr->createLight("Light1");
        light->setType(Light::LT_POINT);
        light->setPosition(Vector3(0, 150, 0));
        light->setDiffuseColour(1.0, 1.0, 1.0);
        light->setSpecularColour(1.0, 1.0, 1.0);

        //createRoadMesh();
		createTubeMesh();
    }

	virtual void createRoadMesh(){
		//////////////////////////////////////////////////////////////
        // Creating our own mesh for an extrusion
        //////////////////////////////////////////////////////////////
        // Raw data

		//we define the contour we want to be extruded
		//number of contour points
		unsigned int ncp = 2;

		//the contour itself
        unsigned int i;
		gleDouble** contour = (gleDouble**)malloc(ncp*sizeof(gleDouble));
		for(i = ncp-1 ; i > 0 ; --i){
			contour[i] = (gleDouble*)malloc(2*sizeof(gleDouble));
		}
		contour[0][0] = -1;
		contour[0][1] = 0;
		contour[1][0] = 1;
		contour[1][1] = 0;
		
		//the normals of the contour
		gleDouble** norm_contour = (gleDouble**)malloc(ncp*sizeof(gleDouble));
		for(i = ncp-1 ; i > 0 ; --i){
			norm_contour[i] = (gleDouble*)malloc(2*sizeof(gleDouble));
		}
		norm_contour[0][0] = -1;
		norm_contour[0][1] = 0;
		norm_contour[1][0] = 1;
		norm_contour[1][1] = 0;
		
		//the up vector for contour
		gleDouble up_vector[3] = {0,1,0};


		//definition of the extrusion path
		//the number of points in the path
		unsigned int npp = 20;

		//don't forget the first and last segments won't be drawn.
		gleDouble** path = (gleDouble**)malloc(npp*sizeof(gleDouble));
		for(i = npp-1 ; i > 0 ; --i){
			path[i] = (gleDouble*)malloc(3*sizeof(gleDouble));
            path[i][0] = 2*(i-1);
            path[i][1] = 1;//sin(i);
            path[i][2] = 0;
		}

		/*
		Rotation :
		|  cos     sin    0   |
        |  -sin    cos    0   |
        using radians
        */
		//definition of the transformation to use along the path
		gleDouble*** xform = (gleDouble***)malloc(npp*sizeof(gleDouble));
		for (i = npp-1 ; i > 0 ; --i)
		{
			xform[i] = (gleDouble**)malloc(2*sizeof(gleDouble));
			xform[i][0] = (gleDouble*)malloc(3*sizeof(gleDouble));
			xform[i][1] = (gleDouble*)malloc(3*sizeof(gleDouble));
			xform[i][0][0] = cos(i*0.5);
			xform[i][0][1] = sin(i*0.5);
			xform[i][0][2] = 0;
			xform[i][1][0] = -sin(i*0.5);
			xform[i][1][1] = cos(i*0.5);
			xform[i][1][2] = 0;
		}

		//creation of the extruder object
		OgreExtrusion extruder;

		//declaration of the mesh name
		char* meshName = "Road";

		//extrusion with the informations we defined just before
		extruder.extrude(meshName, ncp, contour, norm_contour, up_vector, npp, path, NULL);
		
		//we free the arrays used for the extrusion
		for(i = npp-1 ; i > 0 ; --i){
			free(path[i]);
			free(xform[i][0]);
			free(xform[i][1]);
			free(xform[i]);
		}
		
		free(path);
		free(xform);

		//now we create the mesh using the informations given by the extrusion
		extruder.createMesh(meshName);

		char* materialName = meshName;
		//we create, bind and export the associated (default) material
		extruder.createMaterial(materialName);
		extruder.bindMaterialToMesh(meshName, materialName);
		extruder.exportMaterial(materialName);
        
        // Create an entity with the mesh
        Entity *entity = mSceneMgr->createEntity(meshName,meshName);
        SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(meshName);
        node->attachObject(entity);

		//finally export the mesh for later reuse
		extruder.exportMesh(meshName);
	}

	virtual void createTubeMesh(){
		//////////////////////////////////////////////////////////////
        // Creating our own mesh for an extrusion
        //////////////////////////////////////////////////////////////
        // Raw data

		//we define the contour we want to be extruded
		//number of contour points
		unsigned int ncp = 4;

		//the contour itself
        unsigned int i;
		gleDouble** contour = (gleDouble**)malloc(ncp*sizeof(gleDouble));
		for(i = ncp-1 ; i > 0 ; --i){
			contour[i] = (gleDouble*)malloc(2*sizeof(gleDouble));
		}
		contour[0][0] = -1;
		contour[0][1] = 0;
		contour[1][0] = 1;
		contour[1][1] = 0;
		contour[2][0] = 1;
		contour[2][1] = 1;
		contour[3][0] = -1;
		contour[3][1] = 1;
		
		//the normals of the contour
		gleDouble** norm_contour = (gleDouble**)malloc(ncp*sizeof(gleDouble));
		for(i = ncp-1 ; i > 0 ; --i){
			norm_contour[i] = (gleDouble*)malloc(2*sizeof(gleDouble));
		}
		norm_contour[0][0] = -0.707;
		norm_contour[0][1] = -0.707;
		norm_contour[1][0] = 0.707;
		norm_contour[1][1] = -0.707;
		norm_contour[2][0] = 0.707;
		norm_contour[2][1] = 0.707;
		norm_contour[3][0] = -0.707;
		norm_contour[3][1] = 0.707;
		
		//the up vector for contour
		gleDouble up_vector[3] = {1,0,0};


		//definition of the extrusion path
		//the number of points in the path
		unsigned int npp = 6;

		//don't forget the first and last segments won't be drawn.
		gleDouble** path = (gleDouble**)malloc(npp*sizeof(gleDouble));
		for(i = npp-1 ; i > 0 ; --i){
			path[i] = (gleDouble*)malloc(3*sizeof(gleDouble));
            /*path[i][0] = 4;
            path[i][1] = (i-1)*(i-1);//sin(i);
            path[i][2] = i-1;*/
		}
		path[0][0] = -1;
		path[0][1] = 0;
		path[0][2] = 0;
		path[1][0] = 0;
		path[1][1] = 0;
		path[1][2] = 0;
		path[2][0] = 10;
		path[2][1] = 0;
		path[2][2] = 0;
		path[3][0] = 10;
		path[3][1] = 10;
		path[3][2] = 0;
		path[4][0] = 0;
		path[4][1] = 10;
		path[4][2] = 0;
		path[5][0] = -1;
		path[5][1] = 0;
		path[5][2] = 10;

		/*
		Rotation :
		|  cos     sin    0   |
        |  -sin    cos    0   |
        using radians
        */
		//definition of the transformation to use along the path
		gleDouble*** xform = (gleDouble***)malloc(npp*sizeof(gleDouble));
		for (i = npp-1 ; i > 0 ; --i)
		{
			xform[i] = (gleDouble**)malloc(2*sizeof(gleDouble));
			xform[i][0] = (gleDouble*)malloc(3*sizeof(gleDouble));
			xform[i][1] = (gleDouble*)malloc(3*sizeof(gleDouble));
			xform[i][0][0] = cos(i*0.5);
			xform[i][0][1] = sin(i*0.5);
			xform[i][0][2] = 0;
			xform[i][1][0] = -sin(i*0.5);
			xform[i][1][1] = cos(i*0.5);
			xform[i][1][2] = 0;
		}

		//creation of the extruder object
		OgreExtrusion extruder;

		//declaration of the mesh name
		char* meshName = "Tube";

		//extrusion with the informations we defined just before
		extruder.extrude(meshName, ncp, contour, norm_contour, up_vector, npp, path, NULL);
		
		//we free the arrays used for the extrusion
		for(i = npp-1 ; i > 0 ; --i){
			free(path[i]);
			free(xform[i][0]);
			free(xform[i][1]);
			free(xform[i]);
		}
		
		free(path);
		free(xform);

		//now we create the mesh using the informations given by the extrusion
		extruder.createMesh(meshName);

		char* materialName = meshName;
		//we create, bind and export the associated (default) material
		extruder.createMaterial(materialName);
		extruder.bindMaterialToMesh(meshName, materialName);
		extruder.exportMaterial(materialName);
        
        // Create an entity with the mesh
        Entity *entity = mSceneMgr->createEntity(meshName,meshName);
        SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(meshName);
        node->attachObject(entity);

		//finally export the mesh for later reuse
		extruder.exportMesh(meshName);
	}
};

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

//INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
int main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
    // Create application object
    TutorialApplication app;

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
