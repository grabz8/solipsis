/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Peer.h"
#include "OgreHelpers.h"
#include "CTSystem.h"
#include "AvatarNode.h"
#include <CTIO.h>

using namespace RakNet;
using namespace Ogre;
using namespace CommonTools;

namespace Solipsis {

RakNetConnection *RakNetConnection::ms_Singleton = 0;

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
        Peer::ms_Singleton = new Peer(appPath, argc, argv);

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
    mMediaCachePath(""),
    mVerbosity(0),
    mP2NServer(0),
    mPhysicsScene(0),
    mNodeId("00000001"),
    mRakNetHost("localhost"),
    mRakNetPort(8660),
    mRakNetMutex(PTHREAD_MUTEX_INITIALIZER),
    mEvtsToProcessMutex(PTHREAD_MUTEX_INITIALIZER)
{
    assert(Peer::ms_Singleton == 0);

    CommonTools::LogHandler::setLogHandler(&mOgreLogger);
    CommonTools::LogHandler::getLogHandler()->setVerbosityLevel(CommonTools::LogHandler::VL_DEBUG);

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
        if ((strstr(argv[iarg], "-m") != 0) && (argc > iarg+1))
        {
            iarg++;
            mMediaCachePath = argv[iarg];
            continue;
        }
        if ((strstr(argv[iarg], "-rh") != 0) && (argc > iarg+1))
        {
            iarg++;
            mRakNetHost = argv[iarg];
            continue;
        }
        if ((strstr(argv[iarg], "-rp") != 0) && (argc > iarg+1))
        {
            iarg++;
            mRakNetPort = atoi(argv[iarg]);
            continue;
        }
        if ((strstr(argv[iarg], "-id") != 0) && (argc > iarg+1))
        {
            iarg++;
            mNodeId = argv[iarg];
            continue;
        }
    }

    // Retrieve Media/Cache path
    if (mMediaCachePath.empty())
        mMediaCachePath = CommonTools::IO::getCWD() + "\\" + CommonTools::IO::retrieveRelativePathByDescendingCWD(std::string("Media\\cache"));

    mPhysicsEngineManager = new PhysicsEngineManager();
    mNodeManager = new NodeManager();
}

//-------------------------------------------------------------------------------------
Peer::~Peer()
{
    assert(Peer::ms_Singleton == this);
    Peer::ms_Singleton = 0;

    delete mNodeManager;
    delete mPhysicsEngineManager;
}

