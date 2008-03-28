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
    mServer(),
    mLogger(0)
{
    XmlRpc::setVerbosity(verbosity);
}

//-------------------------------------------------------------------------------------
P2NServer::~P2NServer()
{
    for (std::vector<AbstractP2NMethod*>::const_iterator it = mMethods.begin(); it != mMethods.end(); ++it)
    {
        delete (*it);
    }
    mMethods.clear();
}

//-------------------------------------------------------------------------------------
bool P2NServer::init()
{
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
	// Create the server socket on the specified port
	mServer.bindAndListen(mPort);

	// Enable introspection
	mServer.enableIntrospection(true);

	// Wait for requests indefinitely
	mServer.work(-1.0);

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
