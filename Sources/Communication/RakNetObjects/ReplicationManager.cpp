#include "ReplicationManager.h"
#include "RakNetConnection.h"
#include <CTLog.h>

using namespace CommonTools;

namespace Solipsis {

//-------------------------------------------------------------------------------------
template<class T>
static void computeSetsDelta(const std::set<T>& s1, const std::set<T>& s2, std::set<T>& s1Exclusive, std::set<T>& s2Exclusive)
{
    s1Exclusive.clear();
    s2Exclusive.clear();
    std::set<T>::const_iterator it1 = s1.begin();
    std::set<T>::const_iterator it2 = s2.begin();
    while ((it1 != s1.end()) && (it2 != s2.end()))
    {
        if (*it1 < *it2)
        {
            s1Exclusive.insert(*it1);
            it1++;
            continue;
        }
        if (*it1 > *it2)
        {
            s2Exclusive.insert(*it2);
            it2++;
            continue;
        }
        it1++;
        it2++;
    }
    for (;it1 != s1.end(); ++it1)
        s1Exclusive.insert(*it1);
    for (;it2 != s2.end(); ++it2)
        s2Exclusive.insert(*it2);
}

//-------------------------------------------------------------------------------------
ConnectionRM::ConnectionRM()
{
    mSystemAddress = UNASSIGNED_SYSTEM_ADDRESS;
}

//-------------------------------------------------------------------------------------
ConnectionRM::~ConnectionRM()
{
}

//-------------------------------------------------------------------------------------
void ConnectionRM::setSystemAddress(SystemAddress systemAddress)
{
    mSystemAddress = systemAddress;
}

//-------------------------------------------------------------------------------------
SystemAddress ConnectionRM::getSystemAddress() const
{
    return mSystemAddress;
}

//-------------------------------------------------------------------------------------
void ConnectionRM::update(ReplicationManager* replicationManager)
{
    ReplicaSet toConstruct, toDestroy;

    // Query specific-system replicas construction
    for (ReplicaSet::const_iterator replicaIt = replicationManager->mReplicaSet2QueryConstruction.begin(); replicaIt != replicationManager->mReplicaSet2QueryConstruction.end(); ++replicaIt)
    {
        Replica *replica = *replicaIt;
        if (!replica->queryIsReady())
        {
            if (mLastConstructionSet.find(replica) != mLastConstructionSet.end())
                mNextConstructionSet.insert(replica);
            else
                mNextConstructionSet.erase(replica);
            continue;
        }
        Replica::QueryResult qr = replica->queryConstruction(this);
        if ((qr == Replica::QR_YES) || (qr = Replica::QR_ALWAYS))
            mNextConstructionSet.insert(replica);
        else
            mNextConstructionSet.erase(replica);
    }

    // Compute set of replicas to construct or to destroy
    computeSetsDelta(mNextConstructionSet, mLastConstructionSet, toConstruct, toDestroy);
#ifdef LOGRAKNET
    if (!toConstruct.empty() || !toDestroy.empty())
    {
        std::string toConstructStr, toDestroyStr;
        for (ReplicaSet::const_iterator toConstructIt = toConstruct.begin(); toConstructIt != toConstruct.end(); ++toConstructIt)
            toConstructStr += (*toConstructIt)->getUid() + " ";
        for (ReplicaSet::const_iterator toDestroyIt = toDestroy.begin(); toDestroyIt != toDestroy.end(); ++toDestroyIt)
            toDestroyStr += (*toDestroyIt)->getUid() + " ";
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "ConnectionRM::update() toConstruct=%s toDestroy=%s", toConstructStr.c_str(), toDestroyStr.c_str());
    }
#endif

    // Construct replicas
    for (ReplicaSet::const_iterator toConstructIt = toConstruct.begin(); toConstructIt != toConstruct.end(); ++toConstructIt)
        (*toConstructIt)->sendConstruction(mSystemAddress);
    // Destroy replicas
    for (ReplicaSet::const_iterator toDestroyIt = toDestroy.begin(); toDestroyIt != toDestroy.end(); ++toDestroyIt)
        (*toDestroyIt)->sendDestruction(mSystemAddress);
}

//-------------------------------------------------------------------------------------
Replica* ConnectionRM::receiveConstruction(RakNet::BitStream* replicaData, const ReplicaUid& replicaUid, SystemAddress sender, SerializationType serializationType, ReplicationManager* replicationManager)
{
    Replica *replica = 0;
    bool replicaUidCollision = false;

    // Collision (same replica already exist)
    ReplicaMap::const_iterator replicaIt = replicationManager->mReplicaMap.find(replicaUid);
    if (replicaIt != replicationManager->mReplicaMap.end())
    {
        LOGHANDLER_LOGF(LogHandler::VL_WARNING, "ConnectionRM::receiveConstruction() Collision detected, replicaUid %s already exist !", replicaUid.c_str());
        replicaUidCollision = true;
    }

    replica = construct(replicaData, sender, serializationType, replicationManager, replicaUid, replicaUidCollision);
    if (replica != 0)
        replicationManager->addReference(replica);

    return replica;
}

