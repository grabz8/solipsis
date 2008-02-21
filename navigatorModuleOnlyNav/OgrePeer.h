#ifndef __OgrePeer_h__
#define __OgrePeer_h__

#include "XmlDatas.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents a basic Ogre representation of a peer.
 */
class OgrePeer
{
protected:
    /// Associated entity
    XmlEntity* mXmlEntity;
    /// Type
    bool mIsLocal;
    /// Level of detail
    int mLod;

public:
    /** Constructor.
    */
    OgrePeer(XmlEntity* xmlEntity, bool isLocal);
    virtual ~OgrePeer();

    /** Gets the entity. */
    XmlEntity* getXmlEntity();
    /** Gets the updated entity. */
    virtual XmlEntity* getUpdatedXmlEntity() { return 0; }
    /** Test if this peer is a local entity. */
    bool isLocal() { return mIsLocal; }

    /** Update. */
    virtual void update(Real timeSinceLastFrame) = 0;
    /** Update entity. */
    virtual bool update(XmlEntity* xmlEntity) = 0;
};

} // namespace Solipsis

#endif // #ifndef __OgrePeer_h__