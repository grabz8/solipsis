#include "Avatar.h"

using namespace Ogre;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Avatar::Avatar(XmlEntity* xmlEntity) :
    Entity(xmlEntity)
#ifdef PHYSICSPLUGINS
    ,mPhysicsCharacter(0)
#endif
{
}

//-------------------------------------------------------------------------------------
Avatar::~Avatar()
{
}

#ifdef PHYSICSPLUGINS
//-------------------------------------------------------------------------------------
void Avatar::createPhysics(IPhysicsScene* physicsScene)
{
    Entity::createPhysics(physicsScene);

    // Compute radius and height of character
    Vector3 aabbHalfSize = mXmlEntity->getAABoundingBox().getHalfSize();
    mRadius = std::min(aabbHalfSize.x, aabbHalfSize.z);
    mHeight = aabbHalfSize.y*2;

    IPhysicsCharacter::Desc characterDesc;
    characterDesc.position = mXmlEntity->getPosition();
    characterDesc.radius = mRadius;
    characterDesc.height = mHeight;
    characterDesc.stepOffset = mRadius;
    mPhysicsCharacter = mPhysicsScene->createCharacter();
    mPhysicsCharacter->create(characterDesc);

    mDirty = false;
}

//-------------------------------------------------------------------------------------
void Avatar::destroyPhysics()
{
    if ((mPhysicsScene != 0) && (mPhysicsCharacter != 0))
        mPhysicsScene->destroyCharacter(mPhysicsCharacter);

    mPhysicsCharacter = 0;

    Entity::destroyPhysics();
}
#endif

//-------------------------------------------------------------------------------------
bool Avatar::update(Real timeSinceLastFrame)
{
#ifdef PHYSICSPLUGINS
    // Move physics character
    if (mPhysicsCharacter != 0)
    {
        Vector3 displacement = mXmlEntity->getDisplacement()*timeSinceLastFrame;
        if (mGravity)
            displacement.y += -9.80665f*timeSinceLastFrame;
        mPhysicsCharacter->move(displacement);
        Vector3 newPosition;
        mPhysicsCharacter->getPosition(newPosition);
        if ((newPosition - mXmlEntity->getPosition()).squaredLength() > 0.0001f)
            mDirty = true;
        mXmlEntity->setPosition(newPosition);
    }
#endif

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
