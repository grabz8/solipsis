#ifndef __DebugHelpers_h__
#define __DebugHelpers_h__

#include <map>
#include "Ogre.h"
#include "LuaBinding.h"

using namespace Ogre;

class Navigator;

class DebugHelpers
{
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

#endif // #ifndef __DebugHelpers_h__