#include "NavigatorGUI.h"
#include "Navigator.h"
#include "OgreHelpers.h"
#include "DebugHelpers.h"
#include "Navi.h"

using namespace Solipsis;

const std::string NavigatorGUI::mNavisNames[] = {
    "uilogin",
    "uioptions",
    "uichat",
    "uicontext",
    "uimdlrmain",
#ifdef UIDEBUG
    "uidebug"
#endif
};

//-------------------------------------------------------------------------------------
NavigatorGUI::NavigatorGUI(Navigator* navigator) :
    mNavigator(navigator),
    mCurrentNavi(-1)
{
    // Initializing Navi
    mNaviMgr = new NaviLibrary::NaviManager(mNavigator->getRenderWindowPtr(), "NaviLocal", ".");

    for (int n=0;n<NAVI_COUNT;n++)
        mNavisStates[n] = NSNotCreated;
}

//-------------------------------------------------------------------------------------
NavigatorGUI::~NavigatorGUI()
{
    // Hide previous Navi UI
    hidePreviousNavi();

    // Finalizing Navi
    delete mNaviMgr;
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::startup()
{
    // Startup NaviMouse and create the cursors
    NaviMouse* mouse = new NaviMouse();
    NaviCursor* defaultCursor = mouse->createCursor("default_cursor", 3, 2);
	defaultCursor->addFrame(1200, "cursor1.png")->addFrame(100, "cursor2.png")->addFrame(100, "cursor3.png")->addFrame(100, "cursor4.png");
	defaultCursor->addFrame(100, "cursor5.png")->addFrame(100, "cursor6.png")->addFrame(100, "cursor5.png")->addFrame(100, "cursor4.png");
	defaultCursor->addFrame(100, "cursor3.png")->addFrame(100, "cursor2.png");
    mouse->setDefaultCursor("default_cursor");
	NaviCursor* moveCursor = mouse->createCursor("move", 19, 19);
	moveCursor->addFrame(0, "cursorMove.png");

    // Load Lua default GUI
    lua_State* luaState = mNavigator->getLuaState();
    if (luaL_loadfile(luaState, "lua\\defaultGUI.lua") != 0)
    {
        OGRE_LOG("Navigator::startup() Unable to load defaultGUI.lua, error: " + String(lua_tostring(luaState, -1)));
        return false;
    }
    if (lua_pcall(luaState, 0, LUA_MULTRET, 0))
    {
        OGRE_LOG("Navigator::startup() Unable to run defaultGUI.lua, error: " + String(lua_tostring(luaState, -1)));
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::SetMouseVisibility(bool visible)
{
    if (visible)
        NaviLibrary::NaviMouse::Get().show();
    else
        NaviLibrary::NaviMouse::Get().hide();
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isMouseVisible()
{
    return NaviLibrary::NaviMouse::Get().isVisible();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::login()
{
    // Hide previous Navi UI
    hidePreviousNavi();

    if (mNavisStates[NAVI_LOGIN] == NSNotCreated)
    {
        // Create Navi UI login
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(mNavisNames[NAVI_LOGIN], "local://uilogin.html", NaviPosition(Center), 400, 300);
        navi->setMovable(false);
        navi->setAutoUpdateOnFocus(true);
        navi->setMaxUPS(24);
        navi->hide();
        navi->setMask("uilogin.png");
        navi->setOpacity(0.75f);
        navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::loginPageLoaded));
	    navi->bind("connect", NaviDelegate(this, &NavigatorGUI::connect));
	    navi->bind("options", NaviDelegate(this, &NavigatorGUI::options));
	    navi->bind("quit", NaviDelegate(this, &NavigatorGUI::quit));
#ifdef UIDEBUG
        navi->bind("debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
#endif
        mNavisStates[NAVI_LOGIN] = NSCreated;
    }

    // Set next Navi UI
    mCurrentNavi = NAVI_LOGIN;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::inWorld()
{
    // Hide previous Navi UI
    hidePreviousNavi();

    if (mNavisStates[NAVI_CHAT] == NSNotCreated)
    {
        // Create Navi UI chat
        // Lua
        if (!mNavigator->getNavigatorLua()->call("createGUI", "%s", mNavisNames[NAVI_CHAT].c_str()))
        {
            OGRE_LOG("Navigator::inWorld() Unable to create GUI called " + mNavisNames[NAVI_CHAT]);
            return;
        }
#ifdef UIDEBUG
        mNaviMgr->getNavi(mNavisNames[NAVI_CHAT])->bind("debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
#endif
        mNavisStates[NAVI_CHAT] = NSCreated;
    }
    else
        mNaviMgr->getNavi(mNavisNames[NAVI_CHAT])->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::contextShow(int x, int y, const std::string& items)
{
    if (mNavisStates[NAVI_CONTEXT] == NSNotCreated)
    {
        // Create Navi UI context
        // Lua
        if (!mNavigator->getNavigatorLua()->call("createGUI", "%s%d%d%s", mNavisNames[NAVI_CONTEXT].c_str(), x, y, items.c_str()))
        {
            OGRE_LOG("Navigator::contextShow() Unable to create GUI called " + mNavisNames[NAVI_CONTEXT]);
            return;
        }
        mNavisStates[NAVI_CONTEXT] = NSCreated;
    }
    else
        mNaviMgr->getNavi(mNavisNames[NAVI_CONTEXT])->show(true);
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isContextVisible()
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_CONTEXT]);
    return ((navi != 0) && navi->getVisibility());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::contextHide()
{
    if (mNavisStates[NAVI_CONTEXT] != NSNotCreated)
    {
        // Destroy Navi UI context
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_CONTEXT]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[NAVI_CONTEXT] = NSNotCreated;
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainShow()
{
    if (mNavisStates[NAVI_MODELERMAIN] == NSNotCreated)
    {
        // Create Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(mNavisNames[NAVI_MODELERMAIN], "local://uimdlrmain.html", NaviPosition(TopRight), 256, 512);
        navi->setMovable(true);
        navi->hide();
        navi->setMask("uimdlrmain.png");
        navi->setOpacity(0.75f);
        navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::naviToShowPageLoaded));
	    navi->bind("FileOpen", NaviDelegate(this, &NavigatorGUI::modelerMainFileOpen));
	    navi->bind("FileSave", NaviDelegate(this, &NavigatorGUI::modelerMainFileSave));
	    navi->bind("FileExit", NaviDelegate(this, &NavigatorGUI::modelerMainFileExit));
		navi->bind("CreateBox", NaviDelegate(this, &NavigatorGUI::modelerMainCreateBox)); 
		navi->bind("CreateCorner", NaviDelegate(this, &NavigatorGUI::modelerMainCreateCorner)); 
		navi->bind("CreatePyramid", NaviDelegate(this, &NavigatorGUI::modelerMainCreatePyramid));
		navi->bind("CreatePrism", NaviDelegate(this, &NavigatorGUI::modelerMainCreatePrism));
		navi->bind("CreateCylinder", NaviDelegate(this, &NavigatorGUI::modelerMainCreateCylinder)); 
		navi->bind("CreateHalfCylinder", NaviDelegate(this, &NavigatorGUI::modelerMainCreateHalfCylinder)); 
		navi->bind("CreateCone", NaviDelegate(this, &NavigatorGUI::modelerMainCreateCone)); 
		navi->bind("CreateHalfCone", NaviDelegate(this, &NavigatorGUI::modelerMainCreateHalfCone)); 
		navi->bind("CreateSphere", NaviDelegate(this, &NavigatorGUI::modelerMainCreateSphere)); 
		navi->bind("CreateHalfSphere", NaviDelegate(this, &NavigatorGUI::modelerMainCreateHalfSphere)); 
		navi->bind("CreateTorus", NaviDelegate(this, &NavigatorGUI::modelerMainCreateTorus)); 
		navi->bind("CreateTube", NaviDelegate(this, &NavigatorGUI::modelerMainCreateTube)); 
		navi->bind("CreateRing", NaviDelegate(this, &NavigatorGUI::modelerMainCreateRing)); 

		navi->bind("UpdateTaperX",  NaviDelegate(this, &NavigatorGUI::modelerSliderTaperX));
		navi->bind("UpdateTaperY", NaviDelegate(this, &NavigatorGUI::modelerSliderTaperY));
  
		mNavisStates[NAVI_MODELERMAIN] = NSCreated;
    }
    else
        mNaviMgr->getNavi(mNavisNames[NAVI_MODELERMAIN])->show(true);
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isModelerMainVisible()
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERMAIN]);
    return ((navi != 0) && navi->getVisibility());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainHide()
{
    if (!isModelerMainVisible()) return;
    mNaviMgr->getNavi(mNavisNames[NAVI_MODELERMAIN])->hide();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainUnload()
{
    if (mNavisStates[NAVI_MODELERMAIN] != NSNotCreated)
    {
        // Destroy Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERMAIN]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[NAVI_MODELERMAIN] = NSNotCreated;

		mNavigator->setState(Navigator::SInWorld);	
		mNavigator->endModeling();
		modelerMainUnload();
    }
}

#ifdef UIDEBUG
//-------------------------------------------------------------------------------------
void NavigatorGUI::switchDebug()
{
    if (mNavisStates[NAVI_DEBUG] == NSNotCreated)
    {
        // Create Navi UI debug
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(mNavisNames[NAVI_DEBUG], "local://uidebug.html", NaviPosition(TopRight), 300, 256);
        navi->setMovable(true);
        navi->setAutoUpdateOnFocus(true);
        navi->setMaxUPS(24);
        navi->hide();
        navi->setMask("uidebug.png");
        navi->setOpacity(0.50f);
        navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::debugPageLoaded));
        navi->bind("debugRefreshTree", NaviDelegate(this, &NavigatorGUI::debugRefreshTree));
        navi->bind("debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
        mNavisStates[NAVI_DEBUG] = NSCreated;
        mTreeDirty = true;
    }
    else
    {
        // Hide and destroy UI debug
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_DEBUG]);
        if (navi->getVisibility()) {
            navi->hide();
            mNaviMgr->destroyNavi(navi);
            mNavisStates[NAVI_DEBUG] = NSNotCreated;
        }
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::debugPageLoaded(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::debugPageLoaded()");

    // Refresh tree datas
    debugRefreshTree(naviData);

    // Show Navi UI debug
    if (mNavisStates[NAVI_DEBUG] == NSCreated)
        mNaviMgr->getNavi(mNavisNames[NAVI_DEBUG])->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::debugRefreshTree(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::debugRefreshTree()");

    if (!mTreeDirty) return;

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_DEBUG]);

    navi->evaluateJS("allTree.disable()");
    navi->evaluateJS("allTree.root.clear()");
    navi->evaluateJS("allTree.insert({text:'Scenes', id:'Scenes'})");
    String sceneName = mNavigator->getSceneMgrPtr()->getName();
    navi->evaluateJS("allTree.get('Scenes').insert({text:'" + sceneName + "', id:'S_" + sceneName + "'})");
    navi->evaluateJS("allTree.insert({text:'OgrePeers', id:'OgrePeers'})");
    for (std::map<String,OgrePeer*>::iterator ogrePeer = mNavigator->getOgrePeerManager()->getOgrePeersIteratorBegin();ogrePeer != mNavigator->getOgrePeerManager()->getOgrePeersIteratorEnd();ogrePeer++)
    {
        String ogrePeerName = ogrePeer->second->getPeer()->getLogin();
        navi->evaluateJS("allTree.get('OgrePeers').insert({text:'" + ogrePeerName + "', id:'OP_" + ogrePeerName + "'})");
    }
    navi->evaluateJS("allTree.enable()");

    mTreeDirty = false;
}
#endif

//-------------------------------------------------------------------------------------
void NavigatorGUI::loginPageLoaded(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::loginPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_LOGIN]);

    // Set network config into informations text
    char txt[128];
    std::string infosText;
    switch (mNavigator->getConnectionMode())
    {
    case (Navigator::CMStartNewNode):
        sprintf(txt, "%d", mNavigator->getConnectionUdpPort());
        infosText = "Create a new node (UDP port " + String(txt) + ")";
        break;
    case (Navigator::CMExistingNode):
        sprintf(txt, "%d", mNavigator->getConnectionPort());
        infosText = "Connect to " + mNavigator->getConnectionHost() + " (port " + String(txt) + ")";
        break;
    };
    navi->evaluateJS("$('infosText').innerHTML = '" + infosText + "'");

    // Show Navi UI login
    if (mNavisStates[NAVI_LOGIN] == NSCreated)
        navi->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::connect(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::connect()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_LOGIN]);

    // Get login name
	std::string login;
    login = naviData["login"].str();
    OGRE_LOG("login=" + login);

    // Check
    if ((login.length() < 2) || (login.compare("null") == 0) || (login.compare("me") == 0))
        // Malformed login
        navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid login ...'");
    else
    {
        // Valid login
        navi->evaluateJS("$('infosText').innerHTML = 'Connecting ...'");
        // Set avatar name
        mNavigator->getUserAvatar()->setName(login);
        // Call connect
        bool connected = mNavigator->connect();
        char txt[128]; sprintf(txt, "$('infosText').innerHTML = 'Connection %s ...'", (connected) ? "succeeded" : "failed");
        navi->evaluateJS("$('infosText').innerHTML = ''");
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::options(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::options()");

    // Hide previous Navi UI
    hidePreviousNavi();

    if (mNavisStates[NAVI_OPTIONS] == NSNotCreated)
    {
        // Create Navi UI options
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(mNavisNames[NAVI_OPTIONS], "local://uioptions.html", NaviPosition(Center), 400, 400);
        navi->setMovable(false);
        navi->setAutoUpdateOnFocus(true);
        navi->setMaxUPS(8);
        navi->hide();
        navi->setMask("uioptions.png");
        navi->setOpacity(0.75f);
	    navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::optionsPageLoaded));
	    navi->bind("ok", NaviDelegate(this, &NavigatorGUI::optionsOk));
	    navi->bind("back", NaviDelegate(this, &NavigatorGUI::optionsBack));
#ifdef UIDEBUG
        navi->bind("debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
#endif
        mNavisStates[NAVI_OPTIONS] = NSCreated;
    }

    // Set next Navi UI
    mCurrentNavi = NAVI_OPTIONS;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::optionsPageLoaded(const NaviData& naviData)
{
    char txt[256];

    OGRE_LOG("NavigatorGUI::optionsPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_OPTIONS]);

    // Set current values
    if (mNavigator->getConnectionMode() == Navigator::CMStartNewNode)
        navi->evaluateJS("$('radioNewNode').checked = 'checked'");
    else
        navi->evaluateJS("$('radioExistingNode').checked = 'checked'");
    sprintf(txt, "$('inputUdpPort').value = '%d'", mNavigator->getConnectionUdpPort());
    navi->evaluateJS(txt);
    sprintf(txt, "$('inputHost').value = '%s'", mNavigator->getConnectionHost().c_str());
    navi->evaluateJS(txt);
    sprintf(txt, "$('inputPort').value = '%d'", mNavigator->getConnectionPort());
    navi->evaluateJS(txt);
    navi->evaluateJS("$('infosText').innerHTML = ''");
    std::string proxyAutoconfUrl;
    std::string proxyHttpHost;
    int proxyHttpPort;
    int proxyType;
    if (mNaviMgr->getProxyConfig(proxyType, proxyHttpHost, proxyHttpPort, proxyAutoconfUrl))
    {
        switch (proxyType)
        {
        case 0: // 0 for direct connection, no proxy
            navi->evaluateJS("$('radioProxyTypeDirect').checked = 'checked'");
            break;
        case 4: // 4 for auto-detect proxy settings
            navi->evaluateJS("$('radioProxyTypeAutodetect').checked = 'checked'");
            break;
        case 1: // 1 for manual proxy configuration
            navi->evaluateJS("$('radioProxyTypeManual').checked = 'checked'");
            break;
        default: // 2 for proxy auto-conf (PAC)
            navi->evaluateJS("$('radioProxyTypeAutoconf').checked = 'checked'");
            break;
        }
        sprintf(txt, "$('inputProxyHttpHost').value = '%s'", proxyHttpHost.c_str());
        navi->evaluateJS(txt);
        sprintf(txt, "$('inputProxyHttpPort').value = '%d'", proxyHttpPort);
        navi->evaluateJS(txt);
        sprintf(txt, "$('inputProxyAutoconfUrl').value = '%s'", proxyAutoconfUrl.c_str());
        navi->evaluateJS(txt);
    }
#ifdef PHYSICSPLUGINS
    PhysicsEngineManager::EngineList &physicsEngines = PhysicsEngineManager::getSingleton().getEngines();
    sprintf(txt, "$('selectPhysicsEngine').options.length = %d", physicsEngines.size());
    navi->evaluateJS(txt);
    int e = 0;
    for (PhysicsEngineManager::EngineList::iterator it=physicsEngines.begin(); it != physicsEngines.end(); ++it, ++e)
    {
        sprintf(txt, "$('selectPhysicsEngine').options['%d'].value = '%s'", e, (*it)->getName().c_str());
        navi->evaluateJS(txt);
        sprintf(txt, "$('selectPhysicsEngine').options['%d'].text = '%s'", e, (*it)->getName().c_str());
        navi->evaluateJS(txt);
    }
    navi->evaluateJS("$('selectPhysicsEngine').options.selectedIndex = 0");
#endif

    // Show Navi UI options
    if (mNavisStates[NAVI_OPTIONS] == NSCreated)
        navi->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::quit(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::quit()");

    mNavigator->quit();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::optionsOk(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::optionsOk()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_OPTIONS]);

    // Get options
    std::string radioNode;
    int udpPort;
	std::string host;
    int port;
    radioNode = naviData["radioNode"].str();
    udpPort = naviData["udpPort"].toInt();
    host = naviData["host"].str();
    port = naviData["port"].toInt();
    OGRE_LOG("radioNode=" + radioNode + ", udpPort=" + StringConverter::toString(udpPort) + ", host=" + host + ", port=" + StringConverter::toString(port));
    std::string radioProxyType;
	std::string proxyHttpHost;
    int proxyHttpPort;
	std::string proxyAutoconfUrl;
    radioProxyType = naviData["radioProxyType"].str();
    proxyHttpHost = naviData["proxyHttpHost"].str();
    proxyHttpPort = naviData["proxyHttpPort"].toInt();
    proxyAutoconfUrl = naviData["proxyAutoconfUrl"].str();
    OGRE_LOG("radioProxyType=" + radioProxyType + ", proxyHttpHost=" + proxyHttpHost + ", proxyHttpPort=" + StringConverter::toString(proxyHttpPort) + ", proxyAutoconfUrl=" + proxyAutoconfUrl);

    // Check
    bool valid_options = true;
    Navigator::ConnectionMode connectionMode = (radioNode.compare("newNode") == 0) ? Navigator::CMStartNewNode : Navigator::CMExistingNode;
    if (connectionMode == Navigator::CMStartNewNode)
    {
        if (udpPort < 0)
        {
            // Bad UDP port
            navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid UDP Port ...'");
            valid_options = false;
        }
    }
    else
    {
        if (host.length() < 2)
         {
           // Bad hostname
            navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid hostname ...'");
            valid_options = false;
        }
        if (port < 0)
        {
            // Bad port
            navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid Port ...'");
            valid_options = false;
        }
    }
    int proxyType;
    if (radioProxyType.compare("direct") == 0)
    {
        proxyType = 0;
        proxyAutoconfUrl = "";
        proxyHttpHost = "";
        proxyHttpPort = 0;
    }
    else if (radioProxyType.compare("autodetect") == 0)
    {
        proxyType = 4;
        proxyAutoconfUrl = "";
        proxyHttpHost = "";
        proxyHttpPort = 0;
    }
    else if (radioProxyType.compare("manual") == 0)
    {
        proxyType = 1;
        proxyAutoconfUrl = "";
        if (proxyHttpHost.length() == 0)
        {
            // Bad url
            navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid HTTP proxy Address ...'");
            valid_options = false;
        }
        if (proxyHttpPort < 0)
        {
            // Bad port
            navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid HTTP proxy Port ...'");
            valid_options = false;
        }
    }
    else // autoconf
    {
        proxyType = 2;
        if (proxyAutoconfUrl.length() == 0)
        {
            // Bad url
            navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid proxy server URL ...'");
            valid_options = false;
        }
    }

    // Valid options ?
    if (valid_options)
    {
        mNavigator->setConnectionMode(connectionMode);
        mNavigator->setConnectionUdpPort(udpPort);
        mNavigator->setConnectionHost(host);
        mNavigator->setConnectionPort(port);
        navi->evaluateJS("$('infosText').innerHTML = ''");

        mNaviMgr->setProxyConfig(proxyType, proxyHttpHost, proxyHttpPort, proxyAutoconfUrl);

#ifdef PHYSICSPLUGINS
        if (PhysicsEngineManager::getSingleton().getSelectedEngine() != 0)
            PhysicsEngineManager::getSingleton().getSelectedEngine()->shutdown();
        PhysicsEngineManager::getSingleton().selectEngine(naviData["physicsEngine"].str());
        PhysicsEngineManager::getSingleton().getSelectedEngine()->init();
#endif

        // Return to Navi UI login
        login();
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::optionsBack(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::optionsBack()");

    // Return to Navi UI login
    login();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::chatPageLoaded(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::chatPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_CHAT]);

    // Set current values
    navi->evaluateJS("$('inputChat').value = ''");

    // Show Navi UI chat
    if (mNavisStates[NAVI_CHAT] == NSCreated)
        navi->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainFileOpen(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainFileOpen()");
	
    modelerMainUnload();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainFileSave(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainFileSave()");
	
    modelerMainUnload();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainFileExit(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainFileExit()");
	
    modelerMainUnload();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateBox(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateBox()");
	mNavigator->startModeling();
	mNavigator->createBox();
    //modelerMainUnload();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateCorner(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateCorner()");
	mNavigator->startModeling();
	mNavigator->createCorner();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreatePyramid(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreatePyramid()");
	mNavigator->startModeling();
	mNavigator->createPyramid();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreatePrism(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreatePrism()");
	mNavigator->startModeling();
	mNavigator->createPrism();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateCylinder(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateCylinder()");
	mNavigator->startModeling();
	mNavigator->createCylinder();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateHalfCylinder(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateHalfCylinder()");
	mNavigator->startModeling();
	mNavigator->createHalfCyl();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateCone(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateCone()");
	mNavigator->startModeling();
	mNavigator->createCone();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateHalfCone(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateHalfCone()");
	mNavigator->startModeling();
	mNavigator->createHalfCone();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateSphere(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateSphere()");
	mNavigator->startModeling();
	mNavigator->createSphere();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateHalfSphere(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateHalfSphere()");
	mNavigator->startModeling();
	mNavigator->createHalfSphere();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateTorus(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateTorus()");
	mNavigator->startModeling();
	mNavigator->createTorus();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateTube(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateTube()");
	mNavigator->startModeling();
	mNavigator->createTube();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateRing(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateRing()");
	mNavigator->startModeling();
	mNavigator->createRing();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerSliderTaperX(const NaviData& naviData)
{
	static std::string value = "0";
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERMAIN]);
    // Set current values
    navi->evaluateJS("document.getElementById('taperX').innerHTML");

	if( value != "0" )
	{
		OGRE_LOG("slider TAPER X = " + (String)(value.c_str()));
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerSliderTaperY(const NaviData& naviData)
{
	static std::string value = "0";
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERMAIN]);
    // Set current values
    navi->evaluateJS("document.getElementById('taperY').innerHTML");
	if( value != "0" )
	{
		OGRE_LOG("slider TAPER Y = " + (String)(value.c_str()));
	}
}

//-------------------------------------------------------------------------------------
#ifdef UIDEBUG
//-------------------------------------------------------------------------------------
void NavigatorGUI::debugCommand(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::debugCommand()");

    // Get message to send
    std::string cmd;
    std::string params;
    cmd = naviData["cmd"].str();
    params = naviData["params"].str();
    OGRE_LOG("cmd=" + cmd + ", params=" + params);

    // Push debug command
    DebugHelpers::debugCommands[String(cmd)] = String(params);
}
#endif

//-------------------------------------------------------------------------------------
NavigatorGUI::NaviPanel NavigatorGUI::getNaviPanel(const std::string& naviName)
{
    for (int n=0; n < NAVI_COUNT; ++n)
        if (mNavisNames[n].compare(naviName) == 0) return (NaviPanel)n;

    return (NaviPanel)-1;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::naviToShowPageLoaded(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::naviToShowPageLoaded()");

    std::string naviName;
    naviName = naviData["naviName"].str();
    NaviPanel naviPanel = getNaviPanel(naviName);
    OGRE_LOG("naviName=" + naviName + ", naviPanel=" + StringConverter::toString(naviPanel));

    // Show Navi UI
    if (mNavisStates[naviPanel] == NSCreated)
        mNaviMgr->getNavi(mNavisNames[naviPanel])->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::hidePreviousNavi()
{
    // Hide previous Navi UI
    if (mCurrentNavi != -1) {
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[mCurrentNavi]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[mCurrentNavi] = NSNotCreated;
        mCurrentNavi = -1;
    }
}

//-------------------------------------------------------------------------------------
