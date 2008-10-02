/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author 

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

#include "NaviLua.h"
#include "NaviManager.h"
#include "Navi.h"
#include "OgreHelpers.h"

using namespace NaviLibrary;

LuaGlue (_naviMgrCreateNavi)
{
	int argNum = 1;
	const std::string pName = luaL_checkstring(L, argNum++);
	const std::string pUrl  = luaL_checkstring(L, argNum++);
	short x=0,y=0,w,h;
	const char *pPositionCmd = NULL;

	RelativePosition p = Left;
	if(lua_type(L, argNum) == LUA_TSTRING) // position based on string command
	{
		pPositionCmd = luaL_checkstring(L, argNum++);
		// parse
		if(strcmp(pPositionCmd, "Left") == 0)
			p = Left;
		if(strcmp(pPositionCmd, "TopLeft") == 0)
			p = TopLeft;
		if(strcmp(pPositionCmd, "TopCenter") == 0)
			p = TopCenter;
		if(strcmp(pPositionCmd, "TopRight") == 0)
			p = TopRight;
		if(strcmp(pPositionCmd, "Right") == 0)
			p = Right;
		if(strcmp(pPositionCmd, "BottomRight") == 0)
			p = BottomRight;
		if(strcmp(pPositionCmd, "BottomCenter") == 0)
			p = BottomCenter;
		if(strcmp(pPositionCmd, "BottomLeft") == 0)
			p = BottomLeft;
		if(strcmp(pPositionCmd, "Center") == 0)
			p = Center;

	}
	// x and y are now required even after a RelativePosition, they will be used as offsets if rel
	x =  (short) luaL_checkint(L, argNum++);
	y =  (short) luaL_checkint(L, argNum++);

	w =  (short) luaL_checkint(L, argNum++);
	h =  (short) luaL_checkint(L, argNum++);

	bool bMoveable = true;
	if(lua_type(L, argNum) == LUA_TBOOLEAN)
		bMoveable = lua_toboolean(L, argNum) != 0;
	argNum++;

	bool bIsVisible = true;
	if(lua_type(L, argNum) == LUA_TBOOLEAN)
		bIsVisible = lua_toboolean(L, argNum) != 0;
	argNum++;

	unsigned int maxUpdates = (unsigned int)luaL_optnumber(L, argNum++, 0);

	bool bForceMaxUpdate = false;
	if(lua_type(L, argNum) == LUA_TBOOLEAN)
		bForceMaxUpdate = lua_toboolean(L, argNum) != 0;
	argNum++;

	unsigned short zOrder = (unsigned int)luaL_optnumber(L, argNum++, 0);
	float opacity = (float) luaL_optnumber(L, argNum++, 1.0);

	NaviLibrary::NaviPosition *naviPos;

	if(pPositionCmd)
	{
		naviPos = new NaviLibrary::NaviPosition(p, x, y);
	}
	else
	{
		naviPos = new NaviLibrary::NaviPosition(x, y);
	}
	NaviLibrary::Navi* navi = NaviLibrary::NaviManager::Get().createNavi(pName, pUrl, *naviPos, w, h, zOrder);
	navi->setMovable(bMoveable);
	if (bIsVisible)
		navi->show();
	else
		navi->hide();
	navi->setMaxUPS(maxUpdates);
	navi->setForceMaxUpdate(bForceMaxUpdate);
	navi->setOpacity(opacity);
	delete naviPos;
	return 0;
}


LuaGlue (_naviSetMask)
{
	const char *pName = luaL_checkstring(L, 1);
	const char *pMaskName = luaL_checkstring(L, 2);
	NaviLibrary::NaviManager::Get().getNavi(pName)->setMask(pMaskName);
	return 0;
}

/////////////////////////////////////////////////////////////
class cLuaNaviFunctor : public NaviLibrary::NaviEventListener
{
public:
	cLuaNaviFunctor(const char *pLuaFunc, lua_State *pLua) {m_sLuaFunc = pLuaFunc; m_pLuaState = pLua;}
	void onNaviDataEvent(NaviLibrary::Navi *caller, const NaviLibrary::NaviData &naviData);
	void onLinkClicked(NaviLibrary::Navi *caller, const std::string &linkHref);
	void onLocationChange(NaviLibrary::Navi *caller, const std::string &url);
	void onNavigateComplete(NaviLibrary::Navi *caller, const std::string &url, int responseCode);
private:
	std::string m_sLuaFunc;
	lua_State *m_pLuaState;
};