//-------------------------------------------------------------------------------------
Replica::Replica() :
    mReplicationManager(0),
    mLastSerializeTimeMs(0)
{
}

//-------------------------------------------------------------------------------------
Replica::~Replica()
{
    if (mReplicationManager)
        mReplicationManager->removeReference(this);
}

//-------------------------------------------------------------------------------------
void Replica::setReplicationManager(ReplicationManager* replicationManager)
{
    mReplicationManager = replicationManager;
}

//-------------------------------------------------------------------------------------
ReplicationManager* Replica::getReplicationManager() const
{
    return mReplicationManager;
}

//-------------------------------------------------------------------------------------
void Replica::sendConstruction(SystemAddress recipientAddress, SerializationType serializationType)
{
    RakNet::BitStream bs;
    SerializationContext sc(serializationType, UNASSIGNED_SYSTEM_ADDRESS, recipientAddress);
    if (serializationType == ST_UNDEFINED_REASON)
    {
        if (mReplicationManager->mIsAuthority)
            sc.serializationType = ST_SEND_CONSTRUCTION_REQUEST_TO_SERVER;
        else
            sc.serializationType = ST_SEND_CONSTRUCTION_GENERIC_TO_SYSTEM;
    }
    bs.AlignWriteToByteBoundary();
    if (serializeConstruction(&bs, &sc))
        mReplicationManager->sendConstruction(this, &bs, recipientAddress, sc.serializationType);
}

//-------------------------------------------------------------------------------------
void Replica::sendDestruction(SystemAddress recipientAddress, SerializationType serializationType)
{
    RakNet::BitStream bs;
    SerializationContext sc(serializationType, UNASSIGNED_SYSTEM_ADDRESS, recipientAddress);
    if (serializationType == ST_UNDEFINED_REASON)
        sc.serializationType = ST_SEND_DESTRUCTION_GENERIC_TO_SYSTEM;
    SystemAddressSet exclusionSet;
    bs.AlignWriteToByteBoundary();
    if (serializeDestruction(&bs, &sc))
        mReplicationManager->sendDestruction(this, &bs, recipientAddress, exclusionSet, sc.serializationType);
}

//-------------------------------------------------------------------------------------
void Replica::sendSerialize(SystemAddress recipientAddress, SerializationType serializationType)
{
    RakNet::BitStream bs;
    SerializationContext sc(serializationType, UNASSIGNED_SYSTEM_ADDRESS, recipientAddress);
    if (serializationType == ST_UNDEFINED_REASON)
        sc.serializationType = ST_SEND_SERIALIZATION_GENERIC_TO_SYSTEM;
    SystemAddressSet exclusionSet;
    bs.AlignWriteToByteBoundary();
    if (serialize(&bs, &sc))
        mReplicationManager->sendSerialize(this, &bs, recipientAddress, exclusionSet, sc.serializationType);
}

//-------------------------------------------------------------------------------------
void Replica::broadcastSerialize(SerializationContext* serializationContext)
{
	SerializationContext *usedContext = serializationContext;
    SerializationContext sc(ST_BROADCAST_SERIALIZATION_GENERIC_TO_SYSTEM, UNASSIGNED_SYSTEM_ADDRESS, UNASSIGNED_SYSTEM_ADDRESS);
    if (usedContext == 0)
        usedContext = &sc;
    RakNet::BitStream bs;
    if (serialize(&bs, usedContext))
    {
        SystemAddressSet exclusionSet;
        mReplicationManager->sendSerialize(this, &bs, UNASSIGNED_SYSTEM_ADDRESS, exclusionSet, usedContext->serializationType);
    }
}

//-------------------------------------------------------------------------------------
bool Replica::queryIsReady() const
{
    return true;
}

//-------------------------------------------------------------------------------------
Replica::QueryResult Replica::queryConstruction(ConnectionRM* connection)
{
    return QR_ALWAYS;
}

//-------------------------------------------------------------------------------------
bool Replica::queryIsConstructionAuthority() const
{
    return mReplicationManager->mIsAuthority;
}

//-------------------------------------------------------------------------------------
bool Replica::queryIsSerializationAuthority() const
{
    return mReplicationManager->mIsAuthority;
}

