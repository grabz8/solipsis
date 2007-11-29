#include "OgrePeer.h"
#include "OgreHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
OgrePeer::OgrePeer(Peer* peer, String type) :
    mPeer(peer),
    mType(type),
    mLod(0)
{
    OGRE_LOG("OgrePeer::OgrePeer() mType=" + mType);
}

//-------------------------------------------------------------------------------------
OgrePeer::~OgrePeer()
{
}

//-------------------------------------------------------------------------------------
Peer* OgrePeer::getPeer()
{
    return mPeer;
}

//-------------------------------------------------------------------------------------
const String& OgrePeer::getType()
{
    return mType;
}

//-------------------------------------------------------------------------------------
