#ifndef __NodeServer_h__
#define __NodeServer_h__

#include <XmlRpc.h>
#include <pthread.h>
#include <vector>

//#include "AbstractNodeMethod.h"

class AbstractNodeMethod;

/** XMLRPC Server for a Solipsis Node
 */
class NodeServer : public XmlRpc::XmlRpcServer {
private :
	/** Port Number
	 */
	int mPort;

	/** List of XMLRPCMethod declared.
	 * Note on XMLRPC++ library : 
	 * The default XmlRpcServer is not in charge of the destruction of his method objects
	 * --> so we have to do it here(in the destructor) with a reference to the methods
	 */
	std::vector<AbstractNodeMethod* > mMethods;

	/** Listenning Thread
	 */
	pthread_t mThread;

public:

	/** Default Constructor
	 */
	NodeServer(int port=8550);

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