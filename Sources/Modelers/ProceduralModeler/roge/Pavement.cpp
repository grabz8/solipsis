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

#include "Pavement.h"
#include "LineWay.h"

namespace roge {

	using namespace std;
	
	Pavement::Pavement() {
	}
	
	Pavement::Pavement(const Pavement& right) {
	}
	
	Pavement::~Pavement() {
		//lines are deleted by a call to deleteLines
	}
	
	string Pavement::getCovering() const {
		return _covering;
	}
	
	void Pavement::setCovering(string left) {
		_covering = left;
	}
	
	vector<LineWay*> Pavement::getLineList() const {
		return m_lines;
	}
	
	void Pavement::setLineList(vector<LineWay*> left) {
		m_lines = left;
	}
	
	LineWay* Pavement::getLine(int number) const {
		return m_lines[number];
	}
	
	void Pavement::addLine(LineWay* line) {
		m_lines.push_back(line);
	}
	
	void Pavement::remLine(int number) {
		delete m_lines[number];
		m_lines.erase(m_lines.begin()+number);
	}
	
	void Pavement::deleteLines(){
		for(unsigned int i = 0 ; i < m_lines.size() ; ++i){
			delete m_lines[i];
		}
	}
	
	const vector<Vec3<double> >& Pavement::getAxis() {
		return _axis;
	}
	
	void Pavement::setAxis(vector<Vec3<double> >& left) {
		_axis = left;
	}

} //namespace roge