void cLuaNaviFunctor::onNaviDataEvent(NaviLibrary::Navi *caller, const NaviLibrary::NaviData &naviData)
{
	std::string LuaCommand = m_sLuaFunc + "(\"Data\", \"" + caller->getName() + "\", \"" + naviData.getName() + "\", {";
	NaviLibrary::NaviData myData = naviData;

	std::map<std::string,std::string> naviDataMap;
	//myData.getDataMap(naviDataMap);
	naviDataMap = myData.toStringMap(false);



	std::map<std::string,std::string>::iterator it = naviDataMap.begin();
	while(it != naviDataMap.end())
	{
		LuaCommand += (*it).first;
		LuaCommand += "=\"";
		LuaCommand += (*it).second;
		LuaCommand += "\", ";
		++it;
	}

	char buf[16];
	sprintf(buf, "n=%d})", naviDataMap.size());
	LuaCommand += buf;

	if (0 != luaL_loadbuffer(m_pLuaState, LuaCommand.c_str(), LuaCommand.size(), NULL))
	{
		char ebuf[1025];
		sprintf(ebuf, "cLuaNaviFunctor::onNaviDataEvent() Lua load error!\nCommand:%s\nError Message:%s\n", LuaCommand.c_str(), luaL_checkstring(m_pLuaState, -1));
        OGRE_LOG(ebuf);
	}
	if (0 != lua_pcall(m_pLuaState, 0, LUA_MULTRET, 0))
	{
		char ebuf[1025];
		sprintf(ebuf, "cLuaNaviFunctor::onNaviDataEvent() Lua pcall error!\nCommand:%s\nError Message:%s\n", LuaCommand.c_str(), luaL_checkstring(m_pLuaState, -1));
        OGRE_LOG(ebuf);
	}
}

void cLuaNaviFunctor::onLinkClicked(NaviLibrary::Navi *caller, const std::string &linkHref)
{
#if 0
	std::string LuaCommand = m_sLuaFunc + "(\"LinkClicked\", \"" + caller->getName() + "\", \"" + linkHref + "\")";
	if (0 != luaL_loadbuffer(m_pLuaState, LuaCommand.c_str(), LuaCommand.size(), NULL))
	{
		char ebuf[256];
		sprintf(ebuf, "Lua Load Error!\nCommand:%s\nError Message:%s\n", LuaCommand.c_str(), luaL_checkstring(m_pLuaState, -1));
        OGRE_LOG(ebuf);
	}
	if (0 != lua_pcall(m_pLuaState, 0, LUA_MULTRET, 0))
	{
		char ebuf[256];
		sprintf(ebuf, "Lua Run Error!\nCommand:%s\nError Message:%s\n", LuaCommand.c_str(), luaL_checkstring(m_pLuaState, -1));
        OGRE_LOG(ebuf);
	}
#endif
}

void cLuaNaviFunctor::onLocationChange(NaviLibrary::Navi *caller, const std::string &url)
{
}

void cLuaNaviFunctor::onNavigateComplete(NaviLibrary::Navi *caller, const std::string &url, int responseCode)
{
}

static std::map<std::string, cLuaNaviFunctor *> s_mapFunctors;

LuaGlue (_naviAddEventListener)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	std::string luaFuncName = luaL_checkstring(L, argNum++);
	
	s_mapFunctors[luaFuncName] = new cLuaNaviFunctor(luaFuncName.c_str(), L);
	NaviLibrary::NaviManager::Get().getNavi(pName)->addEventListener(s_mapFunctors[luaFuncName]);

	return 0;
}


LuaGlue (_naviMgrDestroyNavi)
{
	const char *pName = luaL_checkstring(L, 1);
	// need to create a way to remove any event listeners associated with this navi
	NaviLibrary::NaviManager::Get().destroyNavi(pName);
	return 0;
}

LuaGlue (_naviMgrIsNaviExists)
{
	const char *pName = luaL_checkstring(L, 1);
	lua_pushboolean(L, NaviLibrary::NaviManager::Get().getNavi(pName) != 0);
	return 1;
}

LuaGlue(_removeNaviEventListener)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	std::string luaFuncName = luaL_checkstring(L, argNum++);

	cLuaNaviFunctor *pFunctor = s_mapFunctors[luaFuncName];
	if(pFunctor)
	{
		NaviLibrary::NaviManager::Get().getNavi(pName)->removeEventListener(pFunctor);
		s_mapFunctors[luaFuncName] = NULL;
		delete pFunctor;
	}

	return 0;
}
/////////////////////////////////////////////////////////////


