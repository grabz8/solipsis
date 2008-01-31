#ifndef __NodeServer_h__
#define __NodeServer_h__

#ifdef ULXR
#include "ulxmlrpcpp/ulxmlrpcpp.h"
#include "ulxmlrpcpp/ulxr_tcpip_connection.h"
#include "ulxmlrpcpp/ulxr_ssl_connection.h"
#include "ulxmlrpcpp/ulxr_http_protocol.h"
#include "ulxmlrpcpp/ulxr_except.h"
#include "ulxmlrpcpp/ulxr_signature.h"
#include "ulxmlrpcpp/ulxr_mtrpc_server.h"
#pragma comment(lib, "ulxmlrpcpp_23.lib")
#else
#include <XmlRpc.h>
#endif

#include <pthread.h>
#include <vector>

class AbstractNodeMethod;

/** XMLRPC Server for a Solipsis Node
 */
#ifdef ULXR
class NodeServer
#else
class NodeServer : public XmlRpc::XmlRpcServer
#endif
{
private:
#ifdef ULXR
    ulxr::MultiThreadRpcServer *mMTServer;
#endif

    /** Port Number
	 */
	int mPort;

	/** List of XMLRPCMethod declared.
	 * Note on XMLRPC++ library : 
	 * The default XmlRpcServer is not in charge of the destruction of his method objects
	 * --> so we have to do it here(in the destructor) with a reference to the methods
	 */
	std::vector<AbstractNodeMethod*> mMethods;

	/** Listenning Thread
	 */
	pthread_t mThread;

public:

	/** Default Constructor
	 */
	NodeServer(int port = 8550, int verbosity = 2);

	/** Destructor
	 */
	~NodeServer(void);

	/** Initialisation
	 */
	void init(void);

	/** Launch the server in a thread
	 */
	bool start();

	/** Launch the server
	 */
	void listen(void);

	/** Stop the server
	 */
	void stop(void);

#ifdef ULXR
    ulxr::MultiThreadRpcServer *getInternalServer() { return mMTServer; }
#endif

private:

	/** Copie Constructor (not allowed)
	 */
	NodeServer(const NodeServer& server);

	/** Copy assignement operator (not allowed)
	 */
	NodeServer& operator=(const NodeServer& server);

	/** Register all method you want
	 */
	void registerAllMethods(void);

	/** Start routine of the thread
	 */
	static void *startThread(void* ptr);
};

#endif // #ifndef __NodeServer_h__