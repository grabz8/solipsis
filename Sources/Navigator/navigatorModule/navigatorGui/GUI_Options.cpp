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

#include "GUI_Options.h"
#include "GUI_login.h"


#include <CTLog.h>
#include <CTStringHelpers.h>
#include <Navi.h>

#include "GUI_ChooseWorld.h"

using namespace Solipsis;
using namespace CommonTools;
GUI_Options * GUI_Options::stGUI_Options = NULL;

GUI_Options::GUI_Options() : GUI_Panel("uioptions")
{
    stGUI_Options = this;
    mNavigator = Navigator::getSingletonPtr();
    mNaviMgr = NavigatorGUI::getNaviMgr();
}

//static 
bool GUI_Options::createAndShowPanel()
{
    if (!stGUI_Options)
    {
        new GUI_Options();
    }

    return stGUI_Options->show();
}


//-------------------------------------------------------------------------------------
bool GUI_Options::show()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Options::show()");

    // Hide previous Navi UI
    NavigatorGUI::destroyAllRegisteredPanels();

    if (m_curState == NSNotCreated)
    {
        // Create Navi UI options
        createNavi("local://uioptions.html", NaviPosition(Center), 400, 400);
        mNavi->setMovable(false);
        mNavi->hide();
        mNavi->setMask("uioptions.png");
        mNavi->setOpacity(0.75f);
        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_Options::onLoaded));
        mNavi->bind("ok", NaviDelegate(this, &GUI_Options::onOk));
        mNavi->bind("back", NaviDelegate(this, &GUI_Options::onBack));
        m_curState = NSCreated;
    }

    // Set next Navi UI
    NavigatorGUI::setCurrentPanel(this);

    return true;
}
//-------------------------------------------------------------------------------------
void GUI_Options::onLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    char txt[256];

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Options::onLoaded()");

#ifdef NULLCLIENTSERVER
    mNavi->evaluateJS("showDiv('peerNCSMessage');");
    mNavi->evaluateJS("hideDiv('peerConfig');");
#endif

    // Set current values
    mNavi->evaluateJS("setInputState('checkboxGeneralResetDisplayConfig', null, null)");

    sprintf(txt, "setInputState('castShadow', %s,  %s)", "null", mNavigator->getCastShadows() ? "'checked'" : "null");
    mNavi->evaluateJS(txt);

    bool facebookAvailable = (
        !mNavigator->getFacebookApiKey().empty() &&
        !mNavigator->getFacebookSecret().empty() &&
        !mNavigator->getFacebookServer().empty() &&
        !mNavigator->getFacebookLoginUrl().empty());

    sprintf(txt, "setInputState('radioIdAuthentTypeFacebook', %s, %s)", !facebookAvailable ? "'disabled'" : "null", (mNavigator->getAuthentType() == ATFacebook) ? "'checked'" : "null");
    mNavi->evaluateJS(txt);

    sprintf(txt, "setInputState('radioIdAuthentTypeSolipsis', %s, %s)", mNavigator->getWorldsServerAddress().empty() ? "'disabled'" : "null", (mNavigator->getAuthentType() == ATSolipsis) ? "'checked'" : "null");
    mNavi->evaluateJS(txt);

    //      sprintf(txt, "setInputState('radioIdAuthentTypeFixed', %s, %s)", mNavigator->getFixedNodeId().empty() ? "'visible'" : "null", (mNavigator->getAuthentType() == ATFixed) ? "'checked'" : "null");
    //      mNavi->evaluateJS(txt);

    std::string wsHost, wsPort;
    CommonTools::StringHelpers::getURLHostPort(mNavigator->getWorldsServerAddress(), wsHost, wsPort);

    sprintf(txt, "setInputText('inputWSHost', '%s');", wsHost.c_str());
    mNavi->evaluateJS(txt); 

    sprintf(txt, "setInputText('inputWSPort', '%s');", wsPort.c_str());
    mNavi->evaluateJS(txt);

    std::string peerHost, peerPort;
    CommonTools::StringHelpers::getURLHostPort(mNavigator->getPeerAddress(), peerHost, peerPort);

    sprintf(txt, "$('inputPeerHost').value = '%s'", peerHost.c_str());
    mNavi->evaluateJS(txt);

    sprintf(txt, "$('inputPeerPort').value = '%s'", peerPort.c_str());
    mNavi->evaluateJS(txt);
    mNavi->evaluateJS("$('infosText').innerHTML = ''");

    std::string proxyAutoconfUrl;
    std::string proxyHttpHost;
