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

#ifndef __ReplicationManager_h__
#define __ReplicationManager_h__

#include <map>
#include <list>
#include <set>
#include <string>
#include <RakPeerInterface.h>
#include <BitStream.h>
#include <CTTimer.h>

namespace Solipsis {

class RakNetConnection;

class ConnectionRM;
class ConnectionRMFactory;
class Replica;
class ReplicationManager;

typedef std::string ReplicaUid;
typedef std::map<SystemAddress, ConnectionRM*> ConnectionMap;
typedef std::list<ConnectionRM*> ConnectionList;
typedef std::map<ReplicaUid, Replica*> ReplicaMap;
typedef std::set<Replica*> ReplicaSet;
typedef std::set<SystemAddress> SystemAddressSet;

/// Types of events that can cause network data to be transmitted. 
enum SerializationType
{
    /// Serialization command initiated by the user
    ST_SEND_SERIALIZATION_GENERIC_TO_SYSTEM,
	/// Serialization command initiated by the user
	ST_BROADCAST_SERIALIZATION_GENERIC_TO_SYSTEM,
    /// Serialization command automatically called after sending construction of the object
    ST_SEND_SERIALIZATION_CONSTRUCTION_TO_SYSTEM,
    /// Automatic serialization of data, based on Replica::addAutoSerializeTimer
    ST_BROADCAST_AUTO_SERIALIZE_TO_SYSTEM,
    /// Received a serialization command, relaying to systems other than the sender
    ST_RELAY_SERIALIZATION_TO_SYSTEMS,

    /// Default reason to send a destruction command
    ST_SEND_DESTRUCTION_GENERIC_TO_SYSTEM,
    /// Received destruction message, relaying to other systems
    ST_RELAY_DESTRUCTION_TO_SYSTEMS,

    /// Default reason to send a construction command
    ST_SEND_CONSTRUCTION_GENERIC_TO_SYSTEM,
	/// ReplicationManager::isAuthority() == false yet we called ReplicationManager::sendConstruction()
	ST_SEND_CONSTRUCTION_REQUEST_TO_SERVER,

    /// Calling Replica::serialize() for the purpose of reading memory to compare against later. This read will not be transmitted
    ST_AUTOSERIALIZE_RESYNCH_ONLY,
    /// Calling Replica::serialize() to compare against a prior call. The serialization may be transmitted
    ST_AUTOSERIALIZE_DEFAULT,

    /// Start your own reasons one unit past this enum
    ST_UNDEFINED_REASON
};

/// Contextual information about serialization, passed to some functions in Replica
class SerializationContext
{
public:
    SerializationContext() {}
    SerializationContext(SerializationType type, SystemAddress sender, SystemAddress recipient)
    {
        serializationType = type;
        senderAddress = sender;
        recipientAddress = recipient;
    }
    ~SerializationContext() {}

    /// The system that sent the message to us.
    SystemAddress senderAddress;
    /// The system that we are sending to.
    SystemAddress recipientAddress;
    /// Type of serialization performed
    SerializationType serializationType;
};

class ConnectionRMFactory
{
public:
    ConnectionRMFactory() {}
    virtual ~ConnectionRMFactory() {}
    virtual ConnectionRM* newConnection() const = 0;
    virtual void freeConnection(ConnectionRM* connection) const = 0;
};

class ConnectionRM
{
    friend class Replica;
    friend class ReplicationManager;

protected:
    /// Address of connected system
    SystemAddress mSystemAddress;

    /// Last set of constructed replicas
    ReplicaSet mLastConstructionSet;
    /// Next set of constructed replicas
    ReplicaSet mNextConstructionSet;

public:
    /// Constructor
    ConnectionRM();
    /// Destructor
    virtual ~ConnectionRM();

