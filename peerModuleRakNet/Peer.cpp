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
    mPort(8880),
    mMediaCachePath(""),
    mVerbosity(0),
    mP2NServer(0),
    mPhysicsScene(0),
    mPhysicsMutex(PTHREAD_MUTEX_INITIALIZER),
    mNodeId(""),
    mName(""),
    mAvatarNode(0),
    mRakNetConnection(&mConnectionFactory, false, "localhost", 8660),
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
    }

    // Retrieve Media/Cache path
    if (mMediaCachePath.empty())
        mMediaCachePath = CommonTools::IO::getCWD() + "\\" + CommonTools::IO::retrieveRelativePathByDescendingCWD(std::string("Media\\cache"));

    mPhysicsEngineManager = new PhysicsEngineManager();
}

//-------------------------------------------------------------------------------------
Peer::~Peer()
{
    assert(Peer::ms_Singleton == this);
    Peer::ms_Singleton = 0;

    delete mPhysicsEngineManager;
}

#ifdef PHYSICSPLUGINS
//-------------------------------------------------------------------------------------
IPhysicsScene* Peer::getPhysicsScene()
{
#ifdef PHYSICSPLUGINS
    pthread_mutex_lock(&mPhysicsMutex);
    if (mPhysicsScene == 0)
    {
        IPhysicsEngine* engine = PhysicsEngineManager::getSingleton().getSelectedEngine();
        if (engine == 0)
        {
            pthread_mutex_unlock(&mPhysicsMutex);
            throw Exception(Exception::ERR_INTERNAL_ERROR,
            "No physics engine selected !",
            "Peer::getPhysicsScene");
        }
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::getPhysicsScene() creating physics scene with engine:%s", engine->getName().c_str());
        // Create the physical scene
        mPhysicsScene = engine->createScene();
        if (!mPhysicsScene->create())
        {
            pthread_mutex_unlock(&mPhysicsMutex);
            throw Exception(Exception::ERR_INTERNAL_ERROR,
            "Unable to create the PhysX scene !",
            "Peer::getPhysicsScene");
        }
    }
    pthread_mutex_unlock(&mPhysicsMutex);
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
        RakPeerInterface *RakPeer = mRakNetConnection.getRakPeer();
        if (RakPeer != 0)
        {
            // Get received packets
            Packet *packet;
            for (packet = RakPeer->Receive(); packet; RakPeer->DeallocatePacket(packet), packet = RakPeer->Receive())
            {
                switch (packet->data[0])
                {
                case ID_CONNECTION_ATTEMPT_FAILED:
                    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::run() ID_CONNECTION_ATTEMPT_FAILED from %s", packet->systemAddress.ToString());
                    break;
                case ID_NO_FREE_INCOMING_CONNECTIONS:
                    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::run() ID_NO_FREE_INCOMING_CONNECTIONS from %s", packet->systemAddress.ToString());
                    break;
                case ID_CONNECTION_REQUEST_ACCEPTED:
                    {
                        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::run() ID_CONNECTION_REQUEST_ACCEPTED from %s", packet->systemAddress.ToString());
                        // Set notifications interval for big file transfer
                        RakPeer->SetSplitMessageProgressInterval(RAKNETCONNECTION_DEFAULT_SPLITMSGPROGRESSINTERVAL_BYTES/RakPeer->GetMTUSize(packet->systemAddress));
                        // Store the server address
                        mRakNetConnection.setServerSystemAddress(packet->systemAddress);
                        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::run() ServerSystemAddress:%s", mRakNetConnection.getServerSystemAddress().ToString());
                        createAvatarNode();
                    }
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
                        mRakNetConnection.getCacheManager()->sendFile(packet->systemAddress, fileListTransferSetID, filename, version);
                    }
                    break;
                case RakNetConnection::ID_ACTION_ON_ENTITY:
                    {
                        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::run() RakNetConnection::ID_ACTION from %s", packet->systemAddress.ToString());
                        BitStream bitStream(packet->data + 1, packet->length - 1, false);
                        mAvatarNode->onActionOnEntity(&bitStream);
                    }
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
            result = mAvatarNode->processEvt(xmlEvt, xmlRespStr);
        }
        pthread_mutex_unlock(&mEvtsToProcessMutex);

        CommonTools::System::sleep(tickDuration*1000.0f);

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

    mName = xmlLogin.getUsername();
    mNodeId = xmlLogin.getNodeId();
    nodeId = mNodeId;
    std::string worldHost = xmlLogin.getWorldHost();
    unsigned short worldPort = xmlLogin.getWorldPort();

    if (mName.empty() || mNodeId.empty() || worldHost.empty())
    {
        xmlRespStr = "Invalid parameters !";
        return IP2NClient::RCError;
    }

    LOGHANDLER_LOGF(LogHandler::VL_INFO, "Peer::login() Connecting on %s:%d ...", worldHost.c_str(), worldPort);
    mRakNetConnection.setHost(worldHost);
    mRakNetConnection.setPort(worldPort);
    if (!mRakNetConnection.connectClient())
    {
        xmlRespStr = "Unable to connect to world host !";
        return IP2NClient::RCError;
    }

    return IP2NClient::RCOk;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode Peer::logout(NodeId& nodeId)
{
    LOGHANDLER_LOGF(LogHandler::VL_INFO, "Peer::logout()");

    // Save the avatar node
    if (mAvatarNode != 0)
        if (!mAvatarNode->saveNode(mMediaCachePath))
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::logout() Unable to save avatar node with nodeId:%s !", mAvatarNode->getNodeId().c_str());

    Entity::cleanUpEntities();
  
    if (mAvatarNode != 0)
    {
        delete mAvatarNode;
        mAvatarNode = 0;
    }

#ifdef PHYSICSPLUGINS
    pthread_mutex_lock(&mPhysicsMutex);
    if (mPhysicsScene != 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::logout() destroying physics scene");
        PhysicsEngineManager::getSingleton().getSelectedEngine()->destroyScene(mPhysicsScene);
        mPhysicsScene = 0;
    }
    pthread_mutex_unlock(&mPhysicsMutex);
#endif

    pthread_mutex_lock(&mRakNetMutex);
    LOGHANDLER_LOGF(LogHandler::VL_INFO, "Peer::logout() Disconnecting from %s ...", mRakNetConnection.getServerSystemAddress().ToString());
    mRakNetConnection.getRakPeer()->CloseConnection(mRakNetConnection.getServerSystemAddress(), true);
    pthread_mutex_unlock(&mRakNetMutex);

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
    RefCntPoolPtr<XmlEvt> xmlEvt = RefCntPoolPtr<XmlEvt>::nullPtr;
#else
    XmlEvt* xmlEvt = 0;
#endif

#ifdef POOL
    if (mAvatarNode != 0)
        xmlEvt = mAvatarNode->getNextEvtToHandle();
    if (!xmlEvt.isNull())
#else
    if (mAvatarNode != 0)
        xmlEvt = mAvatarNode->getNextEvtToHandle();
    if (xmlEvt != 0)
#endif
    {
        std::stringstream s;
        s << "<solipsis>" << xmlEvt->toXmlString() << "</solipsis>";
        mAvatarNode->freeEvt(xmlEvt);
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

    // Initialize the client connection
    pthread_mutex_lock(&mRakNetMutex);
    mRakNetConnection.initialize(mMediaCachePath);
    pthread_mutex_unlock(&mRakNetMutex);

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
    if (mPhysicsScene != 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::_finalize() destroying physics scene");
        PhysicsEngineManager::getSingleton().getSelectedEngine()->destroyScene(mPhysicsScene);
        mPhysicsScene = 0;
    }
    PhysicsEngineManager::getSingleton().getSelectedEngine()->shutdown();
#endif

    OgreHelpers::shutdown();

    // Finalizing the connection
    pthread_mutex_lock(&mRakNetMutex);
    mRakNetConnection.finalize();
    pthread_mutex_unlock(&mRakNetMutex);

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
    pthread_mutex_lock(&mPhysicsMutex);
    // Step physics part 1
    if (mPhysicsScene != 0)
        mPhysicsScene->preStep(timeSinceLastTick);
#endif

    // Tell all listeners
    for (i= mTimeListeners.begin(); i != mTimeListeners.end(); ++i)
    {
        if (!(*i)->tick(timeSinceLastTick))
        {
#ifdef PHYSICSPLUGINS
            pthread_mutex_unlock(&mPhysicsMutex);
#endif
            return false;
        }
    }

#ifdef PHYSICSPLUGINS
    // Step physics part 2
    if (mPhysicsScene != 0)
        mPhysicsScene->postStep();
    pthread_mutex_unlock(&mPhysicsMutex);
#endif

    return true;
}

//-----------------------------------------------------------------------
bool Peer::_fireTick()
{
    unsigned long now = Root::getSingleton().getTimer()->getMilliseconds();
    if (mResetTime)
    {
        mLastTickTime = now;
        mResetTime = false;
    }
    unsigned long elapsedTime = now - mLastTickTime;
    mLastTickTime = now;

    return _fireTick((Real)(elapsedTime)/1000.0f);
}

//-------------------------------------------------------------------------------------
void Peer::createAvatarNode()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::createAvatarNode() avatarNodeId:%s", mNodeId.c_str());

    mAvatarNode = new AvatarNode();
    mAvatarNode->setNodeId(mNodeId);
    mAvatarNode->setName(mName);

    // Load the avatar node
    bool avatarLoaded = mAvatarNode->loadNode(mMediaCachePath);
    if (!avatarLoaded)
    {
        // Create the avatar node
        EntityUID AvatarEntityUid;
        AvatarEntityUid = mAvatarNode->getNodeId() + "_00000000";
        // Randomize the character
        CommonTools::IO::FilenameVector filenames;
        CommonTools::IO::getFilenames(mMediaCachePath, filenames);
        CommonTools::IO::FilenameVector safFilenames;
        for (CommonTools::IO::FilenameVector::const_iterator it = filenames.begin(); it != filenames.end(); ++it)
            if (it->find(".saf") == it->length() - 4)
                safFilenames.push_back(*it);
        int safIdx = time(NULL)%(int)safFilenames.size();
        std::string xmlAvatarStr = "\
<entity uid=\"" + AvatarEntityUid + "\" owner=\"" + mAvatarNode->getNodeId() + "\" type=\"0\" name=\"" + mAvatarNode->getName() + "\" version=\"00000000\">\
 <flags bitmask=\"" + XmlHelpers::convertEntityFlagsToHexString(EFNone) + "\" />\
 <position x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
 <orientation x=\"0.0\" y=\"0.0\" z=\"0.0\" w=\"1.0\" />\
 <aabb>\
  <min x=\"-0.82447118\" y=\"-0.013709042\" z=\"-0.64538133\" />\
  <max x=\"0.82353306\" y=\"1.4500649\" z=\"0.69156337\" />\
 </aabb>\
 <content>\
  <lod level=\"0\">\
   <files>\
    <file name=\"" + safFilenames[safIdx] + "\" version=\"00000000\" />\
   </files>\
  </lod>\
 </content>\
</entity>\
";
        TiXmlDocument xmlAvatarDoc;
        xmlAvatarDoc.Parse(xmlAvatarStr.c_str());
        Entity* avatarEntity = new Entity();
        avatarEntity->setSystemAddress(mRakNetConnection.getMySystemAddress());
        avatarEntity->getXmlEntity()->fromXmlElt(xmlAvatarDoc.RootElement());
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Peer::createAvatarNode() initializing avatar node name:%s with character:%s", avatarEntity->getXmlEntity()->getName().c_str(), safFilenames[safIdx].c_str());
        mAvatarNode->setEntity(avatarEntity);
        avatarEntity->addFilesInCacheManager();
        avatarEntity->mDirty = true;
        RakNetEntity::addEntity(avatarEntity);
        /// Client can serialize
        avatarEntity->addReplicaFlags(RakNetEntity::RFSerializationAuthorized);
        // Entity managed by the Replica2 plugin
        avatarEntity->SetReplicaManager(mRakNetConnection.getReplicaManager());
        // Send out this new entity to all systems
        avatarEntity->BroadcastConstruction();
    }
}

//-------------------------------------------------------------------------------------
AvatarNode* Peer::getAvatarNode()
{
    return mAvatarNode;
}

//-------------------------------------------------------------------------------------
Entity* Peer::loadEntity(TiXmlElement* entityElt)
{
    Entity* entity = new Entity();
    entity->setSystemAddress(mRakNetConnection.getMySystemAddress());
#ifdef POOL
    RefCntPoolPtr<XmlEntity> xmlEntity = entity->getXmlEntity();
#else
    XmlEntity* xmlEntity = entity->getXmlEntity();
#endif
    xmlEntity->fromXmlElt(entityElt);
    entity->addFilesInCacheManager();
    entity->mDirty = true;
    RakNetEntity::addEntity(entity);
    /// Client can serialize
    entity->addReplicaFlags(RakNetEntity::RFSerializationAuthorized);
    // Entity managed by the Replica2 plugin
    entity->SetReplicaManager(mRakNetConnection.getReplicaManager());
    // Send out this new entity to all systems
    entity->BroadcastConstruction();

    return entity;
}

//-----------------------------------------------------------------------

} // namespace Solipsis
