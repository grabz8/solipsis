/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <Ogre.h>

#include "XmlHelpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>

namespace Solipsis {

//-------------------------------------------------------------------------------------
bool XmlHelpers::getAttribute(TiXmlElement* elt, const char* attrName, const char*& attr)
{
    if (elt == 0)
        return false;
    attr = elt->Attribute(attrName);
    if ((attr == 0) || (attr[0] == '\0'))
        return false;
    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlHelpers::convertUCharToHexString(unsigned char value)
{
    char valueStr[3];
    _snprintf(valueStr, sizeof(valueStr) - 1, "%02X", value);
    valueStr[2] = '\0';
    return valueStr;
}

//-------------------------------------------------------------------------------------
unsigned char XmlHelpers::convertHexStringToUChar(const char* str)
{
    unsigned int value;
    sscanf(str, "%02X", &value);
    return value;
}

//-------------------------------------------------------------------------------------
std::string XmlHelpers::convertUIntToHexString(unsigned int value)
{
    char valueStr[9];
    _snprintf(valueStr, sizeof(valueStr) - 1, "%08X", value);
    valueStr[8] = '\0';
    return valueStr;
}

//-------------------------------------------------------------------------------------
unsigned int XmlHelpers::convertHexStringToUInt(const char* str)
{
    unsigned int value;
    sscanf(str, "%08X", &value);
    return value;
}


//-------------------------------------------------------------------------------------
float XmlHelpers::convertStringToFloat(const char* str)
{
    float value;
    sscanf(str, "%d", &value);
    return value;
}


//-------------------------------------------------------------------------------------
std::string XmlHelpers::convertBoolToString(bool value)
{
    return (value ? "true" : "false");
}

//-------------------------------------------------------------------------------------
bool XmlHelpers::convertStringToBool(const char* str)
{
    if (stricmp(str, "false") == 0)
        return false;
    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlHelpers::xmlEscape(const std::string &str)
{
    std::string ret;
    std::string::size_type prev = 0;
    std::string::size_type len = str.length();
    std::string::size_type curs = 0;
    const char *pc = str.data();

    const char *amp = "&amp;";
    const char *lt = "&lt;";
    const char *gt = "&gt;";
    const char *apos = "&#39;";
    const char *quotes = "&quot;";

    while (curs != len)
    {
        char c = *pc++;
        if (c == '&')
        {
            ret += str.substr(prev, curs - prev);
            ret += amp;
            prev = curs + 1;
        }
        else if (c == '<')
        {
            ret += str.substr(prev, curs - prev);
            ret += lt;
            prev = curs + 1;
        }
        else if (c == '>')
        {
            ret += str.substr(prev, curs - prev);
            ret += gt;
            prev = curs + 1;
        }
        else if (c == '\'')
        {
            ret += str.substr(prev, curs - prev);
            ret += apos;
            prev = curs + 1;
        }
        else if (c == '"')
        {
            ret += str.substr(prev, curs - prev);
            ret += quotes;
            prev = curs + 1;
        }
        ++curs;
    }
    ret += str.substr(prev, curs - prev);
    return ret;
}

//-------------------------------------------------------------------------------------
std::string XmlHelpers::xmlUnescape(const std::string &str)
{
    std::string ret;
    std::string::size_type prev = 0;
    std::string::size_type len = str.length();
    std::string::size_type curs = 0;
    std::string s;

    const char *amp = "&amp;";
    const char *lt = "&lt;";
    const char *gt = "&gt;";
    const char *apos = "&#35;";
    const char *quotes = "&quot;";

    while (curs < len)
    {
        const char *pc = str.data() + curs;
        char c = *pc;
        if (c == '&')
        {
            if (curs == len - 1)
                throw std::exception("XmlHelpers::xmlUnescape() conversion failure !");

            if (len - curs >= 5 && strncmp(pc, amp, 5) == 0)
            {
                ret += str.substr(prev, curs - prev);
                ret += "&";
                curs += 5;
                prev = curs;
            }
            else if (len - curs >= 4 && strncmp(pc, lt, 4) == 0)
            {
                ret += str.substr(prev, curs - prev);
                ret += "<";
                curs += 4;
                prev = curs;
            }
            else if (len - curs >= 4 && strncmp(pc, gt, 4) == 0)
            {
                ret += str.substr(prev, curs - prev);
                ret += ">";
                curs += 4;
                prev = curs;
            }
            else if (len - curs >= 5 && strncmp(pc, apos, 5) == 0)
            {
                ret += str.substr(prev, curs - prev);
                ret += "\'";
                curs += 5;
                prev = curs;
            }
            else if (len - curs >= 5 && strncmp(pc, quotes, 6) == 0)
            {
                ret += str.substr(prev, curs - prev);
                ret += "\"";
                curs += 6;
                prev = curs;
            }
            else
                throw std::exception("XmlHelpers::xmlUnescape() conversion failure !");
        }
        else
            ++curs;
    }
    ret += str.substr(prev, curs - prev);

    return ret;
}

//-------------------------------------------------------------------------------------
std::string XmlHelpers::convertWStringToUTF8(const std::string &enc, const std::wstring &wstr)
{
    if (wstr.empty())
        return std::string();

    iconv_t     cd = iconv_open("UTF-8", enc.c_str());
    char        *sptr = (char *)wstr.c_str();
    char        *sptr_oldErr = 0;
    size_t      slen = wstr.length()*sizeof(wchar_t);
    std::string out;
    size_t      olen_alloc = wstr.length()*6; // worst case
    size_t      olen;
    char        *obuf = (char*)malloc(olen_alloc);
    char        *optr;

    while (slen > 0)
    {
        obuf[0] = '\0';
        optr = (char *)obuf;
        olen = olen_alloc - sizeof(char);
        size_t ret = iconv(cd, (const char**)&sptr, &slen, &optr, &olen);
        if (ret == size_t(-1))
        {
            if (sptr_oldErr == sptr)
            {
                iconv_close(cd);
                free(obuf);
                char tmp[256];
                _snprintf(tmp, sizeof(tmp) - 1, "XmlHelpers::convertUTF8ToWString() conversion failure (errno:%d) !", errno);
                throw std::exception(tmp);
            }
            // we save pos where iconv stop conversion and we make another attempt
            // by re-allocating twice the output buffer, if same pos is still on error
            // then we will thow an definitive exception
            sptr_oldErr = sptr;
            olen_alloc *= 2;
            obuf = (char*)realloc(obuf, olen_alloc);
            out.clear();
            sptr = (char *)wstr.c_str();
            slen = wstr.length()*sizeof(wchar_t);
            continue;
        }
        *optr = '\0';
        out  += obuf;
    }
    iconv_close(cd);
    free(obuf);

    return out;
}

//-------------------------------------------------------------------------------------
std::wstring XmlHelpers::convertUTF8ToWString(const std::string &enc, const std::string &utf8str)
{
    if (utf8str.empty())
        return std::wstring();

    iconv_t         cd = iconv_open(enc.c_str(), "UTF-8");
    std::wstring    out;
    char            *sptr = (char *)utf8str.c_str();
    char            *sptr_oldErr = 0;
    size_t          slen = utf8str.length();
    size_t          olen_alloc = utf8str.length()*2; // worst case
    size_t          olen;
    wchar_t         *obuf = (wchar_t*)malloc(olen_alloc);
    char            *optr;

    while (slen > 0)
    {
        obuf[0] = L'\0';
        optr = (char *)obuf;
        olen = olen_alloc - sizeof(wchar_t);
        size_t ret = iconv(cd, (const char**)&sptr, &slen, &optr, &olen);
        if (ret == size_t(-1))
        {
            if (sptr_oldErr == sptr)
            {
                iconv_close(cd);
                free(obuf);
                char tmp[256];
                _snprintf(tmp, sizeof(tmp) - 1, "XmlHelpers::convertUTF8ToWString() conversion failure (errno:%d) !", errno);
                throw std::exception(tmp);
            }
            // we save pos where iconv stop conversion and we make another attempt
            // by re-allocating twice the output buffer, if same pos is still on error
            // then we will thow an definitive exception
            sptr_oldErr = sptr;
            olen_alloc *= 2;
            obuf = (wchar_t*)realloc(obuf, olen_alloc);
            out.clear();
            sptr = (char *)utf8str.c_str();
            slen = utf8str.length();
            continue;
        }
        *((wchar_t *)optr) = L'\0';
        out += obuf;
    }
    iconv_close(cd);
    free(obuf);

    return out;
}

//-------------------------------------------------------------------------------------
std::ostream& XmlHelpers::ostreamVector3(std::ostream& o, const Ogre::Vector3& v)
{
    o << "x=\"" << v.x << "\" y=\"" << v.y << "\" z=\"" << v.z << "\"";
    return o;
}

//-------------------------------------------------------------------------------------
TiXmlElement* XmlHelpers::toXmlEltVector3(const std::string& eltName, const Ogre::Vector3& v)
{
    TiXmlElement* vector3Elt = new TiXmlElement(eltName.c_str());
    vector3Elt->SetAttribute("x", Ogre::StringConverter::toString(v.x).c_str());
    vector3Elt->SetAttribute("y", Ogre::StringConverter::toString(v.y).c_str());
    vector3Elt->SetAttribute("z", Ogre::StringConverter::toString(v.z).c_str());
    return vector3Elt;
}

//-------------------------------------------------------------------------------------
bool XmlHelpers::fromXmlEltVector3(TiXmlElement* xmlElt, Ogre::Vector3& v)
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
std::ostream& XmlHelpers::ostreamQuaternion(std::ostream& o, const Ogre::Quaternion& q)
{
    o << "x=\"" << q.x << "\" y=\"" << q.y << "\" z=\"" << q.z << "\" w=\"" << q.w << "\"";
    return o;
}

//-------------------------------------------------------------------------------------
TiXmlElement* XmlHelpers::toXmlEltQuaternion(const std::string& eltName, const Ogre::Quaternion& q)
{
    TiXmlElement* quaternionElt = new TiXmlElement(eltName.c_str());
    quaternionElt->SetAttribute("x", Ogre::StringConverter::toString(q.x).c_str());
    quaternionElt->SetAttribute("y", Ogre::StringConverter::toString(q.y).c_str());
    quaternionElt->SetAttribute("z", Ogre::StringConverter::toString(q.z).c_str());
    quaternionElt->SetAttribute("w", Ogre::StringConverter::toString(q.w).c_str());
    return quaternionElt;
}

//-------------------------------------------------------------------------------------
bool XmlHelpers::fromXmlEltQuaternion(TiXmlElement* xmlElt, Ogre::Quaternion& q)
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
TiXmlElement* XmlHelpers::toXmlEltLodContentFileStruct(const std::string& eltName, const LodContentFileStruct& s)
{
    TiXmlElement* lodContentFileStructElt = new TiXmlElement(eltName.c_str());
    lodContentFileStructElt->SetAttribute("name", s.mFilename.c_str());
    lodContentFileStructElt->SetAttribute("version", XmlHelpers::convertFileVersionToHexString(s.mVersion).c_str());
    return lodContentFileStructElt;
}

//-------------------------------------------------------------------------------------
bool XmlHelpers::fromXmlEltLodContentFileStruct(TiXmlElement* xmlElt, LodContentFileStruct& s)
{
    const char* attr = 0;
    if (!getAttribute(xmlElt, "name", attr)) return false;
    s.mFilename = std::string(attr);
    if (!getAttribute(xmlElt, "version", attr)) return false;
    s.mVersion = XmlHelpers::convertHexStringToFileVersion(attr);
    return true;
}

//-------------------------------------------------------------------------------------
TiXmlElement* XmlHelpers::toXmlEltEntryGateStruct(const std::string& eltName, const EntryGateStruct& s)
{
    TiXmlElement* entryGateStructElt = new TiXmlElement(eltName.c_str());
    entryGateStructElt->SetAttribute("gravity", XmlHelpers::convertBoolToString(s.mGravity).c_str());
    entryGateStructElt->LinkEndChild(XmlHelpers::toXmlEltVector3("position", s.mPosition));
    return entryGateStructElt;
}

//-------------------------------------------------------------------------------------
bool XmlHelpers::fromXmlEltEntryGateStruct(TiXmlElement* xmlElt, EntryGateStruct& s)
{
    const char* attr = 0;
    if (!getAttribute(xmlElt, "gravity", attr)) return false;
    s.mGravity = XmlHelpers::convertStringToBool(attr);
    TiXmlElement* subElt;
    if ((subElt = xmlElt->FirstChildElement("position")) == 0)
        return false;
    XmlHelpers::fromXmlEltVector3(subElt, s.mPosition);
    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlHelpers::convertAuthentTypeToRepr(const AuthentType& authentType)
{
    char prefix[2];
    prefix[0] = authentType;
    prefix[1] = '\0';
    return std::string(prefix);
}

//-------------------------------------------------------------------------------------
const std::string& XmlHelpers::convertEventTypeToRepr(const EventType& evtType)
{
    static std::string EventTypeRepr[] = {
        "ETNewEntity",
        "ETLostEntity",
        "ETUpdatedEntity",
        "ETActionOnEntity",
        "ETStatusReport"
    };
    return EventTypeRepr[evtType];
}

//-------------------------------------------------------------------------------------
const std::string& XmlHelpers::convertEntityTypeToRepr(const EntityType& entityType)
{
    static std::string EntityTypeRepr[] = {
        "ETAvatar",
        "ETSite",
        "ETObject"
    };
    return EntityTypeRepr[entityType];
}

//-------------------------------------------------------------------------------------
const std::string& XmlHelpers::convertActionTypeToRepr(const ActionType& actionType)
{
    static std::string ActionTypeRepr[] = {
        "ATNone",
        "ATChat"
    };
    return ActionTypeRepr[actionType];
}

//-------------------------------------------------------------------------------------
std::string XmlHelpers::convertEntityFlagsToRepr(const EntityFlags& entityFlags)
{
    std::string entityFlagsRepr;
    if (entityFlags & EFGravity)
        entityFlagsRepr += "EFGravity|";
    if (entityFlagsRepr.empty())
        entityFlagsRepr = "EFNone";
    else
        entityFlagsRepr.erase(entityFlagsRepr.length() - 1, 1);
    return entityFlagsRepr;
}

//-------------------------------------------------------------------------------------
const std::string& XmlHelpers::convertShapeTypeToRepr(const ShapeType& shapeType)
{
    static std::string ShapeTypeRepr[] = {
        "STPoint",
        "STCircle",
        "ST2DRect",
        "STSphere",
        "STBox",
        "STPolygonVolume"
    };
    return ShapeTypeRepr[shapeType];
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