    /// Factory function, used to create instances of your game objects
    /// Encoding is entirely up to you. \a replicaData will hold whatever was written \a bitStream in Replica::serializeConstruction()
    /// One efficient way to do it is to use StringTable.h. This allows you to send predetermined strings over the network at a cost of 9 bits, up to 65536 strings
    virtual Replica* construct(RakNet::BitStream* replicaData, SystemAddress sender, SerializationType serializationType, ReplicationManager* replicationManager, const ReplicaUid& replicaUid, bool replicaUidCollision) = 0;

    /// Set the system address to use with this class instance. This is set internally when the object is created
    void setSystemAddress(SystemAddress systemAddress);
    /// Get the system address associated with this class instance.
    SystemAddress getSystemAddress() const;

    /// Update the connection
    void update(ReplicationManager* replicationManager);

protected:
    /// Receive 1 construction message
    virtual Replica* receiveConstruction(RakNet::BitStream* replicaData, const ReplicaUid& replicaUid, SystemAddress sender, SerializationType serializationType, ReplicationManager* replicationManager);
};

/** This class manages 1 replica.
*/
class Replica
{
    friend class ReplicationManager;

public:
    /// The result of various replica queries
    enum QueryResult
    {
        /// The query is always true, for all systems. Certain optimizations are performed here, but you should not later return any other value without first calling ReplicationManager::resetQueries
        QR_ALWAYS,
        /// True
        QR_YES,
        /// False
        QR_NO,
        /// The query is never true, for all systems. Certain optimizations are performed here, but you should not later return any other value without first calling ReplicationManager::resetQueries
        QR_NEVER
    };

protected:
    /// Replica manager
    ReplicationManager *mReplicationManager;
    /// Last serialize time
    CommonTools::Timer::Time mLastSerializeTimeMs;

    /// Auto-serialization event definition
    typedef struct
    {
        SerializationType serializationType;
        CommonTools::Timer::Time intervalMs;
        CommonTools::Timer::Time remainingMs;
        bool writeToBitStream1;
        RakNet::BitStream lastAutoSerializeBitStream1;
        RakNet::BitStream lastAutoSerializeBitStream2;
    } AutoSerializeEvent;
    typedef std::map<SerializationType, AutoSerializeEvent*> AutoSerializeTimer;
    /// Auto-serialization timers
    AutoSerializeTimer autoSerializeTimers;

public:
    /** Constructor. */
    Replica();
    /** Destructor. */
    virtual ~Replica();

    /// Sets the replica manager to use with this Replica.
    /// Call this before using any of the functions below!
    /// \param[in] rm A pointer to your instance of ReplicationManager
    void setReplicationManager(ReplicationManager* replicationManager);
    /// Returns what was passed to SetReplicationManager(), or 0 if no value ever passed
    /// \return Registered instance of ReplicationManager
    ReplicationManager* getReplicationManager() const;

    /// Returns the unique identifier of this Replica
    virtual const ReplicaUid& getUid() const = 0;
    /// Set the unique identifier of this Replica
    virtual void setUid(const ReplicaUid& uid) {}

    /// Construct this object on other systems
    /// Triggers a call to SerializeConstruction()
    virtual void sendConstruction(SystemAddress recipientAddress, SerializationType serializationType = ST_UNDEFINED_REASON);
    /// Destroy this object on other systems
    /// Triggers a call to SerializeDestruction()
    virtual void sendDestruction(SystemAddress recipientAddress, SerializationType serializationType = ST_UNDEFINED_REASON);
    /// Serialize this object to another system
    /// Triggers a call to Serialize()
    virtual void sendSerialize(SystemAddress recipientAddress, SerializationType serializationType = ST_UNDEFINED_REASON);

    /// Serialize this object to all current connections
    /// \param[in] serializationContext Which system to send to, and the SerializationType. 0 to use defaults.
    virtual void broadcastSerialize(SerializationContext* serializationContext = 0);

