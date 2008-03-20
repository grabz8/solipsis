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
#ifdef POOL
    RefCntPoolPtr<XmlEntity> mXmlEntity;
#else
    XmlEntity* mXmlEntity;
#endif
    /// Type
    bool mIsLocal;
    /// Level of detail
    int mLod;

public:
    /** Constructor.
    */
#ifdef POOL
    OgrePeer(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal);
#else
    OgrePeer(XmlEntity* xmlEntity, bool isLocal);
#endif
    virtual ~OgrePeer();

    /** Gets the entity. */
#ifdef POOL
    RefCntPoolPtr<XmlEntity>& getXmlEntity();
#else
    XmlEntity* getXmlEntity();
#endif
    /** Gets the updated entity. */
#ifdef POOL
    virtual RefCntPoolPtr<XmlEntity>& getUpdatedXmlEntity() { return RefCntPoolPtr<XmlEntity>::nullPtr; }
#else
    virtual XmlEntity* getUpdatedXmlEntity() { return 0; }
#endif
    /** Test if this peer is a local entity. */
    bool isLocal() { return mIsLocal; }

    /** Update. */
    virtual void update(Real timeSinceLastFrame) = 0;
    /** Update entity. */
#ifdef POOL
    virtual bool update(RefCntPoolPtr<XmlEntity>& xmlEntity) = 0;
#else
    virtual bool update(XmlEntity* xmlEntity) = 0;
#endif
};

} // namespace Solipsis

#endif // #ifndef __OgrePeer_h__