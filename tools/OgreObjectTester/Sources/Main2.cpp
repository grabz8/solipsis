#include "Prerequisites.h"
#include "ExampleApplication.h"

#include "OgreGraphicObjects/ProgressBar.h"
#include "OgreGraphicObjects/MovableText.h"
#include "OgreGraphicObjects/SoundIcon.h"
#include "OgreGraphicObjects/MovableBox.h"

using namespace Solipsis;
class RTTListener : public ExampleFrameListener
{
public:
	RTTListener(RenderWindow *win, Camera *cam, ProgressBarWithText * pBar, MovableBox * pBox) 
		: ExampleFrameListener(win, cam)
	{
        mBar = pBar;
        mBox = pBox;
		time = 0;
	} 

	bool frameStarted(const FrameEvent& evt)
	{
		time += evt.timeSinceLastFrame;
		if (mBar)
            mBar->setProgress(Math::Sin(time)/2 +0.5);

        if (mBox)
            mBox->animate(evt.timeSinceLastFrame);

		return ExampleFrameListener::frameStarted(evt);
	}

protected:

	Real time;
	ProgressBarWithText * mBar;
    MovableBox * mBox;
};

class RTTApplication : public ExampleApplication, public RenderTargetListener
{
protected:
	SceneNode		*mPlaneNode;
	Rectangle2D		*mMiniScreen;

	void createScene()
	{
		// Set ambient light
	//	mSceneMgr->setAmbientLight(ColourValue(0.2f, 0.2f, 0.2f));

		mSceneMgr->setSkyBox(true,"Examples/StormySkyBox");

		// Position the camera
		mCamera->setNearClipDistance(0.1);
		mCamera->setPosition(0, 0, -200);
		mCamera->lookAt(0, 0, 0);

		SceneNode* myManualObjectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("manual1_node"); 

		// Name Label
//		MovableText* msg = new MovableText("TXT_001", "this is the caption");
//		msg->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE); // Center horizontally and display above the node
//		msg->setAdditionalHeight( 2.0f ); //msg->setAdditionalHeight( ei.getRadius() )

		//myManualObjectNode->attachObject(msg);

	//	myManualObjectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("manual2_node"); 
 		pBar = new ProgressBarWithText("myfuckinbar", "DTC : ");
        pBar->setBarSize(100,10);
// 		pBar->attach(myManualObjectNode);
// 		pBar->setFont("BerlinSans32", 5, ColourValue::White);
// 		pBar->setTxtPosition(0);

    //    myManualObjectNode->setScale(100,100,100);

    //    SoundIcon * pIcon =  new SoundIcon(mSceneMgr, myManualObjectNode, "icone", 0);
    //    pIcon->setStatus(SoundIcon::Showed);

		//myManualObjectNode->setScale(0.5,0.5,0.5);

	//	SoundIcon * pIcon = new SoundIcon(mSceneMgr, myManualObjectNode, "narden");


        pBox = new MovableBox("narden", Vector3(100,100,100), true);
        pBox->setRotateSpeed(0.5);

        myManualObjectNode->attachObject(pBox);	

	}

	// Create a new frame listener
	void createFrameListener()
	{
		mFrameListener = new RTTListener(mWindow, mCamera, pBar, pBox);
		mRoot->addFrameListener(mFrameListener);
	}

	void preRenderTargetUpdate(const RenderTargetEvent &evt)
	{
		mMiniScreen->setVisible(false);
	}

	void postRenderTargetUpdate(const RenderTargetEvent &evt)
	{
		mMiniScreen->setVisible(true);
	}

    ProgressBarWithText * pBar;
    MovableBox * pBox;
};

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
	int main(int argc, char **argv)
#endif
	{
		// Create application object
		RTTApplication app;

		try {
			app.go();
		} catch(Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBoxA(NULL, e.getFullDescription().c_str(),
				"An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "An exception has occurred: " << e.getFullDescription();
#endif
		}

		return 0;
	}

#ifdef __cplusplus
}
#endif
