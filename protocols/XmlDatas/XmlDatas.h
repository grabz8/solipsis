#ifndef __XmlDatas_h__
#define __XmlDatas_h__

#include <vector>
#include <map>
#include <ostream>
#include "XmlDatasPrerequisites.h"
#include "Ogre.h"
#include "tinyxml.h"

namespace Solipsis {

typedef std::string NodeId;

enum EventType {
    ETNewEntity = 0,        /// New entity
    ETLostEntity = 1,       /// Lost entity
    ETUpdatedEntity = 2,    /// Updated entity
    ETActionOnEntity = 3,   /// Action on entity
    ETStatusReport = 100    /// Status report
};

enum EntityType {
    ETAvatar = 0,       /// Avatar
    ETSite = 1,         /// Site
    ETObject = 2        /// Object (movable)
};

typedef unsigned int EntityFlags;
const EntityFlags EFNone = (EntityFlags)0;
const EntityFlags EFGravity = (EntityFlags)1;  /// Gravity applied

enum ShapeType {
    STPoint = 0,            /// Point
    STCircle = 1,           /// 2D circle
    ST2DRect = 2,           /// 2D rectangle
    STSphere = 100,         /// Sphere
    STBox = 101,            /// 3D box
    STPolygonVolume = 200   /// Volume
};

typedef unsigned int EntityUID;
typedef unsigned int Lod;

inline void convertStringToEventType(const char* str, EventType& evtType) { evtType = (EventType)atoi(str); }
inline void convertStringToEntityType(const char* str, EntityType& entityType) { entityType = (EntityType)atoi(str); }
inline void convertStringToEntityFlags(const char* str, EntityFlags& entityFlags) { entityFlags = (EntityFlags)atoi(str); }
inline void convertStringToShapeType(const char* str, ShapeType& shapeType) { shapeType = (ShapeType)atoi(str); }
inline void convertStringToEntityUID(const char* str, EntityUID& uid) { uid = (EntityUID)atoi(str); }
inline void convertStringToLod(const char* str, Lod& lod) { lod = (Lod)atoi(str); }

#define POOL
#ifdef POOL
#include <list>
#include <pthread.h>

template<class T>
class RefCntPoolPtr;
class Pool;
class PoolEntry
{
    template<class T>
    friend class RefCntPoolPtr;

protected:
    unsigned int mRefCount;
    pthread_mutex_t mMutex;

public:
    PoolEntry() : mMutex(PTHREAD_MUTEX_INITIALIZER) {}
    void lock() { pthread_mutex_lock(&mMutex); }
    void unlock() { pthread_mutex_unlock(&mMutex); }
    virtual Pool& getPool() const = 0;
    virtual void clear() {}
};
class Pool : public std::list<PoolEntry*>
{
protected:
    pthread_mutex_t mMutex;

public:
    Pool() : mMutex(PTHREAD_MUTEX_INITIALIZER) {}
    ~Pool() {
        lock();
        for(iterator it=begin();it!=end();++it)
            delete (*it);
        unlock();
    }
    void lock() { pthread_mutex_lock(&mMutex); }
    void unlock() { pthread_mutex_unlock(&mMutex); }
};
template<class T>
class RefCntPoolPtr
{
protected:
    T* mRep;

public:
    static RefCntPoolPtr<T> nullPtr;

protected:
    inline void addRef() {
        if (mRep == 0)
            return;
        mRep->lock();
        ++(mRep->mRefCount);
        mRep->unlock();
    }
    inline void delRef() {
        if (mRep == 0)
            return;
        Pool* pool = 0;
        T* entryToPushBack = 0;
        mRep->lock();
        if (--(mRep->mRefCount) == 0)
        {
            mRep->clear();
            pool = &(mRep->getPool());
            entryToPushBack = mRep;
            mRep->unlock();
            mRep = 0;
        }
        else mRep->unlock();
        if (entryToPushBack != 0)
        {
            pool->lock();
            pool->push_back(entryToPushBack);
            pool->unlock();
        }
    }
    virtual void swap(RefCntPoolPtr<T> &other) {
        std::swap(mRep, other.mRep);
    }
public:
    RefCntPoolPtr() : mRep(0) { allocate(); }
    template<class Y>
    explicit RefCntPoolPtr(Y* rep) : mRep(0) {
        if (rep != 0)
        {
            mRep = rep;
            mRep->mRefCount = 1;
        }
    }
    RefCntPoolPtr(const RefCntPoolPtr& other) : mRep(other.mRep) { addRef(); }
    RefCntPoolPtr& operator=(const RefCntPoolPtr& other) {
        if (mRep == other.mRep)
            return *this; 
        // Swap current data into a local copy
        // this ensures we deal with rhs and this being dependent
        RefCntPoolPtr<T> tmp(other);
        swap(tmp);
        return *this;
    }
    template<class Y>
    RefCntPoolPtr(const RefCntPoolPtr<Y>& other) : mRep(static_cast<T*>(other.getPointer())) { addRef(); }
    template<class Y>
    RefCntPoolPtr& operator=(const RefCntPoolPtr<Y>& other) {
        if (mRep == other.mRep)
            return *this; 
        // Swap current data into a local copy
        // this ensures we deal with rhs and this being dependent
        RefCntPoolPtr<T> tmp(other);
        swap(tmp);
        return *this;
    }
    virtual ~RefCntPoolPtr() { delRef(); }

