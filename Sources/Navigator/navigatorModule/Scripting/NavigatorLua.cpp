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
#include "Prerequisites.h"

#include "NavigatorLua.h"
#include "MainApplication/Navigator.h"
#include <CTLog.h>
#include <OgreExternalTextureSourceManager.h>
#include "OgreTools/ExternalTextureSourceEx.h"

using namespace Solipsis;
using namespace CommonTools;

const char NavigatorLua::className[] = "NavigatorLua";

Lunar<NavigatorLua>::RegType NavigatorLua::methods[] = {
    LunarMethod(NavigatorLua, bind),
    LunarMethod(NavigatorLua, getRenderWinMetrics),
    LunarMethod(NavigatorLua, mainMenuClick),
    LunarMethod(NavigatorLua, contextItemSelected),
    LunarMethod(NavigatorLua, sendMessage),
    LunarMethod(NavigatorLua, hideNavi),
    LunarMethod(NavigatorLua, extTextSrcExHandleEvt),
    {0, 0}
};

//-------------------------------------------------------------------------------------
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
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorLua::bind()");

    mNavigator = Navigator::getSingletonPtr();
    mNavigator->setNavigatorLua(this);

    lua_pushboolean(luaState, true);
    return 1;
}

//-------------------------------------------------------------------------------------
int NavigatorLua::getRenderWinMetrics(lua_State* luaState)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorLua::getScreenExtents()");

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
int NavigatorLua::mainMenuClick(lua_State* luaState)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorLua::mainMenuClick()");

    // Get item selected
    std::string item = luaL_checkstring(luaState, 1);
    // Perform action
    int rc = mNavigator->mainMenuClick(String(item));

    lua_pushboolean(luaState, rc);
    return 1;
}

//-------------------------------------------------------------------------------------
int NavigatorLua::sendMessage(lua_State* luaState)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorLua::sendMessage()");

    // Get message to send
    std::string msg = luaL_checkstring(luaState, 1);
    // Send message
    int rc = mNavigator->sendMessage(String(msg));

    lua_pushboolean(luaState, rc);
    return 1;
}

//-------------------------------------------------------------------------------------
int NavigatorLua::contextItemSelected(lua_State* luaState)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorLua::contextItemSelected()");

    // Get item selected
    std::string item = luaL_checkstring(luaState, 1);
    // Perform action
    int rc = mNavigator->contextItemSelected(String(item));

    lua_pushboolean(luaState, rc);
    return 1;
}

//-------------------------------------------------------------------------------------
int NavigatorLua::hideNavi(lua_State* luaState)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorLua::hideNavi()");

    // Get navi name
    std::string naviName = luaL_checkstring(luaState, 1);
    // Perform action
    int rc = mNavigator->getNavigatorGUI()->setNaviVisibility(naviName, false);

    lua_pushboolean(luaState, rc);
    return 1;
}

//-------------------------------------------------------------------------------------
int NavigatorLua::extTextSrcExHandleEvt(lua_State* luaState)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorLua::extTextSrcExHandleEvt()");

    // Get parameters
    std::string extTextSrcExPlugin = luaL_checkstring(luaState, 1);
    std::string mtlName = luaL_checkstring(luaState, 2);
    std::string extTextSrcExEvt = luaL_checkstring(luaState, 3);
    // Perform action
    ExternalTextureSourceManager::getSingleton().setCurrentPlugIn(String(extTextSrcExPlugin));
    ExternalTextureSourceEx* vlcExtTextSrcEx = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource(String(extTextSrcExPlugin)));
    String result = vlcExtTextSrcEx->handleEvt(String(mtlName), String(extTextSrcExEvt));

    lua_pushstring(luaState, result.c_str());
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
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorLua:%s() Unable to parse parameters format: %s", method, fmt);
        return false;
    }
    if (Lunar<NavigatorLua>::call(mLuaState, method, nparams, 1) == -1)
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorLua:%s() error, %s", method, lua_tolstring(mLuaState, -1, NULL));
        return false;
    }

    if (!lua_isboolean(mLuaState, -1))
    {
//        luaL_error(mLuaState, "NavigatorLua:%s() boolean result expected", method);
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorLua:%s() boolean result expected", method);
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
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorLua:handleEvent() Unable to parse parameters format: %s", fmt);
        return false;
    }
    if (Lunar<NavigatorLua>::call(mLuaState, "handleEvent", nparams, 1) == -1)
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorLua:handleEvent() error, ", lua_tolstring(mLuaState, -1, NULL));
        return false;
    }
    if (!lua_isboolean(mLuaState, -1))
    {
//        luaL_error(mLuaState, "NavigatorLua:%s() boolean result expected", method);
        LOGHANDLER_LOG(LogHandler::VL_ERROR, "NavigatorLua:handleEvent() boolean result expected");
        return false;
    }
    return lua_toboolean(mLuaState, -1);
}

//-------------------------------------------------------------------------------------
