/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

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

#ifndef __Instance_h__
#define __Instance_h__

#include <pthread.h>
#include "IInstance.h"
#include "IWindow.h"
#include "Queue.h"
#include "Event.h"

#include "OgreApplication.h"
#include "OgreFrameListener.h"

class OgreApplication;

using namespace Ogre;

namespace Solipsis {

/*#define INSTANCE_TLS_NOKEY ((unsigned long)0xFFFFFFFF)
*/

/** This class manages a running instance of an Ogre application.
 */
class Instance : public IInstance
{
protected:
    String mName;
    // attached window
    IWindow* mIWindow;

    bool mAutoCreatedWindow;
    pthread_mutex_t mStartMutex;
    bool mTermRequested;

    Queue<Event> mEventQueue;
    pthread_mutex_t mMouseMutex;
    Event mLastMouseMovedEvent;
    bool mLastMouseMovedValid;

/*    // thread local storage (specific datas)
    static pthread_once_t ms_TlsKeyOnce;
    static pthread_key_t ms_TlsKey;*/

    OgreApplication* mOgreApplication;
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
    bool mNaviSupported;

    Real mTimeUntilNextToggle; 
    int mSceneDetailIndex ; 
    bool mStatsOn; 
    unsigned int mNumScreenShots; 
    TextureFilterOptions mFiltering; 
    int mAniso;

public:
    Instance(const String name, IApplication* application);
    virtual ~Instance();

    /** See IInstance. */
    virtual bool setWindow(IWindow* w);
    /** See IInstance. */
    virtual IWindow* getIWindow() { return mIWindow; }
    /** See IInstance. */
    virtual bool processEvent(const Event& evt);
    /** See IInstance. */
    virtual bool run();
    /** See IInstance. */
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

    OgreApplication* getOgreApplication() { return mOgreApplication; }
    SceneManager* getSceneMgrPtr();
    RenderWindow* getRenderWindowPtr();
    Camera* getCameraPtr();

    bool isNaviSupported() { return mNaviSupported; }

protected: 
    virtual bool initialize();
    virtual bool finalize();

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

private: 
    bool _setWindow();

    void registerFrameListener();
    void unregisterFrameListener();

    bool handleEvent(const Event& evt);
    bool handleEvents();
};

} // namespace Solipsis

#endif // #ifndef __Instance_h__
