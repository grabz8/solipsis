#include "OgrePeer.h"
#include "OgreHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
OgrePeer::OgrePeer(XmlObject* object, bool isLocal) :
    mObject(object),
    mIsLocal(isLocal),
    mLod(0)
{
    OGRE_LOG("OgrePeer::OgrePeer() mIsLocal=" + isLocal);
}

//-------------------------------------------------------------------------------------
OgrePeer::~OgrePeer()
{
}

//-------------------------------------------------------------------------------------
XmlObject* OgrePeer::getObject()
{
    return mObject;
}

//-------------------------------------------------------------------------------------
