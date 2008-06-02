#ifndef __NavigatorLua_h__
#define __NavigatorLua_h__

#include "LuaBinding.h"

namespace Solipsis {

class Navigator;

/** This class represents the LUA instance of a Navigator instance.
 */
class NavigatorLua
{
private:
    friend class Lunar<NavigatorLua>;

    // name of class NavigatorLua
    static const char className[];

    // methods table of class NavigatorLua
    static Lunar<NavigatorLua>::RegType methods[];

protected:
    lua_State* mLuaState;
    Navigator* mNavigator;

public:
    NavigatorLua(lua_State* luaState);
    ~NavigatorLua();

    //-------------------------------------------------------------------------------------
    // Lua to C++
    // bind to the Navigator
    // returns boolean
    int bind(lua_State* luaState);

    // set connection parameters
    // params (connectionHost:string, connectionPort:int)
    // returns boolean
    int setConnectionParams(lua_State* luaState);

    // get render window metrics
    // returns (width:int, height:int, colourDepth:int, left:int, top:int)
    int getRenderWinMetrics(lua_State* luaState);

    // send 1 message on chat
    // params (msg:string)
    // returns boolean
    int sendMessage(lua_State* luaState);

    // perform action associated to item selected
    // params (item:string)
    // returns boolean
    int contextItemSelected(lua_State* luaState);

    // hide 1 navi UI panel
    // params (naviName:string)
    // returns boolean
    int hideNavi(lua_State* luaState);

    // handle 1 event by 1 material created from 1 external texture source (texture based plugin)
    // params (extTextSrcExPlugin:string)
    // params (mtlName:string)
    // params (extTextSrcExEvt:string)
    // returns extTextSrcExResult:string
    int extTextSrcExHandleEvt(lua_State* luaState);

    //-------------------------------------------------------------------------------------
    // C++ to Lua
    // call 1 method
    int call(const char* method, const char *fmt, ...);

    // handle 1 event
    int handleEvent(const char* evt, const char *fmt, ...);
};

} // namespace Solipsis

#endif // #ifndef __NavigatorLua_h__