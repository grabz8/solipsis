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

#include "GUI_Avatar.h"
#include "GUI_MessageBox.h"
#include "GUI_AvatarProperties.h"
#include <AvatarEditor.h>
#include <CharacterManager.h>
#include <Character.h>
#include <CharacterInstance.h>


#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>

using namespace Solipsis;
using namespace CommonTools;

GUI_Avatar * GUI_Avatar::stGUI_Avatar = NULL;

GUI_Avatar::GUI_Avatar() : GUI_Panel("uiavatarmain")
{
    stGUI_Avatar = this;
    mNavigator = Navigator::getSingletonPtr();

}

/*static*/ bool GUI_Avatar::createAndShowPanel()
{
    if (!stGUI_Avatar)
    {
        new GUI_Avatar();
    }

    return stGUI_Avatar->show();
}

/*static*/ void GUI_Avatar::unload()
{
    if (!stGUI_Avatar)
    {
        return;
    } 

     stGUI_Avatar->destroy();
    return;
}  


/*static*/ bool GUI_Avatar::isPanelVisible()
{
    if (!stGUI_Avatar)
    {
        return false;
    } 

    return stGUI_Avatar->isVisible();
}  


//-------------------------------------------------------------------------------------
bool GUI_Avatar::show()
{
    if (GUI_AvatarProperties::isPanelVisible() == NSCreated)
        GUI_AvatarProperties::hidePanel();

    if (m_curState == NSNotCreated)
    {
        // Create Navi UI modeler
        createNavi( "local://uiavatarmain.html", NaviPosition(TopRight), 256, 512);

        mNavi->setMovable(true);
        mNavi->hide();
        mNavi->setMask("uiavatarmain.png");//Eliminate the black shadow at the margin of the menu
        mNavi->setOpacity(0.75f);

        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_Avatar::avatarMainPageLoaded));
        //		mNavi->bind("AvatarPrev", NaviDelegate(this, &GUI_Avatar::avatarMainSelectPrev));
        //		mNavi->bind("AvatarNext", NaviDelegate(this, &GUI_Avatar::avatarMainSelectNext));
        mNavi->bind("AvatarSelected", NaviDelegate(this, &GUI_Avatar::avatarMainSelected));
        mNavi->bind("FileOpen", NaviDelegate(this, &GUI_Avatar::avatarMainFileOpen));
        mNavi->bind("AvatarEdit", NaviDelegate(this, &GUI_Avatar::avatarMainFileEdit));
        mNavi->bind("FileSave", NaviDelegate(this, &GUI_Avatar::avatarMainFileSave));
        mNavi->bind("FileSaveAs", NaviDelegate(this, &GUI_Avatar::avatarMainFileSaveAs));
        mNavi->bind("FileExit", NaviDelegate(this, &GUI_Avatar::avatarMainFileExit));

        m_curState = NSCreated;
    }
    else
        mNavi->show(true);

    mNavigator->startAvatarEdit();
    return true;
}

//-------------------------------------------------------------------------------------
bool GUI_Avatar::isVisible()
{
    return ((mNavi != 0) && mNavi->getVisibility());
}
//-------------------------------------------------------------------------------------
void GUI_Avatar::hide()
{
    if (!isVisible()) return;
    mNavi->hide();
}
//-------------------------------------------------------------------------------------
void GUI_Avatar::destroy()
{
    if (m_curState != NSNotCreated)
    {
        // Destroy Navi UI modeler
        mNavi->hide();
        NaviLibrary::NaviManager::Get().destroyNavi(mNavi);
        m_curState = NSNotCreated;

        mNavigator->setState(Navigator::SInWorld);	
        mNavigator->endAvatarEdit();

        GUI_AvatarProperties::unload();

        // Remove temporary files & folder of the thumbnails
        std::string path ( "NaviLocal\\NaviTmpTexture" );
        std::vector<std::string> fileList;

        if( SOLisDirectory( String( AvatarEditor::getSingletonPtr()->mExecPath + "\\" + path ).c_str() ) )
        {
            SOLlistDirectoryFiles (path.c_str(), &fileList);
            path += "\\";
            for (std::vector<std::string>::iterator f=fileList.begin(); f!=fileList.end(); f++)
                SOLdeleteFile (std::string(path + (*f)).c_str());
            RemoveDirectory (path.c_str());
        }
    }
}

