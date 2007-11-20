#ifndef __NodeEvent_h__
#define __NodeEvent_h__

#include "time.h"
#include "Peer.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

class NodeEvent
{
public:
    enum Type {
        TNew,            // Node connected to a new peer
        TLost,           // Node lost connection with a peer
        TStatusChanged   // Current status has changed
    };
    class Datas
    {
    };
    class DatasPeerNew : public Datas
    {
    public:
        Peer* mPeer;
    };
    class DatasPeerLost : public Datas
    {
    public:
        String mNetworkId;
    };
    class DatasStatusChanged : public Datas
    {
    public:
        String mStatus;
    };

protected:
    time_t mTimestamp;
    Type mType;
    Datas* mDatas;

public:
    NodeEvent(time_t timestamp, Type type, Datas* datas);
    ~NodeEvent();

    time_t getTimestamp();
    Type getType();
    Datas* getDatas();
};

} // namespace Solipsis

#endif // #ifndef __NodeEvent_h__