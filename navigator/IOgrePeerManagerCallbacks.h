#ifndef __IOgrePeerManagerCallbacks__
#define __IOgrePeerManagerCallbacks__

#include "tinyxml.h"
#include "OgrePeer.h"

class IOgrePeerManagerCallbacks
{
public:
	virtual ~IOgrePeerManagerCallbacks() {};

	// Called when an avatar should be created
    virtual bool OnAvatarNodeCreate(TiXmlElement* xmlElt, OgrePeer* ogrePeer) { return true; };

	// Called when a scene should be created
	virtual bool OnSceneNodeCreate(TiXmlElement* xmlElt, OgrePeer* ogrePeer) { return true; };
};

#endif // #ifndef __IOgrePeerManagerCallbacks__