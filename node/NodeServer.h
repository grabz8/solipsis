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

#ifndef __NodeServer_h__
#define __NodeServer_h__

#include <XmlRpc.h>
#include <pthread.h>
#include <vector>

//#include "AbstractNodeMethod.h"

class AbstractNodeMethod;

/** XMLRPC Server for a Solipsis Node
 */
class NodeServer : public XmlRpc::XmlRpcServer {
private :
	/** Port Number
	 */
	int mPort;

	/** List of XMLRPCMethod declared.
	 * Note on XMLRPC++ library : 
	 * The default XmlRpcServer is not in charge of the destruction of his method objects
	 * --> so we have to do it here(in the destructor) with a reference to the methods
	 */
	std::vector<AbstractNodeMethod* > mMethods;

	/** Listenning Thread
	 */
	pthread_t mThread;

public:

	/** Default Constructor
	 */
	NodeServer(int port=8550);

	/** Destructor
	 */
	~NodeServer(void);

	/** Initialisation
	 */
	void init(void);

	/** Launch the server in a thread
	 */
	bool start();

	/** Launch the server
	 */
	void listen(void);

	/** Stop the server
	 */
	void stop(void);

private:

	/** Copie Constructor (not allowed)
	 */
	NodeServer(const NodeServer& server);

	/** Copy assignement operator (not allowed)
	 */
	NodeServer& operator=(const NodeServer& server);

	/** Register all method you want
	 */
	void registerAllMethods(void);

	/** Start routine of the thread
	 */
	static void *startThread(void* ptr);
};

#endif // #ifndef __NodeServer_h__