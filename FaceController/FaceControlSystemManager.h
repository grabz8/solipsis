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

#ifndef __FaceControlSystemManager_h__
#define __FaceControlSystemManager_h__

#ifdef FACECONTROLLER_EXPORTS
	#define FACECONTROLLER_EXPORT __declspec( dllexport )
#else
	#define FACECONTROLLER_EXPORT
#endif 


#include <CTSingleton.h>
#include <vector>

namespace Solipsis
{
	class IFaceControlSystem;

	/**
		@brief	manages phonetizers registration / unregistration into the system
	*/
	class FACECONTROLLER_EXPORT FaceControlSystemManager : public CommonTools::Singleton<FaceControlSystemManager>
	{
	public:
		FaceControlSystemManager();
		virtual ~FaceControlSystemManager();

	private:
		/// List of available engines
		std::vector<IFaceControlSystem*> mFaceControlSystems;
		/// Selected engine
		IFaceControlSystem* mSelectedFaceControlSystem;

	public:
		/**
			@brief	Adds a new face control system.
		*/
		void addFaceControlSystem(IFaceControlSystem* pFaceControlSystem);

		/**
			@brief	Removes the given face control system from the manager
		*/
		void removeFaceControlSystem(IFaceControlSystem* pFaceControlSystem);

		/**
			@brief	Get available face control systems.
		*/
		std::vector<IFaceControlSystem*>& getFaceControlSystems( void );

		/**
			@brief	Set current face control system.
		*/
		void selectFaceControlSystem(const std::string& faceControlSystemNameName);

		/**
			@brief	Get current face control system.
		*/
		IFaceControlSystem* getSelectedFaceControlSystem();

		/** Override standard Singleton retrieval.
		@remarks
		Why do we do this? Well, it's because the Singleton
		implementation is in a .h file, which means it gets compiled
		into anybody who includes it. This is needed for the
		Singleton template to work, but we actually only want it
		compiled into the implementation of the class based on the
		Singleton, not all of them. If we don't change this, we get
		link errors when trying to use the Singleton-based class from
		an outside dll.
		@par
		This method just delegates to the template version anyway,
		but the implementation stays in this single compilation unit,
		preventing link errors.
		*/
		static FaceControlSystemManager& getSingleton(void);
		/** Override standard Singleton retrieval.
		@remarks
		Why do we do this? Well, it's because the Singleton
		implementation is in a .h file, which means it gets compiled
		into anybody who includes it. This is needed for the
		Singleton template to work, but we actually only want it
		compiled into the implementation of the class based on the
		Singleton, not all of them. If we don't change this, we get
		link errors when trying to use the Singleton-based class from
		an outside dll.
		@par
		This method just delegates to the template version anyway,
		but the implementation stays in this single compilation unit,
		preventing link errors.
		*/
		static FaceControlSystemManager* getSingletonPtr(void);
		
	};

} // namespace Solipsis

#endif // #ifndef __FaceControlSystemManager_h__