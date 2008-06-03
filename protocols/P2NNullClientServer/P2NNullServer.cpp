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
    mExtras(extras),
    mLogger(0)
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
