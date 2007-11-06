#include "NavigatorLua.h"
#include "Navigator.h"
#include "OgreHelpers.h"

const char NavigatorLua::className[] = "NavigatorLua";

Lunar<NavigatorLua>::RegType NavigatorLua::methods[] = {
    LunarMethod(NavigatorLua, bind),
    LunarMethod(NavigatorLua, getRenderWinMetrics),
    LunarMethod(NavigatorLua, sendMessage),
    LunarMethod(NavigatorLua, contextItemSelected),
    {0, 0}
};

NavigatorLua::NavigatorLua(lua_State* luaState) :
    mLuaState(luaState),
    mNavigator(0)
{
}

//-------------------------------------------------------------------------------------
NavigatorLua::~NavigatorLua()
{
}

//-------------------------------------------------------------------------------------
int NavigatorLua::bind(lua_State* luaState)
{
    OGRE_LOG("NavigatorLua::bind()");

    mNavigator = Navigator::getSingletonPtr();
    mNavigator->setNavigatorLua(this);

    lua_pushboolean(luaState, true);
    return 1;
}

//-------------------------------------------------------------------------------------
int NavigatorLua::getRenderWinMetrics(lua_State* luaState)
{
    OGRE_LOG("NavigatorLua::getScreenExtents()");

    // Get metrics
    unsigned int width, height, colourDepth;
    int left, top;
    mNavigator->getRenderWindowPtr()->getMetrics(width, height, colourDepth, left, top);
    // push results
    lua_pushinteger(luaState, width);
    lua_pushinteger(luaState, height);
    lua_pushinteger(luaState, colourDepth);
    lua_pushinteger(luaState, left);
    lua_pushinteger(luaState, top);
    return 5;
}

//-------------------------------------------------------------------------------------
int NavigatorLua::sendMessage(lua_State* luaState)
{
    OGRE_LOG("NavigatorLua::sendMessage()");

    // Get message to send
    std::string msg = luaL_checkstring(luaState, 1);
    // Send message
    int rc = mNavigator->sendMessage((Ogre::String)msg);

    lua_pushboolean(luaState, rc);
    return 1;
}

//-------------------------------------------------------------------------------------
int NavigatorLua::contextItemSelected(lua_State* luaState)
{
    OGRE_LOG("NavigatorLua::contextItemSelected()");

    // Get item selected
    std::string item = luaL_checkstring(luaState, 1);
    // Perform action
    int rc = mNavigator->contextItemSelected((Ogre::String)item);

    lua_pushboolean(luaState, rc);
    return 1;
}

//-------------------------------------------------------------------------------------
int NavigatorLua::call(const char* method, const char *fmt, ...)
{
    lua_settop(mLuaState, 0);
    Lunar<NavigatorLua>::push(mLuaState, this);

    va_list argp;
    va_start(argp, fmt);
    int nparams = LuaPushArgs(mLuaState, fmt, argp);
    va_end(argp);
    if (nparams < 0)
    {
        OGRE_LOG("NavigatorLua:" + String(method) + "() Unable to parse parameters format: " + String(fmt));
        return false;
    }
    if (Lunar<NavigatorLua>::call(mLuaState, method, nparams, 1) == -1)
    {
        OGRE_LOG("NavigatorLua:" + String(method) + "() error, " + String(lua_tolstring(mLuaState, -1, NULL)));
        return false;
    }
    if (!lua_isboolean(mLuaState, -1))
    {
//        luaL_error(mLuaState, "NavigatorLua:%s() boolean result expected", method);
        OGRE_LOG("NavigatorLua:" + String(method) + "() boolean result expected");
        return false;
    }
    return lua_toboolean(mLuaState, -1);
}

//-------------------------------------------------------------------------------------
int NavigatorLua::handleEvent(const char* evt, const char *fmt, ...)
{
    lua_settop(mLuaState, 0);
    Lunar<NavigatorLua>::push(mLuaState, this);

    va_list argp;
    va_start(argp, fmt);
    int nparams = LuaPushArgs(mLuaState, fmt, argp);
    va_end(argp);
    if (nparams < 0)
    {
        OGRE_LOG("NavigatorLua:handleEvent() Unable to parse parameters format: " + String(fmt));
        return false;
    }
    if (Lunar<NavigatorLua>::call(mLuaState, "handleEvent", nparams, 1) == -1)
    {
        OGRE_LOG("NavigatorLua:handleEvent() error, " + String(lua_tolstring(mLuaState, -1, NULL)));
        return false;
    }
    if (!lua_isboolean(mLuaState, -1))
    {
//        luaL_error(mLuaState, "NavigatorLua:%s() boolean result expected", method);
        OGRE_LOG("NavigatorLua:handleEvent() boolean result expected");
        return false;
    }
    return lua_toboolean(mLuaState, -1);
}
