#ifndef __OgreApplication_h__
#define __OgreApplication_h__

#include "Ogre.h"
#include "OgreFrameListener.h"

using namespace Ogre;

class OgreApplication
{
public:
    OgreApplication::OgreApplication(const char* title = 0);
    virtual ~OgreApplication();

    virtual void go(); 

    SceneManager* getSceneMgrPtr();
    RenderWindow* getRenderWindowPtr();
    Camera* getCameraPtr();

protected: 
    virtual bool initialise();
    virtual bool shutdown();

    virtual bool initOgreCore();

    virtual void createSceneManager();
    virtual void createCamera();
    virtual void createViewports();
    virtual void createResourceListener();
    virtual void createFrameListener();

    virtual void createScene() = 0; // pure virtual
    virtual void destroyScene();

    virtual bool createGUI();
    virtual void destroyGUI();

    virtual void addResourceLocations();
    virtual void initResources();

    Ogre::String mWindowTitle;
    OgreFrameListener* mFrameListener;
    SceneManager* mSceneMgr;
    RenderWindow* mWindow;
    Camera* mCamera;
    Real mMoveSpeed;
    Degree mRotateSpeed;
    Vector3 mTranslateVector;
    float mMoveScale;
    Degree mRotScale;
    Radian mRotX, mRotY;

private: 

    void registerFrameListener();

    Real mTimeUntilNextToggle;
    int mSceneDetailIndex; 
    bool mStatsOn;
    unsigned int mNumScreenShots;
    TextureFilterOptions mFiltering;
    int mAniso;
};

#endif // #ifndef __OgreApplication_h__