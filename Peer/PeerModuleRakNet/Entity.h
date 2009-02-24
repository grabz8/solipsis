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

#ifndef __Entity_h__
#define __Entity_h__

#include <RakNetEntity.h>
#include "Ogre.h"
#include "XmlDatas.h"
#ifdef PHYSICSPLUGINS
#include "IPhysicsScene.h"
#endif
#ifdef PHYSICSPLUGINS
#include "IPhysicsCharacter.h"
#endif

namespace Solipsis {

/** This class manages 1 entity by its descriptor.
*/
class Entity : public RakNetEntity
{
protected:
    /// Whether to apply the gravity
    bool mGravity;
#ifdef PHYSICSPLUGINS
    /// Physics scene
    IPhysicsScene* mPhysicsScene;
#endif

    /// Collision mesh filename (Site)
    std::string mCollisionMeshFilename;
    /// Radius (Avatar)
    Ogre::Real mRadius;
    /// Height (Avatar)
    Ogre::Real mHeight;
#ifdef PHYSICSPLUGINS
    /// Physics Character (Avatar)
    IPhysicsCharacter* mPhysicsCharacter;
#endif

public:
    /// Dirty flag (Avatar)
    bool mDirty;
    /// Xml Entity cache (Avatar)
#ifdef POOL
    RefCntPoolPtr<XmlEntity> mUpdatedXmlEntity;
#else
    XmlEntity mUpdatedXmlEntity;
#endif

public:
    /** Constructor. */
    Entity();
    /** Destructor. */
    virtual ~Entity();

    /** See RakNetEntity. */
    virtual void onNewEntity();
    /** See RakNetEntity. */
    virtual void onLostEntity();

    /** See RakNet::Replica2. */
	virtual void Deserialize(RakNet::BitStream *bitStream, RakNet::SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);

    /** See CacheManagerCallback. */
    virtual void onTransferComplete(const std::string& filename);

    /** Apply or not the gravity. */
    void applyGravity(bool enabled);
    /** Set whether the gravity is activated or not. */
    void setGravity(bool enabled);
    /** Determines whether the gravity is applied or not. */
    bool isGravityEnabled();
#ifdef PHYSICSPLUGINS
    /** Get the physics scene. */
    IPhysicsScene* getPhysicsScene();
    /** Create physics. */
    virtual void createPhysics(IPhysicsScene* physicsScene);
    /** Destroy physics. */
    virtual void destroyPhysics();
#endif

    /** Update. */
    virtual bool update(Ogre::Real timeSinceLastFrame);
};

} // namespace Solipsis

#endif // #ifndef __Entity_h__