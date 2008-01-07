#include "TokamakBody.h"
#include "TokamakScene.h"
#include "TokamakHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
TokamakBody::TokamakBody(TokamakScene* scene) :
    mScene(scene),
    mNode(0),
    mBody(0)
{
}

//-------------------------------------------------------------------------------------
TokamakBody::~TokamakBody()
{
    if (mBody != 0)
    {
        assert(mNode != 0);
        TokamakScene::BodyMap::iterator it = mScene->mPhysicsBodies.find(mNode->getName());
        if ((it != mScene->mPhysicsBodies.end()) && (it->second == mBody))
            mScene->mPhysicsBodies.erase(it);

        mBody->BeginIterateGeometry();
        neGeometry* geom = mBody->GetNextGeometry();
        while (geom != 0)
        {
            mBody->RemoveGeometry(geom);
            geom = mBody->GetNextGeometry();
        }

        neSimulator* physicsSim = mScene->getSimulator();
        assert(physicsSim != 0);
        physicsSim->FreeRigidBody(mBody);
    }
}

//-------------------------------------------------------------------------------------
void TokamakBody::createBox(SceneNode* node, const Vector3& extents)
{
    assert(mBody == 0);
    assert(mScene != 0);

    neSimulator* physicsSim = mScene->getSimulator();

    mBody = physicsSim->CreateRigidBody();
    mBody->SetMass(1.0f);
    mBody->SetInertiaTensor(neBoxInertiaTensor(TokamakHelpers::Vector32neV3(extents), mBody->GetMass()));
    neGeometry* boxGeom = mBody->AddGeometry();
    boxGeom->SetBoxSize(TokamakHelpers::Vector32neV3(extents));
    boxGeom->SetMaterialIndex(0);
    mBody->UpdateBoundingInfo();
    mBody->SetCollisionID(0);
    mBody->GravityEnable(true);
    mNode = node;
    mBody->SetUserData((u32)this);

    mScene->mPhysicsBodies[node->getName()] = mBody;
}

//-------------------------------------------------------------------------------------
void TokamakBody::setPosition(const Vector3& position)
{
    mBody->SetPos(TokamakHelpers::Vector32neV3(position));
}

//-------------------------------------------------------------------------------------
void TokamakBody::setLinearVelocity(const Vector3& velocity)
{
    mBody->SetVelocity(TokamakHelpers::Vector32neV3(velocity));
}

//-------------------------------------------------------------------------------------
void TokamakBody::setAngularVelocity(const Vector3& velocity)
{
    mBody->SetAngularMomentum(TokamakHelpers::Vector32neV3(velocity));
}

//-------------------------------------------------------------------------------------