//-------------------------------------------------------------------------------------
void GUI_Avatar::avatarMainPageLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Avatar::avatarMainPageLoaded()");

    // Update the properties panel from the selected object datas
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    Avatar* user = mNavigator->getUserAvatar();

    std::string text("$('AvatarName').innerHTML = '<p>Name : <b>");
    text += avatar->getCharacter()->getName();
    text += "</b></p>'";
    mNavi->evaluateJS(text.data());

    // Setup the avatar name list
    //	mNavi->evaluateJS("$('avatarSelectTitre').innerHTML = '" + AvatarEditor::getSingletonPtr()->getName() + "'");
    text = "";
    vector<std::string>* list = AvatarEditor::getSingletonPtr()->getManager()->getNameList();
    vector<std::string>::iterator iter = list->begin();
    int id = 0;
    while(iter!=list->end())
    {
        /*		text += "<div class='itemOut' onmouseout=this.className='itemOut' onmouseover=this.className='itemOver'><a href='#' class='lienMenu' onclick=select('";
        text += (*iter).data();
        text += "')>";
        text += (*iter).data();
        text += "</a></div>";*/
        text += "<option value='" + std::string((*iter).data()) + "'";
        if (std::string((*iter).data()) == avatar->getCharacter()->getName())
            text += " selected='selected'";
        text += ">" + std::string((*iter).data()) + "</option>";
        iter++;
    }
    //	mNavi->evaluateJS("$('avatarSelectItem').innerHTML = \"" + text + "\"");
    mNavi->evaluateJS("$('selectAvatar').innerHTML = \"" + text + "\"");

    // Select the avatar from the user.xml // avatarName
    /*	size_t nbItem = list->size();
    if( nbItem < 7 )
    mNavi->evaluateJS("$('avatarSelectItem').style.height = '" + StringConverter::toString(nbItem*16) + "px'");
    list->clear();*/

    // Show Navi UI
    if (m_curState == NSCreated)
    {
        mNavi->show(true);
        mNavi->focus();
    }
}

//-------------------------------------------------------------------------------------
void GUI_Avatar::avatarMainFileOpen(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Avatar::avatarMainFileOpen()");
    mNavigator->avatarXMLLoad();
}
//-------------------------------------------------------------------------------------
void GUI_Avatar::avatarMainFileEdit(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Avatar::avatarMainFileEdit()");

    // Hide the main modeler panel
    hide();

    // Show the properties modeler panel
    GUI_AvatarProperties::createAndShowPanel();
}
//-------------------------------------------------------------------------------------
void GUI_Avatar::avatarMainFileSave(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Avatar::avatarMainFileSave()");
    mNavigator->avatarXMLSave();
}
//-------------------------------------------------------------------------------------
void GUI_Avatar::avatarMainFileSaveAs(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Avatar::avatarMainFileSaveAs()");
    mNavigator->avatarXMLSaveAs();
}
//-------------------------------------------------------------------------------------
void GUI_Avatar::avatarMainFileExit(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Avatar::avatarMainFileExit()");

    unload();
    mNavigator->setCameraMode(mNavigator->getLastCameraMode());
}
//-------------------------------------------------------------------------------------
/*void GUI_Avatar::avatarMainSelectPrev(Navi* caller, const Awesomium::JSArguments& args)
{
LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Avatar::avatarMainSelectPrev()");
Avatar* userAvatar = mNavigator->getUserAvatar();
userAvatar->detachFromSceneNode();
AvatarEditor::getSingletonPtr()->setPrevAsCurrent();
userAvatar->setCharacterInstance(AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance());

NaviLibrary::Navi* mNavi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARMAIN]);
std::string text( AvatarEditor::getSingletonPtr()->getName() );
mNavi->evaluateJS("$('AvatarName').innerHTML = '<p>Name : <b>" + text + "</b></p>'");
mNavi->evaluateJS("$('avatarSelectTitre').innerHTML = '" + text + "'");
}*/
//-------------------------------------------------------------------------------------
/*void GUI_Avatar::avatarMainSelectNext(Navi* caller, const Awesomium::JSArguments& args)
{
LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Avatar::avatarMainSelectNext()");
Avatar* userAvatar = mNavigator->getUserAvatar();
userAvatar->detachFromSceneNode();
AvatarEditor::getSingletonPtr()->setNextAsCurrent();
userAvatar->setCharacterInstance(AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance());

NaviLibrary::Navi* mNavi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARMAIN]);
std::string text( AvatarEditor::getSingletonPtr()->getName() );
mNavi->evaluateJS("$('AvatarName').innerHTML = '<p>Name : <b>" + text + "</b></p>'");
mNavi->evaluateJS("$('avatarSelectTitre').innerHTML = '" + text + "'");
}*/
//-------------------------------------------------------------------------------------
void GUI_Avatar::avatarMainSelected(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Avatar::avatarMainSelected()");
    std::string item( naviData["item"].str() );

    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    if( avatar->getCharacter()->getName() != item )
    {
        Avatar* userAvatar = mNavigator->getUserAvatar();
        userAvatar->detachFromSceneNode();
        AvatarEditor::getSingletonPtr()->setCurrentByName(item);
        userAvatar->setCharacterInstance(AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance());

        std::string text( AvatarEditor::getSingletonPtr()->getName() );
        mNavi->evaluateJS("$('AvatarName').innerHTML = '<p>Name : <b>" + text + "</b></p>'");
    }
}