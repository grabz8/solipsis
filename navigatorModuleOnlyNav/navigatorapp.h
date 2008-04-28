#ifndef __NavigatorApp_h__
#define __NavigatorApp_h__

#include <vector>
#include <CTBasicThread.h>
#include "OgreApplication.h"
#include "Instance.h"
#include "VoiceEngineManager.h"

using CommonTools::BasicThread;

namespace Solipsis {

/** This class manages the Navigator application composed of several running instances.
 */
class NavigatorApp : public OgreApplication
{
    friend class IApplication;

protected:
    static NavigatorApp* ms_Singleton;

private:
    class MainThread : public BasicThread
    {
    private:
        Instance* mInstance;

    public:
        MainThread(const std::string& name, Instance* instance) :
          BasicThread(name),
          mInstance(instance) {}

        /** See CommonTools::BasicThread. */
        virtual void stop(unsigned int stopTimeoutSec = 5)
        {
            // call inherited
            BasicThread::stop();
            // Stop the instance
            if (mState == SRunning)
                mInstance->requestTerminate();
        }

        // Get/Set
        Instance* getInstance() const { return mInstance; }

    protected:
        /** See CommonTools::BasicThread. */
        virtual void run()
        {
            // run the render loop
            mInstance->run();
        }
    };

protected:
    String mAppPath;
    bool mStandAloneAutoCreateWindow;
    String mWindowTitle;
    bool mInitialized;
    unsigned int mNumInstances;
    std::vector<MainThread*> mThreads;
    Instance* mStandAloneInstance;
    VoiceEngineManager* mVoiceEngineManager;

protected: 
    NavigatorApp(const char* appPath, bool standAloneAutoCreateWindow, const char* windowTitle);
    virtual ~NavigatorApp();

public:
    /** See IApplication. */
    static IApplication* createApplication(const char* appPath, bool standAloneAutoCreateWindow, const char* windowTitle);
    /** See IApplication. */
    virtual bool destroy();
    /** See IApplication. */
    virtual IInstance* createInstance();
    /** See IApplication. */
    virtual bool destroyInstance(IInstance*);

    unsigned int getNumInstances() { return mNumInstances; }

    /** See OgreApplication. */
    virtual bool initialize(bool configManagedByOgre = false, String windowTitle = "");
    /** See OgreApplication. */
    virtual bool finalize();

protected:
    void _initialize();
    void _finalize();
};

} // namespace Solipsis

#endif // #ifndef __NavigatorApp_h__
