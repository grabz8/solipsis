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

using namespace Ogre;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Site::Site(XmlEntity* xmlEntity) :
    Entity(xmlEntity),
    mOSMFilename(""),
    mCollisionMeshFilename(""),
    mEntryGatePosition(Vector3::ZERO),
    mEntryGateGravity(false)
{
    // Set the scene collision mesh
    std::string xmlFilename;
    XmlContent* content = mXmlEntity->getContent();
    XmlContent::ContentFileList& contentFileListLod0 = content->getContentLodMap()[0];
    static const std::basic_string <char>::size_type npos = -1;
    for (XmlContent::ContentFileList::iterator f = contentFileListLod0.begin(); f != contentFileListLod0.end(); ++f)
        if ((*f).rfind(".xml") != std::string::npos)
        {
            xmlFilename = *f;
            break;
        }
    if (xmlFilename.empty())
        return;

    TiXmlDocument xmlFileDoc;
	DataStreamPtr pStream = ResourceGroupManager::getSingleton().openResource(xmlFilename);
	if (!pStream->size())
        return;
	size_t iSize = pStream->size();
	char *pBuf = new char[iSize+1];
	memset(pBuf, 0, iSize+1);
	pStream->read(pBuf, iSize);
	pStream.setNull();
	xmlFileDoc.Parse(pBuf);
	delete[] pBuf;
	// check for errors
    if (xmlFileDoc.Error())
        return;
    TiXmlElement* sceneNodeElt = xmlFileDoc.RootElement()->FirstChildElement("sceneNode");
    if (sceneNodeElt != 0)
    {
        mOSMFilename = sceneNodeElt->Attribute("filename");
        const char* collisionMeshName = sceneNodeElt->Attribute("collision");
        mCollisionMeshName = (collisionMeshName != 0) ? collisionMeshName : "";
        TiXmlElement* entryGateElt = sceneNodeElt->FirstChildElement("entryGate");
        if (entryGateElt != 0)
        {
            const char* entryGateGravity = entryGateElt->Attribute("gravity");
            if (entryGateGravity != 0)
                mEntryGateGravity = (String(entryGateGravity).compare("true") == 0);
            TiXmlElement* entryGatePosElt;
            if ((entryGatePosElt = entryGateElt->FirstChildElement("position")) != 0)
                XmlHelpers::fromXmlEltVector3(entryGatePosElt, mEntryGatePosition);
        }
    }
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

    if (mOSMFilename.empty() || mCollisionMeshName.empty())
        return;

    TiXmlDocument osmFileDoc;
	DataStreamPtr pStream = ResourceGroupManager::getSingleton().openResource(mOSMFilename);
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
        if (strcmp(attr, mCollisionMeshName.c_str()) == 0)
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
