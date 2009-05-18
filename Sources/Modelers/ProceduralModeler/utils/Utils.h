/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author HUIBAN Vincent

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

#ifndef _Utils_h_
#define _Utils_h_

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#undef ERROR
#undef DEBUG

namespace pm_utils {

	typedef enum {
	    PM_ALWAYS,
	    PM_FATAL,
	    PM_ERROR,
	    PM_WARN,
	    PM_INFO,
	    PM_DEBUG,
		PM_VERBOSE_DEBUG
	} NotifyLevel;
      
	std::ostream& notify( NotifyLevel l );
  
	void setNotifyLevel( const std::string& ln );
	void setNotifyLevel( NotifyLevel l );

	NotifyLevel notifyLevel();
  
	void redirectOutputToFile( const std::string& file ); 

	std::vector<std::string> split(const std::string&, const std::string&);
	std::vector<std::string> split(const char*, const char*);
	std::vector<std::string> split(const std::string&, const char*);
	std::vector<std::string> split(const char*, const std::string&);
	template<class Element> std::ostream& displayElements(const std::vector<Element>& arg, std::ostream& output = std::cout);

};

#endif

