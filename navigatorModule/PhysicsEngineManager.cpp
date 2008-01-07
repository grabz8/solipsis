#include "PhysicsEngineManager.h"

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
    mEngines[engine->getName()] = engine;
}

//-------------------------------------------------------------------------------------
void PhysicsEngineManager::removeEngine(IPhysicsEngine* engine)
{
    // Remove only if equal to registered one, since it might overridden
    // by other plugins
    EngineMap::iterator it = mEngines.find(engine->getName());
    if ((it != mEngines.end()) && (it->second == engine))
        mEngines.erase(it);
}

//-------------------------------------------------------------------------------------
PhysicsEngineManager::EngineMap& PhysicsEngineManager::getEngines()
{
    return mEngines;
}

//-------------------------------------------------------------------------------------
void PhysicsEngineManager::selectEngine(const String& name)
{
    mSelected = 0;
    EngineMap::iterator it = mEngines.find(name);
    if (it != mEngines.end())
        mSelected = it->second;
}

//-------------------------------------------------------------------------------------
IPhysicsEngine* PhysicsEngineManager::getSelectedEngine()
{
    return mSelected;
}

//-------------------------------------------------------------------------------------
