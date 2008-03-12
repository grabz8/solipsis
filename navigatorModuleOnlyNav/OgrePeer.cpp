#include "OgrePeer.h"
#include "OgreHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
OgrePeer::OgrePeer(XmlEntity* xmlEntity, bool isLocal) :
    mXmlEntity(xmlEntity),
    mIsLocal(isLocal),
    mLod(0)
{
    OGRE_LOG("OgrePeer::OgrePeer() mIsLocal=" + StringConverter::toString((int)isLocal));
}

//-------------------------------------------------------------------------------------
OgrePeer::~OgrePeer()
{
}

//-------------------------------------------------------------------------------------
XmlEntity* OgrePeer::getXmlEntity()
{
    return mXmlEntity;
}

//-------------------------------------------------------------------------------------
