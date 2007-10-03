#ifndef __PeerNode_h__
#define __PeerNode_h__

#include "Peer.h"
#include "Ogre.h"

using namespace Ogre;

//#define COLLIDE_QUERY_MASK 0x00001000

class OgrePeer
{
protected:
    Peer* mPeer;
    String mType;
    int mLod;

public:
    OgrePeer(Peer* peer, String type);
    virtual ~OgrePeer();

    // Get and set
    Peer* getPeer();
    const String& getType();

    virtual void update(Ogre::Real timeSinceLastFrame) = 0;
};

#endif // #ifndef __PeerNode_h__