    inline T& operator*() const { assert(mRep); return *mRep; }
    inline T* operator->() const { assert(mRep); return mRep; }
    inline T* get() const { return mRep; }
    inline void allocate() {
        delRef();
        Pool& pool = T::getStaticPool();
        pool.lock();
        if (pool.empty())
            mRep = new T();
        else
        {
            mRep = static_cast<T*>(pool.front());
            pool.pop_front();
        }
        pool.unlock();
        mRep->mRefCount = 1;
    }
    inline void bind(T* rep) {
        assert(mRep == 0);
        mRep = rep;
        mRep->mRefCount = 1;
    }
    inline bool unique() const {
        assert(mRep != 0);
        mRep->lock();
        bool unique = (mRep->mRefCount == 1);
        mRep->unlock();
        return unique;
    }
    inline unsigned int refCount() const {
        assert(mRep != 0);
        mRep->lock();
        unsigned int refCount = mRep->mRefCount;
        mRep->unlock();
        return refCount;
    }
    inline T* getPointer() const { return mRep; }
    inline bool isNull(void) const { return mRep == 0; }
    inline void setNull(void) { delRef(); mRep->mRefCount = 0; mRep = 0; }
};
template<class T, class U> inline bool operator==(RefCntPoolPtr<T> const& a, RefCntPoolPtr<U> const& b)
{
    return a.get() == b.get();
}
template<class T, class U> inline bool operator!=(RefCntPoolPtr<T> const& a, RefCntPoolPtr<U> const& b)
{
    return a.get() != b.get();
}
#endif

#ifdef POOL
class XMLDATAS_EXPORT XmlData : public PoolEntry
#else
class XMLDATAS_EXPORT XmlData
#endif
{
public:
    virtual std::string toXmlString() const = 0;
    virtual bool fromXmlElt(TiXmlElement* xmlElt) = 0;

    /** Function for writing to a stream.
    */
    inline friend std::ostream& operator<<(std::ostream& o, const XmlData& data)
    {
        o << data.toXmlString();
        return o;
    }
};

class XMLDATAS_EXPORT XmlLogin : public XmlData
{
#ifdef POOL
protected:
    static Pool mPool;
#endif

protected:
    std::string mUsername;
    std::string mPwd;

public:
    XmlLogin()
    {}
    XmlLogin(const std::string& username, const std::string& pwd) :
      mUsername(username),
      mPwd(pwd)
    {}

#ifdef POOL
    static Pool& getStaticPool();
    virtual Pool& getPool() const;
    virtual void clear() {
        mUsername.clear();
        mPwd.clear();
    }
#endif

    virtual std::string toXmlString() const;
    virtual bool fromXmlElt(TiXmlElement* xmlElt);

    void setUsername(const std::string& username) { mUsername = username; }
    const std::string& getUsername() { return mUsername; }

