#include "NavigatorGUI.h"
#include "Navigator.h"
#include "DebugHelpers.h"

const std::string NavigatorGUI::mNavisNames[] = {
    "uilogin",
    "uioptions",
    "uichat",
    "uimdlrmain",
#ifdef UIDEBUG
    "uidebug"
#endif
};

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
void NavigatorGUI::startup()
{
    // Startup NaviMouse and create the cursors
    NaviMouse* mouse = mNaviMgr.StartupMouse();
    NaviCursor* defaultCursor = mouse->createCursor("default_cursor", 3, 2);
	defaultCursor->addFrame(1200, "cursor1.png")->addFrame(100, "cursor2.png")->addFrame(100, "cursor3.png")->addFrame(100, "cursor4.png");
	defaultCursor->addFrame(100, "cursor5.png")->addFrame(100, "cursor6.png")->addFrame(100, "cursor5.png")->addFrame(100, "cursor4.png");
	defaultCursor->addFrame(100, "cursor3.png")->addFrame(100, "cursor2.png");
    mouse->setDefaultCursor("default_cursor");
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
        mNaviMgr.bind(mNavisNames[NAVI_LOGIN], "pageRefresh", NaviDelegate(this, &NavigatorGUI::loginPageRefresh));
	    mNaviMgr.bind(mNavisNames[NAVI_LOGIN], "connect", NaviDelegate(this, &NavigatorGUI::connect));
	    mNaviMgr.bind(mNavisNames[NAVI_LOGIN], "options", NaviDelegate(this, &NavigatorGUI::options));
	    mNaviMgr.bind(mNavisNames[NAVI_LOGIN], "quit", NaviDelegate(this, &NavigatorGUI::quit));
#ifdef UIDEBUG
        mNaviMgr.bind(mNavisNames[NAVI_LOGIN], "debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
#endif
        mNavisStates[NAVI_LOGIN] = NSLoaded;
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
        mNaviMgr.createNavi(mNavisNames[NAVI_CHAT], "local://uichat.html", NaviPosition(TopLeft), 512, 64, true, false);
        mNaviMgr.setNaviMask(mNavisNames[NAVI_CHAT], "uichat.png");
        mNaviMgr.setNaviOpacity(mNavisNames[NAVI_CHAT], 0.75f);
	    mNaviMgr.bind(mNavisNames[NAVI_CHAT], "pageRefresh", NaviDelegate(this, &NavigatorGUI::chatPageRefresh));
	    mNaviMgr.bind(mNavisNames[NAVI_CHAT], "sendMessage", NaviDelegate(this, &NavigatorGUI::sendMessage));
#ifdef UIDEBUG
        mNaviMgr.bind(mNavisNames[NAVI_CHAT], "debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
#endif
        mNavisStates[NAVI_CHAT] = NSLoaded;
    }

    // Set next Navi UI
    mCurrentNavi = NAVI_CHAT;
}

#ifdef UIDEBUG
//-------------------------------------------------------------------------------------
void NavigatorGUI::switchDebug()
{
    if (mNavisStates[NAVI_DEBUG] == NSNotCreated)
    {
        // Create Navi UI debug
        mNaviMgr.createNavi(mNavisNames[NAVI_DEBUG], "local://uidebug.html", NaviPosition(TopRight), 256, 256, true, false);
        mNaviMgr.setNaviMask(mNavisNames[NAVI_DEBUG], "uidebug.png");
        mNaviMgr.setNaviOpacity(mNavisNames[NAVI_DEBUG], 0.50f);
        mNaviMgr.bind(mNavisNames[NAVI_DEBUG], "pageRefresh", NaviDelegate(this, &NavigatorGUI::naviToShowPageRefresh));
        mNaviMgr.bind(mNavisNames[NAVI_DEBUG], "debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
        mNavisStates[NAVI_DEBUG] = NSLoaded;
    }
    else
    {
        // Hide and destroy UI debug
        if (mCurrentNavi != -1) {
            mNaviMgr.hideNavi(mNavisNames[NAVI_DEBUG]);
            mNaviMgr.destroyNavi(mNavisNames[NAVI_DEBUG]);
            mNavisStates[NAVI_DEBUG] = NSNotCreated;
        }
    }
}
#endif

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainShow()
{
    if (mNavisStates[NAVI_MODELERMAIN] == NSNotCreated)
    {
        // Create Navi UI login
        mNaviMgr.createNavi(mNavisNames[NAVI_MODELERMAIN], "local://uimdlrmain.html", NaviPosition(TopRight), 256, 512, true, false);
        mNaviMgr.setNaviMask(mNavisNames[NAVI_MODELERMAIN], "uimdlrmain.png");
        mNaviMgr.setNaviOpacity(mNavisNames[NAVI_MODELERMAIN], 0.75f);
        mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "pageRefresh", NaviDelegate(this, &NavigatorGUI::naviToShowPageRefresh));
	    mNaviMgr.bind(mNavisNames[NAVI_MODELERMAIN], "FileExit", NaviDelegate(this, &NavigatorGUI::modelerMainFileExit));
        mNavisStates[NAVI_MODELERMAIN] = NSLoaded;
    }
    else
    {
        mNaviMgr.showNavi(mNavisNames[NAVI_MODELERMAIN], true);
        mNavisStates[NAVI_MODELERMAIN] = NSVisible;
    }
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isModelerMainVisible()
{
    return (mNavisStates[NAVI_MODELERMAIN] == NSVisible);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainHide()
{
    if (mNavisStates[NAVI_MODELERMAIN] == NSVisible)
    {
        // Create Navi UI login
        mNaviMgr.hideNavi(mNavisNames[NAVI_MODELERMAIN]);
        mNavisStates[NAVI_MODELERMAIN] = NSLoaded;
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainUnload()
{
    if (mNavisStates[NAVI_MODELERMAIN] != NSNotCreated)
    {
        // Create Navi UI login
        mNaviMgr.hideNavi(mNavisNames[NAVI_MODELERMAIN]);
        mNaviMgr.destroyNavi(mNavisNames[NAVI_MODELERMAIN]);
        mNavisStates[NAVI_MODELERMAIN] = NSNotCreated;
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::loginPageRefresh(const NaviData& naviData)
{
    LogManager::getSingletonPtr()->logMessage("NavigatorGUI::loginPageRefresh()");

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
    if (mNavisStates[NAVI_LOGIN] == NSLoaded)
    {
        mNaviMgr.showNavi(mNavisNames[NAVI_LOGIN], true);
        mNavisStates[NAVI_LOGIN] = NSVisible;
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::connect(const NaviData& naviData)
{
    LogManager::getSingletonPtr()->logMessage("NavigatorGUI::connect()");

    // Get login name
	std::string login;
    login = naviData["login"].str();
    LogManager::getSingletonPtr()->logMessage("login=" + (String)(login.c_str()));

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
    LogManager::getSingletonPtr()->logMessage("NavigatorGUI::options()");

    // Hide previous Navi UI
    hidePreviousNavi();

    if (mNavisStates[NAVI_OPTIONS] == NSNotCreated)
    {
        // Create Navi UI options
        mNaviMgr.createNavi(mNavisNames[NAVI_OPTIONS], "local://uioptions.html", NaviPosition(Center), 400, 400, false, false);
        mNaviMgr.setNaviMask(mNavisNames[NAVI_OPTIONS], "uioptions.png");
        mNaviMgr.setNaviOpacity(mNavisNames[NAVI_OPTIONS], 0.75f);
	    mNaviMgr.bind(mNavisNames[NAVI_OPTIONS], "pageRefresh", NaviDelegate(this, &NavigatorGUI::optionsPageRefresh));
	    mNaviMgr.bind(mNavisNames[NAVI_OPTIONS], "ok", NaviDelegate(this, &NavigatorGUI::optionsOk));
	    mNaviMgr.bind(mNavisNames[NAVI_OPTIONS], "back", NaviDelegate(this, &NavigatorGUI::optionsBack));
#ifdef UIDEBUG
        mNaviMgr.bind(mNavisNames[NAVI_OPTIONS], "debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
#endif
        mNavisStates[NAVI_OPTIONS] = NSLoaded;
    }

    // Set next Navi UI
    mCurrentNavi = NAVI_OPTIONS;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::optionsPageRefresh(const NaviData& naviData)
{
    char txt[256];

    LogManager::getSingletonPtr()->logMessage("NavigatorGUI::optionsPageRefresh()");

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
    if (mNavisStates[NAVI_OPTIONS] == NSLoaded)
    {
        mNaviMgr.showNavi(mNavisNames[NAVI_OPTIONS], true);
        mNavisStates[NAVI_OPTIONS] = NSVisible;
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::quit(const NaviData& naviData)
{
    LogManager::getSingletonPtr()->logMessage("NavigatorGUI::quit()");

    mNavigator->quit();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::optionsOk(const NaviData& naviData)
{
    LogManager::getSingletonPtr()->logMessage("NavigatorGUI::optionsOk()");

    // Get options
    std::string radioNode;
    int udpPort;
	std::string host;
    int port;
    radioNode = naviData["radioNode"].str();
    udpPort = naviData["udpPort"].toInt();
    host = naviData["host"].str();
    port = naviData["port"].toInt();
    LogManager::getSingletonPtr()->logMessage("radioNode=" + (String)(radioNode.c_str()) + ", udpPort=" + StringConverter::toString(udpPort) + ", host=" + (String)(host.c_str()) + ", port=" + StringConverter::toString(port));
    std::string radioProxyType;
	std::string proxyHttpHost;
    int proxyHttpPort;
	std::string proxyAutoconfUrl;
    radioProxyType = naviData["radioProxyType"].str();
    proxyHttpHost = naviData["proxyHttpHost"].str();
    proxyHttpPort = naviData["proxyHttpPort"].toInt();
    proxyAutoconfUrl = naviData["proxyAutoconfUrl"].str();
    LogManager::getSingletonPtr()->logMessage("radioProxyType=" + (String)(radioProxyType.c_str()) + ", proxyHttpHost=" + (String)(proxyHttpHost.c_str()) + ", proxyHttpPort=" + StringConverter::toString(proxyHttpPort) + ", proxyAutoconfUrl=" + (String)(proxyAutoconfUrl.c_str()));

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
    LogManager::getSingletonPtr()->logMessage("NavigatorGUI::optionsBack()");

    // Return to Navi UI login
    login();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::chatPageRefresh(const NaviData& naviData)
{
    LogManager::getSingletonPtr()->logMessage("NavigatorGUI::chatPageRefresh()");

    // Set current values
    mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_CHAT], "$('inputChat').value = ''");

    // Show Navi UI options
    if (mNavisStates[NAVI_CHAT] == NSLoaded)
    {
        mNaviMgr.showNavi(mNavisNames[NAVI_CHAT], true);
        mNavisStates[NAVI_CHAT] = NSVisible;
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::sendMessage(const NaviData& naviData)
{
    LogManager::getSingletonPtr()->logMessage("NavigatorGUI::sendMessage()");

    // Get message to send
    std::string msg;
    msg = naviData["msg"].str();
    LogManager::getSingletonPtr()->logMessage("msg=" + (String)(msg.c_str()));

    // Set current values
    mNaviMgr.naviEvaluateJS(mNavisNames[NAVI_CHAT], "$('inputChat').value = ''");

    // Send message
    mNavigator->sendMessage((Ogre::String)msg);
}

#ifdef UIDEBUG
//-------------------------------------------------------------------------------------
void NavigatorGUI::debugCommand(const NaviData& naviData)
{
    LogManager::getSingletonPtr()->logMessage("NavigatorGUI::debugCommand()");

    // Get message to send
    std::string cmd;
    std::string params;
    cmd = naviData["cmd"].str();
    params = naviData["params"].str();
    LogManager::getSingletonPtr()->logMessage("cmd=" + (String)(cmd.c_str()) + ", params=" + (String)(params.c_str()));

    // Push debug command
    DebugHelpers::debugCommands[(Ogre::String)cmd] = (Ogre::String)params;
}
#endif

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainFileExit(const NaviData& naviData)
{
    LogManager::getSingletonPtr()->logMessage("NavigatorGUI::modelerMainFileExit()");

    modelerMainHide();
}

//-------------------------------------------------------------------------------------
NavigatorGUI::NaviPanel NavigatorGUI::getNaviPanel(const std::string& naviName)
{
    for (int n=0; n < NAVI_COUNT; ++n)
        if (mNavisNames[n].compare(naviName) == 0) return (NaviPanel)n;

    return (NaviPanel)-1;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::naviToShowPageRefresh(const NaviData& naviData)
{
    LogManager::getSingletonPtr()->logMessage("NavigatorGUI::naviToShowPageRefresh()");

    std::string naviName;
    naviName = naviData["naviName"].str();
    NaviPanel naviPanel = getNaviPanel(naviName);
    LogManager::getSingletonPtr()->logMessage("naviName=" + (String)(naviName.c_str()) + ", naviPanel=" + StringConverter::toString(naviPanel));

    // Show Navi UI
    if (mNavisStates[naviPanel] == NSLoaded)
    {
        mNaviMgr.showNavi(mNavisNames[naviPanel], true);
        mNavisStates[naviPanel] = NSVisible;
    }
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
