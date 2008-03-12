#include "Peer.h"
#include "OgreHelpers.h"
#include "Platform.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
// this is a simple C stub so that dll can be loaded dynamically
// and the static method createPeer can be called
extern "C" PEERMODULEDIRECT_EXPORT IPeer* createPeer(const char* appPath, int argc, char** argv)
{
	return (IPeer*)IPeer::createPeer(appPath, argc, argv);
}

//-------------------------------------------------------------------------------------
// this is a simple C stub so that dll can be loaded dynamically
// and the static method getPeer can be called
extern "C" PEERMODULEDIRECT_EXPORT IPeer* getPeer()
{
    return (IPeer*)IPeer::getPeer();
}

Peer* Peer::ms_Singleton = 0;

//-------------------------------------------------------------------------------------
IPeer* IPeer::createPeer(const char* appPath, int argc, char** argv)
{
    if (Peer::ms_Singleton == 0)
    {
        Peer::ms_Singleton = new Peer(appPath, argc, argv);
        assert(Peer::ms_Singleton);
        if (!Peer::ms_Singleton->_initialize())
        {
            delete Peer::ms_Singleton;
            Peer::ms_Singleton = 0;
            return false;
        }
    }

    assert(Peer::ms_Singleton != 0);

    return Peer::ms_Singleton;
}	

//-------------------------------------------------------------------------------------
IPeer* IPeer::getPeer()
{
    return Peer::ms_Singleton;
}

//-------------------------------------------------------------------------------------
Peer::Peer(const char* appPath, int argc, char** argv) :
    BasicThread("Peer"),
    mInitialized(false),
    mHost("localhost"),
    mPort(8550),
    mVerbosity(0),
    mP2NServer(0)
{
    assert(Peer::ms_Singleton == 0);
    if (appPath != 0)
        mAppPath = appPath;

    for (int iarg=1; iarg < argc; iarg++)
    {
        if ((strstr(argv[iarg], "-h") != 0) && (argc > iarg+1))
        {
            iarg++;
            mHost = argv[iarg];
            continue;
        }
        if ((strstr(argv[iarg], "-p") != 0) && (argc > iarg+1))
        {
            iarg++;
            mPort = atoi(argv[iarg]);
            continue;
        }
        if ((strstr(argv[iarg], "-v") != 0) && (argc > iarg+1))
        {
            iarg++;
            mVerbosity = atoi(argv[iarg]);
            continue;
        }
    }

    mPhysicsEngineManager = new PhysicsEngineManager();
    mNodeManager = new NodeManager();

    mConnectionsCount = 0;
}

//-------------------------------------------------------------------------------------
Peer::~Peer()
{
    assert(Peer::ms_Singleton == this);
    Peer::ms_Singleton = 0;

    delete mNodeManager;
    delete mPhysicsEngineManager;
}

//-----------------------------------------------------------------------
void Peer::addTimeListener(TimeListener* newListener)
{
	// Check if the specified listener is scheduled for removal
	std::set<TimeListener*>::iterator i = mRemovedTimeListeners.find(newListener);

	// If yes, cancel the removal. Otherwise add it to other listeners.
	if (i != mRemovedTimeListeners.end())
		mRemovedTimeListeners.erase(*i);
	else
		mTimeListeners.insert(newListener); // Insert, unique only (set)
}

//-----------------------------------------------------------------------
void Peer::removeTimeListener(TimeListener* oldListener)
{
    // Remove, 1 only (set)
    mRemovedTimeListeners.insert(oldListener);
}

//-------------------------------------------------------------------------------------
bool Peer::destroy()
{
    _finalize();

    delete this;

    return true;
}

