/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

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

#ifndef _ExternalTextureSourceEx_H
#define _ExternalTextureSourceEx_H

#include <OgreExternalTextureSource.h>

namespace Solipsis
{
    class Event;

    class ExternalTextureSourceEx : public Ogre::ExternalTextureSource
	{
    public:
		/** Constructor */
        ExternalTextureSourceEx() : Ogre::ExternalTextureSource() {}
		/** Virtual destructor */
		virtual ~ExternalTextureSourceEx() {}

		//Pure virtual functions that plugins must Override
		/** Handle string evt */
		virtual Ogre::String handleEvt(const Ogre::String& material, const Ogre::String& evt) = 0;
		/** Handle evt */
		virtual void handleEvt(const Ogre::String& material, const Event& evt) = 0;
	};
}

#endif
