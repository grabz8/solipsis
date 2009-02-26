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

#ifndef __AbstractP2NMethod_h__
#define __AbstractP2NMethod_h__

#include <string>
#include <XmlRpc.h>

namespace Solipsis {

// Forward declaration
class P2NServer;

/** Abstract Class
 */
class AbstractP2NMethod : public XmlRpc::XmlRpcServerMethod
{
protected:
    std::string mHelp;
    P2NServer* mServer;

protected:
    AbstractP2NMethod(const std::string& name, const std::string& help, P2NServer* s);

public:
    virtual ~AbstractP2NMethod();

    virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result) = 0;

    std::string help();
};

} // namespace Solipsis

#endif // #ifndef __AbstractP2NMethod_h__