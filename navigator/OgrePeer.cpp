#include "OgrePeer.h"

OgrePeer::OgrePeer(Peer* peer, String type) :
    mPeer(peer),
    mType(type),
    mLod(0)
{
    LogManager::getSingletonPtr()->logMessage("OgrePeer::OgrePeer() mType=" + mType);
}

OgrePeer::~OgrePeer()
{
}

Peer* OgrePeer::getPeer()
{
    return mPeer;
}

const String& OgrePeer::getType()
{
    return mType;
}
