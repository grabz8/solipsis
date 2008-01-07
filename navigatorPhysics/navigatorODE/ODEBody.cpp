#include "ODEBody.h"
#include "ODEScene.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
ODEBody::ODEBody(ODEScene* scene) :
    mScene(scene),
    mBody(0)
{
}

//-------------------------------------------------------------------------------------
ODEBody::~ODEBody()
{
    if (mBody != 0)
    {
        SceneNode* parent = mBody->getParentSceneNode();
        assert(parent != 0);
        parent->detachObject(mBody);
        for (int g = 0; g < (int)mBody->getGeometryCount(); ++g)
        {
            OgreOde::Geometry* geom = mBody->getGeometry(g);
            mBody->removeGeometry(geom);
            delete geom;
        }
        delete mBody;
    }
}

//-------------------------------------------------------------------------------------
void ODEBody::createBox(SceneNode* node, const Vector3& extents)
{
    assert(mBody == 0);
    assert(mScene != 0);

    OgreOde::World* world = mScene->getWorld();

    mBody = new OgreOde::Body(world, node->getName() + "_body");
    mBody->setMass(OgreOde::BoxMass(1.0f, extents));
    mBody->setUserData(1);
    node->attachObject(mBody);
    OgreOde::BoxGeometry* boxGeom = new OgreOde::BoxGeometry(extents, world, world->getDefaultSpace());
    boxGeom->setBody(mBody);
}

//-------------------------------------------------------------------------------------
void ODEBody::setPosition(const Vector3& position)
{
    mBody->setPosition(position);
    mBody->wake();
}

//-------------------------------------------------------------------------------------
void ODEBody::setLinearVelocity(const Vector3& velocity)
{
    mBody->setLinearVelocity(velocity);
    mBody->wake();
}

//-------------------------------------------------------------------------------------
void ODEBody::setAngularVelocity(const Vector3& velocity)
{
    mBody->setAngularVelocity(velocity);
    mBody->wake();
}

//-------------------------------------------------------------------------------------
