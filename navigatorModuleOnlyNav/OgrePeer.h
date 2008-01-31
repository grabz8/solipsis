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
    /// Associated object
    XmlObject* mObject;
    /// Type
    bool mIsLocal;
    /// Level of detail
    int mLod;

public:
    /** Constructor.
    */
    OgrePeer(XmlObject* object, bool isLocal);
    virtual ~OgrePeer();

    /** Gets the object. */
    XmlObject* getObject();
    /** Gets the updated object. */
    virtual XmlObject* getUpdatedObject() { return 0; }
    /** Test if this peer is a local object. */
    bool isLocal() { return mIsLocal; }

    /** Update. */
    virtual void update(Real timeSinceLastFrame) = 0;
    /** Update object. */
    virtual bool update(XmlObject* updateObject) = 0;
};

} // namespace Solipsis

#endif // #ifndef __OgrePeer_h__