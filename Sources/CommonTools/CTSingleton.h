/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Guillaume Raffy (Proservia)

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

#ifndef __CTSingleton_h__
#define __CTSingleton_h__

#include <assert.h>

namespace CommonTools
{

	/**
		@brief	This class is the base class for singletons (classes that have at most one instance)

		@note	This code has been copied from Ogre 1.4.9 because it's used in many places where ogre
				is not required
	*/

	template <typename T> class Singleton
	{
	protected:

		static T* ms_Singleton;

	public:

		/**
			@brief	constructor
		*/
		Singleton( void )
		{
			assert( !ms_Singleton );
			#if defined( _MSC_VER ) && _MSC_VER < 1200	 
				int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
				ms_Singleton = (T*)((int)this + offset);
			#else
				ms_Singleton = static_cast< T* >( this );
			#endif
		}

		/**
			@brief	destructor
		*/
		~Singleton( void )
		{
			assert( ms_Singleton );
			ms_Singleton = 0;
		}

		/**
			@brief	
		*/
		static T& getSingleton( void )
		{
			assert( ms_Singleton );
			return ( *ms_Singleton );
		}

		static T* getSingletonPtr( void )
		{ return ms_Singleton; }
	};

} // namespace CommonTools

#endif // #ifndef __CTSingleton_h__
