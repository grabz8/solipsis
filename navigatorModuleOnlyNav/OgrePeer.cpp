#include "OgrePeer.h"
#include "OgreHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
#ifdef POOL
OgrePeer::OgrePeer(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal) :
#else
OgrePeer::OgrePeer(XmlEntity* xmlEntity, bool isLocal) :
#endif
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
#ifdef POOL
RefCntPoolPtr<XmlEntity>& OgrePeer::getXmlEntity()
#else
XmlEntity* OgrePeer::getXmlEntity()
#endif
{
    return mXmlEntity;
}

//-------------------------------------------------------------------------------------
