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