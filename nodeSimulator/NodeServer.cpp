#include "NodeServer.h"
#include "NodeServerMethods.h"
#include <iostream>

NodeServer::NodeServer(int port, int verbosity) : mPort(port)
{
#ifdef ULXR
    mMTServer = 0;
#else
    XmlRpc::setVerbosity(verbosity);
#endif
}

NodeServer::~NodeServer()
{
    for (std::vector<AbstractNodeMethod*>::const_iterator it = mMethods.begin(); it != mMethods.end(); ++it)
    {
        delete (*it);
    }
    mMethods.clear();

#ifdef ULXR
    delete mMTServer;
#endif
}

void NodeServer::init(void)
{
#ifdef ULXR
    try {
        unsigned int num_threads = 2;
        bool wbxml = false;
        bool secure = false;
        bool chunked = false;
        bool persistent = false;
        ulxr::CppString host = ULXR_PCHAR("localhost");
        unsigned port = mPort;
        ulxr::CppString sec = ULXR_PCHAR("unsecured");
        std::cout << ULXR_PCHAR("Serving ") << sec << ULXR_PCHAR(" rpc requests at ") << host << ULXR_PCHAR(":") << port << std::endl;
        std::cout << ULXR_PCHAR("WBXML: ") << wbxml << std::endl
                  << ULXR_PCHAR("Chunked transfer: ") << chunked << std::endl;
        std::auto_ptr<ulxr::TcpIpConnection> conn;
#ifdef _MSC_VER
        {
            std::auto_ptr<ulxr::TcpIpConnection> temp(new ulxr::TcpIpConnection (true, host, port));
            conn = temp;
        }
#else
        conn.reset(new ulxr::TcpIpConnection (true, host, port));
#endif
        conn.reset(new ulxr::TcpIpConnection (true, host, port));
        std::auto_ptr<ulxr::HttpProtocol> prot(new ulxr::HttpProtocol(conn.get()));
        prot->setChunkedTransfer(chunked);
        prot->setPersistent(persistent);
        if (persistent)
          conn->setTcpNoDelay(true);

        if (prot->isPersistent())
          std::cout << ULXR_PCHAR("Using persistent connections\n") ;
        else
          std::cout << ULXR_PCHAR("Using non-persistent connections\n") ;

        mMTServer = new ulxr::MultiThreadRpcServer(prot.get(), num_threads, wbxml);
    }
    catch (ulxr::Exception& ex)
    {
        std::cout << ULXR_PCHAR("Error occured: ") << ULXR_GET_STRING(ex.why()) << std::endl;
        return;
    }
    catch(...)
    {
        std::cout << ULXR_PCHAR("Unknown error occured\n");
        return;
    }
#endif
    registerAllMethods();
}

void NodeServer::registerAllMethods(void)
{
    mMethods.push_back(new LoginMethod(this));
    mMethods.push_back(new LogoutMethod(this));
    mMethods.push_back(new HandleEvtMethod(this));
    mMethods.push_back(new SendEvtMethod(this));
}

bool NodeServer::start()
{
    int rc;

    rc = pthread_create(&mThread, NULL, startThread, this);
    if (rc != 0)
    {
        return false;
    }
    rc = pthread_detach(mThread);
    if (rc != 0)
    {
        return false;
    }

    return true;
}

void NodeServer::listen(void)
{
#ifdef ULXR
    try {
        unsigned started = mMTServer->dispatchAsync();
        std::cout << ULXR_PCHAR("Started ") << started << ULXR_PCHAR(" threads for dispatching rpc requests\n");

        mMTServer->waitAsync(false, true);
        std::cout << ULXR_PCHAR("Done.\n");
    }
    catch (ulxr::Exception& ex)
    {
        std::cout << ULXR_PCHAR("Error occured: ") << ULXR_GET_STRING(ex.why()) << std::endl;
        return;
    }
    catch(...)
    {
        std::cout << ULXR_PCHAR("Unknown error occured\n");
        return;
    }
#else
	// Create the server socket on the specified port
	bindAndListen(mPort);

	// Enable introspection
	enableIntrospection(true);

	// Wait for requests indefinitely
	work(-1.0);
#endif
}

void *NodeServer::startThread(void* ptr)
{
	NodeServer* nodeServer = (NodeServer*)ptr;

	if(nodeServer!=0)
    {
		nodeServer->listen();
	}

	return NULL;
}
