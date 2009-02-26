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

#include "AbstractP2NMethod.h"
#include "P2NServer.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
AbstractP2NMethod::AbstractP2NMethod(const std::string& name, const std::string& help, P2NServer* s) :
    mServer(s),
    XmlRpc::XmlRpcServerMethod(name, s->getInternalServer()), 
    mHelp(help)
{
}

//-------------------------------------------------------------------------------------
AbstractP2NMethod::~AbstractP2NMethod()
{
}

//-------------------------------------------------------------------------------------
std::string AbstractP2NMethod::help()
{
	return mHelp;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