//-------------------------------------------------------------------------------------
void Replica::addAutoSerializeTimer(Timer::Time intervalMs, SerializationType serializationType)
{
    AutoSerializeTimer::const_iterator autoSerializeTimerIt = autoSerializeTimers.find(serializationType);
    if (autoSerializeTimerIt != autoSerializeTimers.end())
    {
        AutoSerializeEvent *ase = autoSerializeTimerIt->second;
        // Elapse this timer immediately
        if (intervalMs == 0)
        {
            ase->remainingMs = ase->intervalMs;
            RakNet::BitStream *lastWrite, *newWrite;
            if (ase->writeToBitStream1)
            {
                newWrite = &ase->lastAutoSerializeBitStream1;
                lastWrite = &ase->lastAutoSerializeBitStream2;
            }
            else
            {
                newWrite = &ase->lastAutoSerializeBitStream2;
                lastWrite = &ase->lastAutoSerializeBitStream1;
            }
            newWrite->Reset();
            onAutoSerializeTimerElapsed(serializationType, newWrite, lastWrite, false);
        }
        else
        {
            ase->intervalMs = intervalMs;
            ase->remainingMs = intervalMs;
        }
    }
    else
    {
        // New timer
        AutoSerializeEvent *ase = new AutoSerializeEvent();
        ase->serializationType = serializationType;
        ase->intervalMs = intervalMs;
        ase->remainingMs = intervalMs;
        ase->writeToBitStream1 = true;

        SerializationContext sc(ST_AUTOSERIALIZE_RESYNCH_ONLY, UNASSIGNED_SYSTEM_ADDRESS, UNASSIGNED_SYSTEM_ADDRESS);
        serialize(&ase->lastAutoSerializeBitStream1, &sc);

        autoSerializeTimers[serializationType] = ase;
    }
}

//-------------------------------------------------------------------------------------
void Replica::elapseAutoSerializeTimers(Timer::Time elapsedTimeMs, bool resynchOnly)
{
    for (AutoSerializeTimer::const_iterator autoSerializeTimerIt = autoSerializeTimers.begin(); autoSerializeTimerIt != autoSerializeTimers.end(); ++autoSerializeTimerIt)
    {
        AutoSerializeEvent *ase = autoSerializeTimerIt->second;
        if (ase->remainingMs > elapsedTimeMs)
        {
            ase->remainingMs -= elapsedTimeMs;
            continue;
        }
        // Elapsed timer
        ase->remainingMs = ase->intervalMs;
        RakNet::BitStream *lastWrite, *newWrite;
        if (ase->writeToBitStream1)
        {
            newWrite = &ase->lastAutoSerializeBitStream1;
            lastWrite = &ase->lastAutoSerializeBitStream2;
        }
        else
        {
            newWrite = &ase->lastAutoSerializeBitStream2;
            lastWrite = &ase->lastAutoSerializeBitStream1;
        }
        newWrite->Reset();
        onAutoSerializeTimerElapsed(ase->serializationType, newWrite, lastWrite, resynchOnly);
        ase->writeToBitStream1 = !ase->writeToBitStream1;
    }
}

//-------------------------------------------------------------------------------------
void Replica::cancelAutoSerializeTimer(SerializationType serializationType)
{
    AutoSerializeTimer::iterator autoSerializeTimerIt = autoSerializeTimers.find(serializationType);
    if (autoSerializeTimerIt == autoSerializeTimers.end())
        return;
    AutoSerializeEvent *ase = autoSerializeTimerIt->second;
    autoSerializeTimers.erase(autoSerializeTimerIt);
    delete ase;
}

//-------------------------------------------------------------------------------------
void Replica::clearAutoSerializeTimers()
{
    for (AutoSerializeTimer::const_iterator autoSerializeTimerIt = autoSerializeTimers.begin(); autoSerializeTimerIt != autoSerializeTimers.end(); ++autoSerializeTimerIt)
        delete autoSerializeTimerIt->second;
    autoSerializeTimers.clear();
}

//-------------------------------------------------------------------------------------
void Replica::onAutoSerializeTimerElapsed(SerializationType serializationType, RakNet::BitStream* output, RakNet::BitStream* lastOutput, bool resynchOnly)
{
    SerializationContext sc(resynchOnly ? ST_AUTOSERIALIZE_RESYNCH_ONLY : serializationType, UNASSIGNED_SYSTEM_ADDRESS, UNASSIGNED_SYSTEM_ADDRESS);
    if (serialize(output, &sc))
    {
        if (!resynchOnly && output->GetNumberOfBitsUsed() > 0 &&
            ((output->GetNumberOfBitsUsed() != lastOutput->GetNumberOfBitsUsed()) ||
            (memcmp(output->GetData(), lastOutput->GetData(), (size_t)output->GetNumberOfBytesUsed()) != 0)))
        {
            SystemAddressSet exclusionSet;
            mReplicationManager->sendSerialize(this, output, UNASSIGNED_SYSTEM_ADDRESS, exclusionSet, ST_BROADCAST_AUTO_SERIALIZE_TO_SYSTEM);
        }
    }
}

//-------------------------------------------------------------------------------------
void Replica::forceElapseAllAutoserializeTimers(bool resynchOnly)
{
    elapseAutoSerializeTimers(99999999, resynchOnly);
}

//-------------------------------------------------------------------------------------
void Replica::update(Timer::Time elapsedTimeMs)
{
    elapseAutoSerializeTimers(elapsedTimeMs, false);
}

