#include "OgrePeer.h"

OgrePeer::OgrePeer(Peer* peer) :
    mPeer(peer)
{
}

OgrePeer::~OgrePeer()
{
}

Peer* OgrePeer::getPeer()
{
    return mPeer;
}
