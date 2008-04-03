#include "PhysXEngine.h"
#include "PhysXScene.h"
#include "PhysXHelpers.h"

using namespace Solipsis;

const String sPhysicsEngineName = "PhysX engine";

//-------------------------------------------------------------------------------------
const String& PhysXEngine::getName() const
{
    return sPhysicsEngineName;
}

//-------------------------------------------------------------------------------------
bool PhysXEngine::init()
{
    return PhysXHelpers::init();
}

//-------------------------------------------------------------------------------------
bool PhysXEngine::shutdown()
{
   return PhysXHelpers::shutdown();
}

//-------------------------------------------------------------------------------------
IPhysicsScene* PhysXEngine::createScene()
{
   return new PhysXScene(this);
}

//-------------------------------------------------------------------------------------
void PhysXEngine::destroyScene(IPhysicsScene* scene)
{
    assert(scene != 0);
    delete scene;
}

//-------------------------------------------------------------------------------------
