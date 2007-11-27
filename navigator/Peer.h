#ifndef __Peer_h__
#define __Peer_h__

#include "BigInt.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

#define PEER_LOCAL ""

/** This class contains datas of a peer.
 */
class Peer
{
protected:
    String mNetworkId;
    String mLogin;
    CBigInt x; 
    CBigInt y;
    CBigInt z;

public:

    Peer(String& networkId, String& login, CBigInt x, CBigInt y, CBigInt z);
    Peer(String& login, CBigInt x, CBigInt y, CBigInt z);
    ~Peer();

    // Get and set
    String getNetworkId();
    String getLogin();
    int getFakeX();
    int getFakeY();
    int getFakeZ();
    bool isLocal();
};

} // namespace Solipsis

#endif // #ifndef __Peer_h__