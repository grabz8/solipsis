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

#include "NavigatorApp.h"
#include "Navigator.h"
#include "Platform.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
// this is a simple C stub so that dll can be loaded dynamically
// and the static method createApplication can be called
extern "C" NAVIGATORMODULEDIRECT_EXPORT IApplication* createApplication(const char* appPath, bool standAloneAutoCreateWindow, const char* windowTitle)
{
	return (IApplication*)IApplication::createApplication(appPath, standAloneAutoCreateWindow, windowTitle);
}

//-------------------------------------------------------------------------------------
// this is a simple C stub so that dll can be loaded dynamically
// and the static method getApplication can be called
extern "C" NAVIGATORMODULEDIRECT_EXPORT IApplication* getNavigatorApp()
{
    return (IApplication*)IApplication::getApplication();
}

NavigatorApp* NavigatorApp::ms_Singleton = 0;

//-------------------------------------------------------------------------------------
IApplication* IApplication::createApplication(const char* appPath, bool standAloneAutoCreateWindow, const char* windowTitle)
{
    if (NavigatorApp::ms_Singleton == 0)
    {
        NavigatorApp::ms_Singleton = new NavigatorApp(appPath, standAloneAutoCreateWindow, windowTitle);
        assert(NavigatorApp::ms_Singleton);
        NavigatorApp::ms_Singleton->_initialize();
    }

    assert(NavigatorApp::ms_Singleton != 0);

    return NavigatorApp::ms_Singleton;
}	

//-------------------------------------------------------------------------------------
IApplication* IApplication::getApplication()
{
    return NavigatorApp::ms_Singleton;
}

//-------------------------------------------------------------------------------------
NavigatorApp::NavigatorApp(const char* appPath, bool standAloneAutoCreateWindow, const char* windowTitle) :
    mStandAloneAutoCreateWindow(standAloneAutoCreateWindow),
    mInitialized(false),
    mNumInstances(0)
{
    assert(NavigatorApp::ms_Singleton == 0);
    if (appPath != 0)
        mAppPath = appPath;
    if (windowTitle != 0)
        mWindowTitle = windowTitle;

    mPhysicsEngineManager = new PhysicsEngineManager();
}

//-------------------------------------------------------------------------------------
NavigatorApp::~NavigatorApp()
{
    assert(NavigatorApp::ms_Singleton == this);
    NavigatorApp::ms_Singleton = 0;

    delete mPhysicsEngineManager;
}

//-------------------------------------------------------------------------------------
bool NavigatorApp::destroy()
{
    _finalize();

    // If there are still alive instances, cannot delete app
    if (mThreads.size() > 0)
        return false;

    delete this;

    return true;
}

//-------------------------------------------------------------------------------------
IInstance* NavigatorApp::createInstance()
{
    if (mStandAloneAutoCreateWindow && (mNumInstances != 0))
        return 0;

    // create the main thread
    std::string instanceName;
    instanceName = "Navigator" + StringConverter::toString(mNumInstances - 1);
    Instance* instance = new Navigator(instanceName, this);
    mNumInstances++;
    if (!mStandAloneAutoCreateWindow)
    {
        MainThread* mainThread = new MainThread(instanceName, instance);
        mainThread->start();
        mThreads.push_back(mainThread);
    }
    else
        mStandAloneInstance = instance;

    return instance;
}

//-------------------------------------------------------------------------------------
bool NavigatorApp::destroyInstance(IInstance* instance)
{
    if (instance == 0)
        return false;
    if (mStandAloneAutoCreateWindow)
        if ((mStandAloneInstance == 0) || (instance != mStandAloneInstance))
            return false;

    assert(mNumInstances > 0);
    mNumInstances--;

    // find instance
    MainThread* mainThread = 0;
    for (unsigned int i = 0; i < mThreads.size(); ++i)
    {
        mainThread = mThreads[i];
        if ((mainThread != 0) && (mainThread->getInstance() == instance))
        {
            mThreads.erase(mThreads.begin() + i, mThreads.begin() + i + 1);
            break;
        }
    }

    if (mainThread != 0)
    {
        mainThread->stop();
        mainThread->finalize();
        delete (Navigator*)instance;
        delete mainThread;
    }
    else
        delete mStandAloneInstance;

    return true;
}

//-------------------------------------------------------------------------------------
void NavigatorApp::_initialize()
{
    assert(mThreads.size() == 0);
    mThreads.clear();
    initialize(mStandAloneAutoCreateWindow, mWindowTitle);

#ifdef PHYSICSPLUGINS
//    PhysicsEngineManager::getSingleton().selectEngine("PhysX engine");
    PhysicsEngineManager::getSingleton().selectEngine("ODE engine");
//    PhysicsEngineManager::getSingleton().selectEngine("Tokamak engine");
    PhysicsEngineManager::getSingleton().getSelectedEngine()->init();
#endif

    mInitialized = true;
}

//-------------------------------------------------------------------------------------
void NavigatorApp::_finalize()
{
    assert(mThreads.size() == 0);

#ifdef PHYSICSPLUGINS
    PhysicsEngineManager::getSingleton().getSelectedEngine()->shutdown();
#endif

    finalize();
    mInitialized = false;
}

//-------------------------------------------------------------------------------------
