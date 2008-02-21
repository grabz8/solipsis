#ifndef __Peer_h__
#define __Peer_h__

#include <pthread.h>
#include "BasicThread.h"
#include "IPeer.h"
#include "PhysicsEngineManager.h"
#include "Ogre.h"
#include "IP2NServer.h"
#include "NodeManager.h"
#include "TimeListener.h"

namespace Solipsis {

/** This class manages the Solipsis peer on 1 host.
*/
class Peer : public IPeer, public IP2NServerRequestsHandler, public BasicThread
{
    friend class IPeer;

protected:
    static Peer* ms_Singleton;

protected:
    bool mInitialized;
    String mAppPath;
    std::string mHost;
    int mPort;
    int mVerbosity;
	IP2NServer* mP2NServer;
    PhysicsEngineManager* mPhysicsEngineManager;
    NodeManager* mNodeManager;

    /** Set of registered frame listeners */
    std::set<TimeListener*> mTimeListeners;

    /** Set of frame listeners marked for removal*/
    std::set<TimeListener*> mRemovedTimeListeners;

    /// Contains the times of recently fired tick
    unsigned long mLastTickTime;
    bool mResetTime;

public:
    int mConnectionsCount;
    time_t mFirstEvtDate;
    int mState;

protected: 
    Peer(const char* appPath, int argc, char** argv);
    virtual ~Peer();

public:
    /** See IPeer. */
    static IPeer* createPeer(const char* appPath, int argc, char** argv);
    /** See IPeer. */
    virtual bool destroy();

    /** See BasicThread. */
    virtual void stop(unsigned int stopTimeoutSec = 5)
    {
        // call inherited
        BasicThread::stop();
        // Stop the peer
//        if (mState == SRunning)
//            ;
    }

    static Peer* getSingletonPtr() { return ms_Singleton; }
    static Peer& getSingleton() { return *ms_Singleton; }

    NodeManager* getNodeManager() { return mNodeManager; }

    /** Registers a TimeListener which will be called back every tick.
        @remarks
            A TimeListener is a class which implements methods which
            will be called every tick.
        @par
            See the TimeListener class for more details on the specifics
            It is imperitive that the instance passed to this method is
            not destroyed before either the processing loop ends, or the
            class is removed from the listening list using
            removeTimeListener.
        @note
            <br>This method can only be called after Root::initialise has
            been called.
    */
    void addTimeListener(TimeListener* newListener);

    /** Removes a TimeListener from the list of listening classes.
    */
    void removeTimeListener(TimeListener* oldListener);

protected:
    /** See BasicThread. */
    virtual void BasicThread::run();

    /** See IP2NServerRequestsHandler. */
    virtual IP2NClient::RetCode login(const std::string& xmlParamsStr, NodeId& nodeId, std::string& xmlRespStr);
    /** See IP2NServerRequestsHandler. */
    virtual IP2NClient::RetCode logout(NodeId& nodeId);
    /** See IP2NServerRequestsHandler. */
    virtual IP2NClient::RetCode handleEvt(const NodeId& nodeId, std::string& xmlRespStr);
    /** See IP2NServerRequestsHandler. */
    virtual IP2NClient::RetCode sendEvt(const NodeId& nodeId, const std::string& xmlEvtStr, std::string& xmlRespStr);

protected:
    bool _initialize();
    void _finalize();
    bool _fireTick(Ogre::Real timeSinceLastTick);
    bool _fireTick();
};

} // namespace Solipsis

#endif // #ifndef __Peer_h__
