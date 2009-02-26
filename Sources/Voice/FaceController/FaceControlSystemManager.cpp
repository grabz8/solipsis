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

#include "FaceControlSystemManager.h"
#include "IFaceControlSystem.h"
#include <assert.h>

template<> Solipsis::FaceControlSystemManager* CommonTools::Singleton<Solipsis::FaceControlSystemManager>::ms_Singleton = 0;

namespace Solipsis
{

	//-------------------------------------------------------------------------------------
	FaceControlSystemManager* FaceControlSystemManager::getSingletonPtr(void)
	{
		return ms_Singleton;
	}

	//-------------------------------------------------------------------------------------
	FaceControlSystemManager& FaceControlSystemManager::getSingleton(void)
	{
		assert(ms_Singleton);
		return (*ms_Singleton);
	}

	FaceControlSystemManager::FaceControlSystemManager() :
		mSelectedFaceControlSystem(0)
	{
	}

	FaceControlSystemManager::~FaceControlSystemManager()
	{
	}

	void FaceControlSystemManager::addFaceControlSystem(IFaceControlSystem* pFaceControlSystem)
	{
		mFaceControlSystems.push_back(pFaceControlSystem);
	}

	void FaceControlSystemManager::removeFaceControlSystem(IFaceControlSystem* pFaceControlSystem)
	{
		// Remove only if equal to registered one, since it might overridden
		// by other plugins
		for (std::vector<IFaceControlSystem*>::iterator it=mFaceControlSystems.begin(); it != mFaceControlSystems.end(); ++it)
		{
			if ((*it) == pFaceControlSystem)
			{
				mFaceControlSystems.erase(it);
				return;
			}
		}
		assert( false ); // if this fails, it means we are attempting to remove a phonetizer that's not here
	}

	std::vector<IFaceControlSystem*>& FaceControlSystemManager::getFaceControlSystems()
	{
		return mFaceControlSystems;
	}

	void FaceControlSystemManager::selectFaceControlSystem(const std::string& phonetizerName)
	{
		mSelectedFaceControlSystem = 0;
		for (std::vector<IFaceControlSystem*>::iterator it=mFaceControlSystems.begin(); it != mFaceControlSystems.end(); ++it)
			if ((*it)->getName().compare(phonetizerName) == 0)
			{
				mSelectedFaceControlSystem = (*it);
				break;
			}
	}

	IFaceControlSystem* FaceControlSystemManager::getSelectedFaceControlSystem()
	{
		return mSelectedFaceControlSystem;
	}
} // end namespace Solipsis