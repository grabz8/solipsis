/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Archivideo / MERLET Jonathan

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

#include "LineWay.h"

namespace roge {
	
	LineWay::LineWay()
	{}
		
	LineWay::LineWay(const LineWay& right) {
		_startingWidth = right._startingWidth;
		_endingWidth = right._endingWidth;
	}
	
	LineWay::~LineWay() 
	{}
	
	double LineWay::getStartingWidth() const
	{
	    return _startingWidth;
	}
	
	double LineWay::getEndingWidth() const
	{
	    return _endingWidth;
	}
	
	
	void LineWay::setStartingWidth(double left)
	{
	    _startingWidth = left;
	}
	
	
	void LineWay::setEndingWidth(double left)
	{
	    _endingWidth = left;
	}

} // of namespace roge
