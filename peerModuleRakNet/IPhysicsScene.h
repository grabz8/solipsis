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

#ifndef __IPhysicsScene_h__
#define __IPhysicsScene_h__

#include "PeerModule.h"
#include "OgrePrerequisites.h"
#include "Ogre.h"

namespace Solipsis {

/** This class represents a generic Physics scene.
*/
class PEERMODULE_EXPORT IPhysicsScene
{
public:
    /// Collision groups assigned to bodies
    enum CollisionGroup
    {
        CG_NON_COLLIDABLE,
        CG_COLLIDABLE_NON_PUSHABLE,
        CG_COLLIDABLE_PUSHABLE,
    };
    static const int CG_COLLIDABLE_MASK = (1<<CG_COLLIDABLE_NON_PUSHABLE) | (1<<CG_COLLIDABLE_PUSHABLE);

    /// Debug flags helpfull to visualize physics
    enum DebugFlag
    {
        DF_NONE = 0,
        DF_CONTACTS = 1,
        DF_GEOMETRIES = DF_CONTACTS<<1
    };

public:
    IPhysicsScene() {}
    virtual ~IPhysicsScene() {}

    /** Create.
    @remarks An implementation must be supplied for this method.
    */
    virtual bool create() = 0;

    /** Get debug flags.
    @remarks An implementation must be supplied for this method.
    */
    virtual DebugFlag getDebugFlags() = 0;
    /** Set debug flags.
    @remarks An implementation must be supplied for this method.
    */
    virtual void setDebugFlags(DebugFlag debugFlags) = 0;

    /** Set the physics timing parameters.
    @remarks An implementation must be supplied for this method.
    @param maxStepInSeconds The max duration of each step in seconds
    */
    virtual void setTiming(Ogre::Real maxStepInSeconds) = 0;
    /** Get the physics timing parameters.
    @remarks An implementation must be supplied for this method.
    @param maxStepInSeconds The max duration of each step in seconds
    @param timeSinceLastFrame The last frame duration
    */
    virtual void getTiming(Ogre::Real& maxStepInSeconds, Ogre::Real& timeSinceLastFrame) = 0;

    /** Pre-compute 1 step.
    @remarks An implementation must be supplied for this method.
    We simulate 1 physics step with 2 calls because some physics engines (like PhysX)
    are working in background(threaded) and we have to avoid blocking render by the physics
    computation.
    @param timeSinceLastFrame The duration of the last frame in seconds
    */
    virtual void preStep(Ogre::Real timeSinceLastFrame) = 0;
    /** Post-compute 1 step.
    @remarks An implementation must be supplied for this method.
    We simulate 1 physics step with 2 calls because some physics engines (like PhysX)
    are working in background(threaded) and we have to avoid blocking render by the physics
    computation.
    */
    virtual void postStep() = 0;

    /** Get the gravity.
    @remarks An implementation must be supplied for this method.
    */
    virtual void getGravity(Ogre::Vector3& gravity) = 0;
    /** Set the gravity.
    @remarks An implementation must be supplied for this method.
    */
    virtual void setGravity(Ogre::Vector3& gravity) = 0;

    /** Set the collision mesh of the terrain.
    @remarks An implementation must be supplied for this method.
    */
    virtual bool setTerrainMesh(const Ogre::MeshPtr mesh,
                                const Ogre::Vector3& position,
                                const Ogre::Quaternion& orientation,
                                const Ogre::Vector3& scale) = 0;
    /** Unset the collision mesh of the terrain.
    @remarks An implementation must be supplied for this method.
    */
    virtual void unsetTerrainMesh() = 0;

    /** Create 1 body.
    @remarks An implementation must be supplied for this method.
    */
    virtual IPhysicsBody* createBody() = 0;
    /** Destroy 1 body.
    @remarks An implementation must be supplied for this method.
    */
    virtual void destroyBody(IPhysicsBody* body) = 0;

    /** Create 1 character.
    @remarks An implementation must be supplied for this method.
    */
    virtual IPhysicsCharacter* createCharacter() = 0;
    /** Destroy 1 character.
    @remarks An implementation must be supplied for this method.
    */
    virtual void destroyCharacter(IPhysicsCharacter* character) = 0;
};

} // end namespace

#endif // #ifndef __IPhysicsScene_h__