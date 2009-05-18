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

#include <iostream>
#include <fstream>

#include "Utils.h"

static pm_utils::NotifyLevel _notifyLevel = pm_utils::PM_INFO;

#if defined( WIN32 ) && !( defined( __CYGWIN__ ) || defined( __MINGW32__ ) )
const char* NullStreamName = "nul";
#else
const char* NullStreamName = "/dev/null";
#endif

std::ostream& pm_utils::notify( NotifyLevel l ) 
{
  // set up global notify null stream for inline notify
  static std::ofstream s_NotifyNulStream( NullStreamName );

  static bool initialized = false;
  if( !initialized ) {
    std::cerr << ""; // dummy op to force construction of cerr, before a reference is passed back to calling code.
    std::cout << ""; // dummy op to force construction of cout, before a reference is passed back to calling code.
    initialized = true;
  }

  if( l <= _notifyLevel ) {
    if ( l <= pm_utils::PM_WARN )
      return std::cerr;
    else
      return std::cout;
  }
  return s_NotifyNulStream;
}

void pm_utils::setNotifyLevel( const std::string& ln ) 
{
	if( ln.find( "ALWAYS" ) != std::string::npos )
		_notifyLevel = pm_utils::PM_ALWAYS;
	else if( ln.find( "FATAL" ) != std::string::npos )
		_notifyLevel = pm_utils::PM_FATAL;
	else if( ln.find( "ERROR" ) != std::string::npos )
		_notifyLevel = pm_utils::PM_ERROR;
	else if( ln.find( "WARN" ) != std::string::npos )
		_notifyLevel = pm_utils::PM_WARN;
	else if( ln.find( "DEBUG" ) != std::string::npos )
		_notifyLevel = pm_utils::PM_DEBUG;
	else if( ln.find( "VERBOSE_DEBUG" ) != std::string::npos )
		_notifyLevel = pm_utils::PM_VERBOSE_DEBUG;
	else _notifyLevel = pm_utils::PM_INFO;
}

void pm_utils::setNotifyLevel( NotifyLevel l ) {
	_notifyLevel = l;
}

pm_utils::NotifyLevel pm_utils::notifyLevel() {
  return _notifyLevel;
}

// Redirect output to a file
void pm_utils::redirectOutputToFile( const std::string& file ) 
{
  FILE* fp = NULL;
#ifdef WIN32
  fopen_s( &fp, file.c_str(), "w" );
#else
  fp = fopen( file.c_str(), "w" );
#endif
  if( fp != NULL ) {
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );
    std::ios::sync_with_stdio();

    pm_utils::notify( pm_utils::PM_INFO ) << "Output redirected to '" << file.c_str() << "'" << std::endl;
  } 
  else
    pm_utils::notify( pm_utils::PM_WARN ) << "Couldn't redirect output to '" << file.c_str() << "'" << std::endl;
}

std::vector<std::string> pm_utils::split(const std::string& myStr, const std::string& token){
	std::vector<std::string> temp (0);
	std::string s;
	for(std::size_t i = 0; i < myStr.size(); i++){
		if( (myStr.substr(i, token.size()).compare(token) == 0)){
			temp.push_back(s);
			s.clear();
			i += token.size() - 1;
		}else{
			s.append(1, myStr[i]);
			if(i == (myStr.size() - 1))
				temp.push_back(s);
		}
	}
	return temp;
}

std::vector<std::string> pm_utils::split (const char* lhs, const char* rhs){
	const std::string m1 (lhs), m2 (rhs);
	return pm_utils::split(m1, m2);
}

std::vector<std::string> pm_utils::split (const char* lhs, const std::string& rhs){
	return pm_utils::split(lhs, rhs.c_str());
}

std::vector<std::string> pm_utils::split (const std::string& lhs, const char* rhs){
	return pm_utils::split(lhs.c_str(), rhs);
}

template<class Element>
std::ostream& pm_utils::displayElements(const std::vector<Element>& arg, std::ostream& output){
	for(std::size_t i = 0; i < arg.size(); i++)
		output << arg[i] << "\n";
	return output;
}
