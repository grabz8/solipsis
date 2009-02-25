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

#include "DummyPhonetizerPlugin.h"
#include "PhonetizerManager.h"
#include "DummyPhonetizer.h"

namespace Solipsis
{
	const Ogre::String sPluginName = "Dummy Phonetizer";

	//-------------------------------------------------------------------------------------
	DummyPhonetizerPlugin::DummyPhonetizerPlugin() :
		mDummyPhonetizer(0)
	{
	}

	//-------------------------------------------------------------------------------------
	const Ogre::String& DummyPhonetizerPlugin::getName() const
	{
		return sPluginName;
	}

	//-------------------------------------------------------------------------------------
	void DummyPhonetizerPlugin::install()
	{
		mDummyPhonetizer = new DummyPhonetizer();
		PhonetizerManager::getSingleton().addPhonetizer(mDummyPhonetizer);
	}

	//-------------------------------------------------------------------------------------
	void DummyPhonetizerPlugin::initialise()
	{
		// nothing to do
	}

	//-------------------------------------------------------------------------------------
	void DummyPhonetizerPlugin::shutdown()
	{
		// nothing to do
	}

	//-------------------------------------------------------------------------------------
	void DummyPhonetizerPlugin::uninstall()
	{
		if (mDummyPhonetizer != 0)
		{
			if (PhonetizerManager::getSingletonPtr())
				PhonetizerManager::getSingleton().removePhonetizer(mDummyPhonetizer);
			delete mDummyPhonetizer;
		}
	}
}