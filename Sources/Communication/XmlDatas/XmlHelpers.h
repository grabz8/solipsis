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

#ifndef __XmlHelpers_h__
#define __XmlHelpers_h__

// 
// #include <list>
// #include <map>
// #include <ostream>
#include "XmlDatasPrerequisites.h"
#include "XmlDatasBasicTypes.h"
#include <Ogre.h>
#include <tinyxml.h>


namespace Solipsis 
{

class XMLDATAS_EXPORT XmlHelpers
{
public:
    static bool getAttribute(TiXmlElement* elt, const char* attrName, const char*& attr);
    static std::string convertUCharToHexString(unsigned char value);
    static unsigned char convertHexStringToUChar(const char* str);
    static std::string convertUIntToHexString(unsigned int value);
    static unsigned int convertHexStringToUInt(const char* str);
    static std::string convertBoolToString(bool value);
    static bool convertStringToBool(const char* str);
    static std::string xmlEscape(const std::string &str);
    static std::string xmlUnescape(const std::string &str);
    static std::string convertWStringToUTF8(const std::string &enc, const std::wstring &wstr);
    static std::wstring convertUTF8ToWString(const std::string &enc, const std::string &utf8str);
    static std::ostream& ostreamVector3(std::ostream& o, const Ogre::Vector3& v);
    static TiXmlElement* toXmlEltVector3(const std::string& eltName, const Ogre::Vector3& v);
    static bool fromXmlEltVector3(TiXmlElement* xmlElt, Ogre::Vector3& v);
    static std::ostream& ostreamQuaternion(std::ostream& o, const Ogre::Quaternion& q);
    static TiXmlElement* toXmlEltQuaternion(const std::string& eltName, const Ogre::Quaternion& q);
    static bool fromXmlEltQuaternion(TiXmlElement* xmlElt, Ogre::Quaternion& q);
    static TiXmlElement* toXmlEltLodContentFileStruct(const std::string& eltName, const LodContentFileStruct& s);
    static bool fromXmlEltLodContentFileStruct(TiXmlElement* xmlElt, LodContentFileStruct& s);
    static TiXmlElement* toXmlEltEntryGateStruct(const std::string& eltName, const EntryGateStruct& s);
    static bool fromXmlEltEntryGateStruct(TiXmlElement* xmlElt, EntryGateStruct& s);

    static std::string convertAuthentTypeToRepr(const AuthentType& authentType);
    static const std::string& convertEventTypeToRepr(const EventType& evtType);
    static inline void convertDecStringToEventType(const char* str, EventType& evtType) { evtType = (EventType)atoi(str); }
    static const std::string& convertEntityTypeToRepr(const EntityType& entityType);
    static inline void convertDecStringToEntityType(const char* str, EntityType& entityType) { entityType = (EntityType)atoi(str); }
    static const std::string& convertActionTypeToRepr(const ActionType& actionType);
    static inline void convertDecStringToActionType(const char* str, ActionType& actionType) { actionType = (ActionType)atoi(str); }
    static std::string convertEntityFlagsToRepr(const EntityFlags& entityFlags);
    static inline std::string convertEntityFlagsToHexString(const EntityFlags& entityFlags) { return XmlHelpers::convertUIntToHexString(entityFlags); }
    static inline EntityFlags convertHexStringToEntityFlags(const char* str) { return XmlHelpers::convertHexStringToUInt(str); }
    static const std::string& convertShapeTypeToRepr(const ShapeType& shapeType);
    static inline void convertDecStringToShapeType(const char* str, ShapeType& shapeType) { shapeType = (ShapeType)atoi(str); }
    static inline std::string convertEntityVersionToHexString(const EntityVersion& entityVersion) { return XmlHelpers::convertUIntToHexString(entityVersion); }
    static inline EntityVersion convertHexStringToEntityVersion(const char* str) { return XmlHelpers::convertHexStringToUInt(str); }
    static inline void convertDecStringToLod(const char* str, Lod& lod) { lod = (Lod)atoi(str); }
    static inline std::string convertFileVersionToHexString(const FileVersion& fileVersion) { return XmlHelpers::convertUIntToHexString(fileVersion); }
    static inline FileVersion convertHexStringToFileVersion(const char* str) { return XmlHelpers::convertHexStringToUInt(str); }
    static inline std::string convertAnimationStateToHexString(AnimationState animationState) { return XmlHelpers::convertUCharToHexString(animationState); }
    static inline AnimationState convertHexStringToAnimationState(const char* str) { return XmlHelpers::convertHexStringToUChar(str); }
};


} // namespace Solipsis

#endif // #ifndef __XmlHelpers_h__