//-------------------------------------------------------------------------------------
void Replica::receiveSerialize(RakNet::BitStream* bitStream, SystemAddress sender, SerializationType serializationType, SystemAddressSet& exclusionSet)
{
    RakNet::BitStream bs;

    // Force all autoserialize timers to go off early, so any variable changes from the Deserialize event do not themselves trigger an autoserialize event
    forceElapseAllAutoserializeTimers(false);

    // Deserialize the new data
    deserialize(bitStream, serializationType, sender);

    // Update last values for all autoserialize timers
    forceElapseAllAutoserializeTimers(true);

    SerializationContext sc(ST_RELAY_SERIALIZATION_TO_SYSTEMS, UNASSIGNED_SYSTEM_ADDRESS, UNASSIGNED_SYSTEM_ADDRESS);

    for (ConnectionMap::const_iterator connectionIt = mReplicationManager->mConnectionMap.begin(); connectionIt != mReplicationManager->mConnectionMap.end(); ++connectionIt)
    {
        if (connectionIt->second->mLastConstructionSet.find(this) == connectionIt->second->mLastConstructionSet.end())
            continue;
        if (exclusionSet.find(connectionIt->first) != exclusionSet.end())
            continue;
        sc.recipientAddress = connectionIt->first;
        bs.Reset();
        if (serialize(&bs, &sc))
            mReplicationManager->sendSerialize(this, &bs, sc.recipientAddress, exclusionSet, sc.serializationType);
    }
}

//-------------------------------------------------------------------------------------
void Replica::receiveDestruction(RakNet::BitStream* bitStream, SystemAddress sender, SerializationType serializationType, SystemAddressSet& exclusionSet)
{
    RakNet::BitStream bs;

    deserializeDestruction(bitStream, serializationType, sender);

    SerializationContext sc(ST_RELAY_DESTRUCTION_TO_SYSTEMS, sender, UNASSIGNED_SYSTEM_ADDRESS);

    for (ConnectionMap::const_iterator connectionIt = mReplicationManager->mConnectionMap.begin(); connectionIt != mReplicationManager->mConnectionMap.end(); ++connectionIt)
    {
        if (connectionIt->second->mLastConstructionSet.find(this) == connectionIt->second->mLastConstructionSet.end())
            continue;
        if (exclusionSet.find(connectionIt->first) != exclusionSet.end())
            continue;
        sc.recipientAddress = connectionIt->first;
        bs.Reset();
        if (serializeDestruction(&bs, &sc))
            mReplicationManager->sendDestruction(this, &bs, sc.recipientAddress, exclusionSet);
    }
    deleteOnReceiveDestruction(bitStream, sender, serializationType, exclusionSet);
}

//-------------------------------------------------------------------------------------
void Replica::deleteOnReceiveDestruction(RakNet::BitStream* bitStream, SystemAddress sender, SerializationType serializationType, SystemAddressSet& exclusionSet)
{
    delete this;
}

//-------------------------------------------------------------------------------------
ReplicationManager::ReplicationManager(RakNetConnection* connection) :
    mConnection(connection),
    mConnectionFactory(0),
    mDefaultOrderingChannel(0),
    mDefaultPacketPriority(HIGH_PRIORITY),
    mDefaultPacketReliablity(RELIABLE_ORDERED),
    mIsAuthority(false),
    mAuthoritySystemAddress(UNASSIGNED_SYSTEM_ADDRESS)
{
    mTimer.reset();
    mLastUpdateTimeMs = mTimer.getMilliseconds();
    mIntervalQueryConstructionMs = 0;
    mQueryConstructionRemainingMs = 0;
}

//-------------------------------------------------------------------------------------
ReplicationManager::~ReplicationManager()
{
}

//-------------------------------------------------------------------------------------
void ReplicationManager::setConnectionFactory(ConnectionRMFactory* factory)
{
    mConnectionFactory = factory;
}

//-------------------------------------------------------------------------------------
void ReplicationManager::setDefaultOrderingChannel(char orderingChannel)
{
    mDefaultOrderingChannel = orderingChannel;
}

//-------------------------------------------------------------------------------------
void ReplicationManager::setDefaultPacketPriority(PacketPriority packetPriority)
{
    mDefaultPacketPriority = packetPriority;
}

//-------------------------------------------------------------------------------------
void ReplicationManager::setDefaultPacketReliability(PacketReliability packetReliability)
{
    mDefaultPacketReliablity = packetReliability;
}

//-------------------------------------------------------------------------------------
void ReplicationManager::setIntervalQueryConstructionMs(CommonTools::Timer::Time intervalQueryConstructionMs)
{
    mIntervalQueryConstructionMs = intervalQueryConstructionMs;
    mQueryConstructionRemainingMs = mIntervalQueryConstructionMs;
}

//-------------------------------------------------------------------------------------
void ReplicationManager::setAuthority(SystemAddress systemAddress)
{
    mAuthoritySystemAddress = systemAddress;
    mIsAuthority = (mAuthoritySystemAddress == mConnection->getRakPeer()->GetInternalID());
}

//-------------------------------------------------------------------------------------
SystemAddress ReplicationManager::getAuthority()
{
    return mAuthoritySystemAddress;
}

//-------------------------------------------------------------------------------------
void ReplicationManager::addReplica(Replica* replica)
{
    addReference(replica);
}