//    int proxyHttpPort;
//    int proxyType;
 /*   if (mNaviMgr->getProxyConfig(proxyType, proxyHttpHost, proxyHttpPort, proxyAutoconfUrl))
    {
        sprintf(txt, "setInputState('radioProxyTypeDirect', null, %s)", (proxyType == 0) ? "'checked'" : "null");
        mNavi->evaluateJS(txt);
        sprintf(txt, "setInputState('radioProxyTypeAutodetect', null, %s)", (proxyType == 4) ? "'checked'" : "null");
        mNavi->evaluateJS(txt);
        sprintf(txt, "setInputState('radioProxyTypeManual', null, %s)", (proxyType == 1) ? "'checked'" : "null");
        mNavi->evaluateJS(txt);
        sprintf(txt, "setInputState('radioProxyTypeAutoconf', null, %s)", (proxyType == 2) ? "'checked'" : "null");
        mNavi->evaluateJS(txt);
        sprintf(txt, "$('inputProxyHttpHost').value = '%s'", proxyHttpHost.c_str());
        mNavi->evaluateJS(txt);
        sprintf(txt, "$('inputProxyHttpPort').value = '%d'", proxyHttpPort);
        mNavi->evaluateJS(txt);
        sprintf(txt, "$('inputProxyAutoconfUrl').value = '%s'", proxyAutoconfUrl.c_str());
        mNavi->evaluateJS(txt);
    }*/

    Navigator::NavigationInterface ni = mNavigator->getNavigationInterface();
    sprintf(txt, "setInputState('radioControlKeyboardAndMouse', null, %s)", (ni==Navigator::NIMouseKeyboard) ? "'checked'" : "null");
    mNavi->evaluateJS(txt);

    sprintf(txt, "setInputState('radioControlWiimoteAndNunchuk', null, %s)", (ni==Navigator::NIWiimoteNunchuk) ? "'checked'" : "null");
    mNavi->evaluateJS(txt);

    sprintf(txt, "setInputState('radioControlWiimoteAndNunchukAndIR', null, %s)", (ni==Navigator::NIWiimoteNunchukIR) ? "'checked'" : "null");
    mNavi->evaluateJS(txt);

    // Show Navi UI options
    if (m_curState == NSCreated)
        mNavi->show(true);
}

