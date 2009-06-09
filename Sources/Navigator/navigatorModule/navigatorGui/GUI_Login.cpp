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

#include "GUI_Login.h"


#include <CTLog.h>
#include <CTStringHelpers.h>
#include <Navi.h>

#include "GUI_ChooseWorld.h"
#include "GUI_Options.h"
#include "GUI_AuthentFacebook.h"
#include "GUI_AuthentWorldServer.h"

using namespace Solipsis;
using namespace CommonTools;

GUI_Login * GUI_Login::stGUI_Login = NULL;

GUI_Login::GUI_Login() : GUI_Panel("uilogin")
{
    stGUI_Login = this;
    mNavigator = Navigator::getSingletonPtr();
}

//static 
bool GUI_Login::createAndShowPanel()
{
    if (!stGUI_Login)
    {
        new GUI_Login();
    }

    return stGUI_Login->show();
}


//-------------------------------------------------------------------------------------
bool GUI_Login::show()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Login::show()");
    NavigatorGUI::destroyAllRegisteredPanels();

    if (m_curState == NSNotCreated)
    {
        // Create Navi UI login
        createNavi(NaviPosition(Center), 400, 300);

        mNavi->loadFile("uilogin.html");

        if (!mNavi)
        {
            return false;
        }

        mNavi->setMovable(false);
        mNavi->setAutoUpdateOnFocus(true);
        mNavi->setMaxUPS(24);
        mNavi->hide();
        mNavi->setOpacity(1);
        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_Login::onPanelLoaded));
        mNavi->bind("world", NaviDelegate(this, &GUI_Login::onChooseWorld));
        mNavi->bind("connect", NaviDelegate(this, &GUI_Login::onConnect));
        mNavi->bind("options", NaviDelegate(this, &GUI_Login::onOptions));
        mNavi->bind("quit", NaviDelegate(this, &GUI_Login::onQuit));
        m_curState = NSCreated;
    }
    
    // Set next Navi UI
    NavigatorGUI::setCurrentPanel(this);

    return true;
}

//-------------------------------------------------------------------------------------
void GUI_Login::applyLoginDatas()
{
    if (mNavi == 0) 
        return;

    std::string login = mNavi->evaluateJSWithResult("$('inputLogin').value").get().toString();
    std::string pwd = mNavi->evaluateJSWithResult("$('inputPwd').value").get().toString();
    Navigator* pNavigator = Navigator::getSingletonPtr();
    if ((login != pNavigator->getLogin()) || (pwd != pNavigator->getPwd()))
        pNavigator->setNodeId("");

    bool rememberPassword = mNavi->evaluateJSWithResult("$('savePassWordCB').checked").get().toString() == "true";

    pNavigator->setLogin(login);
    pNavigator->setPwd(pwd, rememberPassword);

    pNavigator->saveConfiguration();
}

//-------------------------------------------------------------------------------------
void GUI_Login::onPanelLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    char txt[256];

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::loginPageLoaded()");

    if (mNavi == 0) 
        return;
    Navigator* pNavigator = Navigator::getSingletonPtr();

    // Set current values
    sprintf(txt, "$('inputLogin').value = '%s'", mNavigator->getLogin().c_str());
    mNavi->evaluateJS(txt);
    sprintf(txt, "$('inputPwd').value = '%s'", mNavigator->getPwd().c_str());
    mNavi->evaluateJS(txt);
    // Show/Hide password input
    sprintf(txt, "$('pwd').style.visibility = '%s'", (mNavigator->getAuthentType() == ATSolipsis) ? "visible" : "hidden");
    mNavi->evaluateJS(txt);
    std::string world = mNavigator->getWorldAddress();
    sprintf(txt, "$('worldText').innerHTML = '%s'", world.empty() ? "Choose a world ..." : world.c_str());
    mNavi->evaluateJS(txt);
    // Disable World button if no world server specified
    sprintf(txt, "$('worldButton').disabled = %s", mNavigator->getWorldsServerAddress().empty() ? "'disabled'" : "null");
    mNavi->evaluateJS(txt);

    if (!Navigator::getSingletonPtr()->getPwd().empty())
        mNavi->evaluateJS("$('savePassWordCB').checked = 'true'");

    // Show Navi UI login
    if (m_curState == NSCreated)
        mNavi->show(true);
}

void GUI_Login::onChooseWorld(Navi* caller, const Awesomium::JSArguments& args)
{
    applyLoginDatas();
    GUI_ChooseWorld::createAndShowPanel();
}

void GUI_Login::onOptions(Navi* caller, const Awesomium::JSArguments& args)
{
    applyLoginDatas();
    GUI_Options::createAndShowPanel();
}

//-------------------------------------------------------------------------------------
void GUI_Login::onConnect(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Login::onConnect()");

    applyLoginDatas();

    // Check
    bool validLogin = StringHelpers::isAValidLogin(mNavigator->getLogin());
    if (!validLogin)
    {
        // Malformed login
        mNavi->evaluateJS("$('infosText').innerHTML = 'Enter a valid login ...'");
        return;
    }
    if (mNavigator->getWorldAddress().empty())
    {
        // Malformed login
        mNavi->evaluateJS("$('infosText').innerHTML = 'Select a valid world ...'");
        return;
    }

    // Authentication
    if (mNavigator->getAuthentType() == ATFixed)
        mNavigator->setNodeId(XmlHelpers::convertAuthentTypeToRepr(ATFixed) + mNavigator->getFixedNodeId());
    if (mNavigator->getNodeId().empty())
    {
        switch (mNavigator->getAuthentType())
        {
        case ATFacebook:
            GUI_AuthentFacebook::createAndShowPanel();
            break;
        case ATSolipsis:
            bool validPwd = StringHelpers::isAValidPassword(mNavigator->getPwd());
            if (!validPwd)
            {
                // Malformed pwd
                mNavi->evaluateJS("$('infosText').innerHTML = 'Enter a valid password ...'");
                return;
            }
            GUI_AuthentWorldServer::createAndShowPanel(mNavigator->getPwd());
            break;
        }
    }
    else
    {
        // Call connect
        bool connected = mNavigator->connect();
    }
}


//-------------------------------------------------------------------------------------
void GUI_Login::onQuit(Navi* caller, const Awesomium::JSArguments& args)
{
    mNavigator->quit();
}