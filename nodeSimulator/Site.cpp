#include "Site.h"
#include "OgreHelpers.h"

using namespace Ogre;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Site::Site(XmlEntity* xmlEntity) :
    Entity(xmlEntity)
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

    // Set the scene collision mesh
    std::string osmFilename;
    XmlContent* content = mXmlEntity->getContent();
    XmlContent::ContentFileList& contentFileListLod0 = content->getContentLodMap()[0];
    static const std::basic_string <char>::size_type npos = -1;
    for (XmlContent::ContentFileList::iterator f = contentFileListLod0.begin(); f != contentFileListLod0.end(); ++f)
        if ((*f).rfind(".osm") != std::string::npos)
        {
            osmFilename = *f;
            break;
        }
    if (osmFilename.empty())
        return;

    std::string mcFilename;
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;
    TiXmlDocument osmFileDoc;
	DataStreamPtr pStream = ResourceGroupManager::getSingleton().openResource(osmFilename);
	if (!pStream->size())
        return;
	size_t iSize = pStream->size();
	char *pBuf = new char[iSize+1];
	memset(pBuf, 0, iSize+1);
	pStream->read(pBuf, iSize);
	pStream.setNull();
	osmFileDoc.Parse(pBuf);
	delete[] pBuf;

	// check for errors
    if (osmFileDoc.Error())
        return;
	TiXmlElement* entities = osmFileDoc.RootElement()->FirstChildElement("entities");
    TiXmlElement* entity = entities->FirstChildElement("entity");
    while (entity != 0)
    {
        const char* attr = 0;
        attr = entity->Attribute("filename");
        if ((attr == 0) || (attr[0] == '\0'))
            continue;
        if (strstr(attr, "MC_") == attr)
        {
            mcFilename = attr;;
            break;
        }
        entity = entity->NextSiblingElement("entity");
    }
    if (mcFilename.empty())
        return;
	// Position
	TiXmlElement* posElem = entity->FirstChildElement("position");
    if (posElem)
    {
		position.x = StringConverter::parseReal(posElem->Attribute("x"));
		position.y = StringConverter::parseReal(posElem->Attribute("y"));
		position.z = StringConverter::parseReal(posElem->Attribute("z"));
    }
	// Rotation
	TiXmlElement* rotElem = entity->FirstChildElement("rotation");
    if (rotElem)
    {
		rotation.w = StringConverter::parseReal(rotElem->Attribute("w"));
		rotation.x = StringConverter::parseReal(rotElem->Attribute("x"));
		rotation.y = StringConverter::parseReal(rotElem->Attribute("y"));
		rotation.z = StringConverter::parseReal(rotElem->Attribute("z"));
    }
	// Scale
	TiXmlElement* scaleElem = entity->FirstChildElement("scale");
    if (scaleElem)
    {
		scale.x = StringConverter::parseReal(scaleElem->Attribute("x"));
		scale.y = StringConverter::parseReal(scaleElem->Attribute("y"));
		scale.z = StringConverter::parseReal(scaleElem->Attribute("z"));
    }

    Mesh* collisionMesh = OgreHelpers::getSingleton().loadMesh(mcFilename);
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
