/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __Application_h__
#define __Application_h__

#include <Ogre.h>
#include <OgreStringConverter.h>
#include <OgreException.h>
//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

using namespace Ogre;
//using namespace OIS;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <CoreFoundation/CoreFoundation.h>

// This function will locate the path to our application on OS X,
// unlike windows you can not rely on the curent working directory
// for locating your configuration files and resources.
std::string macBundlePath()
{
    char path[1024];
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    assert(mainBundle);

    CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
    assert(mainBundleURL);

    CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
    assert(cfStringRef);

    CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);

    CFRelease(mainBundleURL);
    CFRelease(cfStringRef);

    return std::string(path);
}
#endif


// Include Solipsis standard utilities
#include "SolipsisErrorHandler.h"

//-------------------------------------------------------------------------------------
/// brief This is the framework for an OGRE application.
/// file Application.h
/// author Gilles GAUDIN
/// date yyyy.mm.dd
/// sa FrameListener
/// sa WindowEventListener
/// sa OIS
class Application : 
	public FrameListener, 
	public WindowEventListener,
	public OIS::MouseListener, public OIS::KeyListener
{
public:
	/// brief Constructor
	Application();
	/// brief Destructor
	virtual ~Application();

	/// brief Setup and launch the application
	virtual void go();
	/// Tell if we have to reset the config on exit
	bool mustResetConfig() {return mMustDeleteConfigFile;};

protected:
	/// brief Setup the application
	virtual bool setup();
	/// brief Open the configuration dialog box
	virtual bool configure();

	/// brief Create the scene manager
	virtual void createSceneManager();
	/// brief Create a camera
	virtual void createCamera();
	/// brief Create the viewports
	virtual void createViewports();
	/// brief Create the resource listener
	virtual void createResourceListener();
	/// brief Create the frame listener
	virtual void createFrameListener();

	/// brief Initialize the resources
	virtual void initResources();

	/// brief Create the scene 
	/// details This is a pure virtual, override it!
	virtual void createScene() = 0;
	/// brief Destroy the scene
	virtual void destroyScene();

	/// brief Frame started listener
	/// param evt A frame event callback
	virtual bool frameStarted(const FrameEvent& evt); 
	/// brief Frame ended listener
	/// param evt A frame event callback
	virtual bool frameEnded(const FrameEvent& evt);

	/// brief Mouse displacement listener
	/// param e A Mouse event callback
	virtual bool mouseMoved( const OIS::MouseEvent &e );
	/// brief Mouse button pressed listener
	/// param e A Mouse event callback
	/// param id The type of action from a mouse button event
	virtual bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
	/// brief Mouse button release listener
	/// param e A Mouse event callback
	/// param id The type of action from a mouse button event
	virtual bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );
	/// brief Key pressed listener
	/// param e A Keyboard event callback
	virtual bool keyPressed( const OIS::KeyEvent &e );
	/// brief Key released listener
	/// param e A Keyboard event callback
	virtual bool keyReleased( const OIS::KeyEvent &e );

private:
	/// brief ...
	bool construct(const FrameEvent& evt);			
	/// brief ...
	void updateStats(void);							
	/// brief Adjust mouse clipping area				
	virtual void windowResized(RenderWindow* rw);	
	/// brief Unattach OIS before window shutdown (very important under Linux)
	virtual void windowClosed(RenderWindow* rw);	

protected:
	/// brief Allow to display or not some debug informations
	void showDebugOverlay(bool show);				


protected:
	Root *mRoot;									/// brief The root scene
	Camera* mCamera;								/// brief The current camera
	SceneManager* mSceneMgr;						/// brief The current scene manager
//	FrameListener* mFrameListener;					///
	RenderWindow* mWindow;							/// brief The current rendering window
	Ogre::String mResourcePath;						/// brief The resource path
	Ogre::String mDebugText;						/// brief Some debug inforations that can be display
	Real mRotate;									/// brief The rotate constant
	Real mMove;										/// brief The movement constant
	Vector3 mDirection;								/// brief Value to move in the correct direction

	SceneNode *mCamNode;							/// brief The SceneNode the camera is currently attached to

	bool mContinue;									/// brief Whether to continue rendering or not




	enum DirectionCodes
	{
		X = 0,
		Y = 1,
		Z = 2
	};

private:
	// ????????????????????? MA attributes BEGIN ?????????????????????
	Vector3 mTranslateVector;						/// brief ...
	bool mStatsOn;									/// brief ...

	unsigned int mNumScreenShots;					/// brief ...
	float mMoveScale;								/// brief ...
	Degree mRotScale;								/// brief ...
	// just to stop toggles flipping too fast
	Real mTimeUntilNextToggle ;						/// brief ...
	Radian mRotX, mRotY;							/// brief ...
	TextureFilterOptions mFiltering;				/// brief ...
	int mAniso;										/// brief ...

	int mSceneDetailIndex ;							/// brief ...

	Real mMoveSpeed;								/// brief ...
	Degree mRotateSpeed;							/// brief ...
	Overlay* mDebugOverlay;							/// brief ...
	// ????????????????????? MA attributes END ?????????????????????

protected:
	//OIS Input devices
	OIS::InputManager* mInputManager;				/// brief ... used by the frameListener
	OIS::Mouse*    mMouse;							/// brief ... used by the frameListener
	OIS::Keyboard* mKeyboard;						/// brief ... used by the frameListener
	OIS::JoyStick* mJoy;							/// brief ... used by the frameListener

	//Custom part for Solipsis

	/// Should we delete the config while when exiting the app ?
	bool		mMustDeleteConfigFile;
};

#endif //__Application_h__
