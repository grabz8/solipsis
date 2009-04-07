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

#ifndef __PhysXScene_h__
#define __PhysXScene_h__

#include <IPhysicsScene.h>
#include "PhysXPluginPrerequisites.h"

#include <NxPhysics.h>
#include <ControllerManager.h>

using namespace Ogre;

namespace Solipsis {

/** This class manages a PhysX scene.
*/
class PhysXScene : public IPhysicsScene
{
protected:
    /// Engine
    PhysXEngine* mEngine;
    /// Scene
    NxScene* mNxScene;
    /// Geometry
    NxTriangleMesh* mNxGeometry;
    /// Actor
    NxActor* mNxActor;
    /// Controller manager
    NxControllerManager* mNxControllerManager;

    /// Time since last frame in seconds
    Real mTimeSinceLastFrame;
    /// Max duration of each step in seconds
    Real mMaxStepInSeconds;
    /// First step of simulation
    bool mFirstStep;

    /// True if a terrain mesh is loaded
    bool m_bTerrainMeshLoaded;


public:
    PhysXScene(PhysXEngine* engine);
    virtual ~PhysXScene();

    /// @copydoc IPhysicsScene::create
    virtual bool create();

    /// @copydoc IPhysicsScene::getDebugFlags
    virtual DebugFlag getDebugFlags() { return DF_NONE; }
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
    virtual bool setTerrainMesh(const MeshPtr mesh,
                                const Vector3& position,
                                const Quaternion& orientation,
                                const Vector3& scale);
    /// @copydoc IPhysicsScene::unsetTerrainMesh
    virtual void unsetTerrainMesh();
    /// @copydoc IPhysicsScene::hasTerrainmesh
    virtual bool hasTerrainmesh();

    /// @copydoc IPhysicsScene::createBody
    virtual IPhysicsBody* createBody();
    /// @copydoc IPhysicsScene::destroyBody
    virtual void destroyBody(IPhysicsBody* body);

    /// @copydoc IPhysicsScene::createCharacter
    virtual IPhysicsCharacter* createCharacter();
    /// @copydoc IPhysicsScene::destroyCharacter
    virtual void destroyCharacter(IPhysicsCharacter* character);

    /// Get scene
    NxScene* getNxScene() { return mNxScene; }
    /// Get actor
    NxActor* getNxActor() { return mNxActor; }
    /// Get character controller
    NxControllerManager* getNxControllerManager() { return mNxControllerManager; }
};

} // namespace Solipsis

#endif // #ifndef __PhysXScene_h__