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

#include "Entity.h"
#include "Peer.h"
#include "OgreHelpers.h"
#include <CTLog.h>
#include <CTIO.h>

using namespace RakNet;
using namespace RakNetSolipsis;
using namespace Ogre;
using namespace CommonTools;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Entity::Entity() :
    RakNetEntity(),
    mCollisionMeshFilename("")
    ,mPhysicsScene(0)
    ,mPhysicsCharacter(0)
    ,mDirty(true)
{
    applyGravity(true);
}

//-------------------------------------------------------------------------------------
Entity::~Entity()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Entity::~Entity()");

    removeEntity(this);

    destroyPhysics();
}

//-------------------------------------------------------------------------------------
void Entity::onNewEntity()
{
    RakNetEntity::onNewEntity();
    Peer::getSingleton().getAvatarNode()->onNewEntity(this);
}

//-------------------------------------------------------------------------------------
void Entity::onLostEntity()
{
    Peer::getSingleton().getAvatarNode()->onLostEntity(this);
    RakNetEntity::onLostEntity();
}

//-------------------------------------------------------------------------------------
void Entity::Deserialize(BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
    RakNetEntity::Deserialize(bitStream, serializationType, sender, timestamp);

    RakNetEntityMap& entities = getEntities();
    RakNetEntityMap::const_iterator it = entities.find(mXmlEntity->getUid());
    if (it != entities.end())
    {
        // masking content updates until transfer is complete
        mLastDeserializedDefinedAttributes &= ~XmlEntity::DAContent;
        Peer::getSingleton().getAvatarNode()->onUpdatedEntity(this);
    }
}

//-------------------------------------------------------------------------------------
float Entity::onTransferProgress(const std::string& filename, float fProgress)
{
    float fGlobalProgress = RakNetEntity::onTransferProgress(filename, fProgress);
    mXmlEntity->setDownloadProgress(fGlobalProgress);
    mLastDeserializedDefinedAttributes |= XmlEntity::DAProgress;

    RakNetEntityMap& entities = getEntities();
    RakNetEntityMap::const_iterator it = entities.find(mXmlEntity->getUid());
    if (it == entities.end())
    {
        addEntity(this);
    }
    else
    {
        Peer::getSingleton().getAvatarNode()->onUpdatedEntity(this);
    }

    return fGlobalProgress;
}


//-------------------------------------------------------------------------------------
void Entity::onTransferComplete(const std::string& filename)
{
    RakNetEntity::onTransferComplete(filename);

    if (mMissingFiles.empty())
    {
        mXmlEntity->setDownloadProgress(1.0f);
        mLastDeserializedDefinedAttributes |= XmlEntity::DAProgress;
        RakNetEntityMap& entities = getEntities();
        RakNetEntityMap::const_iterator it = entities.find(mXmlEntity->getUid());
        if (it == entities.end())
        {
            mDirty = true;
            addEntity(this);
        }
        else
        {
            mLastDeserializedDefinedAttributes |= XmlEntity::DAContent;
            Peer::getSingleton().getAvatarNode()->onUpdatedEntity(this);
        }
    }
}

