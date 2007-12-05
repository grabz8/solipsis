#include "NaviLua.h"
#include "NaviManager.h"
#include "OgreHelpers.h"

using namespace NaviLibrary;

LuaGlue (_createNavi)
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
	NaviLibrary::NaviManager::Get().createNavi(pName, pUrl, *naviPos, w, h, bMoveable, bIsVisible, maxUpdates, bForceMaxUpdate, zOrder, opacity);
	delete naviPos;
	return 0;
}


LuaGlue (_setNaviMask)
{
	const char *pName = luaL_checkstring(L, 1);
	const char *pMaskName = luaL_checkstring(L, 2);
	NaviLibrary::NaviManager::Get().setNaviMask(pName, pMaskName);
	return 0;
}

/////////////////////////////////////////////////////////////
class cLuaNaviFunctor : public NaviLibrary::NaviEventListener
{
public:
	cLuaNaviFunctor(const char *pLuaFunc, lua_State *pLua) {m_sLuaFunc = pLuaFunc; m_pLuaState = pLua;}
	void onNaviDataEvent(const std::string &naviName, const NaviLibrary::NaviData &naviData);
	void onNaviLinkClicked(const std::string &naviName, const std::string &linkHref);
private:
	std::string m_sLuaFunc;
	lua_State *m_pLuaState;
};

void cLuaNaviFunctor::onNaviDataEvent(const std::string &naviName, const NaviLibrary::NaviData &naviData)
{
	std::string LuaCommand = m_sLuaFunc + "(\"Data\", \"" + naviName + "\", \"" + naviData.getName() + "\", {";
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

void cLuaNaviFunctor::onNaviLinkClicked(const std::string &naviName, const std::string &linkHref)
{
#if 0
	std::string LuaCommand = m_sLuaFunc + "(\"LinkClicked\", \"" + naviName + "\", \"" + linkHref + "\")";
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

static std::map<std::string, cLuaNaviFunctor *> s_mapFunctors;

LuaGlue (_addNaviEventListener)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	std::string luaFuncName = luaL_checkstring(L, argNum++);
	
	s_mapFunctors[luaFuncName] = new cLuaNaviFunctor(luaFuncName.c_str(), L);
	NaviLibrary::NaviManager::Get().addNaviEventListener(pName, s_mapFunctors[luaFuncName]);

	return 0;
}


LuaGlue (_destroyNavi)
{
	const char *pName = luaL_checkstring(L, 1);
	// need to create a way to remove any event listeners associated with this navi
	NaviLibrary::NaviManager::Get().destroyNavi(pName);
	return 0;
}

LuaGlue(_removeNaviEventListener)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	std::string luaFuncName = luaL_checkstring(L, argNum++);

	cLuaNaviFunctor *pFunctor = s_mapFunctors[luaFuncName];
	if(pFunctor)
	{
		NaviLibrary::NaviManager::Get().removeNaviEventListener(pName, pFunctor);
		s_mapFunctors[luaFuncName] = NULL;
		delete pFunctor;
	}

	return 0;
}
/////////////////////////////////////////////////////////////


LuaGlue (_setNaviColorKey)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	const char *pColorStr = luaL_checkstring(L, argNum++);
	float keyFillOpacity = (float) luaL_optnumber(L, argNum++, 0);
	const char *keyFillColor = luaL_optstring(L, argNum++, "#000000");
	float keyFuzziness = (float) luaL_optnumber(L, argNum++, 0);


	NaviLibrary::NaviManager::Get().setNaviColorKey(pName, pColorStr, keyFillOpacity, keyFillColor, keyFuzziness);
	return 0;
}

LuaGlue(_navigateNaviTo)
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
			naviData[(*it).first] = NaviLibrary::NaviDataValue((*it).second);
			++it;
		}
		NaviLibrary::NaviManager::Get().navigateNaviTo(pName, pUrl, naviData);
	}
	else
	{
		NaviLibrary::NaviManager::Get().navigateNaviTo(pName, pUrl);
	}

	return 0;
}

LuaGlue(_setNaviOpacity)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	float opacity = (float) luaL_optnumber(L, argNum++, 1.0);
	NaviLibrary::NaviManager::Get().setNaviOpacity(pName, opacity);

	return 0;
}

LuaGlue(_setNaviIgnoreTransparent)
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
	NaviLibrary::NaviManager::Get().setNaviIgnoreTransparent(pName, ignoreTrans, defineThreshold);
	return 0;
}

LuaGlue(_setForceMaxUpdate)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	bool bOn = lua_toboolean(L, argNum++) != 0;

	NaviLibrary::NaviManager::Get().setForceMaxUpdate(pName, bOn);
	return 0;
}

LuaGlue(_setMaxUpdatesPerSec)
{
	int argNum = 1;
	const char *pName = luaL_checkstring(L, argNum++);
	unsigned int iRate = (unsigned int) luaL_checkint(L, argNum++);

	NaviLibrary::NaviManager::Get().setMaxUpdatesPerSec(pName, iRate);
	return 0;
}
/////////////////////////////////////////////////////////////

