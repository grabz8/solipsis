#include "TokamakEngine.h"
#include "TokamakScene.h"
#include "TokamakHelpers.h"

using namespace Solipsis;

const String sPhysicsEngineName = "Tokamak engine";

//-------------------------------------------------------------------------------------
const String& TokamakEngine::getName() const
{
    return sPhysicsEngineName;
}

//-------------------------------------------------------------------------------------
bool TokamakEngine::init()
{
    return true;
}

//-------------------------------------------------------------------------------------
bool TokamakEngine::shutdown()
{
   return true;
}

//-------------------------------------------------------------------------------------
IPhysicsScene* TokamakEngine::createScene()
{
   return new TokamakScene();
}

//-------------------------------------------------------------------------------------
void TokamakEngine::destroyScene(IPhysicsScene* scene)
{
    assert(scene != 0);
    delete scene;
}

//-------------------------------------------------------------------------------------