LuaGlue (_naviSetColorKey)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	const char *pColorStr = luaL_checkstring(L, argNum++);
	float keyFillOpacity = (float) luaL_optnumber(L, argNum++, 0);
	const char *keyFillColor = luaL_optstring(L, argNum++, "#000000");
	float keyFuzziness = (float) luaL_optnumber(L, argNum++, 0);


	NaviLibrary::NaviManager::Get().getNavi(pName)->setColorKey(pColorStr, keyFillOpacity, keyFillColor, keyFuzziness);
	return 0;
}

LuaGlue(_naviNavigateTo)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	const char *pUrl = luaL_checkstring(L, argNum++);

	std::map<std::string, std::string> mapTable;
	if(lua_istable(L, argNum))
    {
		lua_pushnil(L);  /* first key */
		while (lua_next(L, argNum) != 0) 
		{
			/* uses 'key' (at index -2) and 'value' (at index -1) */ 
			mapTable[luaL_checkstring(L, -2)] = luaL_checkstring(L, -1);
			/* removes 'value'; keeps 'key' for next iteration */
			lua_pop(L, 1);
		}
	}

	if(mapTable.size())
	{
		std::string naviName = "unnamedNaviData";
		const char *pDataName = mapTable["naviDataName"].c_str();
		if(pDataName)
			naviName = pDataName;

		NaviLibrary::NaviData naviData(naviName);
		std::map<std::string, std::string>::iterator it = mapTable.begin();
		while(it != mapTable.end())
		{
			naviData[(*it).first] = NaviLibrary::NaviUtilities::MultiValue((*it).second);
			++it;
		}
		NaviLibrary::NaviManager::Get().getNavi(pName)->navigateTo(pUrl, naviData);
	}
	else
	{
		NaviLibrary::NaviManager::Get().getNavi(pName)->navigateTo(pUrl);
	}

	return 0;
}

LuaGlue(_naviSetOpacity)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	float opacity = (float) luaL_optnumber(L, argNum++, 1.0);
	NaviLibrary::NaviManager::Get().getNavi(pName)->setOpacity(opacity);

	return 0;
}

LuaGlue(_naviSetPosition)
{
	int argNum = 1;
	const std::string pName = luaL_checkstring(L, argNum++);
	short x=0,y=0;
	const char *pPositionCmd = NULL;

	RelativePosition p = Left;
	if(lua_type(L, argNum) == LUA_TSTRING) // position based on string command
	{
		pPositionCmd = luaL_checkstring(L, argNum++);
		// parse
		if(strcmp(pPositionCmd, "Left") == 0)
			p = Left;
		if(strcmp(pPositionCmd, "TopLeft") == 0)
			p = TopLeft;
		if(strcmp(pPositionCmd, "TopCenter") == 0)
			p = TopCenter;
		if(strcmp(pPositionCmd, "TopRight") == 0)
			p = TopRight;
		if(strcmp(pPositionCmd, "Right") == 0)
			p = Right;
		if(strcmp(pPositionCmd, "BottomRight") == 0)
			p = BottomRight;
		if(strcmp(pPositionCmd, "BottomCenter") == 0)
			p = BottomCenter;
		if(strcmp(pPositionCmd, "BottomLeft") == 0)
			p = BottomLeft;
		if(strcmp(pPositionCmd, "Center") == 0)
			p = Center;

	}
	// x and y are now required even after a RelativePosition, they will be used as offsets if rel
	x =  (short) luaL_checkint(L, argNum++);
	y =  (short) luaL_checkint(L, argNum++);

    NaviLibrary::NaviPosition *naviPos;

	if(pPositionCmd)
	{
		naviPos = new NaviLibrary::NaviPosition(p, x, y);
	}
	else
	{
		naviPos = new NaviLibrary::NaviPosition(x, y);
	}
    NaviLibrary::Navi* navi = NaviLibrary::NaviManager::Get().getNavi(pName)->setPosition(*naviPos);

	return 0;
}

LuaGlue(_naviSetIgnoreBounds)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	bool bIgnoringBounds = lua_toboolean(L, argNum++) != 0;

	NaviLibrary::NaviManager::Get().getNavi(pName)->setIgnoreBounds(bIgnoringBounds);
	return 0;
}

LuaGlue(_naviSetIgnoreTransparent)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	
	bool ignoreTrans = true;
	if(lua_type(L, argNum) == LUA_TBOOLEAN) 
	{
		ignoreTrans = lua_toboolean(L, argNum) != 0;
	}
	argNum++;

	float defineThreshold = (float) luaL_optnumber(L, argNum++, 0.05);
	NaviLibrary::NaviManager::Get().getNavi(pName)->setIgnoreTransparent(ignoreTrans, defineThreshold);
	return 0;
}