#ifdef PHYSICSPLUGINS
//-------------------------------------------------------------------------------------
IPhysicsScene* Peer::getPhysicsScene()
{
#ifdef PHYSICSPLUGINS
    if (mPhysicsScene == 0)
    {
        IPhysicsEngine* engine = PhysicsEngineManager::getSingleton().getSelectedEngine();
        if (engine == 0)
            throw Exception(Exception::ERR_INTERNAL_ERROR,
            "No physics engine selected !",
            "Peer::getPhysicsScene");
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::getPhysicsScene() creating physics scene with engine:%s", engine->getName().c_str());
        // Create the physical scene
        mPhysicsScene = engine->createScene();
        if (!mPhysicsScene->create())
        {
            throw Exception(Exception::ERR_INTERNAL_ERROR,
            "Unable to create the PhysX scene !",
            "Peer::getPhysicsScene");
        }
    }
#endif

    return mPhysicsScene;
}
#endif

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
bool Peer::initialize(IPeerRenderSystemLock* renderSystemLock)
{
    mRenderSystemLock = renderSystemLock;
    return _initialize();
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
        pthread_mutex_lock(&mRakNetMutex);
        if (mRakNetConnection.mRakPeer != 0)
        {
            // Get received packets
            Packet *packet;
            for (packet = mRakNetConnection.mRakPeer->Receive(); packet; mRakNetConnection.mRakPeer->DeallocatePacket(packet), packet = mRakNetConnection.mRakPeer->Receive())
            {
                switch (packet->data[0])
                {
                case ID_CONNECTION_ATTEMPT_FAILED:
                    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::run() ID_CONNECTION_ATTEMPT_FAILED");
                    break;
                case ID_NO_FREE_INCOMING_CONNECTIONS:
                    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::run() ID_NO_FREE_INCOMING_CONNECTIONS");
                    break;
                case ID_CONNECTION_REQUEST_ACCEPTED:
                    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::run() ID_CONNECTION_REQUEST_ACCEPTED");
                    // Set notifications interval for big file transfer
                    mRakNetConnection.mRakPeer->SetSplitMessageProgressInterval(RAKNETCONNECTION_DEFAULT_SPLITMSGPROGRESSINTERVAL_BYTES/mRakNetConnection.mRakPeer->GetMTUSize(packet->systemAddress));
                    // Store the server address
                    mRakNetConnection.mServerSystemAddress = packet->systemAddress;
                    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::run() mRakNetServerSystemAddress:%s", mRakNetConnection.mServerSystemAddress.ToString());
                    break;
                case RakNetConnection::ID_REQUESTING_FILETRANSFER:
                    {
                        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::run() RakNetConnection::ID_REQUESTING_FILETRANSFER from %s", packet->systemAddress.ToString());
                        BitStream bitStream(packet->data, packet->length, false);
                        bitStream.IgnoreBytes(1);
                        unsigned short fileListTransferSetID;
                        bitStream.Read(fileListTransferSetID);
                        std::string filename;
                        RakNetConnection::DeserializeString(&bitStream, filename);
                        FileVersion version;
                        bitStream.Read(version);
                        mRakNetConnection.mCacheManager.sendFile(packet->systemAddress, fileListTransferSetID, filename, version);
                    }
                    break;
                case RakNetConnection::ID_ACTION_ON_ENTITY:
                    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::run() RakNetConnection::ID_ACTION from %s", packet->systemAddress.ToString());
                    BitStream bitStream(packet->data + 1, packet->length - 1, false);
                    mNodeManager->onActionOnEntity(&bitStream);
                    break;
                }
            }
        }
        pthread_mutex_unlock(&mRakNetMutex);

        // Process events in the same thread
        pthread_mutex_lock(&mEvtsToProcessMutex);
        while (!mEvtsToProcessList.empty())
        {
#ifdef POOL
            RefCntPoolPtr<XmlEvt> xmlEvt(RefCntPoolPtr<XmlEvt>::nullPtr);
#else
            XmlEvt* xmlEvt = 0;
#endif
            xmlEvt = mEvtsToProcessList.front();
            mEvtsToProcessList.pop_front();
            bool result;
            std::string xmlRespStr;
            result = mNodeManager->processEvt(mNodeId, xmlEvt, xmlRespStr);
        }
        pthread_mutex_unlock(&mEvtsToProcessMutex);

        CommonTools::System::sleep(tickDuration*1000.0f);

        if (mNodeManager != 0)
            mNodeManager->update();

        if (!_fireTick())
        {
            break;
        }
    }
}

//-------------------------------------------------------------------------------------
void Peer::P2NServerLogger::logMessage(const std::string& message)
{
    OGRE_LOG(message);
}

//-------------------------------------------------------------------------------------
void Peer::PhysicsEngineLogger::logMessage(const std::string& message)
{
    OGRE_LOG(message);
}

