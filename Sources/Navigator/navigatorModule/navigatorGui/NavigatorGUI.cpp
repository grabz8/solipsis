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

#include "NavigatorGUI.h"
#include "GUI_MessageBox.h"
#include "GUI_StatusBar.h"
#include "GUI_MainMenu.h"

#include "MainApplication/Navigator.h"
#include "MainApplication/NavigatorFrameListener.h"
#include "Tools/DebugHelpers.h"
#include <OgreTimer.h>
#include <CTLog.h>
#include <CTIO.h>
#include <CTStringHelpers.h>
#include <CTNetSocket.h>
#include <CTSystem.h>
#include <Navi.h>
#include "World/Modeler.h"

#include <VoiceEngineManager.h>
#include "World/Avatar.h"

#ifdef _MSC_VER
 #pragma warning (disable:4355)
#endif // _MSC_VER

using namespace Solipsis;
using namespace CommonTools;

NavigatorGUI * NavigatorGUI::mNaviGui = NULL;

const std::string NavigatorGUI::ms_NavisNames[] = {

    "fake"

#ifdef DECLARATIVE_MODELER
   "uimdlrscenefromtext",
#endif
};

//-------------------------------------------------------------------------------------
NavigatorGUI::NavigatorGUI(Navigator* navigator) :
    mNavigator(navigator),
    mCurrentNavi(-1),
    mCurrentCtxtPanel(-1),
    mCurrentNaviCreationDate(0),
    mLoginInfosText(""),
    m_pCurrentPanel(NULL)
{
    // Initializing Navi
    mNaviMgr = new NaviLibrary::NaviManager(mNavigator->getRenderWindowPtr(), "NaviLocal", ".");

    // Add ourself as a Window listener
    WindowEventUtilities::addWindowEventListener(mNavigator->getRenderWindowPtr(), this);

    for (int n=0;n<NAVI_COUNT;n++)
        mNavisStates[n] = NSNotCreated;

    mNaviGui = this;
}

