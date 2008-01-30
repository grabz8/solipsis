#include "XmlDatas.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
bool getAttribute(TiXmlElement* elt, const char* attrName, const char*& attr)
{
    if (elt == 0)
        return false;
    attr = elt->Attribute(attrName);
    if ((attr == 0) || (attr[0] == '\0'))
        return false;
    return true;
}

//-------------------------------------------------------------------------------------
std::ostream& ostreamVector3(std::ostream& o, const Ogre::Vector3& v)
{
    o << "x=\"" << v.x << "\" y=\"" << v.y << "\" z=\"" << v.z << "\"";
    return o;
}

//-------------------------------------------------------------------------------------
std::ostream& ostreamQuaternion(std::ostream& o, const Ogre::Quaternion& q)
{
    o << "x=\"" << q.x << "\" y=\"" << q.y << "\" z=\"" << q.z << "\" w=\"" << q.w << "\"";
    return o;
}

//-------------------------------------------------------------------------------------
bool fromXmlEltVector3(TiXmlElement* xmlElt, Ogre::Vector3& v)
{
    const char* attr = 0;
    if (!getAttribute(xmlElt, "x", attr)) return false;
    v.x = atof(attr);
    if (!getAttribute(xmlElt, "y", attr)) return false;
    v.y = atof(attr);
    if (!getAttribute(xmlElt, "z", attr)) return false;
    v.z = atof(attr);
    return true;
}

//-------------------------------------------------------------------------------------
bool fromXmlEltQuaternion(TiXmlElement* xmlElt, Ogre::Quaternion& q)
{
    const char* attr = 0;
    if (!getAttribute(xmlElt, "x", attr)) return false;
    q.x = atof(attr);
    if (!getAttribute(xmlElt, "y", attr)) return false;
    q.y = atof(attr);
    if (!getAttribute(xmlElt, "z", attr)) return false;
    q.z = atof(attr);
    if (!getAttribute(xmlElt, "w", attr)) return false;
    q.w = atof(attr);
    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlLogin::toXmlString() const
{
    std::stringstream s;
    s << "<username>" << mUsername << "</username>";
    s << "<pwd>" << mPwd << "</pwd>";
    s << "<ctxt>";
    s << "<cnxMode>";
    s << 0;
    s << "</cnxMode>";
    s << "</ctxt>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlLogin::fromXmlElt(TiXmlElement* xmlElt)
{
    TiXmlElement* elt;

    if ((elt = xmlElt->FirstChildElement("username")) == 0)
        return false;
    mUsername = elt->GetText();

    if ((elt = xmlElt->FirstChildElement("pwd")) == 0)
        return false;
    mPwd = elt->GetText();

    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlContent::toXmlString() const
{
    std::stringstream s;
    s << "<content>";
    for (ContentLodMap::const_iterator lod = contentLodMap.begin(); lod != contentLodMap.end(); ++lod)
    {
        const ContentFileList& contentFileList = lod->second;
        s << "<lod level=" << lod->first << ">";
        s << "<files>";
        for (ContentFileList::const_iterator file = contentFileList.begin(); file != contentFileList.end(); ++file)
        {
            s << "<file name=" << (*file) << "/>";
        }
        s << "</files>";
        s << "</lod>";
    }
    s << "</content>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlContent::fromXmlElt(TiXmlElement* xmlElt)
{
    contentLodMap.clear();

    ContentFileList contentFileList;
    TiXmlElement* elt;
    const char* attr = 0;

    for (TiXmlElement* lodElt = xmlElt->FirstChildElement("lod"); lodElt != 0; lodElt = lodElt->NextSiblingElement("lod"))
    {
        if (!getAttribute(lodElt, "level", attr)) return false;
        Lod lod = static_cast<Lod>(strtoul(attr, 0, 10));

        if ((elt = lodElt->FirstChildElement("files")) == 0)
            return false;

        contentFileList.clear();
        for (TiXmlElement* fileElt = elt->FirstChildElement("file"); fileElt != 0; fileElt = fileElt->NextSiblingElement("file"))
        {
            if (!getAttribute(fileElt, "name", attr)) return false;
            contentFileList.push_back(std::string(attr));
        }

        contentLodMap[lod] = contentFileList;
    }

    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlObject::toXmlString() const
{
    std::stringstream s;
    if (!mDefinedAttributes.Test(DAUid)) return s.str();
    s << "<object uid=" << mUid;
    if (mDefinedAttributes.Test(DAType)) s << " type=" << mType;
    if (mDefinedAttributes.Test(DAName)) s << " name=" << mName;
    s << ">";
    if (mDefinedAttributes.Test(DAPosition)) ostreamVector3(s << "<position ", mPosition) << " />";
    if (mDefinedAttributes.Test(DAOrientation)) ostreamQuaternion(s << "<orientation ", mOrientation) << " />";
    if (mAnimation != 0) s << mAnimation->toXmlString();
    if (mShape != 0) s << mShape->toXmlString();
    if (mContent != 0) s << mContent->toXmlString();
    s << "</object>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlObject::fromXmlElt(TiXmlElement* xmlElt)
{
    delete mAnimation;
    mAnimation = 0;
    delete mShape;
    mShape = 0;
    delete mContent;
    mContent = 0;

    TiXmlElement* elt;
    const char* attr = 0;

    if (!getAttribute(xmlElt, "uid", attr)) return false;
    mUid = static_cast<ObjectUID>(strtoul(attr, 0, 10));

    if (getAttribute(xmlElt, "type", attr))
    {
        mType = static_cast<ObjectType>(strtoul(attr, 0, 10));
        mDefinedAttributes.Set(DAType);
    }

    if (getAttribute(xmlElt, "name", attr))
    {
        mName = attr;
        mDefinedAttributes.Set(DAName);
    }

    if ((elt = xmlElt->FirstChildElement("position")) != 0)
    {
        fromXmlEltVector3(elt, mPosition);
        mDefinedAttributes.Set(DAPosition);
    }
    if ((elt = xmlElt->FirstChildElement("orientation")) != 0)
    {
        fromXmlEltQuaternion(elt, mOrientation);
        mDefinedAttributes.Set(DAOrientation);
    }
    if ((elt = xmlElt->FirstChildElement("animation")) != 0)
    {
        mDefinedAttributes.Set(DAAnimation);
    }
    if ((elt = xmlElt->FirstChildElement("shape")) != 0)
    {
//        mShape = new XmlShape();
//        mShape->fromXmlElt(elt);
    }
    if ((elt = xmlElt->FirstChildElement("content")) != 0)
    {
        mContent = new XmlContent();
        mContent->fromXmlElt(elt);
    }

    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlEvt::toXmlString() const
{
    std::stringstream s;
    s << "<evt type=\"" << mType << "\">";
    if (mDatas != 0) s << mDatas->toXmlString();
    s << "</evt>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlEvt::fromXmlElt(TiXmlElement* xmlElt)
{
    delete mDatas;
    mDatas = 0;

    TiXmlElement* elt;
    const char* attr = 0;

    if ((elt = xmlElt->FirstChildElement("evt")) == 0)
        return false;

    if (!getAttribute(elt, "type", attr)) return false;
    mType = static_cast<EventType>(strtoul(attr, 0, 10));

    if ((elt = elt->FirstChildElement("object")) != 0)
    {
        XmlObject* object = new XmlObject();
        object->fromXmlElt(elt);
        mDatas = object;
    }

    return true;
}

//-------------------------------------------------------------------------------------
