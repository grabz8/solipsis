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
#include "DebugHelpers.h"

#include "OgreGraphicObjects/OgreHelpers.h"
#include "MainApplication/Navigator.h"
#include <CTLog.h>

using namespace Solipsis;
using namespace CommonTools;

#ifdef UIDEBUG
std::map<String,String> DebugHelpers::debugCommands;

//-------------------------------------------------------------------------------------
bool DebugHelpers::frameStarted(const FrameEvent& evt, Navigator* navigator, SceneManager* sceneMgr)
{
    std::map<String,String>::iterator dbgCmd;

#ifdef DEMO_NAVI1
    // Launch demoNavi1 ?
    dbgCmd = debugCommands.find("demoNavi1");
    if (dbgCmd != debugCommands.end())
    {
        debugCommands.erase(dbgCmd);
        navigator->demoNavi1();
    }
#endif

#ifdef DEMO_NAVI2
    // Launch demoNavi2 ?
    dbgCmd = debugCommands.find("demoNavi2");
    if (dbgCmd != debugCommands.end())
    {
        navigator->demoNavi2(dbgCmd->second);
        debugCommands.erase(dbgCmd);
    }
#endif

#ifdef DEMO_VNC
    // Launch demoVNC ?
    dbgCmd = debugCommands.find("demoVNC");
    if (dbgCmd != debugCommands.end())
    {
        navigator->demoVNC(dbgCmd->second);
        debugCommands.erase(dbgCmd);
    }
#endif

#ifdef DEMO_VLC
    // Launch demoVLC ?
    dbgCmd = debugCommands.find("demoVLC");
    if (dbgCmd != debugCommands.end())
    {
        navigator->demoVLC(dbgCmd->second);
        debugCommands.erase(dbgCmd);
    }
#endif

#ifdef DEMO_VOICE
    // Launch demoVoice ?
    dbgCmd = debugCommands.find("demoVoice");
    if (dbgCmd != debugCommands.end())
    {
        navigator->demoVoice(dbgCmd->second);
        debugCommands.erase(dbgCmd);
    }
#endif

#ifdef DEMO_PHYSICS1
    // Launch demoPhysics1 ?
    dbgCmd = debugCommands.find("demoPhysics1");
    if (dbgCmd != debugCommands.end())
    {
        debugCommands.erase(dbgCmd);
        navigator->demoPhysics1();
    }
#endif

    // Rotate sun light ?
    static bool rotateSunLight = false;
    dbgCmd = debugCommands.find("rotateSunLight");
    if (dbgCmd != debugCommands.end())
    {
        debugCommands.erase(dbgCmd);
        rotateSunLight = !rotateSunLight;
    }
    if (!sceneMgr->hasLight("SunLight"))
        rotateSunLight = false;
    if (rotateSunLight)
    {
        Light* light = sceneMgr->getLight("SunLight");
        if (light != 0) {
            Matrix4 r(Quaternion(Radian(evt.timeSinceLastFrame), Vector3::UNIT_Y));
            Vector3 dir = r.transformAffine(light->getDirection());
            light->setDirection(dir);
        }
    }

    // Display axis ?
    static bool toggleAxis = false;
    static Real unitScale = 0.1f;
    dbgCmd = debugCommands.find("toggleAxis");
    if (dbgCmd != debugCommands.end())
    {
        debugCommands.erase(dbgCmd);
        SceneNode *dbgAxisSceneNode = 0;
        if (sceneMgr->hasSceneNode("dbgAxis"))
            dbgAxisSceneNode = sceneMgr->getSceneNode("dbgAxis");
        if (dbgAxisSceneNode == 0)
        {
            dbgAxisSceneNode = sceneMgr->getRootSceneNode()->createChildSceneNode("dbgAxis");
            Entity* dbgAxisSceneEntity = sceneMgr->createEntity("dbgAxis", "axes.mesh");
            dbgAxisSceneNode->attachObject(dbgAxisSceneEntity);
            dbgAxisSceneNode->setScale(unitScale, unitScale, unitScale);
        }
        toggleAxis = !toggleAxis;
        dbgAxisSceneNode->setVisible(toggleAxis);
    }
    dbgCmd = debugCommands.find("posAxis");
    if (dbgCmd != debugCommands.end())
    {
        Vector3 pos = Vector3::ZERO;
        if (OgreHelpers::convertString2Vector3(dbgCmd->second, pos) && sceneMgr->hasSceneNode("dbgAxis"))
            sceneMgr->getSceneNode("dbgAxis")->setPosition(pos);
        debugCommands.erase(dbgCmd);
    }
    dbgCmd = debugCommands.find("sclAxis");
    if (dbgCmd != debugCommands.end())
    {
        Real factor = 1.0f;
        if (OgreHelpers::convertString2Real(dbgCmd->second, factor) && sceneMgr->hasSceneNode("dbgAxis"))
            sceneMgr->getSceneNode("dbgAxis")->setScale(factor*unitScale, factor*unitScale, factor*unitScale);
        debugCommands.erase(dbgCmd);
    }

    return true;
}

#endif

//-------------------------------------------------------------------------------------
bool DebugHelpers::initLua(lua_State* luaState)
{
    lua_atpanic(luaState, DebugHelpers::luaLogMessage);
    lua_register(luaState, "logMessage", luaLogMessage);

    return true;
}

//-------------------------------------------------------------------------------------
int DebugHelpers::luaLogMessage(lua_State *L)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "luaLog> %s", luaL_checkstring(L, 1));
	return 0;
}

//-------------------------------------------------------------------------------------