//-------------------------------------------------------------------------------------
NavigatorGUI::~NavigatorGUI()
{
    // Remove ourself as a Window listener
    WindowEventUtilities::removeWindowEventListener(mNavigator->getRenderWindowPtr(), this);

    // Hide previous Navi UI
    hidePreviousNavi();

    for (std::map<std::string, GUI_Panel *>::iterator it = m_panels.begin(); 
        it != m_panels.end(); 
        it = m_panels.begin())
    {
        GUI_Panel * pPanel = it->second;
        pPanel->destroy();
        // panel deletion will remove it from the list
        delete pPanel;
    }

    // Finalizing Navi
    delete mNaviMgr;
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::startup()
{
    // Startup NaviMouse and create the cursors
    NaviMouse* mouse = new NaviMouse();
    NaviCursor* defaultCursor = mouse->createCursor("default_cursor", 1, 0);
	defaultCursor->addFrame(1200, "cursor1.png")->addFrame(100, "cursor2.png")->addFrame(100, "cursor3.png")->addFrame(100, "cursor4.png");
	defaultCursor->addFrame(100, "cursor5.png")->addFrame(100, "cursor6.png")->addFrame(100, "cursor5.png")->addFrame(100, "cursor4.png");
	defaultCursor->addFrame(100, "cursor3.png")->addFrame(100, "cursor2.png");
    mouse->setDefaultCursor("default_cursor");
	NaviCursor* moveCursor = mouse->createCursor("move", 19, 19);
	moveCursor->addFrame(0, "cursorMove.png");

    // Load Lua default GUI
    lua_State* luaState = mNavigator->getLuaState();
    std::string defaultGUIluaFilename = std::string("lua") + IO::getPathSeparator() + "defaultGUI.lua";
    if (luaL_loadfile(luaState, defaultGUIluaFilename.c_str()) != 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorGUI::startup() Unable to load defaultGUI.lua, error: %s", lua_tostring(luaState, -1));
        return false;
    }
    if (lua_pcall(luaState, 0, LUA_MULTRET, 0))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorGUI::startup() Unable to run defaultGUI.lua, error: %s", lua_tostring(luaState, -1));
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::update()
{
    unsigned long now = Ogre::Root::getSingleton().getTimer()->getMilliseconds();

    if (m_pCurrentPanel)
    {
        m_pCurrentPanel->update();
    }

    GUI_StatusBar::updateBar();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::windowResized(RenderWindow* rw)
{
    mNaviMgr->resetAllPositions();

    // send new position to all registered panels
    for (std::map<std::string, GUI_Panel *>::iterator it = mNaviGui->m_panels.begin(); it != mNaviGui->m_panels.end(); it++)
    {
        it->second->windowResized(rw);
    }
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
void NavigatorGUI::inWorld()
{
    destroyAllRegisteredPanels();

    // show menu and status bar
    GUI_MainMenu::createAndShowPanel();
    GUI_StatusBar::createAndShowPanel();
}

 void NavigatorGUI::connectionServerError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::connectionError()");
    Navigator::getSingletonPtr()->disconnect();

    GUI_MessageBox::getMsgBox()->show("Network error", "Connection to server error...", 
        GUI_MessageBox::MBB_OK, 
        GUI_MessageBox::MBB_EXCLAMATION);

}

//-------------------------------------------------------------------------------------
void NavigatorGUI::connectionLostError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::connectionLostError()");

    GUI_MessageBox::getMsgBox()->show(
        "Network error", "Peer lost its connection, re-connection in progress ...", 
        GUI_MessageBox::MBB_OK, 
        GUI_MessageBox::MBB_EXCLAMATION);
}

//-------------------------------------------------------------------------------------
#ifdef UIDEBUG

#endif
//-------------------------------------------------------------------------------------
NavigatorGUI::NaviPanel NavigatorGUI::getNaviPanel(const std::string& naviName)
{
    for (int n=0; n < NAVI_COUNT; ++n)
        if (ms_NavisNames[n] == naviName) return (NaviPanel)n;

    return (NaviPanel)-1;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::naviToShowPageLoaded(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::naviToShowPageLoaded()");

    std::string naviName;
    naviName = naviData["naviName"].str();
    NaviPanel naviPanel = getNaviPanel(naviName);
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "naviName=%s, naviPanel=%d", naviName.c_str(), naviPanel);

    // Show Navi UI
    if (mNavisStates[naviPanel] == NSCreated)
    {
        Navi* navi = mNaviMgr->getNavi(ms_NavisNames[naviPanel]);
        navi->show(true);
        navi->focus();
    }

    mCurrentNaviCreationDate = 0;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::hidePreviousNavi()
{
    // Hide previous Navi UI
    if (mCurrentNavi != -1) 
    {
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[mCurrentNavi]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[mCurrentNavi] = NSNotCreated;
        mCurrentNavi = -1;
        mCurrentNaviCreationDate = 0;
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::destroyNavi(NaviPanel naviPanel)
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[naviPanel]);
    if (navi == 0) return;
    mNaviMgr->destroyNavi(navi);
    mNavisStates[naviPanel] = NSNotCreated;
    if (mCurrentNavi == naviPanel) mCurrentNavi = -1;
    if (mCurrentCtxtPanel == naviPanel) mCurrentCtxtPanel = -1;
    if (mCurrentNavi == -1) mCurrentNaviCreationDate = 0;
}

//-------------------------------------------------------------------------------------
const std::string& NavigatorGUI::getNaviName(NaviPanel naviPanel)
{
    return ms_NavisNames[naviPanel];
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::setNaviVisibility(const std::string& naviName, bool show)
{
    // Hide 1 Navi UI
    NaviPanel panel = getNaviPanel(naviName);

    if (panel == -1) 
        return false;

    if (mNavisStates[panel] == NSNotCreated) 
        return false;

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(naviName);
    if (navi == 0) 
        return false;

    if (show)
    {
        if (navi->getVisibility()) 
            return true;

        navi->show(true);
        mCurrentNavi = panel;
    }
    else
    {
        if (!navi->getVisibility()) 
            return true;

        navi->hide(true);
        if (mCurrentNavi == panel) 
            mCurrentNavi = -1;

        if (mCurrentCtxtPanel == panel) 
            mCurrentCtxtPanel = -1;

        if (mCurrentNavi == -1) 
            mCurrentNaviCreationDate = 0;
    }

    return true;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::switchLuaNavi(NaviPanel naviPanel, bool createDestroy)
{
    if (mNavisStates[naviPanel] == NSNotCreated)
    {
        // Create Navi panel
        // Lua
        if (!mNavigator->getNavigatorLua()->call("createGUI", "%s", ms_NavisNames[naviPanel].c_str()))
        {
            LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorGUI::switchLuaNavi() Unable to create GUI called %s", ms_NavisNames[naviPanel].c_str());
            return;
        }
        mNavisStates[naviPanel] = NSCreated;
    }
    else
    {
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[naviPanel]);
        if (!navi->getVisibility())
            navi->show(true);
        else
        {
            if (!createDestroy)
            {
                navi->hide(true);
                NaviManager::Get().deFocusAllNavis();
            }
            else
            {
                mNaviMgr->destroyNavi(navi);
                mNavisStates[naviPanel] = NSNotCreated;

            }
        }
    }
}

//-------------------------------------------------------------------------------------
#ifdef DECLARATIVE_MODELER
void NavigatorGUI::modelerSceneFromTextShow()
{
	if (mNavisStates[NAVI_MODELERSCENEFROMTEXT] == NSNotCreated)
	{
		LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerSceneFromTextShow()");
    
		// Reset the remoteMRL on local IP address with UDP
        CommonTools::NetSocket::IPAddressVector myIPAddresses;
        if (!CommonTools::NetSocket::getMyIP(myIPAddresses))
            myIPAddresses.push_back("");
        std::string firstLocalIP = myIPAddresses.front();

		// Create Navi UI modeler
		NaviLibrary::Navi* navi = mNaviMgr->createNavi(ms_NavisNames[NAVI_MODELERSCENEFROMTEXT], "local://uimdlrscenefromtext.html" /*?localIP=" + firstLocalIP*/, NaviPosition(TopRight), 512, 512);
		navi->setMovable(true);
		navi->hide();
		navi->setMask("uimdlrscenefromtext.png");//Eliminate the black shadow at the margin of the menu
		//navi->setOpacity(0.75f);

		// page loaded
		navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::modelerSceneFromTextPageLoaded));

		navi->bind("MdlrSFTCreate", NaviDelegate(this, &NavigatorGUI::modelerSceneFromTextExec));
		navi->bind("MdlrSFTCancel", NaviDelegate(this, &NavigatorGUI::modelerSceneFromTextCancelled));
		
		mNavisStates[NAVI_MODELERSCENEFROMTEXT] = NSCreated;

	}
	else {
		mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERSCENEFROMTEXT])->show(true);
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerSceneFromTextPageLoaded(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerSceneFromTextPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERSCENEFROMTEXT]);

    // Show Navi UI
    if (mNavisStates[NAVI_MODELERSCENEFROMTEXT] == NSCreated)
        navi->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerSceneFromTextExec(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerSceneFromTextExec()");
	
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERSCENEFROMTEXT]);

	std::string value = navi->evaluateJS("document.getElementById('scenedescription').value");

	//modelerSceneSetUpUnload(); // no unloading of the window unless the user explicitely closes it.
	std::string errMsg( "" );
	std::string warnMsg( "" );
	if( !mNavigator->createSceneFromText( value, errMsg, warnMsg ) )
		if( errMsg != "" )
			GUI_MessageBox::getMsgBox()->show( "Declarative modeling error", "Current text is:<br/>'" + value + "'<br/>" + errMsg.c_str() , MBB_OK, MBB_ERROR );
		else if( warnMsg != "" )
			GUI_MessageBox::getMsgBox()->show( "Declarative modeling error", "Current text is:<br/>'" + value + "'<br/>" + warnMsg.c_str() , MBB_OK, MBB_ERROR );
		else 
			GUI_MessageBox::getMsgBox()->show( "Declarative modeling error", "Current text is:<br/>'" + value + "'<br/> UNKNOWN ERROR", MBB_OK, MBB_ERROR );
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerSceneFromTextCancelled(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerSceneFromTextSetUpCancelled()");
	modelerSceneFromTextUnload();
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isModelerSceneFromTextVisible()
{
    return false;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerSceneFromTextHide()
{
    if (!isModelerPropVisible()) return;
    mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERSCENEFROMTEXT])->hide();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerSceneFromTextUnload()
{
    if (mNavisStates[NAVI_MODELERSCENEFROMTEXT] != NSNotCreated)
	{
        // Destroy Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERSCENEFROMTEXT]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[NAVI_MODELERSCENEFROMTEXT] = NSNotCreated;
    }
}
#endif

void NavigatorGUI::registerGuiPanel(GUI_Panel *pPanel)
{
    mNaviGui->m_panels[pPanel->getPanelName()] = pPanel;
}

void NavigatorGUI::unregisterGuiPanel(GUI_Panel *pPanel)
{
    mNaviGui->m_panels.erase(pPanel->getPanelName());//  .remove(pPanel->getPanelName());
}

void NavigatorGUI::destroyAllRegisteredPanels()
{
    for (std::map<std::string, GUI_Panel *>::iterator it = mNaviGui->m_panels.begin(); it != mNaviGui->m_panels.end(); it++)
    {
        it->second->destroy();
    }
}

void NavigatorGUI::destroyCurrentPanel()
{
    if (mNaviGui->m_pCurrentPanel)
    {
        mNaviGui->m_pCurrentPanel->destroy();
    }
}