//-------------------------------------------------------------------------------------
void ReplicationManager::removeReplica(Replica* replica)
{
    // Send destruction of replica immediately, this instance will certainly be deleted
    RakNet::BitStream bs;
    SerializationContext sc(ST_SEND_DESTRUCTION_GENERIC_TO_SYSTEM, UNASSIGNED_SYSTEM_ADDRESS, UNASSIGNED_SYSTEM_ADDRESS);
    SystemAddressSet exclusionSet;
    for (ConnectionMap::const_iterator connectionIt = mConnectionMap.begin(); connectionIt != mConnectionMap.end(); ++connectionIt)
        if (connectionIt->second->mLastConstructionSet.find(replica) != connectionIt->second->mLastConstructionSet.end())
        {
            exclusionSet.clear();
            for (ConnectionMap::const_iterator connectionIt2 = mConnectionMap.begin(); connectionIt2 != mConnectionMap.end(); ++connectionIt2)
                if ((connectionIt2->first != connectionIt->first) &&
                    (connectionIt2->second->mLastConstructionSet.find(replica) != connectionIt2->second->mLastConstructionSet.end()))
                    exclusionSet.insert(connectionIt2->first);
            sc.recipientAddress = connectionIt->first;
            bs.Reset();
            if (replica->serializeDestruction(&bs, &sc))
                sendDestruction(replica, &bs, sc.recipientAddress, exclusionSet);
        }

    removeReference(replica);
}

//-------------------------------------------------------------------------------------
void ReplicationManager::addConnection(SystemAddress systemAddress)
{
    ConnectionMap::const_iterator connectionIt = mConnectionMap.find(systemAddress);
    if (connectionIt != mConnectionMap.end())
        return;
    ConnectionRM *connection = mConnectionFactory->newConnection();
    connection->setSystemAddress(systemAddress);
    mConnectionMap[systemAddress] = connection;
    // Update this new connection
    updateNewConnection(connection);
}

//-------------------------------------------------------------------------------------
void ReplicationManager::removeConnection(SystemAddress systemAddress)
{
    ConnectionMap::iterator connectionIt = mConnectionMap.find(systemAddress);
    if (connectionIt == mConnectionMap.end())
        return;
    mConnectionFactory->freeConnection(connectionIt->second);
    mConnectionMap.erase(connectionIt);
}

//-------------------------------------------------------------------------------------
ConnectionRM* ReplicationManager::getConnection(SystemAddress systemAddress)
{
    ConnectionMap::const_iterator connectionIt = mConnectionMap.find(systemAddress);
    if (connectionIt != mConnectionMap.end())
        return connectionIt->second;
    return 0;
}

//-------------------------------------------------------------------------------------
void ReplicationManager::sendConstruction(Replica* replica, RakNet::BitStream* replicaData, SystemAddress recipient, SerializationType serializationType,
                                          PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
    if (replica == 0)
        return;
    if ((recipient == UNASSIGNED_SYSTEM_ADDRESS) && (mConnectionMap.size() == 0))
        return;

    ConnectionRM* connection = getConnection(recipient);
    if (connection == 0)
        return;

    RakNet::BitStream bs;
    writeHeader(&bs, RakNetConnection::ID_RM_CONSTRUCTION, serializationType);
    RakNetConnection::SerializeString(&bs, replica->getUid());
    bs.AlignWriteToByteBoundary();
    bs.Write(replicaData);
    addConstructionReference(connection, replica);
    // Send the packet
    send(&bs, recipient, priority, reliability, orderingChannel);
    // Serialize
    replica->sendSerialize(recipient, ST_SEND_SERIALIZATION_CONSTRUCTION_TO_SYSTEM);
}

//-------------------------------------------------------------------------------------
void ReplicationManager::sendDestruction(Replica* replica, RakNet::BitStream* replicaData, SystemAddress recipient, SystemAddressSet& exclusionSet, SerializationType serializationType,
                                         PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
    if (replica == 0)
        return;
    if ((recipient == UNASSIGNED_SYSTEM_ADDRESS) && (mConnectionMap.size() == 0))
        return;

    ConnectionRM* connection = getConnection(recipient);
    if (connection == 0)
        return;

    RakNet::BitStream bs;
    writeHeader(&bs, RakNetConnection::ID_RM_DESTRUCTION, serializationType);
    RakNetConnection::SerializeString(&bs, replica->getUid());
    if (!addRecipientAndWriteExclusionSet(recipient, &bs, exclusionSet))
        return;
    bs.AlignWriteToByteBoundary();
    bs.Write(replicaData);
    removeConstructionReference(connection, replica);
    // Send the packet
    send(&bs, recipient, priority, reliability, orderingChannel);
}

