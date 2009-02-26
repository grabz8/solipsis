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

#include "CTException.h"
#include "CTLog.h"

#include <sstream>

namespace CommonTools {

//-------------------------------------------------------------------------------------
Exception::Exception(int num, const string& desc, const string& src) :
    line(0),
    number(num),
    description(desc),
    source(src)
{
    LogHandler::getLogHandler()->log(LogHandler::VL_CRITICAL, this->getFullDescription().c_str());
}

//-------------------------------------------------------------------------------------
Exception::Exception(int num, const string& desc, const string& src, 
	const char* typ, const char* fil, long lin) :
    line(lin),
    number(num),
	typeName(typ),
    description(desc),
    source(src),
    file(fil)
{
    LogHandler::getLogHandler()->log(LogHandler::VL_CRITICAL, this->getFullDescription().c_str());
}

//-------------------------------------------------------------------------------------
Exception::Exception(const Exception& rhs)
    : line(rhs.line), number(rhs.number), description(rhs.description), source(rhs.source), file(rhs.file)
{
}

//-------------------------------------------------------------------------------------
void Exception::operator=(const Exception& rhs)
{
    description = rhs.description;
    number = rhs.number;
    source = rhs.source;
    file = rhs.file;
    line = rhs.line;
	typeName = rhs.typeName;
}

//-------------------------------------------------------------------------------------
const string& Exception::getFullDescription(void) const
{
	if (fullDesc.empty())
	{
		std::stringstream desc;
		desc <<  "CT EXCEPTION(" << number << ":" << typeName << "): "
			<< description 
			<< " in " << source;
		if (line > 0)
		{
			desc << " at " << file << " (line " << line << ")";
		}
		fullDesc = desc.str();
	}

	return fullDesc;
}

//-------------------------------------------------------------------------------------
int Exception::getNumber(void) const throw()
{
    return number;
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools
