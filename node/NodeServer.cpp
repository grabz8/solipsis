/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author 

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

#include "NodeServer.h"
#include "NodeServerMethods.h"

NodeServer::NodeServer(int port) : mPort(port) {
	XmlRpc::setVerbosity(2);
}

NodeServer::~NodeServer(){
	//std::cout<<"Desctructor of NodeServer"<<std::endl;
	for (std::vector<AbstractNodeMethod*>::const_iterator it = mMethods.begin(); 
		it != mMethods.end(); ++it){
			delete (*it);
	}
	mMethods.clear();
}

void NodeServer::init(void){
	registerAllMethods();
}

void NodeServer::registerAllMethods(void){
	//std::cout<<"Register all methods"<<std::endl;
	mMethods.push_back(new HelloMethod(this));
}

bool NodeServer::start(){
    int rc;

	 rc = pthread_create(&mThread, NULL, startThread, this);
    if (rc != 0)
    {
        //LogManager::getSingletonPtr()->logMessage("NodeEventListener::start() pthread_create returned " + StringConverter::toString(rc));
        return false;
    }
    rc = pthread_detach(mThread);
    if (rc != 0)
    {
        //LogManager::getSingletonPtr()->logMessage("NodeEventListener::start() pthread_detach returned " + StringConverter::toString(rc));
        return false;
    }

    //mState = SRunning;
    //mStop = false;

    return true;
}

void NodeServer::listen(void){
	// Create the server socket on the specified port
	bindAndListen(mPort);
		
	// Enable introspection
	enableIntrospection(true);
		
	// Wait for requests indefinitely
	work(-1.0);
}

void *NodeServer::startThread(void* ptr) {
	
	NodeServer* nodeServer = (NodeServer*)ptr;

	if(nodeServer!=0){
		nodeServer->listen();
		
	}

	return NULL;
}