LuaGlue(_naviEvaluateJS)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	std::string script = luaL_checkstring(L, argNum++);

	std::string r = NaviLibrary::NaviManager::Get().naviEvaluateJS(naviName, script);
	lua_pushstring(L, r.c_str());
	return 1;
}

LuaGlue(_createNaviMaterial)
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

	std::string retString = NaviLibrary::NaviManager::Get().createNaviMaterial(naviName, homepage, w, h, 
			isVisible, maxUpdatesPerSec, forceMaxUpdate, opacity, texFiltering);

	lua_pushstring(L, retString.c_str());
	return 1;
}

LuaGlue(_canNavigateBack)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	lua_pushboolean(L, NaviLibrary::NaviManager::Get().canNavigateBack(naviName));
	return 1;
}

LuaGlue(_navigateNaviBack)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	NaviLibrary::NaviManager::Get().navigateNaviBack(naviName);
	return 0;
}

LuaGlue(_canNavigateForward)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	lua_pushboolean(L, NaviLibrary::NaviManager::Get().canNavigateForward(naviName));
	return 1;
}

LuaGlue(_navigateNaviForward)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	NaviLibrary::NaviManager::Get().navigateNaviForward(naviName);
	return 0;
}

LuaGlue(_navigateNaviStop)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	NaviLibrary::NaviManager::Get().navigateNaviStop(naviName);
	return 0;
}

LuaGlue(_setNaviBackgroundColor)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);
	float r = (float) luaL_optnumber(L, argNum++, 1.0);
	float g = (float) luaL_optnumber(L, argNum++, 1.0);
	float b = (float) luaL_optnumber(L, argNum++, 1.0);

	NaviLibrary::NaviManager::Get().setNaviBackgroundColor(naviName, r, g, b);
	return 0;
}


LuaGlue(_isAnyNaviFocused)
{
	lua_pushboolean(L, NaviLibrary::NaviManager::Get().isAnyNaviFocused());
	return 1;
}

LuaGlue(_getFocusedNaviName)
{
	lua_pushstring(L, NaviLibrary::NaviManager::Get().getFocusedNaviName().c_str());
	return 1;
}

LuaGlue(_getNaviMaterialName)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);

	lua_pushstring(L, NaviLibrary::NaviManager::Get().getNaviMaterialName(naviName).c_str());
	return 1;
}

LuaGlue(_getNaviVisibility)
{
	int argNum = 1;
	std::string naviName = luaL_checkstring(L, argNum++);

	lua_pushboolean(L, NaviLibrary::NaviManager::Get().getNaviVisibility(naviName));
	return 1;
}

LuaGlue(_deFocusAllNavis)
{
	NaviLibrary::NaviManager::Get().deFocusAllNavis();
	return 0;
}

LuaGlue(_showNavi)
{
	int argNum = 1;
	const char *naviName = luaL_checkstring(L, argNum++);

	if(naviName)
		NaviLibrary::NaviManager::Get().showNavi(naviName);
	return 0;
}

LuaGlue(_hideNavi)
{
	int argNum = 1;
	const char *naviName = luaL_checkstring(L, argNum++);

	if(naviName)
		NaviLibrary::NaviManager::Get().hideNavi(naviName);
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
	{"createNavi",					_createNavi},
	{"destroyNavi",					_destroyNavi},
	{"setNaviMask",					_setNaviMask},
	{"addNaviEventListener",		_addNaviEventListener},
	{"setNaviColorKey",				_setNaviColorKey},
	{"navigateNaviTo",				_navigateNaviTo},
	{"setNaviOpacity",				_setNaviOpacity},
	{"setForceMaxUpdate",			_setForceMaxUpdate},
	{"setMaxUpdatesPerSec",			_setMaxUpdatesPerSec},
	{"naviEvaluateJS",				_naviEvaluateJS},
	{"createNaviMaterial",			_createNaviMaterial},
	{"canNavigateBack",				_canNavigateBack},
	{"navigateNaviBack",			_navigateNaviBack},
	{"canNavigateForward",			_canNavigateForward},
	{"navigateNaviForward",			_navigateNaviForward},
	{"navigateNaviStop",			_navigateNaviStop},
	{"setNaviBackgroundColor",		_setNaviBackgroundColor},
	{"setNaviIgnoreTransparent",	_setNaviIgnoreTransparent},
	{"isAnyNaviFocused",			_isAnyNaviFocused},
	{"getFocusedNaviName",			_getFocusedNaviName},
	{"getNaviMaterialName",			_getNaviMaterialName},
	{"getNaviVisibility",			_getNaviVisibility},
	{"deFocusAllNavis",				_deFocusAllNavis},
	{"showNavi",					_showNavi},
	{"hideNavi",					_hideNavi},

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