//-------------------------------------------------------------------------------------
void ReplicationManager::sendSerialize(Replica* replica, RakNet::BitStream* replicaData, SystemAddress recipient, SystemAddressSet& exclusionSet, SerializationType serializationType,
                                       PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
    if (replica == 0)
        return;
    if ((recipient == UNASSIGNED_SYSTEM_ADDRESS) && (mConnectionMap.size() == 0))
        return;
    if (!replica->queryIsSerializationAuthority())
        return;

    RakNet::BitStream bs;
    writeHeader(&bs, RakNetConnection::ID_RM_SERIALIZE, serializationType);
    RakNetConnection::SerializeString(&bs, replica->getUid());
    if (recipient != UNASSIGNED_SYSTEM_ADDRESS)
    {
        ConnectionRM* connection = getConnection(recipient);
        if (connection == 0)
            return;

        // Replica constructed ?
        if (connection->mLastConstructionSet.find(replica) == connection->mLastConstructionSet.end())
            return;
        if (!addRecipientAndWriteExclusionSet(recipient, &bs, exclusionSet))
            return;
        bs.AlignWriteToByteBoundary();
        bs.Write(replicaData);
        // Send to the recipient
        send(&bs, recipient, priority, reliability, orderingChannel);
    }
    else
    {
        // Get recipients on which replica is constructed 
        SystemAddressSet recipients;
        for (ConnectionMap::const_iterator connectionIt = mConnectionMap.begin(); connectionIt != mConnectionMap.end(); ++connectionIt)
        {
            if (connectionIt->second->mLastConstructionSet.find(replica) != connectionIt->second->mLastConstructionSet.end())
            {
                recipients.insert(connectionIt->first);
                exclusionSet.insert(connectionIt->first);
            }
        }
        writeExclusionSet(&bs, exclusionSet);
        bs.AlignWriteToByteBoundary();
        bs.Write(replicaData);
        // Send to each recipient
        for (SystemAddressSet::const_iterator recipientIt = recipients.begin(); recipientIt != recipients.end(); ++recipientIt)
            send(&bs, *recipientIt, priority, reliability, orderingChannel);
    }
}

//-------------------------------------------------------------------------------------
ReplicationManager::OnReceiveResult ReplicationManager::onReceive(Packet* packet)
{
    unsigned char packetIdentifier = (unsigned char)packet->data[0];
    unsigned char packetDataOffset = sizeof(unsigned char);

    switch (packetIdentifier)
    {
        case ID_CONNECTION_REQUEST_ACCEPTED:
//            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "ReplicationManager::onReceive() ID_CONNECTION_REQUEST_ACCEPTED from %s", packet->systemAddress.ToString());
            // Add this connection (client)
            addConnection(packet->systemAddress);
            return ORR_CONTINUE_PROCESSING;
            break;
        case ID_NEW_INCOMING_CONNECTION:
//            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "ReplicationManager::onReceive() ID_NEW_INCOMING_CONNECTION from %s", packet->systemAddress.ToString());
            // Add this connection (server)
            addConnection(packet->systemAddress);
            return ORR_CONTINUE_PROCESSING;
            break;
        case ID_DISCONNECTION_NOTIFICATION:
//            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "ReplicationManager::onReceive() ID_DISCONNECTION_NOTIFICATION from %s", packet->systemAddress.ToString());
            // Remove this connection
            onCloseConnection(packet->systemAddress);
            return ORR_CONTINUE_PROCESSING;
            break;
        case ID_CONNECTION_LOST:
//            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "ReplicationManager::onReceive() ID_CONNECTION_LOST from %s", packet->systemAddress.ToString());
            // Remove this connection
            onCloseConnection(packet->systemAddress);
            return ORR_CONTINUE_PROCESSING;
            break;
        case RakNetConnection::ID_RM_CONSTRUCTION:
//            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "ReplicationManager::onReceive() ID_RM_CONSTRUCTION from %s", packet->systemAddress.ToString());
            // Construct replica
            return onConstruction(packet->data + packetDataOffset, packet->length - packetDataOffset, packet->systemAddress);
            break;
        case RakNetConnection::ID_RM_DESTRUCTION:
//            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "ReplicationManager::onReceive() ID_RM_DESTRUCTION from %s", packet->systemAddress.ToString());
            // Destroy replica
            return onDestruction(packet->data + packetDataOffset, packet->length - packetDataOffset, packet->systemAddress);
            break;
        case RakNetConnection::ID_RM_SERIALIZE:
//            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "ReplicationManager::onReceive() ID_RM_SERIALIZE from %s", packet->systemAddress.ToString());
            // Replica serialized
            return onSerialize(packet->data + packetDataOffset, packet->length - packetDataOffset, packet->systemAddress);
            break;
        default:
            break;
    }
    return ORR_CONTINUE_PROCESSING;
}

//-------------------------------------------------------------------------------------
void ReplicationManager::onCloseConnection(SystemAddress systemAddress)
{
    // Remove this connection
    removeConnection(systemAddress);
}

//-------------------------------------------------------------------------------------
void ReplicationManager::onShutdown()
{
    clear();
}