//-------------------------------------------------------------------------------------
void Entity::applyGravity(bool applied)
{
    if (!applied)
        mGravity = false;
    else
        mGravity = mXmlEntity->getFlags() & EFGravity;
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

//-------------------------------------------------------------------------------------
IPhysicsScene* Entity::getPhysicsScene()
{
    return mPhysicsScene;
}

//-------------------------------------------------------------------------------------
void Entity::createPhysics(IPhysicsScene* physicsScene)
{
    if (mXmlEntity->getType() == ETSite && mXmlEntity->getDownloadProgress() >= 1)
    {
        destroyPhysics();
        mPhysicsScene = physicsScene;

       // Get the scene content for LOD 0
        XmlContent::ContentLodMap& contentLodMap = mXmlEntity->getContent()->getContentLodMap();
        RefCntPoolPtr<XmlSceneLodContent> xmlSceneLodContent0 = RefCntPoolPtr<XmlSceneLodContent>(contentLodMap[0]->getDatas());
        // Collision ?
        if (xmlSceneLodContent0->getCollision().empty())
            return;

        // Find .ssf file
        XmlLodContent::LodContentFileList::const_iterator lodContent0File = contentLodMap[0]->getLodContentFileList().begin();
        for(;lodContent0File!=contentLodMap[0]->getLodContentFileList().end();++lodContent0File)
            if (lodContent0File->mFilename.find(".ssf") == lodContent0File->mFilename.length() - 4)
                break;
        if (lodContent0File == contentLodMap[0]->getLodContentFileList().end())
            return;

        // Create the resource group
        String resourceGroup = mXmlEntity->getUid() + "PeerResources";
        ResourceGroupManager::getSingleton().createResourceGroup(resourceGroup);
        ResourceGroupManager::getSingleton().addResourceLocation(Peer::getSingleton().getMediaCachePath() + IO::getPathSeparator() + lodContent0File->mFilename, "Zip", resourceGroup);

        // Load .osm or .scene
        TiXmlDocument osmFileDoc;
        DataStreamPtr pStream = ResourceGroupManager::getSingleton().openResource(xmlSceneLodContent0->getMainFilename());
        if (!pStream->size())
            return;
        size_t iSize = pStream->size();
        char *pBuf = new char[iSize+1];
        memset(pBuf, 0, iSize+1);
        pStream->read(pBuf, iSize);
        pStream.setNull();
        osmFileDoc.Parse(pBuf);
        delete[] pBuf;

        Vector3 position;
        Quaternion rotation;
        Vector3 scale;

        // Check the scene type (.OSM with oFusion or .SCENE with ogreMax)
        TiXmlElement* entity;
        std::string sceneFilename( xmlSceneLodContent0->getMainFilename().c_str() );
        if( sceneFilename.find(".osm") == sceneFilename.length() - 4 )
        {
            // Load .osm
            TiXmlElement* entities = osmFileDoc.RootElement()->FirstChildElement("entities");
            entity = entities->FirstChildElement("entity");
            while (entity != 0)
            {
                const char* attr = 0;
                attr = entity->Attribute("name");
                if ((attr == 0) || (attr[0] == '\0'))
                    continue;
                if (strcmp(attr, xmlSceneLodContent0->getCollision().c_str()) == 0)
                    break;
                entity = entity->NextSiblingElement("entity");
            }

            if (entity == 0)
                return;

            mCollisionMeshFilename = entity->Attribute("filename");

            // Position
            TiXmlElement* posElem = entity->FirstChildElement("position");
            if (posElem)
                XmlHelpers::fromXmlEltVector3(posElem, position);
            // Rotation
            TiXmlElement* rotElem = entity->FirstChildElement("rotation");
            if (rotElem)
                XmlHelpers::fromXmlEltQuaternion(rotElem, rotation);
            // Scale
            TiXmlElement* scaleElem = entity->FirstChildElement("scale");
            if (scaleElem)
                XmlHelpers::fromXmlEltVector3(scaleElem, scale);
        }
        else if( sceneFilename.find(".scene") == sceneFilename.length() - 6 )
        {
            // Load .scene
            TiXmlElement* nodes = osmFileDoc.RootElement()->FirstChildElement("nodes");
            TiXmlElement* node = nodes->FirstChildElement("node");
            if (node != 0)
            {
                entity = node->FirstChildElement("entity");
                while (entity != 0)
                {
                    const char* attr = 0;
                    attr = entity->Attribute("name");
                    if ((attr == 0) || (attr[0] == '\0'))
                        continue;
                    if (strcmp(attr, xmlSceneLodContent0->getCollision().c_str()) == 0)
                        break;

                    entity = node->NextSiblingElement("entity");
                    while (entity == 0)
                    {
                        node = node->NextSiblingElement("node");
                        entity = node->FirstChildElement("entity");
                    }
                }
            }

            if (entity == 0)
                return;

            mCollisionMeshFilename = entity->Attribute("meshFile");

            // Position
            TiXmlElement* posElem = node->FirstChildElement("position");
            if (posElem)
                XmlHelpers::fromXmlEltVector3(posElem, position);
            // Rotation
            TiXmlElement* rotElem = node->FirstChildElement("rotation");
            if (rotElem)
                XmlHelpers::fromXmlEltQuaternion(rotElem, rotation);
            // Scale
            TiXmlElement* scaleElem = node->FirstChildElement("scale");
            if (scaleElem)
                XmlHelpers::fromXmlEltVector3(scaleElem, scale);
        }
        else
            throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create any file scene " + String(xmlSceneLodContent0->getMainFilename()), "Scene::update");

        IPeerRenderSystemLock* renderSystemLock = Peer::getSingleton().getRenderSystemLock();
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Entity::createPhysics() loading mesh%s", (renderSystemLock != 0) ? " locking" : "");
        if (renderSystemLock != 0)
            renderSystemLock->lock();
        Mesh* collisionMesh = OgreHelpers::getSingleton().loadMesh(mCollisionMeshFilename);
        MeshPtr collisionMeshPtr(collisionMesh);
        mPhysicsScene->setTerrainMesh(collisionMeshPtr, getXmlEntity()->getPosition() + position, getXmlEntity()->getOrientation()*rotation, scale);

        // Mesh should be freed when no more referenced
/*        MeshManager::getSingleton().remove(collisionMeshPtr->getHandle());*/

        // Destroy the resource group
        ResourceGroupManager::getSingleton().destroyResourceGroup(resourceGroup);
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Entity::createPhysics() mesh loaded%s", (renderSystemLock != 0) ? " unlocking" : "");
        if (renderSystemLock != 0)
            renderSystemLock->unlock();
    }
    else
    {
        destroyPhysics();
        mPhysicsScene = physicsScene;
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
    }

}

//-------------------------------------------------------------------------------------
void Entity::destroyPhysics()
{
    if (mXmlEntity->getType() == ETSite)
    {
        if (mPhysicsScene != 0)
            mPhysicsScene->unsetTerrainMesh();
    }
    else
    {
        if ((mPhysicsScene != 0) && (mPhysicsCharacter != 0))
            mPhysicsScene->destroyCharacter(mPhysicsCharacter);

        mPhysicsCharacter = 0;
    }

    mPhysicsScene = 0;
}

//-------------------------------------------------------------------------------------
bool Entity::update(Real timeSinceLastFrame)
{
    // Move physics character
    if (mPhysicsCharacter != 0 && mPhysicsScene->hasTerrainmesh())
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

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