LuaGlue(_naviSetForceMaxUpdate)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	bool bOn = lua_toboolean(L, argNum++) != 0;

	NaviLibrary::NaviManager::Get().getNavi(pName)->setForceMaxUpdate(bOn);
	return 0;
}

LuaGlue(_naviSetMaxUpdatesPerSec)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	unsigned int iRate = (unsigned int) luaL_checkint(L, argNum++);

	NaviLibrary::NaviManager::Get().getNavi(pName)->setMaxUPS(iRate);
	return 0;
}

LuaGlue(_naviSetAutoUpdateOnFocus)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	
	bool isAutoUpdatedOnFocus = false;
	if(lua_type(L, argNum) == LUA_TBOOLEAN) 
	{
		isAutoUpdatedOnFocus = lua_toboolean(L, argNum) != 0;
	}
	argNum++;

    NaviLibrary::NaviManager::Get().getNavi(pName)->setAutoUpdateOnFocus(isAutoUpdatedOnFocus);
	return 0;
}

LuaGlue(_naviGetCurrentLocation)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);

	std::string r = NaviLibrary::NaviManager::Get().getNavi(naviName)->getCurrentLocation();
	lua_pushstring(L, r.c_str());
	return 1;
}
/////////////////////////////////////////////////////////////

LuaGlue(_naviEvaluateJS)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	std::string script = luaL_checkstring(L, argNum++);

	std::string r = NaviLibrary::NaviManager::Get().getNavi(naviName)->evaluateJS(script);
	lua_pushstring(L, r.c_str());
	return 1;
}

LuaGlue(_naviMgrCreateNaviMaterial)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	std::string homepage = luaL_checkstring(L, argNum++);
	unsigned short w =  (unsigned short) luaL_checkint(L, argNum++);
	unsigned short h =  (unsigned short) luaL_checkint(L, argNum++);

	bool isVisible = true; 
	if(lua_type(L, argNum) == LUA_TBOOLEAN) 
	{
		isVisible = lua_toboolean(L, argNum) != 0;
	}
	++argNum;

	unsigned int maxUpdatesPerSec = (unsigned int)luaL_optnumber(L, argNum++, 0);
	bool forceMaxUpdate = false;
	if(lua_type(L, argNum) == LUA_TBOOLEAN) 
	{
		forceMaxUpdate = lua_toboolean(L, argNum) != 0;
	}
	++argNum;
	float opacity = (float) luaL_optnumber(L, argNum++, 1.0);
	Ogre::FilterOptions texFiltering = Ogre::FO_ANISOTROPIC;

	NaviLibrary::Navi* navi = NaviLibrary::NaviManager::Get().createNaviMaterial(naviName, homepage, w, h, texFiltering);
	if (isVisible)
		navi->show();
	else
		navi->hide();
	navi->setMaxUPS(maxUpdatesPerSec);
	navi->setForceMaxUpdate(forceMaxUpdate);
	navi->setOpacity(opacity);
	std::string retString = navi->getName();

	lua_pushstring(L, retString.c_str());
	return 1;
}

LuaGlue(_naviCanNavigateBack)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	lua_pushboolean(L, NaviLibrary::NaviManager::Get().getNavi(naviName)->canNavigateBack());
	return 1;
}

LuaGlue(_naviNavigateBack)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	NaviLibrary::NaviManager::Get().getNavi(naviName)->navigateBack();
	return 0;
}

LuaGlue(_naviCanNavigateForward)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	lua_pushboolean(L, NaviLibrary::NaviManager::Get().getNavi(naviName)->canNavigateForward());
	return 1;
}

LuaGlue(_naviNavigateForward)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	NaviLibrary::NaviManager::Get().getNavi(naviName)->navigateForward();
	return 0;
}

LuaGlue(_naviNavigateStop)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	NaviLibrary::NaviManager::Get().getNavi(naviName)->navigateStop();
	return 0;
}

LuaGlue(_naviSetBackgroundColor)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	float r = (float) luaL_optnumber(L, argNum++, 1.0);
	float g = (float) luaL_optnumber(L, argNum++, 1.0);
	float b = (float) luaL_optnumber(L, argNum++, 1.0);

	NaviLibrary::NaviManager::Get().getNavi(naviName)->setBackgroundColor(r, g, b);
	return 0;
}


