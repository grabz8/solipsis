#ifndef __IOgrePeerManagerCallbacks__
#define __IOgrePeerManagerCallbacks__

#include "tinyxml.h"
#include "OgrePeer.h"

namespace Solipsis {

/** This class defines a callbacks interface on OgrePeerManager.
 */
class IOgrePeerManagerCallbacks
{
public:
	virtual ~IOgrePeerManagerCallbacks() {};

	// Called when an avatar should be created
    virtual bool OnAvatarNodeCreate(TiXmlElement* xmlElt, OgrePeer* ogrePeer) { return true; };

	// Called when a scene should be created
	virtual bool OnSceneNodeCreate(TiXmlElement* xmlElt, OgrePeer* ogrePeer) { return true; };
};

} // namespace Solipsis

#endif // #ifndef __IOgrePeerManagerCallbacks__