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

#include "GUI_AuthentFacebook.h"
#include <CTStringHelpers.h>
#include <CTSystem.h>

#include <Navi.h>

#include "GUI_Login.h"
#include "GUI_MessageBox.h"

#include <Facebook.h>


using namespace Solipsis;
using namespace CommonTools;
//-------------------------------------------------------------------------------------

GUI_AuthentFacebook * GUI_AuthentFacebook::stGUI_AuthentFacebook = NULL;

GUI_AuthentFacebook::GUI_AuthentFacebook() : GUI_FromServer("uiauthentfb")
{
    stGUI_AuthentFacebook = this;
    mFacebook = 0;
}

bool GUI_AuthentFacebook::createAndShowPanel()
{
    if (!stGUI_AuthentFacebook)
    {
        new GUI_AuthentFacebook();
    }

    return stGUI_AuthentFacebook->show();
}

//-------------------------------------------------------------------------------------
bool GUI_AuthentFacebook::show()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AuthentFacebook::show()");

    // Hide previous Navi UI
    NavigatorGUI::destroyCurrentPanel();

    // Create the Facebook instance
    if (mFacebook != 0)
        delete mFacebook;

    mFacebook = new Facebook(mNavigator->getFacebookApiKey(), mNavigator->getFacebookSecret(), mNavigator->getFacebookServer());
 
    // Grab a token from the server.
    if ((mFacebook == 0) || !mFacebook->authenticate())
    {
        // Destroy Facebook instance
        if (mFacebook != 0)
        {
            delete mFacebook;
            mFacebook = 0;
        }
        onError();
        return false;
    }

    if (m_curState == NSNotCreated)
    {
        // Create Navi UI authentication on Facebook
        createNavi( "local://uiauthentfb.html", NaviPosition(Center), 256, 128);
        mNavi->setMovable(false);
        mNavi->hide();
        mNavi->setOpacity(0.75f);
        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_AuthentFacebook::onLoaded));
        mNavi->bind("ok", NaviDelegate(this, &GUI_AuthentFacebook::onOk));
        mNavi->bind("cancel", NaviDelegate(this, &GUI_AuthentFacebook::onCancel));
        m_curState = NSCreated;
    }

    // Set next Navi UI
    NavigatorGUI::setCurrentPanel(this);

    return true;

}

//-------------------------------------------------------------------------------------
void GUI_AuthentFacebook::onError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentFacebookError()");

    GUI_MessageBox::getMsgBox()->show("Network error", "Unable to connect to Facebook !<br/>Check your Internet connection.", 
        GUI_MessageBox::MBB_OK, 
        GUI_MessageBox::MBB_ERROR);

    GUI_Login::createAndShowPanel();
}

//-------------------------------------------------------------------------------------
void GUI_AuthentFacebook::onLoaded(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentFacebookPageLoaded()");

    mNavi->evaluateJS("$('msgText').innerHTML = 'Use browser to log on Facebook ... then press Ok'");
    // Run external Web browser on the login URL
    CommonTools::System::runExternalWebBrowser(mFacebook->getLoginUrl(mNavigator->getFacebookLoginUrl()).c_str());

    // Show Navi UI authent Facebook
    if (m_curState == NSCreated)
        mNavi->show(true);
}

//-------------------------------------------------------------------------------------
void GUI_AuthentFacebook::onOk(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentFacebookOk()");

    // Session ?
    if (!mFacebook->getSession())
    {
        mNavi->evaluateJS("$('msgText').innerHTML = 'Unable to get session ... Are you logged ?'");
        return;
    }
    // Get uid
    NodeId nodeId = mFacebook->getUid();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentFacebookOk() nodeId=%s", nodeId.c_str());
    mNavigator->setNodeId(XmlHelpers::convertAuthentTypeToRepr(ATFacebook) + nodeId);

    // Destroy Facebook instance
    if (mFacebook != 0)
    {
        delete mFacebook;
        mFacebook = 0;
    }

    // Call connect
    bool connected = mNavigator->connect();
}

//-------------------------------------------------------------------------------------
void GUI_AuthentFacebook::onCancel(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentFacebookCancel()");

    // Destroy Facebook instance
    if (mFacebook != 0)
    {
        delete mFacebook;
        mFacebook = 0;
    }

    // Return to Navi UI login
    GUI_Login::createAndShowPanel();
}