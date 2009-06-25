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

#include <CTIO.h>
#include <Navi.h>

#ifdef _MSC_VER
 #pragma warning (disable:4355)
#endif // _MSC_VER

using namespace Solipsis;
using namespace CommonTools;

NavigatorGUI * NavigatorGUI::mNaviGui = NULL;

//-------------------------------------------------------------------------------------
NavigatorGUI::NavigatorGUI(Navigator* navigator) :
    mNavigator(navigator),
    mLoginInfosText(""),
    m_pCurrentPanel(NULL)
{
    // Initializing Navi
    mNaviMgr = new NaviLibrary::NaviManager(mNavigator->getRenderWindowPtr(), "NaviLocal", ".");
    mNaviMgr->setZOrderMinMax(200, 299);

    // Initializing 2D Panels manager
    mPanel2DMgr = new Panel2DMgr();
    mPanel2DMgr->setZOrderMinMax(100, 199);

    // Add ourself as a Window listener
    WindowEventUtilities::addWindowEventListener(mNavigator->getRenderWindowPtr(), this);

    mNaviGui = this;
}

//-------------------------------------------------------------------------------------
NavigatorGUI::~NavigatorGUI()
{
    // Remove ourself as a Window listener
    WindowEventUtilities::removeWindowEventListener(mNavigator->getRenderWindowPtr(), this);

    // Finalizing 2D Panels manager
    delete mPanel2DMgr;

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

    mPanel2DMgr->initializeMouseCursors();

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

//-------------------------------------------------------------------------------------
void NavigatorGUI::ConnectionServerErrorMsgBoxResponse::onResponse(const std::string& response)
{
    Navigator::getSingletonPtr()->disconnect(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::connectionServerError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::connectionError()");

    GUI_MessageBox::getMsgBox()->show("Network error",
        "Connection to server error...", 
        GUI_MessageBox::MBB_OK, 
        GUI_MessageBox::MBB_EXCLAMATION,
        &connectionServerErrorMsgBoxResponse);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::connectionLostError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::connectionLostError()");

    GUI_MessageBox::getMsgBox()->show("Network error",
        "Peer lost its connection, re-connection in progress ...", 
        GUI_MessageBox::MBB_OK, 
        GUI_MessageBox::MBB_EXCLAMATION);
}

//-------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
bool NavigatorGUI::setNaviVisibility(const String& naviName, bool show)
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(naviName);
    if (navi == 0) 
        return false;

    if (show)
    {
        if (navi->getVisibility()) 
            return true;

        navi->show(true);
    }
    else
    {
        if (!navi->getVisibility()) 
            return true;

        navi->hide(true);
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::createPanel2D(const String& type, const String& name)
{
    Panel2D *newPanel = mPanel2DMgr->createPanel(type, name);
    mPanel2DMgr->focusPanel(0, 0, newPanel);
    return true;
}

void NavigatorGUI::registerGuiPanel(GUI_Panel *pPanel)
{
    mNaviGui->m_panels[pPanel->getPanelName()] = pPanel;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::unregisterGuiPanel(GUI_Panel *pPanel)
{
    mNaviGui->m_panels.erase(pPanel->getPanelName());//  .remove(pPanel->getPanelName());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::destroyAllRegisteredPanels()
{
    for (std::map<std::string, GUI_Panel *>::iterator it = mNaviGui->m_panels.begin(); it != mNaviGui->m_panels.end(); it++)
    {
        it->second->destroy();
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::destroyCurrentPanel()
{
    if (mNaviGui->m_pCurrentPanel)
    {
        mNaviGui->m_pCurrentPanel->destroy();
    }
}

//-------------------------------------------------------------------------------------