    /// Override in order to write to \a bitStream data identifying this class for the class factory. Will be received by ConnectionRM::construct() to create an instance of this class.
    /// \param[out] bitStream Data used to identify this class, along with any data you also want to send when constructing the class
    /// \param[in] serializationContext Describes which system we are sending to
    virtual bool serializeConstruction(RakNet::BitStream* bitStream, SerializationContext* serializationContext) = 0;
    /// Override in order to write to \a bitStream data to send along with destruction requests. Will be received by DeserializeDestruction()
    /// \param[out] bitStream Data to send
    /// \param[in] serializationContext Describes which system we are sending to
    virtual bool serializeDestruction(RakNet::BitStream* bitStream, SerializationContext* serializationContext) { return true; }
    /// Override in order to write to \a bitStream data to send as regular class serialization, for normal per-tick data. Will be received by Deserialize()
    /// \param[out] bitStream Data to send
    /// \param[in] serializationContext Describes which system we are sending to
    virtual bool serialize(RakNet::BitStream* bitStream, SerializationContext* serializationContext) { return true; }

    /// Receives data written by SerializeDestruction()
    /// \param[in] bitStream Data sent
    /// \param[in] serializationType SerializationContext::serializationType
    /// \param[in] sender Which system sent this message to us
    virtual void deserializeDestruction(RakNet::BitStream* bitStream, SerializationType serializationType, SystemAddress sender) {}
    /// Receives data written by Serialize()
    /// \param[in] bitStream Data sent
    /// \param[in] serializationType SerializationContext::serializationType
    /// \param[in] sender Which system sent this message to us
    virtual void deserialize(RakNet::BitStream* bitStream, SerializationType serializationType, SystemAddress sender) {}

    /// Does this replica is ready to be managed?
    /// Defaults to true
    /// \return True if this object instance is ready to be constructed/serialized/visibility tested/...
    virtual bool queryIsReady() const;
	/// For a given connection, should this object exist?
	/// Defaults to QR_ALWAYS
	/// \param[in] connection Which connection we are referring to. 0 means unknown, in which case the system is checking for QR_ALWAYS or QR_NEVER as an optimization.
	/// \return QR_NO and the object will be destroyed. QR_YES and the object will be created. QR_ALWAYS is YES for all connections, and is optimized to only be checked once.
	virtual QueryResult queryConstruction(ConnectionRM* connection);
	/// Does this system have control over construction of this object?
	/// While not strictly required, it is best to have this consistently return true for only one system. Otherwise systems may fight and override each other.
	/// Defaults to ReplicationManager::isAuthority();
	/// \return True if an authority over this operation, for this object instance
    virtual bool queryIsConstructionAuthority() const;
    /// Does this system have control over serialization of object members of this object?
    /// It is reasonable to have this be true for more than one system, but you would want to serialize different variables so those systems do not conflict.
    /// Defaults to ReplicationManager::isAuthority();
    /// \return True if an authority over this operation, for this object instance
    virtual bool queryIsSerializationAuthority() const;

    /// Adds a timer that will elapse every \a countdown milliseconds, calling Serialize with AUTOSERIALIZE_DEFAULT or whatever value was passed to \a serializationType
    /// Every time this timer elapses, the value returned from Serialize() will be compared to the last value returned by Serialize().
    /// If different, SendSerialize() will be called automatically.
    /// It is possible to create your own AUTOSERIALIZE enumerations and thus control which parts of the object is serialized
    /// Use CancelAutoSerializeTimer() or ClearAutoSerializeTimers() to stop the timer.
    /// If this timer already exists, it will simply override the existing countdown
    /// This timer will automatically repeat every \a intervalMs milliseconds
    /// \param[in] serializationType User-defined identifier for what type of serialization operation to perform. Returned in Deserialize() as the \a serializationType parameter.
    /// \param[in] intervalMs Time in milliseconds between autoserialize ticks. Use 0 to process immediately, and every tick
    virtual void addAutoSerializeTimer(CommonTools::Timer::Time intervalMs, SerializationType serializationType = ST_AUTOSERIALIZE_DEFAULT);

