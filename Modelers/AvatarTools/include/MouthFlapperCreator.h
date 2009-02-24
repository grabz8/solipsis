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

#ifndef __MouthFlapperCreator_h__
#define __MouthFlapperCreator_h__

#include "IFaceControllerCreator.h"
#include <Ogre.h>

namespace Solipsis 
{

	/**
		@brief	a simple face controller that just performs very basic mouth flapping
	*/
	class MouthFlapperCreator : public IFaceControllerCreator
	{
	public:
		/**
			@brief Contructor

			@param	jawBoneName	the name of the bone controlled by the mouth flapper. Make sure this bone is not animated
			@param	closedJawOrientation the orientation of the jaw bone that results in a closed mouth
			@param	openJawOrientation the orientation of the jaw bone that results in a fully open mouth
		*/
		MouthFlapperCreator( const std::string & jawBoneName, const Ogre::Quaternion & closedJawOrientation, const Ogre::Quaternion & openJawOrientation );
		///brief Destructor
		virtual ~MouthFlapperCreator();

		//!@name IFaceControllerCreator implementation
		//!{
			virtual IFaceController* createFaceController( CharacterInstance* pFaceControllerOwner );
		//!}

		const std::string & getJawBoneName( void ) const { return mJawBoneName; }
		const Ogre::Quaternion & getClosedJawOrientation( void ) const {return mClosedJawOrientation;}
		const Ogre::Quaternion & getOpenJawOrientation( void ) const {return mOpenJawOrientation;}

	private:
		std::string	mJawBoneName;
		Ogre::Quaternion	mClosedJawOrientation;	///< the orientation of the jaw bone that results in a closed mouth
		Ogre::Quaternion	mOpenJawOrientation;		///< the orientation of the jaw bone that results in a fully open mouth
	};

}

#endif //__MouthFlapperCreator_h__
