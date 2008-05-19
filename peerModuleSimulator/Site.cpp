#include "Site.h"
#include "OgreHelpers.h"

using namespace Ogre;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Site::Site(XmlEntity* xmlEntity) :
    Entity(xmlEntity),
    mOSMFilename(""),
    mCollisionMeshFilename(""),
    mGatePosition(Vector3::ZERO)
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
        mCollisionMeshFilename = sceneNodeElt->Attribute("collision");
        TiXmlElement* gatePosElt;
        if ((gatePosElt = sceneNodeElt->FirstChildElement("gatePos")) != 0)
            XmlHelpers::fromXmlEltVector3(gatePosElt, mGatePosition);
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

    if (mOSMFilename.empty() || mCollisionMeshFilename.empty())
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
        if (strcmp(attr, mCollisionMeshFilename.c_str()) == 0)
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