    /// Elapse time for all timers added with AddAutoSerializeTimer()
    /// \param[in] timeElapsed How many milliseconds have elapsed since the last call
    /// \param[in] resynchOnly True to only update what was considered the last send, without actually doing a send.
    virtual void elapseAutoSerializeTimers(CommonTools::Timer::Time elapsedTimeMs, bool resynchOnly);

    /// Stop calling an autoSerialize timer previously setup with AddAutoSerializeTimer()
    /// \param[in] serializationType Corresponding value passed to serializationType
    virtual void cancelAutoSerializeTimer(SerializationType serializationType = ST_AUTOSERIALIZE_DEFAULT);

    /// Remove and deallocate all previously added autoSerialize timers
    virtual void clearAutoSerializeTimers();

    /// A timer has elapsed. Compare the last value sent to the current value, and if different, send the new value
	virtual void onAutoSerializeTimerElapsed(SerializationType serializationType, RakNet::BitStream* output, RakNet::BitStream* lastOutput, bool resynchOnly);

    /// Immediately elapse all autoserialize timers
	/// Used internally when a Deserialize() event occurs, so that the deserialize does not trigger an autoserialize itself
	/// \param[in] resynchOnly If true, do not send a Serialize() message if the data has changed
	virtual void forceElapseAllAutoserializeTimers(bool resynchOnly);

    /// Update the replica
    void update(CommonTools::Timer::Time elapsedTimeMs);

protected:
    /// Receive 1 serialization message
    virtual void receiveSerialize(RakNet::BitStream* bitStream, SystemAddress sender, SerializationType serializationType, SystemAddressSet& exclusionSet);
    /// Receive 1 destruction message
    virtual void receiveDestruction(RakNet::BitStream* bitStream, SystemAddress sender, SerializationType serializationType, SystemAddressSet& exclusionSet);
    /// Delete replica on destruction message
    virtual void deleteOnReceiveDestruction(RakNet::BitStream* bitStream, SystemAddress sender, SerializationType serializationType, SystemAddressSet& exclusionSet);
};

/** This class manages replicas.
*/
class ReplicationManager
{
    friend class Replica;
    friend class ConnectionRM;

public:
    /// Possible return values for packet reception method
    enum OnReceiveResult
    {
        // The manager used this message so it can be ignored by caller and deallocated.
        ORR_STOP_PROCESSING = 0,
        // The manager maybe used this message but can also be usefull to caller.
        ORR_CONTINUE_PROCESSING
};

protected:
    /// Connection
    RakNetConnection *mConnection;

    /// Connection factory
    ConnectionRMFactory *mConnectionFactory;

    /// Default network oredering channel
    char mDefaultOrderingChannel;
    /// Default network packet priority
    PacketPriority mDefaultPacketPriority;
    /// Default network packet reliability
    PacketReliability mDefaultPacketReliablity;

    /// Timer
    CommonTools::Timer mTimer;
    /// Last update time (in milliseconds)
    CommonTools::Timer::Time mLastUpdateTimeMs;
    /// Interval between each query construction pass (in milliseconds)
    CommonTools::Timer::Time mIntervalQueryConstructionMs;
    /// Remaining time before next query construction pass (in milliseconds)
    CommonTools::Timer::Time mQueryConstructionRemainingMs;

    /// Authority manager or local
    bool mIsAuthority;
    /// Authority manager system address
    SystemAddress mAuthoritySystemAddress;

    /// Map of managed replicas
    ReplicaMap mReplicaMap;
    /// Set of replicas to query about construction
    ReplicaSet mReplicaSet2QueryConstruction;
    /// Set of replicas to always construct
    ReplicaSet mReplicaSet2AlwaysConstruct;

    /// Map of managed connections
    ConnectionMap mConnectionMap;

public:
    /** Constructor. */
    ReplicationManager(RakNetConnection* connection);
    /** Destructor. */
    virtual ~ReplicationManager();

