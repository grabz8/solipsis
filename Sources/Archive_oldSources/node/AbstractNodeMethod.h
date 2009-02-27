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

#ifndef __AbstractNodeMethod_h__
#define __AbstractNodeMethod_h__

#include <iostream>
#include <string>

#include <XmlRpc.h>

//forward declaration
class NodeServer;

/** Abstract Class
 */
class AbstractNodeMethod : public XmlRpc::XmlRpcServerMethod {
private:
	std::string mHelp;

protected:
	AbstractNodeMethod(const std::string& name, const std::string& help, NodeServer* s);

public:
	virtual ~AbstractNodeMethod();
	
	virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result) = 0;
	
	std::string help(void);

};

#endif // #ifndef __AbstractNodeMethod_h__