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

class XMLDATAS_EXPORT XmlData
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

    virtual std::string toXmlString() const;
    virtual bool fromXmlElt(TiXmlElement* xmlElt);

    void setUsername(const std::string& username) { mUsername = username; }
    const std::string& getUsername() { return mUsername; }

    void setPwd(const std::string& pwd) { mPwd = pwd; }
    const std::string& getPwd() { return mPwd; }
};

class XMLDATAS_EXPORT XmlContent : public XmlData
{
public:
    typedef std::vector<std::string> ContentFileList;
    typedef std::map<Lod, ContentFileList> ContentLodMap;

protected:
    ContentLodMap contentLodMap;

public:
    XmlContent()
    {}

    virtual std::string toXmlString() const;
    virtual bool fromXmlElt(TiXmlElement* xmlElt);

    ContentLodMap& getContentLodMap() { return contentLodMap; }
};

class XMLDATAS_EXPORT XmlEntity : public XmlData
{
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
public:
    XmlEvt() :
        mType(ETNewEntity),
        mDatas(0)
    {}
    XmlEvt(const EventType& type) :
        mType(type),
        mDatas(0)
    {}

    virtual std::string toXmlString() const;
    virtual bool fromXmlElt(TiXmlElement* xmlElt);

    void setType(const EventType& type) { mType = type; }
    EventType getType() { return mType; }

    void setDatas(XmlData* datas) { mDatas = datas; }
    XmlData* getDatas() { return mDatas; }

protected:
    EventType mType;
    XmlData* mDatas;
};

} // namespace Solipsis

#endif // #ifndef __XmlDatas_h__