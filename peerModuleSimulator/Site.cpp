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

#include "Site.h"
#include "OgreHelpers.h"
#include <CTIO.h>

using namespace Ogre;

namespace Solipsis {

//-------------------------------------------------------------------------------------
#ifdef POOL
Site::Site(RefCntPoolPtr<XmlEntity>& xmlEntity, const NodeId& managerNodeId) :
#else
Site::Site(XmlEntity* xmlEntity, const NodeId& managerNodeId) :
#endif
    Entity(xmlEntity, managerNodeId),
    mCollisionMeshFilename("")
{
}

//-------------------------------------------------------------------------------------
Site::~Site()
{
}

#ifdef PHYSICSPLUGINS
//-------------------------------------------------------------------------------------
void Site::createPhysics(IPhysicsScene* physicsScene)
{
    Entity::createPhysics(physicsScene);

    // Get the scene content for LOD 0
    XmlContent::ContentLodMap& contentLodMap = mXmlEntity->getContent()->getContentLodMap();
    RefCntPoolPtr<XmlSceneLodContent> xmlSceneLodContent0 = RefCntPoolPtr<XmlSceneLodContent>(contentLodMap[0]->getDatas());
    // Collision ?
    if (xmlSceneLodContent0->getCollision().empty())
        return;

    // Find .ssf file
    XmlLodContent::LodContentFileList::const_iterator lodContent0File = contentLodMap[0]->getLodContentFileList().begin();
    for(;lodContent0File!=contentLodMap[0]->getLodContentFileList().end();++lodContent0File)
        if (lodContent0File->filename.find(".ssf") == lodContent0File->filename.length() - 4)
            break;
    if (lodContent0File == contentLodMap[0]->getLodContentFileList().end())
        return;

    // Create the resource group
    std::string mediaCacheSceneRelativePath = CommonTools::IO::retrieveRelativePathByDescendingCWD(std::string("Media\\cache\\scenes"));
    String resourceGroup = mXmlEntity->getUidString() + "Resources";
    ResourceGroupManager::getSingleton().createResourceGroup(resourceGroup);
    ResourceGroupManager::getSingleton().addResourceLocation(mediaCacheSceneRelativePath + "\\" + lodContent0File->filename, "Zip", resourceGroup);

    // Load .osm
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

    TiXmlElement* entities = osmFileDoc.RootElement()->FirstChildElement("entities");
    TiXmlElement* entity = entities->FirstChildElement("entity");
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

    Vector3 position;
    Quaternion rotation;
    Vector3 scale;

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

    Mesh* collisionMesh = OgreHelpers::getSingleton().loadMesh(mCollisionMeshFilename);
    MeshPtr collisionMeshPtr(collisionMesh);
    mPhysicsScene->setTerrainMesh(collisionMeshPtr, getXmlEntity()->getPosition() + position, getXmlEntity()->getOrientation()*rotation, scale);

    // Destroy the resource group
    ResourceGroupManager::getSingleton().destroyResourceGroup(resourceGroup);
}

//-------------------------------------------------------------------------------------
void Site::destroyPhysics()
{
    Entity::destroyPhysics();
}
#endif

//-------------------------------------------------------------------------------------
bool Site::update(Real timeSinceLastFrame)
{
    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