    void setPwd(const std::string& pwd) { mPwd = pwd; }
    const std::string& getPwd() { return mPwd; }
};

class XMLDATAS_EXPORT XmlContent : public XmlData
{
#ifdef POOL
protected:
    static Pool mPool;
#endif

public:
    typedef std::vector<std::string> ContentFileList;
    typedef std::map<Lod, ContentFileList> ContentLodMap;

protected:
    ContentLodMap contentLodMap;

public:
    XmlContent()
    {}

#ifdef POOL
    static Pool& getStaticPool();
    virtual Pool& getPool() const;
    virtual void clear() {
        contentLodMap.clear();
    }
#endif

    virtual std::string toXmlString() const;
    virtual bool fromXmlElt(TiXmlElement* xmlElt);

    ContentLodMap& getContentLodMap() { return contentLodMap; }
};

class XMLDATAS_EXPORT XmlEntity : public XmlData
{
#ifdef POOL
protected:
    static Pool mPool;
#endif

public:
    typedef unsigned int DefinedAttributes;
    static const DefinedAttributes DANone = (DefinedAttributes)0;
    static const DefinedAttributes DAUid = (DefinedAttributes)1;
    static const DefinedAttributes DAOwner = (DefinedAttributes)(DAUid << 1);
    static const DefinedAttributes DAType = (DefinedAttributes)(DAOwner << 1);
    static const DefinedAttributes DAName = (DefinedAttributes)(DAType << 1);
    static const DefinedAttributes DAFlags = (DefinedAttributes)(DAName << 1);
    static const DefinedAttributes DADisplacement = (DefinedAttributes)(DAFlags << 1);
    static const DefinedAttributes DAPosition = (DefinedAttributes)(DADisplacement << 1);
    static const DefinedAttributes DAOrientation = (DefinedAttributes)(DAPosition << 1);
    static const DefinedAttributes DAAABoundingBox = (DefinedAttributes)(DAOrientation << 1);
    static const DefinedAttributes DAAnimation = (DefinedAttributes)(DAAABoundingBox << 1);

protected:
    DefinedAttributes mDefinedAttributes;
    EntityUID mUid;
    NodeId mOwner;
    EntityType mType;
    std::string mName;
    EntityFlags mFlags;
    Ogre::Vector3 mDisplacement;
    Ogre::Vector3 mPosition;
    Ogre::Quaternion mOrientation;
    Ogre::AxisAlignedBox mAABoundingBox;
    XmlData* mAnimation;
    XmlData* mShape;
    XmlContent* mContent;

public:
    XmlEntity() :
      mDefinedAttributes(DANone),
      mUid(0),
      mOwner(""),
      mType(ETAvatar),
      mName(""),
      mFlags(EFNone),
      mDisplacement(Ogre::Vector3::ZERO),
      mPosition(Ogre::Vector3::ZERO),
      mOrientation(Ogre::Quaternion::IDENTITY),
      mAnimation(0),
      mShape(0),
      mContent(0)
    {}
    XmlEntity(const EntityUID& uid) :
      mDefinedAttributes(DAUid),
      mUid(uid),
      mOwner(""),
      mType(ETAvatar),
      mName(""),
      mFlags(EFNone),
      mDisplacement(Ogre::Vector3::ZERO),
      mPosition(Ogre::Vector3::ZERO),
      mOrientation(Ogre::Quaternion::IDENTITY),
      mAnimation(0),
      mShape(0),
      mContent(0)
    {}
    virtual ~XmlEntity();

#ifdef POOL
    static Pool& getStaticPool();
    virtual Pool& getPool() const;
    virtual void clear() {
        mDefinedAttributes = DANone;
        mUid = 0;
        mOwner.clear();
        mType = ETAvatar;
        mName.clear();
        mFlags = EFNone;
        mDisplacement = Ogre::Vector3::ZERO;
        mPosition = Ogre::Vector3::ZERO;
        mOrientation = Ogre::Quaternion::IDENTITY;
        mAnimation = 0;
        mShape = 0;
        mContent = 0;
    }
#endif

    virtual std::string toXmlString() const;
    virtual bool fromXmlElt(TiXmlElement* xmlElt);

