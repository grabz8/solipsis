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
