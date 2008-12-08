/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO - François FOURNEL

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

/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __IFaceControllerCreator_h__
#define __IFaceControllerCreator_h__


namespace Solipsis 
{
	class IFaceController;
	class CharacterInstance;

	/**
		@brief	interface that allows the instanciation of face controllers
	*/
	class IFaceControllerCreator
	{
	public:
		///brief Contructor
		IFaceControllerCreator( void );
		///brief Destructor
		virtual ~IFaceControllerCreator();

		/**
			@brief	creates a new face controller instance

			@param	timeSinceLastFrame	in seconds
		*/
		virtual IFaceController* createFaceController( CharacterInstance* pFaceControllerOwner ) = 0;
	};

}

#endif //__IFaceControllerCreator_h__
