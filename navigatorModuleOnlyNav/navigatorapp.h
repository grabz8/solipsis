#ifndef __NavigatorApp_h__
#define __NavigatorApp_h__

#include <vector>
#include <pthread.h>
#include "BasicThread.h"
#include "OgreApplication.h"
#include "Instance.h"

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

        /** See BasicThread. */
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
        /** See BasicThread. */
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

protected:
    void _initialize();
    void _finalize();
};

} // namespace Solipsis

#endif // #ifndef __NavigatorApp_h__