//-----------------------------------------------------------------------
void Peer::run()
{
    float tickDuration = 1.0f/60.0f;
    mResetTime = true;
    while (!mStopRequested)
    {
        Platform::sleep(tickDuration*1000.0f);

        if (mNodeManager != 0)
            mNodeManager->update();

        if (!_fireTick())
        {
            break;
        }
    }
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode Peer::login(const std::string& xmlParamsStr, NodeId& nodeId, std::string& xmlRespStr)
{
    nodeId.clear();
    xmlRespStr.clear();

    TiXmlDocument xmlDoc;
    xmlDoc.Parse(xmlParamsStr.c_str());
    // check for errors
    if (xmlDoc.Error() || (strcmp(xmlDoc.RootElement()->Value(), "solipsis") != 0))
    {
        std::stringstream errorStr;
        errorStr << "Invalid parameters ! xmlDoc error(" << xmlDoc.ErrorId() << ", " << xmlDoc.ErrorDesc() << ")";
        xmlRespStr = errorStr.str();
        return IP2NClient::RCError;
    }
    XmlLogin xmlLogin;
    if (!xmlLogin.fromXmlElt(xmlDoc.RootElement()))
    {
        xmlRespStr = "Invalid parameters !";
        return IP2NClient::RCError;
    }

    AvatarNode* avatarNode = mNodeManager->login(&xmlLogin);
    nodeId = avatarNode->getNodeId();
    Entity::EntityMap& ownedEntities = avatarNode->getOwnedEntities();
    std::stringstream s;
    s << "<solipsis><entities>";
    for (Entity::EntityMap::iterator entity = ownedEntities.begin(); entity != ownedEntities.end(); ++entity)
        s << "<entity uid=" << entity->first << " />";
    s << "</entities></solipsis>";
    xmlRespStr = s.str();

    /////////////////////////////////////
    mConnectionsCount++;
    if (mConnectionsCount == 1)
    {
        mState = -1;
    }
    /////////////////////////////////////

    return IP2NClient::RCOk;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode Peer::logout(NodeId& nodeId)
{
    mNodeManager->logout(nodeId);
    /////////////////////////////////////
    mConnectionsCount--;
    /////////////////////////////////////

    return IP2NClient::RCOk;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode Peer::handleEvt(const NodeId& nodeId, std::string& xmlRespStr)
{
    xmlRespStr.clear();

    time_t now;

    time(&now);
    XmlEvt* evt = mNodeManager->getNextEvtToHandle(nodeId);
    if (evt != 0)
    {
        std::stringstream s;
        s << "<solipsis>" << evt->toXmlString() << "</solipsis>";
        mNodeManager->freeEvt(nodeId, evt);
        xmlRespStr = s.str();
    }

    return (xmlRespStr.empty() ? IP2NClient::RCNoEvt : IP2NClient::RCOk);
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode Peer::sendEvt(const NodeId& nodeId, const std::string& xmlEvtStr, std::string& xmlRespStr)
{
    xmlRespStr.clear();

    TiXmlDocument xmlDoc;
    xmlDoc.Parse(xmlEvtStr.c_str());
    // check for errors
    if (xmlDoc.Error() || (strcmp(xmlDoc.RootElement()->Value(), "solipsis") != 0))
    {
        std::stringstream errorStr;
        errorStr << "Invalid parameters ! xmlDoc error(" << xmlDoc.ErrorId() << ", " << xmlDoc.ErrorDesc() << ")";
        xmlRespStr = errorStr.str();
        return IP2NClient::RCError;
    }

    XmlEvt xmlEvt;
    if (!xmlEvt.fromXmlElt(xmlDoc.RootElement()))
    {
        xmlRespStr = "Invalid parameters !";
        return IP2NClient::RCError;
    }
    if (!mNodeManager->processEvt(nodeId, xmlEvt, xmlRespStr))
        return IP2NClient::RCError;

    return IP2NClient::RCOk;
}

//-------------------------------------------------------------------------------------
bool Peer::_initialize()
{
    OgreHelpers::initialize();

#ifdef PHYSICSPLUGINS
    PhysicsEngineManager::getSingleton().selectEngine("PhysX engine");
//    PhysicsEngineManager::getSingleton().selectEngine("ODE engine");
//    PhysicsEngineManager::getSingleton().selectEngine("Tokamak engine");
    if (PhysicsEngineManager::getSingleton().getSelectedEngine() == 0)
        return false;
    PhysicsEngineManager::getSingleton().getSelectedEngine()->init();
#endif

    // create and start the Node server
    mP2NServer = IP2NServer::createServer(this, mHost, mPort, mVerbosity, "nthreads=4");
	mP2NServer->init();
	if (!mP2NServer->start())
        return false;

    mInitialized = true;

    // start this thread
    return start();
}

//-------------------------------------------------------------------------------------
void Peer::_finalize()
{
    // stop this thread
    stop();
    // wait end of thread
    finalize();

    delete mP2NServer;

#ifdef PHYSICSPLUGINS
    PhysicsEngineManager::getSingleton().getSelectedEngine()->shutdown();
#endif

    OgreHelpers::shutdown();

    mInitialized = false;
}

//-----------------------------------------------------------------------
bool Peer::_fireTick(Real timeSinceLastTick)
{
    // Remove all marked listeners
    std::set<TimeListener*>::iterator i;
    for (i = mRemovedTimeListeners.begin(); i != mRemovedTimeListeners.end(); i++)
    {
        mTimeListeners.erase(*i);
    }
    mRemovedTimeListeners.clear();

    // Tell all listeners
    for (i= mTimeListeners.begin(); i != mTimeListeners.end(); ++i)
    {
        if (!(*i)->tick(timeSinceLastTick))
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------
bool Peer::_fireTick()
{
    unsigned long now = OgreHelpers::getSingleton().getTimer()->getMilliseconds();
    if (mResetTime)
    {
        mLastTickTime = now;
        mResetTime = false;
    }
    unsigned long elapsedTime = now - mLastTickTime;
    mLastTickTime = now;

    return _fireTick((Real)(elapsedTime)/1000.0f);
}

//-----------------------------------------------------------------------

} // namespace Solipsis
