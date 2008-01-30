#include "Node.h"

Node* Node::mSingleton = 0;

Node::Node(int port, int verbosity) :
    mXMLPRCServer(port, verbosity)
{
    mSingleton = this;

	mXMLPRCServer.init();

	//mXMLPRCServer.listen();//listen not in a thread (blocking the node)
	mXMLPRCServer.start();//listen in a thread

    mConnectionsCount = 0;
    mAvatarDirty = false;
}

Node::~Node()
{
}