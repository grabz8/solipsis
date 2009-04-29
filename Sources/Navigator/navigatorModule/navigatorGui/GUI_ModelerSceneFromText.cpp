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

#include "GUI_ModelerSceneFromText.h"
#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>
#include <CTNetSocket.h>

using namespace Solipsis;
using namespace CommonTools;
//-------------------------------------------------------------------------------------

GUI_ModelerSceneFromText * GUI_ModelerSceneFromText::stGUI_ModelerSceneFromText = NULL;

GUI_ModelerSceneFromText::GUI_ModelerSceneFromText() : GUI_Panel("uimdlrscenefromtext")
{
    stGUI_ModelerSceneFromText = this;
}

void GUI_ModelerSceneFromText::showPanel()
{
    if (!stGUI_ModelerSceneFromText)
    {
        new GUI_ModelerSceneFromText();
    }

    stGUI_ModelerSceneFromText->show();
}

/*static*/ void GUI_ModelerSceneFromText::unloadPanel()
{
    if (!stGUI_ModelerSceneFromText)
    {
        return;
    }

    stGUI_ModelerSceneFromText->destroy();
}

bool GUI_ModelerSceneFromText::show()
{
    if (m_curState == NSNotCreated)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_ModelerSceneFromText::modelerSceneFromTextShow()");

        // Reset the remoteMRL on local IP address with UDP
        CommonTools::NetSocket::IPAddressVector myIPAddresses;
        if (!CommonTools::NetSocket::getMyIP(myIPAddresses))
            myIPAddresses.push_back("");
        std::string firstLocalIP = myIPAddresses.front();

        // Create Navi UI modeler
        createNavi( "local://uimdlrscenefromtext.html" /*?localIP=" + firstLocalIP*/, NaviPosition(TopRight), 512, 512);
        mNavi->setMovable(true);
        mNavi->hide();
        mNavi->setMask("uimdlrscenefromtext.png");//Eliminate the black shadow at the margin of the menu
        //mNavi->setOpacity(0.75f);

        // page loaded
        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_ModelerSceneFromText::onPageLoaded));
        mNavi->bind("MdlrSFTCreate", NaviDelegate(this, &GUI_ModelerSceneFromText::onExec));
        mNavi->bind("MdlrSFTCancel", NaviDelegate(this, &GUI_ModelerSceneFromText::onCancelled));

        m_curState = NSCreated;

    }
    else {
        mNavi->show(true);
    }

    return true;
}

//-------------------------------------------------------------------------------------
void GUI_ModelerSceneFromText::onPageLoaded(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_ModelerSceneFromText::modelerSceneFromTextPageLoaded()");

    // Show Navi UI
    if (m_curState == NSCreated)
        mNavi->show(true);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerSceneFromText::onExec(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_ModelerSceneFromText::modelerSceneFromTextExec()");

    std::string value = mNavi->evaluateJS("document.getElementById('scenedescription').value");

    //modelerSceneSetUpUnload(); // no unloading of the window unless the user explicitely closes it.
    std::string errMsg( "" );
    std::string warnMsg( "" );
#ifdef DECLARATIVE_MODELER
    if( !mNavigator->createSceneFromText( value, errMsg, warnMsg ) )
        if( errMsg != "" )
            GUI_MessageBox::getMsgBox()->show( "Declarative modeling error", "Current text is:<br/>'" + value + "'<br/>" + errMsg.c_str() , MBB_OK, MBB_ERROR );
        else if( warnMsg != "" )
            GUI_MessageBox::getMsgBox()->show( "Declarative modeling error", "Current text is:<br/>'" + value + "'<br/>" + warnMsg.c_str() , MBB_OK, MBB_ERROR );
        else 
            GUI_MessageBox::getMsgBox()->show( "Declarative modeling error", "Current text is:<br/>'" + value + "'<br/> UNKNOWN ERROR", MBB_OK, MBB_ERROR );
#endif
}

//-------------------------------------------------------------------------------------
void GUI_ModelerSceneFromText::onCancelled(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_ModelerSceneFromText::modelerSceneFromTextSetUpCancelled()");
    destroy();
}

