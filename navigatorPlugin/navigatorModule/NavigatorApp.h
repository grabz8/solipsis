#ifndef __NAVMODNAVIGATORAPP_H__
#define __NAVMODNAVIGATORAPP_H__

#include <vector>
#include <pthread.h>
#include "INavigatorApp.h"

namespace NavigatorModule {

class NavigatorApp : public INavigatorApp
{
    friend class INavigatorApp;

public:
    /** These methods implement IInstance
    */
    static INavigatorApp* createNavigatorApp(const char* appPath);
    virtual bool destroy();
    virtual IInstance* createInstance();
    virtual bool destroyInstance(IInstance*);

private:
    class MainThread {
    public:
        enum State {
            SInit,      // Listener is initialized
            SRunning,   // Listener is running
            SStopped    // Listener is stopped
        };

        MainThread(Instance* instance);
        virtual ~MainThread();

        void start();
        void run();
        void stop(unsigned int timeoutSec = 5);

        Instance* getInstance() const { return mInstance; }

    private:
        static void *start_routine(void* args);

    private:
        Instance* mInstance;
        pthread_t mThread;
        State mState;
        bool mStop;
    };

protected:
    NavigatorApp();
    virtual ~NavigatorApp();

    void _initialize(const char* appPath);
    void _finalize();

    static NavigatorApp* ms_Singleton;

    std::vector<MainThread*> mThreads;
};

} // end namespace

#endif // __NAVMODNAVIGATORAPP_H__
