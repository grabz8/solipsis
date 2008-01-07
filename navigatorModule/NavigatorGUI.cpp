#include "NavigatorGUI.h"
#include "Navigator.h"
#include "OgreHelpers.h"
#include "DebugHelpers.h"

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
    mNaviMgr(NaviManager::Get()),
    mCurrentNavi(-1)
{
    for (int n=0;n<NAVI_COUNT;n++)
        mNavisStates[n] = NSNotCreated;
}

//-------------------------------------------------------------------------------------
NavigatorGUI::~NavigatorGUI()
{
    // Hide previous Navi UI
    hidePreviousNavi();
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::startup()
{
    // Startup NaviMouse and create the cursors
    NaviMouse* mouse = mNaviMgr.StartupMouse();
    NaviCursor* defaultCursor = mouse->createCursor("default_cursor", 3, 2);
	defaultCursor->addFrame(1200, "cursor1.png")->addFrame(100, "cursor2.png")->addFrame(100, "cursor3.png")->addFrame(100, "cursor4.png");
	defaultCursor->addFrame(100, "cursor5.png")->addFrame(100, "cursor6.png")->addFrame(100, "cursor5.png")->addFrame(100, "cursor4.png");
	defaultCursor->addFrame(100, "cursor3.png")->addFrame(100, "cursor2.png");
    mouse->setDefaultCursor("default_cursor");

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
        mNaviMgr.getMouse()->show();
    else
        mNaviMgr.getMouse()->hide();
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isMouseVisible()
{
    return mNaviMgr.getMouse()->isVisible();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::login()
{
    // Hide previous Navi UI
    hidePreviousNavi();

    if (mNavisStates[NAVI_LOGIN] == NSNotCreated)
    {
        // Create Navi UI login
        mNaviMgr.createNavi(mNavisNames[NAVI_LOGIN], "local://uilogin.html", NaviPosition(Center), 400, 300, false, false);
        mNaviMgr.setNaviMask(mNavisNames[NAVI_LOGIN], "uilogin.png");
        mNaviMgr.setNaviOpacity(mNavisNames[NAVI_LOGIN], 0.75f);
        mNaviMgr.bind(mNavisNames[NAVI_LOGIN], "pageLoaded", NaviDelegate(this, &NavigatorGUI::loginPageLoaded));
	    mNaviMgr.bind(mNavisNames[NAVI_LOGIN], "connect", NaviDelegate(this, &NavigatorGUI::connect));
	    mNaviMgr.bind(mNavisNames[NAVI_LOGIN], "options", NaviDelegate(this, &NavigatorGUI::options));
	    mNaviMgr.bind(mNavisNames[NAVI_LOGIN], "quit", NaviDelegate(this, &NavigatorGUI::quit));
#ifdef UIDEBUG
        mNaviMgr.bind(mNavisNames[NAVI_LOGIN], "debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
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
        mNaviMgr.bind(mNavisNames[NAVI_CHAT], "debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
#endif
        mNavisStates[NAVI_CHAT] = NSCreated;
    }
    else
        mNaviMgr.showNavi(mNavisNames[NAVI_CHAT], true);
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
        mNaviMgr.showNavi(mNavisNames[NAVI_CONTEXT], true);
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isContextVisible()
{
    return (mNaviMgr.getNaviVisibility(mNavisNames[NAVI_CONTEXT]));
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::contextHide()
{
    if (mNavisStates[NAVI_CONTEXT] != NSNotCreated)
    {
        // Destroy Navi UI context
        mNaviMgr.hideNavi(mNavisNames[NAVI_CONTEXT]);
        mNaviMgr.destroyNavi(mNavisNames[NAVI_CONTEXT]);
        mNavisStates[NAVI_CONTEXT] = NSNotCreated;
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainShow()
{
    if (mNavisStates[NAVI_MODELERMAIN] == NSNotCreated)
    {
        // Create Navi UI modeler
        mNaviMgr.createNavi(mNavisNames[NAVI_MODELERMAIN], "local://uimdlrmain.html", NaviPosition(TopRight), 256, 512, true, false);
        mNaviMgr.setNaviMask(mNavisNames[NAVI_MODELERMAIN], "uimdlrmain.png");
        mNaviMgr.setNaviOpacity(mNavisNames[NAVI_MODELERMAIN], 0.75f);
        mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "pageLoaded", NaviDelegate(this, &NavigatorGUI::naviToShowPageLoaded));
	    mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "FileOpen", NaviDelegate(this, &NavigatorGUI::modelerMainFileOpen));
	    mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "FileSave", NaviDelegate(this, &NavigatorGUI::modelerMainFileSave));
	    mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "FileExit", NaviDelegate(this, &NavigatorGUI::modelerMainFileExit));
		mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "CreateBox", NaviDelegate(this, &NavigatorGUI::modelerMainCreateBox)); 
		mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "CreateCorner", NaviDelegate(this, &NavigatorGUI::modelerMainCreateCorner)); 
		mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "CreatePyramid", NaviDelegate(this, &NavigatorGUI::modelerMainCreatePyramid));
		mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "CreatePrism", NaviDelegate(this, &NavigatorGUI::modelerMainCreatePrism));
		mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "CreateCylinder", NaviDelegate(this, &NavigatorGUI::modelerMainCreateCylinder)); 
		mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "CreateHalfCylinder", NaviDelegate(this, &NavigatorGUI::modelerMainCreateHalfCylinder)); 
		mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "CreateCone", NaviDelegate(this, &NavigatorGUI::modelerMainCreateCone)); 
		mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "CreateHalfCone", NaviDelegate(this, &NavigatorGUI::modelerMainCreateHalfCone)); 
		mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "CreateSphere", NaviDelegate(this, &NavigatorGUI::modelerMainCreateSphere)); 
		mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "CreateHalfSphere", NaviDelegate(this, &NavigatorGUI::modelerMainCreateHalfSphere)); 
		mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "CreateTorus", NaviDelegate(this, &NavigatorGUI::modelerMainCreateTorus)); 
		mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "CreateTube", NaviDelegate(this, &NavigatorGUI::modelerMainCreateTube)); 
		mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "CreateRing", NaviDelegate(this, &NavigatorGUI::modelerMainCreateRing)); 
 
		mNavisStates[NAVI_MODELERMAIN] = NSCreated;
    }
    else
        mNaviMgr.showNavi(mNavisNames[NAVI_MODELERMAIN], true);
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isModelerMainVisible()
{
    return (mNaviMgr.getNaviVisibility(mNavisNames[NAVI_MODELERMAIN]));
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainHide()
{
    if (!isModelerMainVisible()) return;
    mNaviMgr.hideNavi(mNavisNames[NAVI_MODELERMAIN]);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainUnload()
{
    if (mNavisStates[NAVI_MODELERMAIN] != NSNotCreated)
    {
        // Destroy Navi UI modeler
        mNaviMgr.hideNavi(mNavisNames[NAVI_MODELERMAIN]);
        mNaviMgr.destroyNavi(mNavisNames[NAVI_MODELERMAIN]);
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
        mNaviMgr.createNavi(mNavisNames[NAVI_DEBUG], "local://uidebug.html", NaviPosition(TopRight), 300, 256, true, false);
        mNaviMgr.setNaviMask(mNavisNames[NAVI_DEBUG], "uidebug.png");
        mNaviMgr.setNaviOpacity(mNavisNames[NAVI_DEBUG], 0.50f);
        mNaviMgr.bind(mNavisNames[NAVI_DEBUG], "pageLoaded", NaviDelegate(this, &NavigatorGUI::naviToShowPageLoaded));
        mNaviMgr.bind(mNavisNames[NAVI_DEBUG], "debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
        mNavisStates[NAVI_DEBUG] = NSCreated;
    }
    else
    {
        // Hide and destroy UI debug
        if (mNaviMgr.getNaviVisibility(mNavisNames[NAVI_DEBUG])) {
            mNaviMgr.hideNavi(mNavisNames[NAVI_DEBUG]);
            mNaviMgr.destroyNavi(mNavisNames[NAVI_DEBUG]);
            mNavisStates[NAVI_DEBUG] = NSNotCreated;
        }
    }
}
#endif

//-------------------------------------------------------------------------------------
void NavigatorGUI::loginPageLoaded(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::loginPageLoaded()");

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
    mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_LOGIN], "$('infosText').innerHTML = '" + infosText + "'");

    // Show Navi UI login
    if (mNavisStates[NAVI_LOGIN] == NSCreated)
        mNaviMgr.showNavi(mNavisNames[NAVI_LOGIN], true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::connect(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::connect()");

    // Get login name
	std::string login;
    login = naviData["login"].str();
    OGRE_LOG("login=" + (String)(login.c_str()));

    // Check
    if ((login.length() < 2) || (login.compare("null") == 0) || (login.compare("me") == 0))
        // Malformed login
        mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_LOGIN], "$('infosText').innerHTML = 'Enter a valid login ...'");
    else
    {
        // Valid login
        mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_LOGIN], "$('infosText').innerHTML = 'Connecting ...'");
        // Set avatar name
        mNavigator->getUserAvatar()->setName(login.c_str());
        // Call connect
        bool connected = mNavigator->connect();
        char txt[128]; sprintf(txt, "$('infosText').innerHTML = 'Connection %s ...'", (connected) ? "succeeded" : "failed");
        mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('infosText').innerHTML = ''");
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
        mNaviMgr.createNavi(mNavisNames[NAVI_OPTIONS], "local://uioptions.html", NaviPosition(Center), 400, 400, false, false);
        mNaviMgr.setNaviMask(mNavisNames[NAVI_OPTIONS], "uioptions.png");
        mNaviMgr.setNaviOpacity(mNavisNames[NAVI_OPTIONS], 0.75f);
	    mNaviMgr.bind(mNavisNames[NAVI_OPTIONS], "pageLoaded", NaviDelegate(this, &NavigatorGUI::optionsPageLoaded));
	    mNaviMgr.bind(mNavisNames[NAVI_OPTIONS], "ok", NaviDelegate(this, &NavigatorGUI::optionsOk));
	    mNaviMgr.bind(mNavisNames[NAVI_OPTIONS], "back", NaviDelegate(this, &NavigatorGUI::optionsBack));
#ifdef UIDEBUG
        mNaviMgr.bind(mNavisNames[NAVI_OPTIONS], "debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
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

    // Set current values
    if (mNavigator->getConnectionMode() == Navigator::CMStartNewNode)
        mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('radioNewNode').checked = 'checked'");
    else
        mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('radioExistingNode').checked = 'checked'");
    sprintf(txt, "$('inputUdpPort').value = '%d'", mNavigator->getConnectionUdpPort());
    mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], txt);
    sprintf(txt, "$('inputHost').value = '%s'", mNavigator->getConnectionHost().c_str());
    mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], txt);
    sprintf(txt, "$('inputPort').value = '%d'", mNavigator->getConnectionPort());
    mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], txt);
    mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('infosText').innerHTML = ''");
    std::string proxyAutoconfUrl;
    std::string proxyHttpHost;
    int proxyHttpPort;
    int proxyType;
    if (mNaviMgr.getProxyConfig(proxyType, proxyHttpHost, proxyHttpPort, proxyAutoconfUrl))
    {
        switch (proxyType)
        {
        case 0: // 0 for direct connection, no proxy
            mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('radioProxyTypeDirect').checked = 'checked'");
            break;
        case 4: // 4 for auto-detect proxy settings
            mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('radioProxyTypeAutodetect').checked = 'checked'");
            break;
        case 1: // 1 for manual proxy configuration
            mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('radioProxyTypeManual').checked = 'checked'");
            break;
        default: // 2 for proxy auto-conf (PAC)
            mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('radioProxyTypeAutoconf').checked = 'checked'");
            break;
        }
        sprintf(txt, "$('inputProxyHttpHost').value = '%s'", proxyHttpHost.c_str());
        mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], txt);
        sprintf(txt, "$('inputProxyHttpPort').value = '%d'", proxyHttpPort);
        mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], txt);
        sprintf(txt, "$('inputProxyAutoconfUrl').value = '%s'", proxyAutoconfUrl.c_str());
        mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], txt);
    }

    // Show Navi UI options
    if (mNavisStates[NAVI_OPTIONS] == NSCreated)
        mNaviMgr.showNavi(mNavisNames[NAVI_OPTIONS], true);
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

    // Get options
    std::string radioNode;
    int udpPort;
	std::string host;
    int port;
    radioNode = naviData["radioNode"].str();
    udpPort = naviData["udpPort"].toInt();
    host = naviData["host"].str();
    port = naviData["port"].toInt();
    OGRE_LOG("radioNode=" + (String)(radioNode.c_str()) + ", udpPort=" + StringConverter::toString(udpPort) + ", host=" + (String)(host.c_str()) + ", port=" + StringConverter::toString(port));
    std::string radioProxyType;
	std::string proxyHttpHost;
    int proxyHttpPort;
	std::string proxyAutoconfUrl;
    radioProxyType = naviData["radioProxyType"].str();
    proxyHttpHost = naviData["proxyHttpHost"].str();
    proxyHttpPort = naviData["proxyHttpPort"].toInt();
    proxyAutoconfUrl = naviData["proxyAutoconfUrl"].str();
    OGRE_LOG("radioProxyType=" + (String)(radioProxyType.c_str()) + ", proxyHttpHost=" + (String)(proxyHttpHost.c_str()) + ", proxyHttpPort=" + StringConverter::toString(proxyHttpPort) + ", proxyAutoconfUrl=" + (String)(proxyAutoconfUrl.c_str()));

    // Check
    bool valid_options = true;
    Navigator::ConnectionMode connectionMode = (radioNode.compare("newNode") == 0) ? Navigator::CMStartNewNode : Navigator::CMExistingNode;
    if (connectionMode == Navigator::CMStartNewNode)
    {
        if (udpPort < 0)
        {
            // Bad UDP port
            mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('infosText').innerHTML = 'Enter a valid UDP Port ...'");
            valid_options = false;
        }
    }
    else
    {
        if (host.length() < 2)
         {
           // Bad hostname
            mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('infosText').innerHTML = 'Enter a valid hostname ...'");
            valid_options = false;
        }
        if (port < 0)
        {
            // Bad port
            mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('infosText').innerHTML = 'Enter a valid Port ...'");
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
            mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('infosText').innerHTML = 'Enter a valid HTTP proxy Address ...'");
            valid_options = false;
        }
        if (proxyHttpPort < 0)
        {
            // Bad port
            mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('infosText').innerHTML = 'Enter a valid HTTP proxy Port ...'");
            valid_options = false;
        }
    }
    else // autoconf
    {
        proxyType = 2;
        if (proxyAutoconfUrl.length() == 0)
        {
            // Bad url
            mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('infosText').innerHTML = 'Enter a valid proxy server URL ...'");
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
        mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_OPTIONS], "$('infosText').innerHTML = ''");

        mNaviMgr.setProxyConfig(proxyType, proxyHttpHost, proxyHttpPort, proxyAutoconfUrl);

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

    // Set current values
    mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_CHAT], "$('inputChat').value = ''");

    // Show Navi UI chat
    if (mNavisStates[NAVI_CHAT] == NSCreated)
        mNaviMgr.showNavi(mNavisNames[NAVI_CHAT], true);
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
    OGRE_LOG("cmd=" + (String)(cmd.c_str()) + ", params=" + (String)(params.c_str()));

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
    OGRE_LOG("naviName=" + (String)(naviName.c_str()) + ", naviPanel=" + StringConverter::toString(naviPanel));

    // Show Navi UI
    if (mNavisStates[naviPanel] == NSCreated)
        mNaviMgr.showNavi(mNavisNames[naviPanel], true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::hidePreviousNavi()
{
    // Hide previous Navi UI
    if (mCurrentNavi != -1) {
        mNaviMgr.hideNavi(mNavisNames[mCurrentNavi]);
        mNaviMgr.destroyNavi(mNavisNames[mCurrentNavi]);
        mNavisStates[mCurrentNavi] = NSNotCreated;
        mCurrentNavi = -1;
    }
}

//-------------------------------------------------------------------------------------
