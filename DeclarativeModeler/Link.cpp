/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Sebastien LAIGRE

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

#include "Link.h"

Link::Link(const std::string & operation, Actor * a1, Actor * a2)
: _a1(a1), _a2(a2), _relation(operation), _actor2Constraint(false)
{ }

Link::~Link()
{
	delete _a1;
	delete _a2;
}

std::ostream& operator<<(std::ostream& os, Link & l)
{
	os << *(l._a1) << "  \""<< l._relation << "\"  "<< *(l._a2) ;
	return os;
}
