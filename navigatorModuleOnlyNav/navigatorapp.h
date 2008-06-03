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
