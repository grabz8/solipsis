#include "Prerequisites.h"
#include "OgreGraphicObjects/SoundIcon.h"

using namespace Ogre;
using namespace Solipsis;

class LightsFrameListener : public ExampleFrameListener
{
public:
	LightsFrameListener( RenderWindow* win, Camera* cam, SceneManager *sceneMgr )
		: ExampleFrameListener(win, cam, false, false)
	{
		// key and mouse state tracking
		mSpaceDown = false;
	//	mKeyboard->setBuffered(true); 
	} 

	virtual bool processUnbufferedKeyInput(const FrameEvent& evt)
	{
		if (!ExampleFrameListener::processUnbufferedKeyInput(evt))
		{
			return false;
		}

// 		if(mKeyboard->isKeyDown(OIS::KC_SPACE) )
// 		{
// 			if (!mSpaceDown)
// 			{
// 				mSpaceDown = true;
// 				switch (m_pIcon->getStatus())
// 				{
// 				case SoundIcon::Invisible:
// 					m_pIcon->setStatus(SoundIcon::Showed);
// 					break;
// 				case SoundIcon::Showed:
// 					m_pIcon->setStatus(SoundIcon::Animated);
// 					break;
// 				case SoundIcon::Animated:
// 					m_pIcon->setStatus(SoundIcon::Invisible);
// 					break;
// 				}
// 			}	
// 		}
// 		else
// 		{
// 			mSpaceDown = false;
// 		}

		return true;
	}


	virtual bool processUnbufferedMouseInput(const FrameEvent& evt)
	{
		if (!ExampleFrameListener::processUnbufferedMouseInput(evt))
		{
			return false;
		}

		return true;
	}

	bool frameStarted(const FrameEvent &evt)
	{
		//m_pIcon->animate(evt.timeSinceLastFrame);
		return true;
	}

public:
	bool mSpaceDown;       // Whether or not the left mouse button was down last frame

	SoundIcon * m_pIcon;
};

class LightsApplication : public ExampleApplication
{
protected:
public:
	LightsApplication()
	{

	}

	~LightsApplication() 
	{

	}
protected:
	virtual void createCamera(void)
	{
		// Create the camera
		mCamera = mSceneMgr->createCamera("PlayerCam");

		// Position it at 500 in Z direction
		mCamera->setPosition(Vector3(500,100,200));
		// Look back along -Z
		mCamera->lookAt(Vector3(0,0,-300));
		mCamera->setNearClipDistance(5);
	}


	void createScene(void)
	{
		mSceneMgr->setAmbientLight( ColourValue( 1, 1, 1 ) );
// 		Entity *ent1 = mSceneMgr->createEntity( "Robot", "robot.mesh" );
// 		ent1->setCastShadows(true);
// 		mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

// 		SceneNode *node1 = mSceneMgr->getRootSceneNode()->createChildSceneNode( "RobotNode" );
// 		node1->attachObject( ent1 );

// 		BillboardSet* pSet = mSceneMgr->createBillboardSet("myBillboardSet", 1);
// 		Billboard *pBoard = pSet->createBillboard(0,100,0);
// 		pSet->setMaterialName("Examples/Sound");
// 		pBoard->setDimensions(50,50);
// 		node1->attachObject(pSet);

//		mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", 5, 8);

// 		Plane plane(Vector3::UNIT_Y, 0);
// 		MeshManager::getSingleton().createPlane("ground",
// 			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
// 			1500,1500,20,20,true,1,5,5,Vector3::UNIT_Z);
// 		Entity  * ent = mSceneMgr->createEntity("GroundEntity", "ground");
// 		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
// 
// 		ent->setMaterialName("Examples/Rockwall");
// 		ent->setCastShadows(false);

// 		 SceneNode* pNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
// 		 pNode->setScale(100,100,100);
// 
// 		m_pIcon = new SoundIcon(mSceneMgr, pNode,"toto", 2);
// 		m_pIcon->setStatus(SoundIcon::Showed);
// 
// 		m_pIcon = new SoundIcon(mSceneMgr, pNode,"toto2", 4);
// 		m_pIcon->setStatus(SoundIcon::Invisible);

		ManualObject* myManualObject =  mSceneMgr->createManualObject("manual1"); 
		SceneNode* myManualObjectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("manual1_node"); 

		MaterialPtr myManualObjectMaterial = MaterialManager::getSingleton().create("manual1Material","debugger"); 
		myManualObjectMaterial->setReceiveShadows(false); 
		myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true); 
		myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0); 
		myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(0,0,1); 
		myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1); 

		myManualObject->begin("manual1Material", Ogre::RenderOperation::OT_LINE_LIST); 
		myManualObject->position(3, 2, 1); 
		myManualObject->position(4, 1, 0); 
		// etc 
		myManualObject->end(); 

		myManualObjectNode->attachObject(myManualObject);
	}

	void createFrameListener(void)
	{
		mFrameListener = new LightsFrameListener(mWindow, mCamera, mSceneMgr);
		mRoot->addFrameListener(mFrameListener);

		// Show the frame stats overlay
		mFrameListener->showDebugOverlay(true);
	//	ExampleApplication::createFrameListener();

// 		if (m_pIcon)
// 		{
// 			((LightsFrameListener*)mFrameListener)->m_pIcon = m_pIcon;
// 		}
	};

protected:
	LightsFrameListener * pFrameListener;
	SoundIcon * m_pIcon;
};

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char **argv)
#endif
{
	// Create application object
	LightsApplication app;

	try {
		app.go();
	} catch( Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
		MessageBox( NULL, e.what(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		fprintf(stderr, "An exception has occurred: %s\n",
			e.what());
#endif
	}

	return 0;
}