LuaGlue(_naviMgrIsAnyNaviFocused)
{
	lua_pushboolean(L, NaviLibrary::NaviManager::Get().isAnyNaviFocused());
	return 1;
}

LuaGlue(_naviMgrGetFocusedNaviName)
{
	lua_pushstring(L, NaviLibrary::NaviManager::Get().getFocusedNavi()->getName().c_str());
	return 1;
}

LuaGlue(_naviGetMaterialName)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);

	lua_pushstring(L, NaviLibrary::NaviManager::Get().getNavi(naviName)->getMaterialName().c_str());
	return 1;
}

LuaGlue(_naviGetVisibility)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);

    lua_pushboolean(L, NaviLibrary::NaviManager::Get().getNavi(naviName)->getVisibility());
	return 1;
}

LuaGlue(_naviMgrDeFocusAllNavis)
{
	NaviLibrary::NaviManager::Get().deFocusAllNavis();
	return 0;
}

LuaGlue(_naviShow)
{
	int argNum = 1;
	const char *naviName = luaL_checkstring(L, argNum++);

	if(naviName)
		NaviLibrary::NaviManager::Get().getNavi(naviName)->show();
	return 0;
}

LuaGlue(_naviHide)
{
	int argNum = 1;
	const char *naviName = luaL_checkstring(L, argNum++);

	if(naviName)
		NaviLibrary::NaviManager::Get().getNavi(naviName)->hide();
	return 0;
}

extern "C" {
typedef struct
{
	const char *name;
	int (*func)(lua_State *);
}luaDef;
};

luaDef NaviGlue[] =
{
	{"naviMgrCreateNavi",			_naviMgrCreateNavi},
	{"naviMgrDestroyNavi",			_naviMgrDestroyNavi},
	{"naviMgrIsNaviExists",			_naviMgrIsNaviExists},
	{"naviSetMask",					_naviSetMask},
	{"naviAddEventListener",		_naviAddEventListener},
	{"naviSetColorKey",				_naviSetColorKey},
	{"naviNavigateTo",				_naviNavigateTo},
	{"naviSetOpacity",				_naviSetOpacity},
    {"naviSetPosition",				_naviSetPosition},
	{"naviSetForceMaxUpdate",		_naviSetForceMaxUpdate},
	{"naviSetMaxUpdatesPerSec",		_naviSetMaxUpdatesPerSec},
    {"naviSetAutoUpdateOnFocus",	_naviSetAutoUpdateOnFocus},
    {"naviGetCurrentLocation",		_naviGetCurrentLocation},
	{"naviEvaluateJS",				_naviEvaluateJS},
	{"naviMgrCreateNaviMaterial",	_naviMgrCreateNaviMaterial},
	{"naviCanNavigateBack",			_naviCanNavigateBack},
	{"naviNavigateBack",			_naviNavigateBack},
	{"naviCanNavigateForward",		_naviCanNavigateForward},
	{"naviNavigateForward",			_naviNavigateForward},
	{"naviNavigateStop",			_naviNavigateStop},
	{"naviSetBackgroundColor",		_naviSetBackgroundColor},
	{"naviSetIgnoreBounds",			_naviSetIgnoreBounds},
	{"naviSetIgnoreTransparent",	_naviSetIgnoreTransparent},
	{"naviMgrIsAnyNaviFocused",		_naviMgrIsAnyNaviFocused},
	{"naviMgrGetFocusedNaviName",	_naviMgrGetFocusedNaviName},
	{"naviGetMaterialName",			_naviGetMaterialName},
	{"naviGetVisibility",			_naviGetVisibility},
	{"naviMgrDeFocusAllNavis",		_naviMgrDeFocusAllNavis},
	{"naviShow",					_naviShow},
	{"naviHide",					_naviHide},

	{NULL,							NULL},
};

/*
**	List of unimplemented NaviManager methods:
**
**	getNaviInternalPanel - Doesn't make sense to do, returns an Orge internal that I don't implement in Lua, if you do, then feel free
**
**  Mouse events are handled in the c++ code in my world, so no need to expose then to Lua
**		injectMouseMove
**		injectNaviMaterialMouseMove
**		injectMouseWheel
**		injectNaviMaterialMouseWheel
**		injectMouseDown
**		injectMouseUp
**		injectNaviMaterialMouseDown
**		injectNaviMaterialMouseUp
*/

void initNaviLua(lua_State *pLua)
{
	for(int i=0; NaviGlue[i].name; i++)
	{
		lua_register(pLua, NaviGlue[i].name, NaviGlue[i].func);
	}
}
