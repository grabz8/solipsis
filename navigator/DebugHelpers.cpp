#include "DebugHelpers.h"

#ifdef UIDEBUG

#include "OgreHelpers.h"
#include "Navigator.h"

std::map<String,String> DebugHelpers::debugCommands;

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
        debugCommands.erase(dbgCmd);
        navigator->demoNavi2();
    }
#endif

#ifdef PHYSICS
#ifdef DEMO_PHYSICS1
    // Launch demoPhysics1 ?
    dbgCmd = debugCommands.find("demoPhysics1");
    if (dbgCmd != debugCommands.end())
    {
        debugCommands.erase(dbgCmd);
        navigator->demoPhysics1();
    }
#endif
#endif

    // Rotate sun light ?
    static bool rotateSunLight = false;
    dbgCmd = debugCommands.find("rotateSunLight");
    if (dbgCmd != debugCommands.end())
    {
        debugCommands.erase(dbgCmd);
        rotateSunLight = !rotateSunLight;
    }
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
        Real factor = 1;
        if (OgreHelpers::convertString2Real(dbgCmd->second, factor) && sceneMgr->hasSceneNode("dbgAxis"))
            sceneMgr->getSceneNode("dbgAxis")->setScale(factor, factor, factor);
        debugCommands.erase(dbgCmd);
    }

    return true;
}

#endif