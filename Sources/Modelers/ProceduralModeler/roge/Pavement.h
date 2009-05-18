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

#ifndef _Pavement_h_
#define _Pavement_h_ 1

#include <string>
#include <vector>

#include "Linear/Linear.h"

namespace roge {

	class LineWay;
	
	class Pavement {
	public:
		Pavement();
	
		Pavement(const Pavement& right);
	
		virtual ~Pavement();
	
		std::string getCovering() const;
	
		void setCovering(std::string left);
	
		std::vector< LineWay* > getLineList() const;
	
		void setLineList( std::vector< LineWay* > left);
	
		LineWay* getLine(int number) const;
	
		unsigned int getNbLine() {
			return m_lines.size();
		}
	
		void addLine(LineWay* line);
	
		void remLine(int number);
		
		void deleteLines();
	
		const std::vector<Vec3<double> >& getAxis();
	
		void setAxis(std::vector<Vec3<double> >& left);
	
	private:
		std::string _covering;
	
		std::vector<LineWay*> m_lines;
	
		std::vector<Vec3d > _axis;

	}; // of Pavement class

} // of roge namespace

#endif /* ARPAVEMENT_H_HEADER_INCLUDED_BFA8DCFB */
