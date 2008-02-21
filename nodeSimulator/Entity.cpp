#include "Entity.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
Entity::Entity(XmlEntity* xmlEntity) :
    mXmlEntity(xmlEntity),
    mGravity(false)
#ifdef PHYSICSPLUGINS
    ,mPhysicsScene(0)
#endif
{
}

//-------------------------------------------------------------------------------------
Entity::~Entity()
{
#ifdef PHYSICSPLUGINS
    destroyPhysics();
#endif

    delete mXmlEntity;
}

//-------------------------------------------------------------------------------------
XmlEntity* Entity::getXmlEntity()
{
    return mXmlEntity;
}

//-------------------------------------------------------------------------------------
void Entity::setGravity(bool enabled)
{
    mGravity = enabled;
}

//-------------------------------------------------------------------------------------
bool Entity::isGravityEnabled()
{
    return mGravity;
}

#ifdef PHYSICSPLUGINS
//-------------------------------------------------------------------------------------
IPhysicsScene* Entity::getPhysicsScene()
{
    return mPhysicsScene;
}

//-------------------------------------------------------------------------------------
void Entity::createPhysics(IPhysicsScene* physicsScene)
{
    destroyPhysics();

    mPhysicsScene = physicsScene;
}

//-------------------------------------------------------------------------------------
void Entity::destroyPhysics()
{
    mPhysicsScene = 0;
}
#endif

//-------------------------------------------------------------------------------------

} // namespace Solipsis