//-------------------------------------------------------------------------------------
void ReplicationManager::update()
{
    Timer::Time currentUpdateTimeMs = mTimer.getMilliseconds();
    Timer::Time elapsedTimeMs = currentUpdateTimeMs - mLastUpdateTimeMs;

    // Query construction (and destruction)
    if (mQueryConstructionRemainingMs > elapsedTimeMs)
        mQueryConstructionRemainingMs -= elapsedTimeMs;
    else
    {
        mQueryConstructionRemainingMs = mIntervalQueryConstructionMs;
        // Query all-systems replicas construction
        ReplicaSet::iterator replicaIt = mReplicaSet2QueryConstruction.begin();
        while (replicaIt != mReplicaSet2QueryConstruction.end())
        {
            Replica *replica = *replicaIt;
            if (!replica->queryIsReady())
            {
                replicaIt++;
                continue;
            }
            Replica::QueryResult qr = replica->queryConstruction(0);
            if (qr == Replica::QR_NEVER)
            {
                for (ConnectionMap::const_iterator connectionIt = mConnectionMap.begin(); connectionIt != mConnectionMap.end(); ++connectionIt)
                    connectionIt->second->mNextConstructionSet.erase(replica);
                replicaIt = mReplicaSet2QueryConstruction.erase(replicaIt);
            }
            else if (qr == Replica::QR_ALWAYS)
            {
                mReplicaSet2AlwaysConstruct.insert(replica);
                for (ConnectionMap::const_iterator connectionIt = mConnectionMap.begin(); connectionIt != mConnectionMap.end(); ++connectionIt)
                    connectionIt->second->mNextConstructionSet.insert(replica);
                replicaIt = mReplicaSet2QueryConstruction.erase(replicaIt);
            }
            else
                replicaIt++;
        }

        // Update replicas construction/destruction per connection
        for (ConnectionMap::const_iterator connectionIt = mConnectionMap.begin(); connectionIt != mConnectionMap.end(); ++connectionIt)
            connectionIt->second->update(this);
    }

    // Automatic updates of replicas
    for (ReplicaMap::const_iterator replicaIt = mReplicaMap.begin(); replicaIt != mReplicaMap.end(); ++replicaIt)
        replicaIt->second->update(elapsedTimeMs);

    mLastUpdateTimeMs = currentUpdateTimeMs;
}

//-------------------------------------------------------------------------------------
void ReplicationManager::resetQueries(Replica* replica)
{
    if (replica->queryIsConstructionAuthority())
        mReplicaSet2QueryConstruction.insert(replica);
}

//-------------------------------------------------------------------------------------
ReplicationManager::OnReceiveResult ReplicationManager::onConstruction(unsigned char* packetData, int packetDataLength, SystemAddress sender)
{
    ConnectionRM* connection = getConnection(sender);
    if (connection == 0)
        return ORR_CONTINUE_PROCESSING;

    RakNet::BitStream bs(packetData, packetDataLength, false);
    SerializationType serializationType;
    readSerializationType(&bs, serializationType);
    ReplicaUid replicaUid;
    RakNetConnection::DeserializeString(&bs, replicaUid);
    bs.AlignReadToByteBoundary();
    Replica* replica = connection->receiveConstruction(&bs, replicaUid, sender, serializationType, this);
    if (replica)
        addConstructionReference(connection, replica);

    return ORR_STOP_PROCESSING;
}

//-------------------------------------------------------------------------------------
ReplicationManager::OnReceiveResult ReplicationManager::onDestruction(unsigned char* packetData, int packetDataLength, SystemAddress sender)
{
    ConnectionRM* connection = getConnection(sender);
    if (connection == 0)
        return ORR_CONTINUE_PROCESSING;

    RakNet::BitStream bs(packetData, packetDataLength, false);
    SerializationType serializationType;
    readSerializationType(&bs, serializationType);
    ReplicaUid replicaUid;
    RakNetConnection::DeserializeString(&bs, replicaUid);
    SystemAddressSet exclusionSet;
    readExclusionSet(&bs, exclusionSet);
    exclusionSet.insert(sender);
    bs.AlignReadToByteBoundary();
    ReplicaMap::const_iterator replicaIt = mReplicaMap.find(replicaUid);
    if (replicaIt != mReplicaMap.end())
        replicaIt->second->receiveDestruction(&bs, sender, serializationType, exclusionSet);

    return ORR_STOP_PROCESSING;
}

//-------------------------------------------------------------------------------------
ReplicationManager::OnReceiveResult ReplicationManager::onSerialize(unsigned char* packetData, int packetDataLength, SystemAddress sender)
{
    ConnectionRM* connection = getConnection(sender);
    if (connection == 0)
        return ORR_CONTINUE_PROCESSING;

    RakNet::BitStream bs(packetData, packetDataLength, false);
    SerializationType serializationType;
    readSerializationType(&bs, serializationType);
    ReplicaUid replicaUid;
    RakNetConnection::DeserializeString(&bs, replicaUid);
    SystemAddressSet exclusionSet;
    readExclusionSet(&bs, exclusionSet);
    exclusionSet.insert(sender);
    bs.AlignReadToByteBoundary();
    ReplicaMap::const_iterator replicaIt = mReplicaMap.find(replicaUid);
    if (replicaIt != mReplicaMap.end())
        replicaIt->second->receiveSerialize(&bs, sender, serializationType, exclusionSet);

    return ORR_STOP_PROCESSING;
}

