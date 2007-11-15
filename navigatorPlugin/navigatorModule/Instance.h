#ifndef __NAVMODINSTANCE_H__
#define __NAVMODINSTANCE_H__

#include <pthread.h>
#include "IInstance.h"
#include "IWindow.h"
#include "Queue.h"
#include "Event.h"

#include "OgreApplication.h"
#include "OgreFrameListener.h"
#include "Peer.h"
#include "NavigatorXMLRPCClient.h"
#include "NodeEventListener.h"
#include "NavigatorGUI.h"
#include "Avatar.h"
#include "Scene.h"

#ifdef PHYSICS
#include "OgreOde_Core.h"
#endif

class OgreApplication;

/*#define NAVMODINSTANCE_TLS_NOKEY ((unsigned long)0xFFFFFFFF)
*/
namespace NavigatorModule {

class Instance : public IInstance
{
protected:
    static OgreApplication* ms_OgreApplication;

public:
    Instance();
    virtual ~Instance();

    /** These methods implement IInstance
    */
    virtual bool setWindow(IWindow* w);
    virtual IWindow* getIWindow() { return mIWindow; }

    virtual bool processEvent(const Event& evt);
    virtual bool run();
    virtual void requestTerminate();

/*    static void allocThreadLocalStorage()
    {
        pthread_key_create(ms_TlsKey, 0);
    }
    virtual void setThreadOwner(bool take)
    {
        pthread_once(&ms_TlsKeyOnce, allocThreadLocalStorage);
        if (pthread_setspecific(ms_TlsKey, take ? (void*)this : 0) != 0)
            assert(false);
    }
    static Instance* getInstance()
    {
        return static_cast<Instance*>(pthread_getspecific(ms_TlsKeyOnce));
    }
    static void setInstance(Instance* i)
    {
        if (pthread_setspecific(ms_TlsKey, instance)
            assert(false);
    }
    static IInstance* getIInstance()		
    { 
        return getInstance(); 
    }
    static void setIInstance(IInstance* instance)		
    { 
        return setInstance(dynamic_cast<Instance*>(instance));
    }
*/

    bool initialize();
    bool finalize();

    SceneManager* getSceneMgrPtr();
    RenderWindow* getRenderWindowPtr();
    Camera* getCameraPtr();

protected: 
    virtual bool initOgreCore();
    virtual bool initPostOgreCore();

    virtual void createSceneManager(); 
    virtual void createCamera(); 
    virtual void createViewports(); 
    virtual void createFrameListener(); 

    virtual void createScene() = 0; // pure virtual
    virtual void destroyScene(); 

    virtual bool createGUI();
    virtual void destroyGUI();

public:
    pthread_mutex_t mNaviMutex;

protected:
    // attached window
    IWindow* mIWindow;

	bool mReady;
    bool mTermRequested;

    Queue<Event> mEventQueue;
    pthread_mutex_t mMouseMutex;
    Event mLastMouseMovedEvent;
    bool mLastMouseMovedValid;

/*    // thread local storage (specific datas)
    static pthread_once_t ms_TlsKeyOnce;
    static pthread_key_t ms_TlsKey;*/

protected:
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

    bool handleEvent(const Event& evt);
    bool handleEvents();

    Real mTimeUntilNextToggle; 
    int mSceneDetailIndex ; 
    bool mStatsOn; 
    unsigned int mNumScreenShots; 
    TextureFilterOptions mFiltering; 
    int mAniso;
};

} // end namespace

#endif // __NAVMODINSTANCE_H__