//-------------------------------------------------------------------------------------
void GUI_Options::onOk(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::optionsOk()");

    // Get options
    bool generalResetDisplayConfig = args.at(0).toBoolean();
    mNavigator->setCastShadows(args.at(1).toBoolean());

    std::string radioIdAuthentType;
    radioIdAuthentType = args.at(2).toString();
    std::string wsHost;
    unsigned short wsPort;
    wsHost = args.at(3).toString();
    wsPort = args.at(4).toInteger();
    std::string peerHost;
    unsigned short peerPort;
    peerHost = args.at(5).toString();
    peerPort = args.at(6).toInteger();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "peerHost=%s, peerPort=%d", peerHost.c_str(), peerPort);
    std::string radioProxyType;
    std::string proxyHttpHost;
    std::string radioControlType;
    int proxyHttpPort;
    std::string proxyAutoconfUrl;
    radioProxyType = args.at(7).toString();
    proxyHttpHost = args.at(8).toString();
    proxyHttpPort = args.at(9).toInteger();
    proxyAutoconfUrl = args.at(10).toString();
    radioControlType =  args.at(11).toString();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "radioProxyType=%s, proxyHttpHost=%s, proxyHttpPort=%d, proxyAutoconfUrl=%s, controlType=%s", radioProxyType.c_str(), proxyHttpHost.c_str(), proxyHttpPort, proxyAutoconfUrl.c_str(), radioControlType.c_str());

    // Check
    AuthentType authentType = (AuthentType)radioIdAuthentType.c_str()[0];
    bool valid_options = true;
    if (wsHost.length() < 2)
    {
        // Bad hostname
        mNavi->evaluateJS("$('infosText').innerHTML = 'Enter a valid server hostname ...'");
        valid_options = false;
    }
    if (wsPort < 0)
    {
        // Bad port
        mNavi->evaluateJS("$('infosText').innerHTML = 'Enter a valid server port ...'");
        valid_options = false;
    }
    if (peerHost.length() < 2)
    {
        // Bad hostname
        mNavi->evaluateJS("$('infosText').innerHTML = 'Enter a valid peer hostname ...'");
        valid_options = false;
    }
    if (peerPort < 0)
    {
        // Bad port
        mNavi->evaluateJS("$('infosText').innerHTML = 'Enter a valid peer port ...'");
        valid_options = false;
    }
    int proxyType;
    if (radioProxyType == "direct")
    {
        proxyType = 0;
        proxyAutoconfUrl = "";
        proxyHttpHost = "";
        proxyHttpPort = 0;
    }
    else if (radioProxyType == "autodetect")
    {
        proxyType = 4;
        proxyAutoconfUrl = "";
        proxyHttpHost = "";
        proxyHttpPort = 0;
    }
    else if (radioProxyType == "manual")
    {
        proxyType = 1;
        proxyAutoconfUrl = "";
        if (proxyHttpHost.length() == 0)
        {
            // Bad url
            mNavi->evaluateJS("$('infosText').innerHTML = 'Enter a valid HTTP proxy Address ...'");
            valid_options = false;
        }
        if (proxyHttpPort < 0)
        {
            // Bad port
            mNavi->evaluateJS("$('infosText').innerHTML = 'Enter a valid HTTP proxy Port ...'");
            valid_options = false;
        }
    }
    else // autoconf
    {
        proxyType = 2;
        if (proxyAutoconfUrl.length() == 0)
        {
            // Bad url
            mNavi->evaluateJS("$('infosText').innerHTML = 'Enter a valid proxy server URL ...'");
            valid_options = false;
        }
    }

    Navigator::NavigationInterface controlType = Navigator::NIMouseKeyboard;
    if (radioControlType == "KeyboardAndMouse")
    {
        controlType = Navigator::NIMouseKeyboard;
    }
    else if (radioControlType == "WiimoteAndNunchuk")
    {
        controlType = Navigator::NIWiimoteNunchuk;
    }
    else if (radioControlType == "WiimoteAndNunchukAndIR")
    {
        controlType = Navigator::NIWiimoteNunchukIR;
    }

    // Valid options ?
    if (valid_options)
    {
        if (generalResetDisplayConfig)
        {
            mNavigator->getOgreApplication()->resetDisplayConfig();
        }
        if (authentType != mNavigator->getAuthentType())
        {
            mNavigator->setNodeId("");
            // reset password
            mNavigator->setPwd("", true);
        }

        mNavigator->setAuthentType(authentType);
        mNavigator->setWorldsServerAddress(CommonTools::StringHelpers::getURL(wsHost, wsPort));
        mNavigator->setPeerAddress(CommonTools::StringHelpers::getURL(peerHost, peerPort));

        // saved in Navi manager
  //      mNaviMgr->setProxyConfig(proxyType, proxyHttpHost, proxyHttpPort, proxyAutoconfUrl);

        mNavigator->setNavigationInterface(controlType);

        // empty info text
        mNavi->evaluateJS("$('infosText').innerHTML = ''");

        mNavigator->saveConfiguration();

        // Return to Navi UI login
        GUI_Login::createAndShowPanel();
    }
}

//-------------------------------------------------------------------------------------
void GUI_Options::onBack(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::optionsBack()");

    // Return to Navi UI login
    GUI_Login::createAndShowPanel();
}


