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

#ifndef __XmlDatasBasicTypes_h__
#define __XmlDatasBasicTypes_h__

#include <string>
#include <Ogre.h>


namespace Solipsis {

#define SOLIPSIS_MAX_NODEID_LEN 64
typedef std::string NodeId;

typedef std::string EntityUID;


enum AuthentType {
    ATFacebook = 'F',   // User is authenticated by facebook site on external browser
    ATSolipsis = 'S',   // User is authenticated by the Solipsis Worlds server
    ATFixed = 'f'       // User specified its own nodeId into boot.lua (no authentication)
};

enum EventType {
    ETNewEntity = 0,            /// New entity
    ETLostEntity = 1,           /// Lost entity
    ETUpdatedEntity = 2,        /// Updated entity
    ETActionOnEntity = 3,       /// Action on entity
    ETConnectionRestored = 98,  /// Connection restored
    ETConnectionLost = 99,      /// Connection lost
    ETStatusReport = 100        /// Status report
};

enum EntityType {
    ETAvatar = 0,       /// Avatar
    ETSite = 1,         /// Site
    ETObject = 2        /// Object (movable)
};

enum ActionType {
    ATNone = 0,         /// No action
    ATChat = 1,         /// Chat message sent by the source entity
    ATURLUpdate = 2,    /// URL updated on 1 Navi-mapped entity object
};

typedef unsigned int EntityFlags;
static const EntityFlags EFNone = (EntityFlags)0;
static const EntityFlags EFGravity = (EntityFlags)1;  /// Gravity applied

enum ShapeType {
    STPoint = 0,            /// Point
    STCircle = 1,           /// 2D circle
    ST2DRect = 2,           /// 2D rectangle
    STSphere = 100,         /// Sphere
    STBox = 101,            /// 3D box
    STPolygonVolume = 200   /// Volume
};

typedef unsigned int EntityVersion;
typedef unsigned int Lod;
typedef unsigned int FileVersion;

typedef struct {
    std::string mFilename;
    FileVersion mVersion;
} LodContentFileStruct;

typedef unsigned char AnimationState;
static const AnimationState ASNone = (AnimationState)0;
static const AnimationState ASAvatarNone = (AnimationState)0;      /// Avatar no animation
static const AnimationState ASAvatarIdle = (AnimationState)1;      /// Avatar on idle
static const AnimationState ASAvatarWalk = (AnimationState)2;      /// Avatar is walking
static const AnimationState ASAvatarRun = (AnimationState)3;       /// Avatar is running
static const AnimationState ASAvatarFly = (AnimationState)4;       /// Avatar is flying
static const AnimationState ASAvatarSwim = (AnimationState)5;      /// Avatar is swimming
static const AnimationState ASAvatarAnimCount = (ASAvatarSwim - ASAvatarNone + 1); /// Number of animations for avatar

typedef struct {
    bool mGravity;
    Ogre::Vector3 mPosition;
} EntryGateStruct;



} // namespace Solipsis

#endif // #ifndef __XmlDatasBasicTypes_h__