//-------------------------------------------------------------------------------------
void Peer::OgreLogger::log(int level, const char* msg)
{ 
    if (level > mVerbosity) return;
    OGRE_LOG(std::string(msg));
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

    nodeId = mNodeId;
    mName = xmlLogin.getUsername();

    pthread_mutex_lock(&mRakNetMutex);

    // We are the client
    mRakNetConnection.mServer = false;
    // Get 1 instance of the RakNet peer interface
    mRakNetConnection.mRakPeer = RakNetworkFactory::GetRakPeerInterface();
    // ObjectMemberRPC and ReplicaManager2 require that you call SetNetworkIDManager()
    mRakNetConnection.mRakPeer->SetNetworkIDManager(&mRakNetConnection.mNetworkIdManager);
    // The network ID authority is the system that creates the common numerical identifier used to lookup pointers.
    // For client/server this is the server
    // For peer to peer this would be true on every system, and you would also have call NetworkID::peerToPeerMode=true;
    mRakNetConnection.mNetworkIdManager.SetIsNetworkIDAuthority(mRakNetConnection.mServer);
    // Start RakNet
    mRakNetConnection.mSocketDescriptor.port = 0;
    mRakNetConnection.mRakPeer->Startup(1, 100, &mRakNetConnection.mSocketDescriptor, 1);
    // Attach the ReplicaManager2 plugin
    mRakNetConnection.mRakPeer->AttachPlugin(&mRakNetConnection.mReplicaManager);
    // Register our custom connection factory
    mRakNetConnection.mReplicaManager.SetConnectionFactory(&mRakNetConnection.mConnectionFactory);
    // Attach the FileListTransfer plugin
    mRakNetConnection.mRakPeer->AttachPlugin(&mRakNetConnection.mFileListTransfer);

	// Here I use the string table class to efficiently send strings I know in advance.
	// The encoding is used in in Replica2::SerializeConstruct
	// The decoding is used in in Connection_RM2::Construct
	// The stringTable class will also send strings that weren't registered but this just falls back to the stringCompressor and wastes 1 extra bit on top of that
	// 2nd parameter of false means a static string so it's not necessary to copy it
	StringTable::Instance()->AddString("AvatarNode", false);
	StringTable::Instance()->AddString("SiteNode", false);
	StringTable::Instance()->AddString("Entity", false);

    // Initializing the cache
    mRakNetConnection.mCacheManager.initialize(mMediaCachePath);
    LOGHANDLER_LOGF(LogHandler::VL_INFO, "Peer::login() Initializing cache manager");

    mRakNetConnection.mRakPeer->Connect(mRakNetHost.c_str(), mRakNetPort, 0, 0, 0);
    LOGHANDLER_LOGF(LogHandler::VL_INFO, "Peer::login() Connecting ...");

    pthread_mutex_unlock(&mRakNetMutex);

    return IP2NClient::RCOk;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode Peer::logout(NodeId& nodeId)
{
    pthread_mutex_lock(&mRakNetMutex);
    mRakNetConnection.mRakPeer->Shutdown(100, 0);
    RakNetworkFactory::DestroyRakPeerInterface(mRakNetConnection.mRakPeer);
    mRakNetConnection.mRakPeer = 0;
    pthread_mutex_unlock(&mRakNetMutex);

    mNodeManager->onLostNode(mNodeId);

    // Finalizing the cache
    mRakNetConnection.mCacheManager.finalize();

    return IP2NClient::RCOk;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode Peer::handleEvt(const NodeId& nodeId, std::string& xmlRespStr)
{
    xmlRespStr.clear();

#ifdef HANDLEVTSYNC
    while (true)
    {
#endif

#ifdef POOL
    RefCntPoolPtr<XmlEvt> xmlEvt = mNodeManager->getNextEvtToHandle(mNodeId);
    if (!xmlEvt.isNull())
#else
    XmlEvt* xmlEvt = mNodeManager->getNextEvtToHandle(mNodeId);
    if (xmlEvt != 0)
#endif
    {
        std::stringstream s;
        s << "<solipsis>" << xmlEvt->toXmlString() << "</solipsis>";
#ifdef POOL
        mNodeManager->freeEvt(mNodeId, xmlEvt);
#else
        mNodeManager->freeEvt(mNodeId, xmlEvt);
#endif
        xmlRespStr = s.str();
    }

#ifdef HANDLEVTSYNC
        if (!xmlRespStr.empty()) break;
    }
#endif

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

#ifdef POOL
    RefCntPoolPtr<XmlEvt> xmlEvt;
#else
    XmlEvt* xmlEvt = new XmlEvt();
#endif
    if (!xmlEvt->fromXmlElt(xmlDoc.RootElement()))
    {
        xmlRespStr = "Invalid parameters !";
        return IP2NClient::RCError;
    }

    pthread_mutex_lock(&mEvtsToProcessMutex);
    mEvtsToProcessList.push_back(xmlEvt);
    pthread_mutex_unlock(&mEvtsToProcessMutex);

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
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "No physics engine selected !");
        return false;
    }
    PhysicsEngineManager::getSingleton().getSelectedEngine()->init();
    PhysicsEngineManager::getSingleton().getSelectedEngine()->setLogger(&mPhysicsEngineLogger);
#endif

    // create and start the Node server
    mP2NServer = IP2NServer::createServer(this, mHost, mPort, mVerbosity, "nthreads=8 persistent=1");
    mP2NServer->setLogger(&mP2NServerLogger);
	mP2NServer->init();
	if (!mP2NServer->start())
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Unable to start the Peer/Navigator server !");
        return false;
    }

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
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::_finalize() destroying physics scene");
    if (mPhysicsScene != 0)
        PhysicsEngineManager::getSingleton().getSelectedEngine()->destroyScene(mPhysicsScene);
    mPhysicsScene = 0;
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

#ifdef PHYSICSPLUGINS
    // Step physics part 1
    if (mPhysicsScene != 0)
        mPhysicsScene->preStep(timeSinceLastTick);
#endif

    // Tell all listeners
    for (i= mTimeListeners.begin(); i != mTimeListeners.end(); ++i)
    {
        if (!(*i)->tick(timeSinceLastTick))
            return false;
    }

#ifdef PHYSICSPLUGINS
    // Step physics part 2
    if (mPhysicsScene != 0)
        mPhysicsScene->postStep();
#endif

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