    /// Sets the factory class used to allocate connection objects
    void setConnectionFactory(ConnectionRMFactory* factory);

    /// \param[in] Default ordering channel to use when passing -1 to a function that takes orderingChannel as a parameter
    void setDefaultOrderingChannel(char orderingChannel);
    /// \param[in] Default packet priority to use when passing NUMBER_OF_PRIORITIES to a function that takes priority as a parameter
    void setDefaultPacketPriority(PacketPriority packetPriority);
    /// \param[in] Default packet reliability to use when passing NUMBER_OF_RELIABILITIES to a function that takes reliability as a parameter
    void setDefaultPacketReliability(PacketReliability packetReliability);

    /// Set the interval time in milliseconds between query construction ticks (0 to use default behaviour, queries are performed on each update call)
    /// \param[in] intervalQueryConstructionMs Time in milliseconds between query construction ticks. Use 0 to process immediately, and every tick
    void setIntervalQueryConstructionMs(CommonTools::Timer::Time intervalQueryConstructionMs);

    /// Set the current authority which is responsible of replication consistency
    void setAuthority(SystemAddress systemAddress);
    /// Get the current authority which is responsible of replication consistency
    SystemAddress getAuthority();
    /// Return true if this system is the current authority
    bool isAuthority() { return mIsAuthority; }

    /// Add 1 replica to manage
    void addReplica(Replica* replica);
    /// Remove 1 replica to manage
    void removeReplica(Replica* replica);

    /// Callback to process received packets
    OnReceiveResult onReceive(Packet* packet);
    /// Callback to deal with closed connection
	void onCloseConnection(SystemAddress systemAddress);
    /// Callback to deal with shutdown
	void onShutdown();

    /// Update replicas states (construction, destruction, serialization, ...)
    void update();

    /// Reset query results about 1 replica
    void resetQueries(Replica* replica);

    /// Add 1 reference on this replica
    void addReference(Replica* replica);
    /// Remove 1 reference on this replica
    void removeReference(Replica* replica);

protected:
    /// Add 1 connection
    void addConnection(SystemAddress systemAddress);
    /// Remove 1 connection
    void removeConnection(SystemAddress systemAddress);
    /// Get 1 connection from 1 system address
    ConnectionRM* getConnection(SystemAddress systemAddress);

    /// Sends a construction command to one system.
    /// \param[in] replica The class to construct remotely
    /// \param[in] replicaData User-defined serialized data representing how to construct the class. Could be the name of the class, a unique identifier, or other methods
    /// \param[in] recipient Which system to send to. Use UNASSIGNED_SYSTEM_ADDRESS to send to all previously created connections. ConnectionRMFactory::newConnection will be called if this connection has not been previously used.
	/// \param[in] priority PacketPriority to send with. Use NUMBER_OF_PRIORITIES to use the default defined by setDefaultPacketPriority().
	/// \param[in] reliability PacketReliability to send with. Use NUMBER_OF_RELIABILITIES to use the default defined by setDefaultPacketReliability();
	/// \param[in] orderingChannel ordering channel to send on. Use -1 to use the default defined by setDefaultOrderingChannel()
    void sendConstruction(Replica* replica, RakNet::BitStream* replicaData, SystemAddress recipient, SerializationType serializationType = ST_SEND_CONSTRUCTION_GENERIC_TO_SYSTEM,
        PacketPriority priority = NUMBER_OF_PRIORITIES, PacketReliability reliability = NUMBER_OF_RELIABILITIES, char orderingChannel = -1);

