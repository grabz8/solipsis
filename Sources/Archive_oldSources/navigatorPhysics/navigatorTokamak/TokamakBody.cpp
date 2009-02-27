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
    mBody->SetUserData((u32)((TokamakCollider*)this));

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
