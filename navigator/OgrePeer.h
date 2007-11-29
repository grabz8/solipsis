#ifndef __PeerNode_h__
#define __PeerNode_h__

#include "Peer.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

//#define COLLIDE_QUERY_MASK 0x00001000

/** This class represents a basic Ogre representation of a peer.
 */
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

    virtual void update(Real timeSinceLastFrame) = 0;
};

} // namespace Solipsis

#endif // #ifndef __PeerNode_h__