    void setDefinedAttributes(const DefinedAttributes& definedAttributes) { mDefinedAttributes = definedAttributes; }
    DefinedAttributes getDefinedAttributes() { return mDefinedAttributes; }

    void setUid(const EntityUID& uid) { mUid = uid; mDefinedAttributes |= DAUid; }
    const EntityUID& getUid() { return mUid; }

    void setOwner(const NodeId& owner) { mOwner = owner; mDefinedAttributes |= DAOwner; }
    const NodeId& getOwner() { return mOwner; }

    void setType(const EntityType& type) { mType = type; mDefinedAttributes |= DAType; }
    EntityType getType() { return mType; }

    void setName(const std::string& name) { mName = name; mDefinedAttributes |= DAName; }
    const std::string& getName() { return mName; }

    void setFlags(const EntityFlags& flags) { mFlags = flags; mDefinedAttributes |= DAFlags; }
    EntityFlags getFlags() { return mFlags; }

    void setDisplacement(const Ogre::Vector3& displacement) { mDisplacement = displacement; mDefinedAttributes |= DADisplacement; }
    const Ogre::Vector3& getDisplacement() { return mDisplacement; }

    void setPosition(const Ogre::Vector3& position) { mPosition = position; mDefinedAttributes |= DAPosition; }
    const Ogre::Vector3& getPosition() { return mPosition; }

    void setOrientation(const Ogre::Quaternion& orientation) { mOrientation = orientation; mDefinedAttributes |= DAOrientation; }
    const Ogre::Quaternion& getOrientation() { return mOrientation; }

    void setAABoundingBox(const Ogre::AxisAlignedBox& AABoundingBox) { mAABoundingBox = AABoundingBox; mDefinedAttributes |= DAAABoundingBox; }
    const Ogre::AxisAlignedBox& getAABoundingBox() { return mAABoundingBox; }

    void setContent(XmlContent* content) { mContent = content; }
    XmlContent* getContent() { return mContent; }
};

class XMLDATAS_EXPORT XmlEvt : public XmlData
{
#ifdef POOL
protected:
    static Pool mPool;
#endif

protected:
    EventType mType;
#ifdef POOL
    RefCntPoolPtr<XmlData> mDatas;
#else
    XmlData* mDatas;
#endif

public:
    XmlEvt() :
        mType(ETNewEntity),
#ifdef POOL
        mDatas(RefCntPoolPtr<XmlData>::nullPtr)
#else
        mDatas(0)
#endif
    {}
    XmlEvt(const EventType& type) :
        mType(type),
#ifdef POOL
        mDatas(RefCntPoolPtr<XmlData>::nullPtr)
#else
        mDatas(0)
#endif
    {}

#ifdef POOL
    static Pool& getStaticPool();
    virtual Pool& getPool() const;
    virtual void clear() {
        mType = ETNewEntity;
        mDatas = RefCntPoolPtr<XmlData>::nullPtr;
    }
#endif

    virtual std::string toXmlString() const;
    virtual bool fromXmlElt(TiXmlElement* xmlElt);

    void setType(const EventType& type) { mType = type; }
    EventType getType() { return mType; }

#ifdef POOL
    void setDatas(RefCntPoolPtr<XmlData>& datas) { mDatas = datas; }
    RefCntPoolPtr<XmlData>& getDatas() { return mDatas; }
#else
    void setDatas(XmlData* datas) { mDatas = datas; }
    XmlData* getDatas() { return mDatas; }
#endif
};

#ifdef POOL
RefCntPoolPtr<XmlData> RefCntPoolPtr<XmlData>::nullPtr((XmlData*)0);
RefCntPoolPtr<XmlLogin> RefCntPoolPtr<XmlLogin>::nullPtr((XmlLogin*)0);
RefCntPoolPtr<XmlContent> RefCntPoolPtr<XmlContent>::nullPtr((XmlContent*)0);
RefCntPoolPtr<XmlEntity> RefCntPoolPtr<XmlEntity>::nullPtr((XmlEntity*)0);
RefCntPoolPtr<XmlEvt> RefCntPoolPtr<XmlEvt>::nullPtr((XmlEvt*)0);
#endif

} // namespace Solipsis

#endif // #ifndef __XmlDatas_h__
