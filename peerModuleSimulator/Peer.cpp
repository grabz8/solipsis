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
    mSceneDemoLoaded(""),
    mMediaCachePath(""),
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
        if ((strstr(argv[iarg], "-s") != 0) && (argc > iarg+1))
        {
            iarg++;
            mSceneDemoLoaded = argv[iarg];
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
    mNodeManager = new NodeManager();

    mConnectionsCount = 0;
    for(int c=0;c<MAX_CONNECTIONS;c++)
        mConnections[c] = false;
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
void Peer::getConnectionsString(std::string& str)
{
    bool comma = false;
    str.clear();
    for(int c=0;c<MAX_CONNECTIONS;c++)
        if (mConnections[c])
        {
            if (comma) str += ", ";
            comma = true;
            str += Ogre::StringConverter::toString(c);
        }
}

//-----------------------------------------------------------------------
int Peer::allocConnection()
{
    for(int c=0;c<MAX_CONNECTIONS;c++)
        if (!mConnections[c])
        {
            mConnections[c] = true;
            return c;
        }
    return -1;
}

//-----------------------------------------------------------------------
bool Peer::releaseConnection(int connection)
{
    if (!mConnections[connection])
        return false;
    mConnections[connection] = false;
    return true;
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
bool Peer::initialize()
{
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
    if (avatarNode == 0)
    {
        xmlRespStr = "Unable to create avatarNode !";
        return IP2NClient::RCError;
    }
    nodeId = avatarNode->getNodeId();

    /////////////////////////////////////
    mConnectionsCount++;
    std::string connections;
    getConnectionsString(connections);
    OGRE_LOG("Peer::login() mConnectionsCount=" + Ogre::StringConverter::toString(mConnectionsCount) + " (" + connections + ")");
    /////////////////////////////////////

    return IP2NClient::RCOk;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode Peer::logout(NodeId& nodeId)
{
    mNodeManager->logout(nodeId);
    /////////////////////////////////////
    mConnectionsCount--;
    std::string connections;
    getConnectionsString(connections);
    OGRE_LOG("Peer::logout() mConnectionsCount=" + Ogre::StringConverter::toString(mConnectionsCount) + " (" + connections + ")");
    /////////////////////////////////////

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
    RefCntPoolPtr<XmlEvt> xmlEvt = mNodeManager->getNextEvtToHandle(nodeId);
    if (!xmlEvt.isNull())
#else
    XmlEvt* xmlEvt = mNodeManager->getNextEvtToHandle(nodeId);
    if (xmlEvt != 0)
#endif
    {
        std::stringstream s;
        s << "<solipsis>" << xmlEvt->toXmlString() << "</solipsis>";
#ifdef POOL
        mNodeManager->freeEvt(nodeId, xmlEvt);
#else
        mNodeManager->freeEvt(nodeId, xmlEvt);
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
    if (!mNodeManager->processEvt(nodeId, xmlEvt, xmlRespStr))
        return IP2NClient::RCError;

    return IP2NClient::RCOk;
}

//-------------------------------------------------------------------------------------
void Peer::PhysicsEngineLogger::logMessage(const std::string& message)
{
    OGRE_LOG(message);
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
        OGRE_LOG("No physics engine selected !");
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
        OGRE_LOG("Unable to start the Peer/Navigator server !");
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
