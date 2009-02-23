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

#ifndef _VEC4_H_
#define _VEC4_H_

#include <iostream>

class Vec4
{
public:
	Vec4(int r=0, int g=0, int b=0, int a=255);

	~Vec4();

	int r() { return _r;}
	int g() { return _g;}
	int b() { return _b;}
	int a() { return _a;}

	void setR(const int r) { _r = r;}
	void setG(const int g) { _g = g;}
	void setB(const int b) { _b = b;}
	void setA(const int a) { _a = a;}
	
	void setRGBA(const int r, const int g, const int b, const int a) { _r = r;  _g = g; _b = b; _a = a;}

	friend std::ostream& operator<<(std::ostream& os, Vec4 & v);

private:
	int _r;
	int _g;
	int _b;
	int _a;
};

#endif /*VEC4_H_*/
