#include "ODEEngine.h"
#include "ODEScene.h"

using namespace Solipsis;

const String sPhysicsEngineName = "ODE engine";

//-------------------------------------------------------------------------------------
const String& ODEEngine::getName() const
{
    return sPhysicsEngineName;
}

//-------------------------------------------------------------------------------------
bool ODEEngine::init()
{
    return true;
}

//-------------------------------------------------------------------------------------
bool ODEEngine::shutdown()
{
   return true;
}

//-------------------------------------------------------------------------------------
IPhysicsScene* ODEEngine::createScene()
{
   return new ODEScene();
}

//-------------------------------------------------------------------------------------
void ODEEngine::destroyScene(IPhysicsScene* scene)
{
    assert(scene != 0);
    delete scene;
}

//-------------------------------------------------------------------------------------
