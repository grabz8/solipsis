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

#ifndef __DebugHelpers_h__
#define __DebugHelpers_h__

#include <map>
#include "Ogre.h"
#include "Scripting/LuaBinding.h"

using namespace Ogre;

namespace Solipsis {

class Navigator;

/** This static class contains several methods to help debugging.
 */
class DebugHelpers
{
private:
    // No instanciation
    DebugHelpers() {};

public:
#ifdef UIDEBUG
    // Map of debug commands with parameters
    static std::map<String,String> debugCommands;

    // Debug callbacks
    static bool frameStarted(const FrameEvent& evt, Navigator* navigator, SceneManager* sceneMgr);
#endif

    // Bind lua
    static bool initLua(lua_State* luaState);

protected:
    // Lua message logging
    LuaStaticCppGlue(luaLogMessage);
};

} // namespace Solipsis

#endif // #ifndef __DebugHelpers_h__