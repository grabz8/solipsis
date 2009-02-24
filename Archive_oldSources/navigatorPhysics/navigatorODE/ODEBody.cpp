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