//-------------------------------------------------------------------------------------
bool ReplicationManager::addRecipientAndWriteExclusionSet(SystemAddress recipient, RakNet::BitStream* bs, SystemAddressSet& exclusionSet)
{
    if (exclusionSet.find(recipient) != exclusionSet.end())
        return false;
    exclusionSet.insert(recipient);
    writeExclusionSet(bs, exclusionSet);
    return true;
}

//-------------------------------------------------------------------------------------
void ReplicationManager::writeExclusionSet(RakNet::BitStream* bs, SystemAddressSet& exclusionSet)
{
    bs->WriteCompressed((unsigned int)exclusionSet.size());
    for (SystemAddressSet::const_iterator exclusionSetIt = exclusionSet.begin(); exclusionSetIt != exclusionSet.end(); ++exclusionSetIt)
        bs->Write(*exclusionSetIt);
}

//-------------------------------------------------------------------------------------
void ReplicationManager::readExclusionSet(RakNet::BitStream* bs, SystemAddressSet& exclusionSet)
{
    unsigned int exclusionListSize;
    bs->ReadCompressed(exclusionListSize);
    for (unsigned int exclusionListIndex = 0; exclusionListIndex < exclusionListSize; exclusionListIndex++)
    {
        SystemAddress systemAddress;
        bs->Read(systemAddress);
        exclusionSet.insert(systemAddress);
    }
}

//-------------------------------------------------------------------------------------
void ReplicationManager::send(RakNet::BitStream* bs, SystemAddress recipient, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
    if (priority == NUMBER_OF_PRIORITIES)
        priority = mDefaultPacketPriority;
    if (reliability == NUMBER_OF_RELIABILITIES)
        reliability = mDefaultPacketReliablity;
    if (orderingChannel == -1)
        orderingChannel = mDefaultOrderingChannel;
    mConnection->getRakPeer()->Send(bs, priority, reliability, orderingChannel, recipient, false);
}

//-------------------------------------------------------------------------------------
void ReplicationManager::clear()
{
    mReplicaMap.clear();
    mReplicaSet2QueryConstruction.clear();
    mReplicaSet2AlwaysConstruct.clear();
    for (ConnectionMap::iterator connectionIt = mConnectionMap.begin(); connectionIt != mConnectionMap.end(); ++connectionIt)
        mConnectionFactory->freeConnection(connectionIt->second);
    mConnectionMap.clear();
}

//-------------------------------------------------------------------------------------
void ReplicationManager::updateNewConnection(ConnectionRM* connection)
{
    for (ReplicaSet::const_iterator replicaIt = mReplicaSet2AlwaysConstruct.begin(); replicaIt != mReplicaSet2AlwaysConstruct.end(); ++replicaIt)
        connection->mNextConstructionSet.insert(*replicaIt);
}

//-------------------------------------------------------------------------------------
void ReplicationManager::addReference(Replica* replica)
{
    if (replica == 0)
        return;
	replica->setReplicationManager(this);

    if (replica->getUid().empty())
        return;

    mReplicaMap[replica->getUid()] = replica;
    if (replica->queryIsConstructionAuthority())
        mReplicaSet2QueryConstruction.insert(replica);
}

//-------------------------------------------------------------------------------------
void ReplicationManager::addConstructionReference(ConnectionRM* connection, Replica* replica)
{
    connection->mLastConstructionSet.insert(replica);
    connection->mNextConstructionSet.insert(replica);
}

//-------------------------------------------------------------------------------------
void ReplicationManager::removeConstructionReference(ConnectionRM* connection, Replica* replica)
{
    connection->mLastConstructionSet.erase(replica);
    connection->mNextConstructionSet.erase(replica);
}

//-------------------------------------------------------------------------------------
void ReplicationManager::removeReference(Replica* replica)
{
    if (replica == 0)
        return;

    for (ConnectionMap::const_iterator connectionIt = mConnectionMap.begin(); connectionIt != mConnectionMap.end(); ++connectionIt)
        removeConstructionReference(connectionIt->second, replica);

    for (ReplicaMap::iterator replicaIt = mReplicaMap.begin(); replicaIt != mReplicaMap.end(); ++replicaIt)
        if (replicaIt->second == replica)
        {
            mReplicaMap.erase(replicaIt);
            mReplicaSet2QueryConstruction.erase(replica);
            mReplicaSet2AlwaysConstruct.erase(replica);
            break;
        }
}

//-------------------------------------------------------------------------------------
void ReplicationManager::writeHeader(RakNet::BitStream* bs, MessageID messageId, SerializationType serializationType)
{
    bs->Write(messageId);
    bs->Write((unsigned char)serializationType);
}

//-------------------------------------------------------------------------------------
void ReplicationManager::readSerializationType(RakNet::BitStream* bs, SerializationType& serializationType)
{
    unsigned char c;
    bs->Read(c);
    serializationType = (SerializationType)c;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis