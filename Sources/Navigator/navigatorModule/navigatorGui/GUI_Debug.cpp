7/*
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

#include "GUI_Debug.h"

#ifdef UIDEBUG

#include <CTLog.h>
#include <CTStringHelpers.h>
#include <Navi.h>
#include "Tools/DebugHelpers.h"

using namespace Solipsis;
using namespace CommonTools;
GUI_Debug * GUI_Debug::stGUI_Debug = NULL;

//-------------------------------------------------------------------------------------
GUI_Debug::GUI_Debug() : GUI_Panel("uidebug")
{
    stGUI_Debug = this;

    mNavigator = Navigator::getSingletonPtr();
    mNaviMgr = NavigatorGUI::getNaviMgr();
}

//-------------------------------------------------------------------------------------
/*static*/ void GUI_Debug::switchPanel()
{
    if (!stGUI_Debug)
    {
        new GUI_Debug();
        stGUI_Debug->show();
    }
    else
    {
        if (stGUI_Debug->isVisible())
        {
            stGUI_Debug->destroy();
        }
        else
        {
            stGUI_Debug->show();
        }
    }
}

//-------------------------------------------------------------------------------------
/*static*/ void GUI_Debug::setTreeDirty(bool dirty)
{
    if (!stGUI_Debug) return;

    stGUI_Debug->mTreeDirty = dirty;
}


//-------------------------------------------------------------------------------------
bool GUI_Debug::show()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Debug::show()");

    if (m_curState == NSNotCreated)
    {
        // Create Navi UI debug
        createNavi(NaviPosition(TopRight), 300, 256);
        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_Debug::onPageLoaded));
        mNavi->loadFile("uidebug.html");
        mNavi->setMovable(true);
        mNavi->setAutoUpdateOnFocus(true);
        mNavi->setMaxUPS(24);
        mNavi->hide();
        mNavi->setMask("uidebug.png");
        mNavi->setOpacity(0.50f);
        mNavi->bind("pageClosed", NaviDelegate(this, &GUI_Debug::onPageClosed));
        mNavi->bind("debugRefreshTree", NaviDelegate(this, &GUI_Debug::refreshTree));
        mNavi->bind("debugCommand", NaviDelegate(this, &GUI_Debug::debugCommand));
        mNavi->bind("navCommand", NaviDelegate(this, &GUI_Debug::navCommand));
        m_curState = NSCreated;
        mTreeDirty = true;
    }

    // Set next Navi UI
    NavigatorGUI::setCurrentPanel(this);

    return true;
}


//-------------------------------------------------------------------------------------
void GUI_Debug::refreshUrl()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::debugRefreshUrl()");

    if (!stGUI_Debug) return;
    if (stGUI_Debug->m_curState != NSCreated) return;

#ifdef DEMO_NAVI2
    NaviLibrary::Navi* naviDemoNavi2 = stGUI_Debug->mNaviMgr->getNavi("WWW_demoNavi2Video");
    if (naviDemoNavi2 == 0) return;
    // Set current url
    char txt[256];
    sprintf(txt, "$('inputUrl').value = '%s'", naviDemoNavi2->getCurrentLocation().c_str());
    stGUI_Debug->mNavi->evaluateJS(txt);
    // Activate/Deactivate Back/Forward buttons
    // Awesomium ?
    //sprintf(txt, "$('navBackButton').disabled = %s", naviDemoNavi2->canNavigateBack() ? "false" : "true");
    //stGUI_Debug->mNavi->evaluateJS(txt);
    //sprintf(txt, "$('navForwardButton').disabled = %s", naviDemoNavi2->canNavigateForward() ? "false" : "true");
    //stGUI_Debug->mNavi->evaluateJS(txt);
#endif
}

//-------------------------------------------------------------------------------------
void GUI_Debug::refreshDemoVoiceTalkButtonName()
{
    char txt[256];

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::debugRefreshDemoVoiceTalkButtonName()");

    if (!stGUI_Debug) return;
    if (stGUI_Debug->m_curState != NSCreated) return;

    stGUI_Debug->mNavi->evaluateJS("$('inputVoice').value = '" + stGUI_Debug->mNavigator->getVoIPServerAddress() + "'");
    sprintf(txt, "$('inputVoiceSilenceLvl').value = '%.2f'", stGUI_Debug->mNavigator->getVoIPSilenceLevel());
    stGUI_Debug->mNavi->evaluateJS(txt);
    sprintf(txt, "$('inputVoiceSilenceLat').value = '%d'", stGUI_Debug->mNavigator->getVoIPSilenceLatency());
    stGUI_Debug->mNavi->evaluateJS(txt);

    // get voice engine
    IVoiceEngine* voiceEngine = VoiceEngineManager::getSingleton().getSelectedEngine();
    if (voiceEngine == 0)
        return;

    sprintf(txt, "$('demoVoiceToggleTalkButton').innerHTML = '%s'", (voiceEngine->isRecording() ? "Stop" : "Start"));
    stGUI_Debug->mNavi->evaluateJS(txt);
}

//-------------------------------------------------------------------------------------
void GUI_Debug::onPageLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::debugPageLoaded()");

    // Refresh url
    refreshUrl();

    // Refresh tree datas
    refreshTree(caller, args);

    // Refresh voice engine state
    refreshDemoVoiceTalkButtonName();

    // Show Navi UI debug
    GUI_Panel::onPanelLoaded(caller, args);
}

//-------------------------------------------------------------------------------------
void GUI_Debug::onPageClosed(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::debugPageClosed()");

    switchPanel();
}

//-------------------------------------------------------------------------------------
void GUI_Debug::refreshTree(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::debugRefreshTree()");

    if (m_curState != NSCreated) return;
    if (!mTreeDirty) return;

    mNavi->evaluateJS("allTree.disable()");
    mNavi->evaluateJS("allTree.root.clear()");
    mNavi->evaluateJS("allTree.insert({text:'Scenes', id:'Scenes'})");
    String sceneName = mNavigator->getSceneMgrPtr()->getName();
    mNavi->evaluateJS("allTree.get('Scenes').insert({text:'" + sceneName + "', id:'S_" + sceneName + "'})");
    mNavi->evaluateJS("allTree.insert({text:'OgrePeers', id:'OgrePeers'})");
    for (OgrePeerManager::OgrePeersMap::iterator ogrePeer = mNavigator->getOgrePeerManager()->getOgrePeersIteratorBegin();ogrePeer != mNavigator->getOgrePeerManager()->getOgrePeersIteratorEnd();ogrePeer++)
    {
        String ogrePeerName = ogrePeer->second->getXmlEntity()->getName();
        mNavi->evaluateJS("allTree.get('OgrePeers').insert({text:'" + ogrePeerName + "', id:'OP_" + ogrePeerName + "'})");
    }
    mNavi->evaluateJS("allTree.enable()");

    mTreeDirty = false;
}

//-------------------------------------------------------------------------------------
void GUI_Debug::debugCommand(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::debugCommand()");

    // Get message to send
    std::string cmd = args[0].toString();
    std::string params = args[1].toString();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "cmd=%s, params=%s", cmd.c_str(), params.c_str());

    // Push debug command
    DebugHelpers::debugCommands[String(cmd)] = String(params);
}

//-------------------------------------------------------------------------------------
void GUI_Debug::navCommand(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::navCommand()");

    // Get command
    std::string cmd = args[0].toString();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "cmd=%s", cmd.c_str());

#ifdef DEMO_NAVI2
    NaviLibrary::Navi* naviDemoNavi2 = mNaviMgr->getNavi("WWW_demoNavi2Video");
    if (naviDemoNavi2 == 0) return;
    // Awesomium ?
    //if (cmd == "back")
    //    naviDemoNavi2->navigateBack();
    //else if (cmd == "forward")
    //    naviDemoNavi2->navigateForward();
    //else if (cmd == "stop")
    //    naviDemoNavi2->navigateStop();
    //else if (cmd == "go")
    if (cmd == "go")
    {
        std::string url = mNavi->evaluateJSWithResult("$('inputUrl').value").get().toString();
        naviDemoNavi2->loadURL(url);
    }
#endif
}

//-------------------------------------------------------------------------------------

#endif