    /// Sends a destruction command to one system.
    /// \param[in] replica The class to destroy remotely
    /// \param[in] replicaData User-defined serialized data. Passed to Replica::receiveDestruction()
    /// \param[in] recipient Which system to send to. Use UNASSIGNED_SYSTEM_ADDRESS to send to all previously created connections. ConnectionRMFactory::newConnection will be called if this connection has not been previously used.
	/// \param[in] priority PacketPriority to send with. Use NUMBER_OF_PRIORITIES to use the default defined by setDefaultPacketPriority().
	/// \param[in] reliability PacketReliability to send with. Use NUMBER_OF_RELIABILITIES to use the default defined by setDefaultPacketReliability();
	/// \param[in] orderingChannel ordering channel to send on. Use -1 to use the default defined by setDefaultOrderingChannel()
    void sendDestruction(Replica* replica, RakNet::BitStream* replicaData, SystemAddress recipient, SystemAddressSet& exclusionSet, SerializationType serializationType = ST_SEND_DESTRUCTION_GENERIC_TO_SYSTEM,
        PacketPriority priority = NUMBER_OF_PRIORITIES, PacketReliability reliability = NUMBER_OF_RELIABILITIES, char orderingChannel = -1);

    /// Sends a serialized object to one system.
    /// \param[in] replica The class to serialize
    /// \param[in] replicaData User-defined serialized data. Passed to Replica::receiveSerialization()
    /// \param[in] recipient Which system to send to. Use UNASSIGNED_SYSTEM_ADDRESS to send to all previously created connections. ConnectionRMFactory::newConnection will be called if this connection has not been previously used.
	/// \param[in] priority PacketPriority to send with. Use NUMBER_OF_PRIORITIES to use the default defined by setDefaultPacketPriority().
	/// \param[in] reliability PacketReliability to send with. Use NUMBER_OF_RELIABILITIES to use the default defined by setDefaultPacketReliability();
	/// \param[in] orderingChannel ordering channel to send on. Use -1 to use the default defined by setDefaultOrderingChannel()
    void sendSerialize(Replica* replica, RakNet::BitStream* replicaData, SystemAddress recipient, SystemAddressSet& exclusionSet, SerializationType serializationType = ST_SEND_SERIALIZATION_GENERIC_TO_SYSTEM,
        PacketPriority priority = NUMBER_OF_PRIORITIES, PacketReliability reliability = NUMBER_OF_RELIABILITIES, char orderingChannel = -1);

    /// Process construction message
    OnReceiveResult onConstruction(unsigned char* packetData, int packetDataLength, SystemAddress sender);
    /// Process destruction message
    OnReceiveResult onDestruction(unsigned char* packetData, int packetDataLength, SystemAddress sender);
    /// Process serialization message
    OnReceiveResult onSerialize(unsigned char* packetData, int packetDataLength, SystemAddress sender);

    /// Add 1 recipient in 1 exclusion set and write it into 1 bitstream
    bool addRecipientAndWriteExclusionSet(SystemAddress recipient, RakNet::BitStream* bs, SystemAddressSet& exclusionSet);
    /// Write 1 exclusion set into 1 bitstream
    void writeExclusionSet(RakNet::BitStream* bs, SystemAddressSet& exclusionSet);
    /// Read 1 exclusion set from 1 bitstream
    void readExclusionSet(RakNet::BitStream* bs, SystemAddressSet& exclusionList);

    /// Send 1 network packet to 1 recipient with parameters priority, reliability, ordering channel
    void send(RakNet::BitStream* bs, SystemAddress recipient, PacketPriority priority, PacketReliability reliability, char orderingChannel);
    /// Clear (before connection or after disconnection)
    void clear();
    /// Update 1 new connection
    void updateNewConnection(ConnectionRM* connection);

    /// Add 1 construction reference on this replica
    void addConstructionReference(ConnectionRM* connection, Replica* replica);
    /// Remove 1 construction reference on this replica
    void removeConstructionReference(ConnectionRM* connection, Replica* replica);

    /// Write packet header specific to our manager
    void writeHeader(RakNet::BitStream* bs, MessageID messageId, SerializationType serializationType);
    /// Read serialization type from 1 bitstream
    void readSerializationType(RakNet::BitStream* bs, SerializationType& serializationType);
};

} // namespace Solipsis

#endif // #ifndef __ReplicationManager_h__