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
