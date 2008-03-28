#include "P2NServer.h"
#include "P2NServerMethods.h"
#include <iostream>

namespace Solipsis {

//-------------------------------------------------------------------------------------
IP2NServer* IP2NServer::createServer(IP2NServerRequestsHandler* requestsHandler, const std::string& host, int port, int verbosity, const std::string& extras)
{
    return new P2NServer(requestsHandler, host, port, verbosity, extras);
}

//-------------------------------------------------------------------------------------
bool IP2NServer::destroyServer(IP2NServer* server)
{
    delete server;

    return true;
}

//-------------------------------------------------------------------------------------
P2NServer::P2NServer(IP2NServerRequestsHandler* requestsHandler, const std::string& host, int port, int verbosity, const std::string& extras) :
    mRequestsHandler(requestsHandler),
    mHost(host),
    mPort(port),
    mExtras(extras),
    mMTServer(0),
    mLogger(0)
{
}

//-------------------------------------------------------------------------------------
P2NServer::~P2NServer()
{
    for (std::vector<AbstractP2NMethod*>::const_iterator it = mMethods.begin(); it != mMethods.end(); ++it)
    {
        delete (*it);
    }
    mMethods.clear();

    delete mMTServer;
}

//-------------------------------------------------------------------------------------
bool P2NServer::init()
{
    try {
        unsigned int nthreads = 2;
        bool wbxml = false;
        bool secure = false;
        bool chunked = false;
        bool persistent = false;
        ulxr::CppString host = ULXR_PCHAR(mHost);
        unsigned port = mPort;
        ulxr::CppString sec = ULXR_PCHAR("unsecured");

        // process extra informations
        std::string value;
        if (getExtraInformation(mExtras, "nthreads=", value))
            nthreads = atoi(value.c_str());
        if (getExtraInformation(mExtras, "wbxml=", value))
            wbxml = atoi(value.c_str()) ? true : false;
        if (getExtraInformation(mExtras, "secure=", value))
            secure = atoi(value.c_str()) ? true : false;
        if (getExtraInformation(mExtras, "chunked=", value))
            chunked = atoi(value.c_str()) ? true : false;
        if (getExtraInformation(mExtras, "persistent=", value))
            persistent = atoi(value.c_str()) ? true : false;

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

        mMTServer = new ulxr::MultiThreadRpcServer(prot.get(), nthreads, wbxml);
    }
    catch (ulxr::Exception& ex)
    {
        std::cout << ULXR_PCHAR("Error occured: ") << ULXR_GET_STRING(ex.why()) << std::endl;
        return false;
    }
    catch(...)
    {
        std::cout << ULXR_PCHAR("Unknown error occured\n");
        return false;
    }

    return registerAllMethods();
}

//-------------------------------------------------------------------------------------
bool P2NServer::registerAllMethods()
{
    mMethods.push_back(new LoginMethod(this));
    mMethods.push_back(new LogoutMethod(this));
    mMethods.push_back(new HandleEvtMethod(this));
    mMethods.push_back(new SendEvtMethod(this));

    return true;
}

//-------------------------------------------------------------------------------------
bool P2NServer::start()
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

//-------------------------------------------------------------------------------------
bool P2NServer::listen()
{
    try {
        unsigned started = mMTServer->dispatchAsync();
        std::cout << ULXR_PCHAR("Started ") << started << ULXR_PCHAR(" threads for dispatching rpc requests\n");

        mMTServer->waitAsync(false, true);
        std::cout << ULXR_PCHAR("Done.\n");
    }
    catch (ulxr::Exception& ex)
    {
        std::cout << ULXR_PCHAR("Error occured: ") << ULXR_GET_STRING(ex.why()) << std::endl;
        return false;
    }
    catch(...)
    {
        std::cout << ULXR_PCHAR("Unknown error occured\n");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool P2NServer::stop()
{
    return true;
}

//-------------------------------------------------------------------------------------
void *P2NServer::startThread(void* ptr)
{
	P2NServer* server = (P2NServer*)ptr;

	if (server != 0)
		server->listen();

	return NULL;
}

//-------------------------------------------------------------------------------------
bool P2NServer::getExtraInformation(const std::string& extras, const std::string& information, std::string& value)
{
    std::string::size_type idx, spc;
    idx = extras.find(information);
    if (idx == std::string::npos)
        return false;
    idx += information.length();
    if (extras.length() < idx)
        return false;
    spc = extras.find(' ', idx);
    if (spc == idx)
        return false;
    if (spc == std::string::npos)
        spc = extras.length();
    value = extras.substr(idx, spc - idx + 1);
    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
