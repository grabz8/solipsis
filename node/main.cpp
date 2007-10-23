//#include "NodeServer.h"

#include <iostream>
#include "Node.h"

using namespace XmlRpc;

int main(int argc, char** argv){
	//launch of a Node with a XML-RPC inside
	Node myNode;

	//******************** use of an RPC Client
	XmlRpcClient c("localhost", 8550);
	XmlRpcValue result;

	// Use introspection API to look up the supported methods
	XmlRpcValue noArgs;
	if (c.execute("system.listMethods", noArgs, result))
		std::cout << "\nMethods:\n " << result << "\n\n";
	else
		std::cout << "Error calling 'listMethods'\n\n";

	// Use introspection API to get the help string for the Hello method
	XmlRpcValue oneArg;
	oneArg[0] = "Hello";
	if (c.execute("system.methodHelp", oneArg, result))
		std::cout << "Help for 'Hello' method: " << result << "\n\n";
	else
		std::cout << "Error calling 'methodHelp'\n\n";

	// Call the Hello method
	if (c.execute("Hello", noArgs, result))
		std::cout << result << "\n\n";
	else
		std::cout << "Error calling 'Hello'\n\n";

	return 0;
}