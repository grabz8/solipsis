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

#ifndef __DummyPhonetizerPlugin_h__
#define __DummyPhonetizerPlugin_h__

#include <OgrePlugin.h>
#include "DummyPhonetizerExports.h"
namespace Solipsis
{
	class DummyPhonetizer;

	/**
		@brief	plugin implementation that registers the dummy phonetizer into the navigator
	*/
	class DUMMYPHONETIZER_EXPORT /* just so that we can use this phonetizer directly, without using ogre's plugin framework */ DummyPhonetizerPlugin : public Ogre::Plugin
	{
	protected:
		DummyPhonetizer* mDummyPhonetizer;

	public:
		DummyPhonetizerPlugin();

		//! @name Ogre::Plugin implementation
		//! @{
			/// @copydoc Plugin::getName
			virtual const Ogre::String& getName() const;

			/// @copydoc Plugin::install
			virtual void install();

			/// @copydoc Plugin::initialise
			virtual void initialise();

			/// @copydoc Plugin::shutdown
			virtual void shutdown();

			/// @copydoc Plugin::uninstall
			virtual void uninstall();
		//! @}

	};

} // namespace Solipsis

#endif // #ifndef __DummyPhonetizerPlugin_h__