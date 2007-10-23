#include <iostream>
#include "Node.h"

Node::Node(){
	mXMLPRCServer.init();
	//mXMLPRCServer.listen();//listen not in a thread (blocking the node)
	mXMLPRCServer.start();//listen in a thread
}

Node::~Node(){
	//std::cout<<"Desctructor of Node"<<std::endl;
}