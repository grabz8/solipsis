#include "PhysicsEngineManager.h"

using namespace Ogre;
using namespace Solipsis;

template<> PhysicsEngineManager* Singleton<PhysicsEngineManager>::ms_Singleton = 0;

//-------------------------------------------------------------------------------------
PhysicsEngineManager* PhysicsEngineManager::getSingletonPtr(void)
{
    return ms_Singleton;
}

//-------------------------------------------------------------------------------------
PhysicsEngineManager& PhysicsEngineManager::getSingleton(void)
{
    assert(ms_Singleton);
    return (*ms_Singleton);
}

//-------------------------------------------------------------------------------------
PhysicsEngineManager::PhysicsEngineManager() :
    mSelected(0)
{
}

//-------------------------------------------------------------------------------------
PhysicsEngineManager::~PhysicsEngineManager()
{
}

//-------------------------------------------------------------------------------------
void PhysicsEngineManager::addEngine(IPhysicsEngine* engine)
{
    mEngines.push_back(engine);
}

//-------------------------------------------------------------------------------------
void PhysicsEngineManager::removeEngine(IPhysicsEngine* engine)
{
    // Remove only if equal to registered one, since it might overridden
    // by other plugins
    for (EngineList::iterator it=mEngines.begin(); it != mEngines.end(); ++it)
        if ((*it) == engine)
        {
            mEngines.erase(it);
            break;
        }
}

//-------------------------------------------------------------------------------------
PhysicsEngineManager::EngineList& PhysicsEngineManager::getEngines()
{
    return mEngines;
}

//-------------------------------------------------------------------------------------
void PhysicsEngineManager::selectEngine(const String& name)
{
    mSelected = 0;
    for (EngineList::iterator it=mEngines.begin(); it != mEngines.end(); ++it)
        if ((*it)->getName().compare(name) == 0)
        {
            mSelected = (*it);
            break;
        }
}

//-------------------------------------------------------------------------------------
IPhysicsEngine* PhysicsEngineManager::getSelectedEngine()
{
    return mSelected;
}

//-------------------------------------------------------------------------------------
