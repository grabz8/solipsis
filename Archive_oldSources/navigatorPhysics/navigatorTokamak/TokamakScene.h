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

#ifndef __TokamakScene_h__
#define __TokamakScene_h__

#include "IPhysicsScene.h"
#include "TokamakPluginPrerequisites.h"

#include "tokamak.h"
#include <map>

using namespace Ogre;

namespace Solipsis {

/** This class manages a Tokamak scene.
*/
class TokamakScene : public IPhysicsScene
{
    friend class TokamakBody;

protected:
    /// Scene manager
    SceneManager* mSceneManager;
    /// Gravity vector initialized when simulator is created
    Vector3 mGravity;
    /// Simulator
    neSimulator* mPhysicsSim;
    /// World geometry
    neTriangleMesh mWorldGeometry;
    /// Map of bodies
    typedef std::map<String, neRigidBody*> BodyMap;
    /// Bodies
    BodyMap mPhysicsBodies;

    /// Time since last frame in seconds
    Real mTimeSinceLastFrame;
    /// Max duration of each step in seconds
    Real mMaxStepInSeconds;

public:
    TokamakScene();
    virtual ~TokamakScene();

    /// @copydoc IPhysicsScene::create
    virtual bool create(SceneManager* sceneManager);

    /// @copydoc IPhysicsScene::getDebugFlags
    virtual DebugFlag getDebugFlags() { return IPhysicsScene::DF_NONE; }
    /// @copydoc IPhysicsScene::setDebugFlags
    virtual void setDebugFlags(DebugFlag debugFlags) {}

    /// @copydoc IPhysicsScene::setTiming
    virtual void setTiming(Real maxStepInSeconds);
    /// @copydoc IPhysicsScene::getTiming
    virtual void getTiming(Real& maxStepInSeconds, Real& timeSinceLastFrame);

    /// @copydoc IPhysicsScene::preStep
    virtual void preStep(Real timeSinceLastFrame);
    /// @copydoc IPhysicsScene::postStep
    virtual void postStep();

    /// @copydoc IPhysicsScene::getGravity
    virtual void getGravity(Vector3& gravity);
    /// @copydoc IPhysicsScene::setGravity
    virtual void setGravity(Vector3& gravity);

    /// @copydoc IPhysicsScene::setTerrainMesh
    virtual bool setTerrainMesh(const Entity& entity);

    /// @copydoc IPhysicsScene::createBody
    virtual IPhysicsBody* createBody();
    /// @copydoc IPhysicsScene::destroyBody
    virtual void destroyBody(IPhysicsBody* body);

    /// @copydoc IPhysicsScene::createCharacter
    virtual IPhysicsCharacter* createCharacter();
    /// @copydoc IPhysicsScene::destroyCharacter
    virtual void destroyCharacter(IPhysicsCharacter* character);

    /// Get simulator
    neSimulator* getSimulator() { return mPhysicsSim; }
    /// Get world geometry
    neTriangleMesh* getWorldGeometry() { return &mWorldGeometry; }

protected:
    /** See Tokamak. */
    static void collisionCallback(neCollisionInfo& collisionInfo);
};

} // namespace Solipsis

#endif // #ifndef __TokamakScene_h__