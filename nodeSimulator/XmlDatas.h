#ifndef __XmlDatas_h__
#define __XmlDatas_h__

#include <vector>
#include <map>
#include <ostream>
#include "Ogre.h"
#include "tinyxml.h"

namespace Solipsis {

template<typename Holder, typename FlagEnum>
class Bitmask
{
public:
	Bitmask(Holder defval = 0) : Bitfield(defval) { }

	// Sets all bitmask bits which are set in "flags"
	inline void Set(FlagEnum flags)
	{ Bitfield |= flags; }

	// Clears all bitmask bits which are set in "flags"
	inline void Clear(FlagEnum flags)
	{ Bitfield &= (~flags); }

	// Return true if ANY of the bits set in "flags" are
	// also set for the bitmask
	inline bool Test(FlagEnum flags) const
	{ return ((Bitfield & flags) != 0); }

	// Return true only if ALL bits set in "flags" are
	// also set for the bitmask
	inline bool TestAllFlags(FlagEnum flags) const
	{ return ((Bitfield & flags) == flags); }

protected:
	Holder Bitfield;
};

enum EventType {
    ETNewObject = 0,        /// New object
    ETLostObject = 1,       /// Lost object
    ETUpdatedObject = 2,    /// Updated object
    ETActionOnObject = 3,   /// Action on object
    ETStatusReport = 100    /// Status report
};

enum ObjectType {
    OTAvatar = 0,       /// Avatar
    OTScene = 1,        /// Scene
    OTObject = 2        /// Object (movable)
};

enum ShapeType {
    STPoint = 0,            /// Avatar
    STCircle = 1,           /// Scene
    ST2DRect = 2,           /// Object (movable)
    STSphere = 100,         /// Object (movable)
    STBox = 101,            /// Object (movable)
    STPolygonVolume = 200   /// Object (movable)
};

typedef unsigned int ObjectUID;
typedef unsigned int Lod;

class XmlData
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

class XmlLogin : public XmlData
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

class XmlContent : public XmlData
{
protected:
    typedef std::vector<std::string> ContentFileList;
    typedef std::map<Lod, ContentFileList> ContentLodMap;
    ContentLodMap contentLodMap;

public:
    XmlContent()
    {}

    virtual std::string toXmlString() const;
    virtual bool fromXmlElt(TiXmlElement* xmlElt);
};

class XmlObject : public XmlData
{
public:
    enum DefinedAttributes {
        DANone = 0,
        DAUid = 1,
        DAType = DAUid << 1,
        DAName = DAType << 1,
        DAPosition = DAName << 1,
        DAOrientation = DAPosition << 1,
        DAAnimation = DAOrientation << 1
    };

protected:
    Bitmask<unsigned int, DefinedAttributes> mDefinedAttributes;
    ObjectUID mUid;
    ObjectType mType;
    std::string mName;
    Ogre::Vector3 mPosition;
    Ogre::Quaternion mOrientation;
    XmlData* mAnimation;
    XmlData* mShape;
    XmlData* mContent;

public:
    XmlObject() :
      mDefinedAttributes(DANone),
      mUid(0),
      mAnimation(0),
      mShape(0),
      mContent(0)
    {}
    XmlObject(const ObjectUID& uid) :
      mDefinedAttributes(DAUid),
      mUid(uid),
      mAnimation(0),
      mShape(0),
      mContent(0)
    {}

    virtual std::string toXmlString() const;
    virtual bool fromXmlElt(TiXmlElement* xmlElt);

    void setDefinedAttributes(DefinedAttributes definedAttributes) { mDefinedAttributes = definedAttributes; }
    const Bitmask<unsigned int, DefinedAttributes>& getDefinedAttributes() { return mDefinedAttributes; }

    void setUid(const ObjectUID& uid) { mUid = uid; mDefinedAttributes.Set(DAUid); }
    const ObjectUID& getUid() { return mUid; }

    void setType(const ObjectType& type) { mType = type; mDefinedAttributes.Set(DAType); }
    ObjectType getType() { return mType; }

    void setName(const std::string& name) { mName = name; mDefinedAttributes.Set(DAName); }
    const std::string& getName() { return mName; }

    void setPosition(const Ogre::Vector3& position) { mPosition = position; mDefinedAttributes.Set(DAPosition); }
    const Ogre::Vector3& getPosition() { return mPosition; }

    void setOrientation(const Ogre::Quaternion& orientation) { mOrientation = orientation; mDefinedAttributes.Set(DAOrientation); }
    const Ogre::Quaternion& getOrientation() { return mOrientation; }
};

class XmlEvt : public XmlData
{
public:
    XmlEvt() :
        mType(ETNewObject),
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