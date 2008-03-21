#include "P2NNullServer.h"

namespace Solipsis {

P2NNullServer* P2NNullServer::ms_singletonPtr = 0;

//-------------------------------------------------------------------------------------
IP2NServer* IP2NServer::createServer(IP2NServerRequestsHandler* requestsHandler, const std::string& host, int port, int verbosity, const std::string& extras)
{
    return new P2NNullServer(requestsHandler, host, port, verbosity, extras);
}

//-------------------------------------------------------------------------------------
bool IP2NServer::destroyServer(IP2NServer* server)
{
    delete server;

    return true;
}

//-------------------------------------------------------------------------------------
P2NNullServer::P2NNullServer(IP2NServerRequestsHandler* requestsHandler, const std::string& host, int port, int verbosity, const std::string& extras) :
    mRequestsHandler(requestsHandler),
    mHost(host),
    mPort(port),
    mExtras(extras)
{
    ms_singletonPtr = this;
}

//-------------------------------------------------------------------------------------
P2NNullServer::~P2NNullServer()
{
    ms_singletonPtr = 0;
}

//-------------------------------------------------------------------------------------
bool P2NNullServer::init()
{
    return true;
}

//-------------------------------------------------------------------------------------
bool P2NNullServer::start()
{
    return true;
}

//-------------------------------------------------------------------------------------
bool P2NNullServer::listen()
{
    return true;
}

//-------------------------------------------------------------------------------------
bool P2NNullServer::stop()
{
    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
