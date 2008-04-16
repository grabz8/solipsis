#ifndef _OgreExternalTextureSourceEx_H
#define _OgreExternalTextureSourceEx_H

#include "OgreExternalTextureSource.h"

namespace Ogre
{
	class ExternalTextureSourceEx : public ExternalTextureSource
	{
    public:
	/** Enum for mouse/kbd events */
	enum eMouseKbdEvent
	{
		MKE_MOUSELBTNDWN = 0,
		MKE_MOUSELBTNUP = 1,
		MKE_MOUSEMBTNDWN = 2,
		MKE_MOUSEMBTNUP = 3,
		MKE_MOUSERBTNDWN = 4,
		MKE_MOUSERBTNUP = 5,
		MKE_MOUSEMOVED = 6,
        MKE_COUNT = 7
	};

    public:
		/** Constructor */
        ExternalTextureSourceEx() : ExternalTextureSource() {}
		/** Virtual destructor */
		virtual ~ExternalTextureSourceEx() {}

		//Pure virtual functions that plugins must Override
		/** Handle mouse evt */
		virtual void mouseEvt(const String& material, const Vector2& xy, eMouseKbdEvent mouseKbdEvent) = 0;
	};
}

#endif
