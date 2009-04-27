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
#include <AvatarEditor.h>
#include <CharacterManager.h>
#include <Character.h>
#include <CharacterInstance.h>
#include <VoiceEngineManager.h>
#include "World/Avatar.h"

#ifdef _MSC_VER
 #pragma warning (disable:4355)
#endif // _MSC_VER

using namespace Solipsis;
using namespace CommonTools;

NavigatorGUI * NavigatorGUI::mNaviGui = NULL;


const std::string NavigatorGUI::ms_NavisNames[] = {
    "uiabout",
    "uicommands",
    "uimdlrprop",

#ifdef DECLARATIVE_MODELER
   "uimdlrscenefromtext",
#endif

    "uiavatarmain",
    "uiavatarprop",

#ifdef UIDEBUG
    "uidebug"
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

    for (std::map<std::string, GUI_Panel *>::iterator it = mNaviGui->m_panels.begin(); 
        it != mNaviGui->m_panels.end(); 
        it++)
    {
        GUI_Panel * pPanel = it->second;
        pPanel->destroy();
        // CF : should destroy the panels here, but it crashes the hash table iteration
        // needed to be corrected
      //  delete pPanel;
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

//-------------------------------------------------------------------------------------
void NavigatorGUI::addChatText(const std::wstring& message)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::addChatText()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_CHAT]);
    if (navi == 0)
        return;

    // Navi MultiValue will encode the wstring in URI encoded string and add 1 call to decodeURIComponent on it
    navi->evaluateJS("$('textChat').value += ?", NaviLibrary::NaviUtilities::Args(message));
    navi->evaluateJS("$('textChat').value += '\\n'");
    navi->evaluateJS("$('textChat').scrollTop = $('textChat').scrollHeight;");
}
 

//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainShow()
{
    if (mNavisStates[NAVI_AVATARPROP] == NSCreated)
        avatarPropHide();

    if (mNavisStates[NAVI_AVATARMAIN] == NSNotCreated)
    {
        // Create Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(ms_NavisNames[NAVI_AVATARMAIN], "local://uiavatarmain.html", NaviPosition(TopRight), 256, 512);
        navi->setMovable(true);
        navi->hide();
        navi->setMask("uiavatarmain.png");//Eliminate the black shadow at the margin of the menu
        navi->setOpacity(0.75f);
        
        navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::avatarMainPageLoaded));
//		navi->bind("AvatarPrev", NaviDelegate(this, &NavigatorGUI::avatarMainSelectPrev));
//		navi->bind("AvatarNext", NaviDelegate(this, &NavigatorGUI::avatarMainSelectNext));
		navi->bind("AvatarSelected", NaviDelegate(this, &NavigatorGUI::avatarMainSelected));
	    navi->bind("FileOpen", NaviDelegate(this, &NavigatorGUI::avatarMainFileOpen));
		navi->bind("AvatarEdit", NaviDelegate(this, &NavigatorGUI::avatarMainFileEdit));
		navi->bind("FileSave", NaviDelegate(this, &NavigatorGUI::avatarMainFileSave));
        navi->bind("FileSaveAs", NaviDelegate(this, &NavigatorGUI::avatarMainFileSaveAs));
	    navi->bind("FileExit", NaviDelegate(this, &NavigatorGUI::avatarMainFileExit));

		mNavisStates[NAVI_AVATARMAIN] = NSCreated;
    }
    else
        mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARMAIN])->show(true);

    mNavigator->startAvatarEdit();
/*
    Modeler *modeler = mNavigator->getModeler();
    if (modeler)
        modeler->lockSelection(false);
*/
}
//-------------------------------------------------------------------------------------
bool NavigatorGUI::isAvatarMainVisible()
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARMAIN]);
    return ((navi != 0) && navi->getVisibility());
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainHide()
{
    if (!isAvatarMainVisible()) return;
    mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARMAIN])->hide();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainUnload()
{
    if (mNavisStates[NAVI_AVATARMAIN] != NSNotCreated)
    {
        // Destroy Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARMAIN]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[NAVI_AVATARMAIN] = NSNotCreated;

		mNavigator->setState(Navigator::SInWorld);	
		mNavigator->endAvatarEdit();
		avatarPropUnload();
		avatarMainUnload();

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
void NavigatorGUI::avatarPropShow()
{
    if (mNavisStates[NAVI_AVATARPROP] == NSNotCreated)
	{
		// Create Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(ms_NavisNames[NAVI_AVATARPROP], "local://uiavatarprop.html", NaviPosition(TopRight), 512, 512);
		navi->setMovable(true);
		navi->hide();
		navi->setMask("uiavatarprop.png");//Eliminate the black shadow at the margin of the menu
		navi->setOpacity(0.75f);

		// page loaded
		navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::avatarPropPageLoaded));
        navi->bind("pageClosed", NaviDelegate(this, &NavigatorGUI::avatarPropPageClosed));

		// detect a changement on the properties tabber
		navi->bind("ClickOnTabber", NaviDelegate(this, &NavigatorGUI::avatarTabberChange));

		// animation
		navi->bind("AnimPlayPause", NaviDelegate(this, &NavigatorGUI::avatarPropAnimPlayPause));
		navi->bind("AnimStop", NaviDelegate(this, &NavigatorGUI::avatarPropAnimStop));
		navi->bind("AnimNext", NaviDelegate(this, &NavigatorGUI::avatarPropAnimNext));
		navi->bind("AnimPrev", NaviDelegate(this, &NavigatorGUI::avatarPropAnimPrev));
		// properties
		navi->bind("AvatarHeight", NaviDelegate(this, &NavigatorGUI::avatarPropHeight));
		navi->bind("BonePrev", NaviDelegate(this, &NavigatorGUI::avatarPropBonePrev));
		navi->bind("BoneNext", NaviDelegate(this, &NavigatorGUI::avatarPropBoneNext));
		navi->bind("BodyPartPrev", NaviDelegate(this, &NavigatorGUI::avatarPropBPPrev));
		navi->bind("BodyPartNext", NaviDelegate(this, &NavigatorGUI::avatarPropBPNext));
		navi->bind("BodyPartModelPrev", NaviDelegate(this, &NavigatorGUI::avatarPropBPMPrev));
		navi->bind("BodyPartModelNext", NaviDelegate(this, &NavigatorGUI::avatarPropBPMNext));
		navi->bind("BodyPartModelEdit", NaviDelegate(this, &NavigatorGUI::avatarPropBPMEdit));
		navi->bind("BodyPartModelRemove", NaviDelegate(this, &NavigatorGUI::avatarPropBPMRemove));
		navi->bind("AttachPrev", NaviDelegate(this, &NavigatorGUI::avatarPropAttPrev));
		navi->bind("AttachNext", NaviDelegate(this, &NavigatorGUI::avatarPropAttNext));
		navi->bind("AttachModelPrev", NaviDelegate(this, &NavigatorGUI::avatarPropAttMPrev));
		navi->bind("AttachModelNext", NaviDelegate(this, &NavigatorGUI::avatarPropAttMNext));
		navi->bind("AttachModelEdit", NaviDelegate(this, &NavigatorGUI::avatarPropAttMEdit));
		navi->bind("AttachModelRemove", NaviDelegate(this, &NavigatorGUI::avatarPropAttMRemove));
		navi->bind("AvatarPropSliders", NaviDelegate(this, &NavigatorGUI::avatarPropSliders));
		navi->bind("SelectionReset", NaviDelegate(this, &NavigatorGUI::avatarPropReset));
		// material
		navi->bind("AvatarAmbient", NaviDelegate(this, &NavigatorGUI::avatarColorAmbient));
		navi->bind("AvatarDiffuse", NaviDelegate(this, &NavigatorGUI::avatarColorDiffuse));
		navi->bind("AvatarSpecular", NaviDelegate(this, &NavigatorGUI::avatarColorSpecular));
		navi->bind("AvatarLockAmbientDiffuse", NaviDelegate(this, &NavigatorGUI::avatarColorLockAmbientDiffuse));
		navi->bind("AvatarDoubleSide", NaviDelegate(this, &NavigatorGUI::avatarDoubleSide));
		navi->bind("AvatarShininess", NaviDelegate(this, &NavigatorGUI::avatarPropShininess));
		navi->bind("AvatarTransparency", NaviDelegate(this, &NavigatorGUI::avatarPropTransparency));
		navi->bind("AvatarScrollU", NaviDelegate(this, &NavigatorGUI::avatarPropScrollU));
		navi->bind("AvatarScrollV", NaviDelegate(this, &NavigatorGUI::avatarPropScrollV));
		navi->bind("AvatarScaleU", NaviDelegate(this, &NavigatorGUI::avatarPropScaleU));
		navi->bind("AvatarScaleV", NaviDelegate(this, &NavigatorGUI::avatarPropScaleV));
		navi->bind("AvatarRotateU", NaviDelegate(this, &NavigatorGUI::avatarPropRotateU));
		navi->bind("AvatarAddTexture", NaviDelegate(this, &NavigatorGUI::avatarPropTextureAdd));
		navi->bind("AvatarRemoveTexture", NaviDelegate(this, &NavigatorGUI::avatarPropTextureRemove));
		navi->bind("AvatarPrevTexture", NaviDelegate(this, &NavigatorGUI::avatarPropTexturePrev));
		navi->bind("AvatarNextTexture", NaviDelegate(this, &NavigatorGUI::avatarPropTextureNext));
		navi->bind("AvatarResetColour", NaviDelegate(this, &NavigatorGUI::avatarPropResetColour));
		navi->bind("AvatarPropSound", NaviDelegate(this, &NavigatorGUI::avatarPropSound));

        mNavisStates[NAVI_AVATARPROP] = NSCreated;
	}
	else //if(!isAvatarMainVisible())
	{
		NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
				
		// Update the properties panel from the selected object datas
		avatarTabberLoad(1);
		navi->evaluateJS("$('avatarTabbers').tabber.tabShow(1)");
		navi->show(true);
	}
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isAvatarPropVisible()
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
    return ((navi != 0) && navi->getVisibility());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropHide()
{
    if (!isAvatarPropVisible()) return;
    mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP])->hide();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropUnload()
{
    if (mNavisStates[NAVI_AVATARPROP] != NSNotCreated)
    {
        // Destroy Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[NAVI_AVATARPROP] = NSNotCreated;

		// Update the selected objet dats from the properties panel
//		modelerTabberSave();
    }
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarTabberChange(const NaviData& naviData)
{
	unsigned tab;
	tab = atoi(naviData["tab"].str().c_str());
 	avatarTabberLoad (tab);
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarTabberLoad(unsigned pTab)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	// get the current avatar
	CharacterInstance* avatar = mNavigator->getAvatarEditor()->getManager()->getCurrentInstance();
	if( avatar != 0 )
	{
		ColourValue col;
		Ogre::Vector2 UV;
		std::string text;
		
		switch( pTab )
		{
		case 0:	// animations tab
			{
				navi->evaluateJS("$('avatarTabbers').tabber.tabShow(0)");

				// Setup the animation list
				vector<std::string> list;
                for(int i=0; i<avatar->getCharacter()->getNumAnimations(); i++)
					list.push_back( avatar->getEntity()->getSkeleton()->getAnimation( i )->getName() );
				std::string text = avatar->getEntity()->getSkeleton()->getAnimation( avatar->getCurrentAnimation() )->getName();
				navi->evaluateJS("$('animationSelectTitre').innerHTML = '" + text + "'");
				text = "";
				vector<std::string>::iterator iter = list.begin();
				int id = 0;
				while(iter!=list.end())
				{
					text += "<div class='itemOut' onmouseout=this.className='itemOut' onmouseover=this.className='itemOver'><a href='#' class='lienMenu' onclick=select('";
					text += (*iter).data();
					text += "')>";
					text += (*iter).data();
					text += "</a></div>";
					iter++;
				}
				navi->evaluateJS("$('animationSelectItem').innerHTML = \"" + text + "\"");

				size_t nbItem = list.size();
				if( nbItem < 7 )
					navi->evaluateJS("$('animationSelectItem').style.height = '" + StringConverter::toString(nbItem*16) + "px'");
				list.clear();
			}
			break;
		case 1:	//  properties tab
			{
				navi->evaluateJS("$('avatarTabbers').tabber.tabShow(1)");

				// height
				navi->evaluateJS("height.onchange = function() {}");
				Vector3 size = avatar->getEntity()->getBoundingBox().getSize();
				navi->evaluateJS("height.setValue(" + StringConverter::toString(int((size.y-0.5)*100)) + ")");
				navi->evaluateJS("height.onchange = function() {elementClicked('AvatarHeight')}");

				navi->evaluateJS("$('HeightValue').value=height.getValue()/100.+0.5+'m'");

				// bones
				Bone* bone = avatar->getCurrentBone();
				std::string name(bone->getName());
				std::string temp("");
				for(int i=0; i<(int)name.length(); i++)
				{
					char c = name[i];
					if (! ((c < 48 && c != 32) || c == 255 ||
						c == 208 || c == 209 ||	c == 215 || c == 216 ||
						(c < 192 && c > 122) || (c < 65 && c > 57)) ) 
					{
						temp += c;
					}
				}
				navi->evaluateJS("$('BoneName').innerHTML = '" + temp + "'");

				// body parts
				name = "None";
				if(avatar->getCharacter()->getNumBodyParts() > 0)
                    name = avatar->getCurrentBodyPart()->getBodyPart()->getName();
				navi->evaluateJS("$('BodyPartName').innerHTML = '" + name + "'");

				name = "None";
				if(avatar->getCharacter()->getNumBodyParts() > 0)
					name = avatar->getCurrentBodyPart()->getCurrentBodyPartModelName();
				navi->evaluateJS("$('BodyPartModelName').innerHTML = '" + name + "'");
				if( name == "None" )
					navi->evaluateJS("$('BodyPartEdit').disabled = 1");

				// attachements / goodies
				name = "None";
				if(avatar->getCharacter()->getNumGoodies() > 0)
                    name = avatar->getCurrentGoody()->getGoody()->getName();
				navi->evaluateJS("$('AttachName').innerHTML = '" + name + "'");

				name = "None";
				if(avatar->getCharacter()->getNumGoodies() > 0)
					name = avatar->getCurrentGoody()->getCurrentGoodyModelName();
				navi->evaluateJS("$('AttachModelName').innerHTML = '" + name + "'");
				if( name == "None" )
					navi->evaluateJS("$('AttachEdit').disabled = 1");

				AvatarEditor::getSingletonPtr()->selectType = 0;
				Quaternion q = bone->getOrientation();
				Vector3 orientation(q.getYaw().valueRadians(), q.getPitch().valueRadians(), q.getRoll().valueRadians());
				avatarUpdateSliders( bone->getPosition()+0.5, orientation/Math::TWO_PI+0.5, bone->getScale()/2 );
			}
			break;
		case 2: // material
			{
				navi->evaluateJS("$('avatarTabbers').tabber.tabShow(2)");

				int type = AvatarEditor::getSingletonPtr()->selectType;
				ModifiableMaterialObject* object;
				
				if( type == 2 ) // Goody
					object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
				else //if( type <= 1 ) // BodyPart
					object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

				ModifiedMaterial* material = object->getModifiedMaterial();

				text = navi->evaluateJS("decToHex(" + StringConverter::toString(material->getAmbient().r * 255) + ")");
				text += navi->evaluateJS("decToHex(" + StringConverter::toString(material->getAmbient().g * 255) + ")");
				text += navi->evaluateJS("decToHex(" + StringConverter::toString(material->getAmbient().b * 255) + ")");
				navi->evaluateJS("$S('pAmbient').background='#" + text + "'");
				text = navi->evaluateJS("decToHex(" + StringConverter::toString(material->getDiffuse().r * 255) + ")");
				text += navi->evaluateJS("decToHex(" + StringConverter::toString(material->getDiffuse().g * 255) + ")");
				text += navi->evaluateJS("decToHex(" + StringConverter::toString(material->getDiffuse().b * 255) + ")");
				navi->evaluateJS("$S('pDiffuse').background='#" + text + "'");
				text = navi->evaluateJS("decToHex(" + StringConverter::toString(material->getSpecular().r * 255) + ")");
				text += navi->evaluateJS("decToHex(" + StringConverter::toString(material->getSpecular().g * 255) + ")");
				text += navi->evaluateJS("decToHex(" + StringConverter::toString(material->getSpecular().b * 255) + ")");
				navi->evaluateJS("$S('pSpecular').background='#" + text + "'");

				navi->evaluateJS("shininess.onchange = function() {}");
				navi->evaluateJS("transparency.onchange = function() {}");
				navi->evaluateJS("scrollU.onchange = function() {}");
				navi->evaluateJS("scrollV.onchange = function() {}");
				navi->evaluateJS("scaleU.onchange = function() {}");
				navi->evaluateJS("scaleV.onchange = function() {}");
				navi->evaluateJS("rotateU.onchange = function() {}");

				navi->evaluateJS("shininess.setValue(" + StringConverter::toString(Real(material->getShininess()/128.)*100) + ")");
				navi->evaluateJS("transparency.setValue("+ StringConverter::toString(material->getAlpha()*100) + ")");
				UV = material->getTextureScroll() ;
				navi->evaluateJS("scrollU.setValue(" + StringConverter::toString(UV.x*100+50) + ")");
				navi->evaluateJS("scrollV.setValue(" + StringConverter::toString(UV.y*100+50) + ")");
				UV = material->getTextureScale() ;
				navi->evaluateJS("scaleU.setValue(" + StringConverter::toString(UV.x*100-50) + ")");
				navi->evaluateJS("scaleU.setValue(" + StringConverter::toString(UV.y*100-50) + ")");
				navi->evaluateJS("rotateU.setValue(" + StringConverter::toString(material->getTextureRotate()/Math::TWO_PI*100) + ")");

				navi->evaluateJS("shininess.onchange = function() {elementClicked('AvatarShininess')}");
				navi->evaluateJS("transparency.onchange = function() {elementClicked('AvatarTransparency')}");
				navi->evaluateJS("scrollU.onchange = function() {elementClicked('AvatarScrollU')}");
				navi->evaluateJS("scrollV.onchange = function() {elementClicked('AvatarScrollV')}");
				navi->evaluateJS("scaleU.onchange = function() {elementClicked('AvatarScaleU')}");
				navi->evaluateJS("scaleV.onchange = function() {elementClicked('AvatarScaleV')}");
				navi->evaluateJS("rotateU.onchange = function() {elementClicked('AvatarRotateU')}");

				MaterialPtr mat = object->getModifiedMaterial()->getOwner();
				CullingMode mode = mat->getTechnique(0)->getPass(0)->getCullingMode();
				navi->evaluateJS("document.getElementById('doubleSide').checked = " + (mode == CULL_NONE)?"true":"false" );

				avatarUpdateTextures( object );
			}
			break;
		case 3:	//  sound tab
			{
				navi->evaluateJS("$('avatarTabbers').tabber.tabShow(3)");

                float minDist, maxDist;
	            Avatar* user = mNavigator->getUserAvatar();
				user->getVoiceDistances(minDist, maxDist);
				// sound3DMinDist
				navi->evaluateJS("sound3DMinDist.onchange = function() {}");
				navi->evaluateJS("sound3DMinDist.setValue(" + StringConverter::toString(minDist) + ")");
				navi->evaluateJS("sound3DMinDist.onchange = function() {soundDistances()}");
				navi->evaluateJS("$('Sound3DMinDistValue').value=sound3DMinDist.getValue()");
				// sound3DMaxDist
				navi->evaluateJS("sound3DMaxDist.onchange = function() {}");
				navi->evaluateJS("sound3DMaxDist.setValue(" + StringConverter::toString(maxDist) + ")");
				navi->evaluateJS("sound3DMaxDist.onchange = function() {soundDistances()}");
				navi->evaluateJS("$('Sound3DMaxDistValue').value=sound3DMaxDist.getValue()");
			}
			break;
		}
		//mNaviMgr->Update();
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarTabberSave()
{}



#ifdef UIDEBUG
//-------------------------------------------------------------------------------------
void NavigatorGUI::switchDebug()
{
    if (mNavisStates[NAVI_DEBUG] == NSNotCreated)
    {
        // Create Navi UI debug
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(ms_NavisNames[NAVI_DEBUG], "local://uidebug.html", NaviPosition(TopRight), 300, 256);
        navi->setMovable(true);
        navi->setAutoUpdateOnFocus(true);
        navi->setMaxUPS(24);
        navi->hide();
        navi->setMask("uidebug.png");
        navi->setOpacity(0.50f);
        navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::debugPageLoaded));
        navi->bind("pageClosed", NaviDelegate(this, &NavigatorGUI::debugPageClosed));
        navi->bind("debugRefreshTree", NaviDelegate(this, &NavigatorGUI::debugRefreshTree));
        navi->bind("debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
        navi->bind("navCommand", NaviDelegate(this, &NavigatorGUI::navCommand));
        mNavisStates[NAVI_DEBUG] = NSCreated;
        mTreeDirty = true;
    }
    else
    {
        // Hide and destroy UI debug
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_DEBUG]);
        if (navi->getVisibility()) {
            navi->hide();
            mNaviMgr->destroyNavi(navi);
            mNavisStates[NAVI_DEBUG] = NSNotCreated;
        }
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::debugRefreshUrl()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::debugRefreshUrl()");

    if (mNavisStates[NAVI_DEBUG] != NSCreated) return;

#ifdef DEMO_NAVI2
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_DEBUG]);
    NaviLibrary::Navi* naviDemoNavi2 = mNaviMgr->getNavi("WWW_demoNavi2Video");
    if (naviDemoNavi2 == 0) return;
    // Set current url
    char txt[256];
    sprintf(txt, "$('inputUrl').value = '%s'", naviDemoNavi2->getCurrentLocation().c_str());
    navi->evaluateJS(txt);
    // Activate/Deactivate Back/Forward buttons
    sprintf(txt, "$('navBackButton').disabled = %s", naviDemoNavi2->canNavigateBack() ? "false" : "true");
    navi->evaluateJS(txt);
    sprintf(txt, "$('navForwardButton').disabled = %s", naviDemoNavi2->canNavigateForward() ? "false" : "true");
    navi->evaluateJS(txt);
#endif
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::debugRefreshDemoVoiceTalkButtonName()
{
    char txt[256];

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::debugRefreshDemoVoiceTalkButtonName()");

    if (mNavisStates[NAVI_DEBUG] != NSCreated) return;

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_DEBUG]);

    navi->evaluateJS("$('inputVoice').value = '" + mNavigator->getVoIPServerAddress() + "'");
    sprintf(txt, "$('inputVoiceSilenceLvl').value = '%.2f'", mNavigator->getVoIPSilenceLevel());
    navi->evaluateJS(txt);
    sprintf(txt, "$('inputVoiceSilenceLat').value = '%d'", mNavigator->getVoIPSilenceLatency());
    navi->evaluateJS(txt);

    // get voice engine
    IVoiceEngine* voiceEngine = VoiceEngineManager::getSingleton().getSelectedEngine();
    if (voiceEngine == 0)
        return;
    sprintf(txt, "$('demoVoiceToggleTalkButton').innerHTML = '%s'", (voiceEngine->isRecording() ? "Stop" : "Start"));
    navi->evaluateJS(txt);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::debugPageLoaded(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::debugPageLoaded()");

    // Refresh url
    debugRefreshUrl();

    // Refresh tree datas
    debugRefreshTree(naviData);

    // Refresh voice engine state
    debugRefreshDemoVoiceTalkButtonName();

    // Show Navi UI debug
    if (mNavisStates[NAVI_DEBUG] == NSCreated)
        mNaviMgr->getNavi(ms_NavisNames[NAVI_DEBUG])->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::debugPageClosed(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::debugPageClosed()");

    switchDebug();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::debugRefreshTree(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::debugRefreshTree()");

    if (mNavisStates[NAVI_DEBUG] != NSCreated) return;
    if (!mTreeDirty) return;

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_DEBUG]);

    navi->evaluateJS("allTree.disable()");
    navi->evaluateJS("allTree.root.clear()");
    navi->evaluateJS("allTree.insert({text:'Scenes', id:'Scenes'})");
    String sceneName = mNavigator->getSceneMgrPtr()->getName();
    navi->evaluateJS("allTree.get('Scenes').insert({text:'" + sceneName + "', id:'S_" + sceneName + "'})");
    navi->evaluateJS("allTree.insert({text:'OgrePeers', id:'OgrePeers'})");
    for (OgrePeerManager::OgrePeersMap::iterator ogrePeer = mNavigator->getOgrePeerManager()->getOgrePeersIteratorBegin();ogrePeer != mNavigator->getOgrePeerManager()->getOgrePeersIteratorEnd();ogrePeer++)
    {
        String ogrePeerName = ogrePeer->second->getXmlEntity()->getName();
        navi->evaluateJS("allTree.get('OgrePeers').insert({text:'" + ogrePeerName + "', id:'OP_" + ogrePeerName + "'})");
    }
    navi->evaluateJS("allTree.enable()");

    mTreeDirty = false;
}
#endif






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
void NavigatorGUI::avatarMainPageLoaded(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarMainPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARMAIN]);

    // Update the properties panel from the selected object datas
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	Avatar* user = mNavigator->getUserAvatar();

	std::string text("$('AvatarName').innerHTML = '<p>Name : <b>");
    text += avatar->getCharacter()->getName();
	text += "</b></p>'";
	navi->evaluateJS(text.data());

	// Setup the avatar name list
//	navi->evaluateJS("$('avatarSelectTitre').innerHTML = '" + AvatarEditor::getSingletonPtr()->getName() + "'");
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
//	navi->evaluateJS("$('avatarSelectItem').innerHTML = \"" + text + "\"");
	navi->evaluateJS("$('selectAvatar').innerHTML = \"" + text + "\"");

	// Select the avatar from the user.xml // avatarName
/*	size_t nbItem = list->size();
	if( nbItem < 7 )
		navi->evaluateJS("$('avatarSelectItem').style.height = '" + StringConverter::toString(nbItem*16) + "px'");
	list->clear();*/

    // Show Navi UI
    if (mNavisStates[NAVI_AVATARMAIN] == NSCreated)
    {
        navi->show(true);
        navi->focus();
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainFileOpen(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarMainFileOpen()");
    mNavigator->avatarXMLLoad();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainFileEdit(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarMainFileEdit()");

	// Hide the main modeler panel
	avatarMainHide();
	// Show the properties modeler panel
	avatarPropShow();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainFileSave(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarMainFileSave()");
    mNavigator->avatarXMLSave();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainFileSaveAs(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarMainFileSaveAs()");
    mNavigator->avatarXMLSaveAs();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainFileExit(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarMainFileExit()");

    avatarMainUnload();
    mNavigator->setCameraMode(mNavigator->getLastCameraMode());
}
//-------------------------------------------------------------------------------------
/*void NavigatorGUI::avatarMainSelectPrev(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarMainSelectPrev()");
    Avatar* userAvatar = mNavigator->getUserAvatar();
    userAvatar->detachFromSceneNode();
	AvatarEditor::getSingletonPtr()->setPrevAsCurrent();
    userAvatar->setCharacterInstance(AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance());

	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARMAIN]);
	std::string text( AvatarEditor::getSingletonPtr()->getName() );
	navi->evaluateJS("$('AvatarName').innerHTML = '<p>Name : <b>" + text + "</b></p>'");
	navi->evaluateJS("$('avatarSelectTitre').innerHTML = '" + text + "'");
}*/
//-------------------------------------------------------------------------------------
/*void NavigatorGUI::avatarMainSelectNext(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarMainSelectNext()");
    Avatar* userAvatar = mNavigator->getUserAvatar();
    userAvatar->detachFromSceneNode();
	AvatarEditor::getSingletonPtr()->setNextAsCurrent();
    userAvatar->setCharacterInstance(AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance());

	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARMAIN]);
	std::string text( AvatarEditor::getSingletonPtr()->getName() );
	navi->evaluateJS("$('AvatarName').innerHTML = '<p>Name : <b>" + text + "</b></p>'");
	navi->evaluateJS("$('avatarSelectTitre').innerHTML = '" + text + "'");
}*/
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainSelected(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarMainSelected()");
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARMAIN]);
	std::string item( naviData["item"].str() );
	
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    if( avatar->getCharacter()->getName() != item )
    {
        Avatar* userAvatar = mNavigator->getUserAvatar();
        userAvatar->detachFromSceneNode();
        AvatarEditor::getSingletonPtr()->setCurrentByName(item);
        userAvatar->setCharacterInstance(AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance());

		NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARMAIN]);
		std::string text( AvatarEditor::getSingletonPtr()->getName() );
		navi->evaluateJS("$('AvatarName').innerHTML = '<p>Name : <b>" + text + "</b></p>'");
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropPageLoaded(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

    // Update the properties panel from the selected object datas
    avatarTabberLoad( 1 );

    // Show Navi UI
    if (mNavisStates[NAVI_AVATARPROP] == NSCreated)
        navi->show(true);
#ifdef DECLARATIVE_MODELER

	std::string msg = "AvatarProp window loaded";
	MessageBox(0, msg.c_str(), "NavigatorGUI Avatar", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropPageClosed(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropPageClosed()");

	if (!isAvatarMainVisible())
	{
		avatarPropHide();
		avatarMainShow();
	}
	else 
		avatarMainUnload();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAnimPlayPause(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropAnimPlayPause()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	Avatar* user = mNavigator->getUserAvatar();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	user->stopAnimation();

	std::string state(navi->evaluateJS("$('AnimPlayPause').value"));
	if(state == "Pause")
	{
		navi->evaluateJS(std::string("$('AnimPlayPause').value = 'Play'"));
		navi->evaluateJS(std::string("$('AnimTime').style = 'display: none'"));
		user->stopAnimation();
	}
	else //Play
	{
		navi->evaluateJS(std::string("$('AnimPlayPause').value = 'Pause'"));
		navi->evaluateJS(std::string("$('AnimTime').style = 'display: block'"));
		user->setState(AnimationState(avatar->getCurrentAnimation()));
		user->startAnimation(user->getEntity()->getSkeleton()->getAnimation(avatar->getCurrentAnimation())->getName());
	}
		
	// ...
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAnimStop(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropAnimStop()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	Avatar* user = mNavigator->getUserAvatar();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	navi->evaluateJS(std::string("$('AnimPlayPause').value = 'Play'"));
	navi->evaluateJS(std::string("$('AnimTime').style = 'display: none'"));
	user->setState(ASAvatarNone);
	user->stopAnimation();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAnimNext(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropAnimNext()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	Avatar* user = mNavigator->getUserAvatar();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	unsigned int numAnim = avatar->getCharacter()->getNumAnimations();
	unsigned int current = avatar->getCurrentAnimation();
	if(++current >= numAnim) current = 0;
	avatar->setCurrentAnimation(current);
	
	std::string text( user->getEntity()->getSkeleton()->getAnimation(current)->getName() );
	user->stopAnimation();
	user->setState(AnimationState(current+1));
	user->startAnimation( text );

	navi->evaluateJS("$('animationSelectTitre').innerHTML = '" + text + "'");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAnimPrev(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropAnimPrev()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	Avatar* user = mNavigator->getUserAvatar();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	unsigned int numAnim = avatar->getCharacter()->getNumAnimations();
	int current = avatar->getCurrentAnimation();
	if(--current < 0) current = numAnim-1;
	avatar->setCurrentAnimation(current);

	std::string text( user->getEntity()->getSkeleton()->getAnimation(current)->getName() );
	user->stopAnimation();
	user->setState(AnimationState(current+1));
	user->startAnimation( text );

	navi->evaluateJS("$('animationSelectTitre').innerHTML = '" + text + "'");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropHeight(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropAnimPrev()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	float height = atoi(navi->evaluateJS("height.getValue()").data()) / 100. + 0.5;
	float scale = height / avatar->getEntity()->getBoundingBox().getSize().y;
	SceneNode* node = avatar->getSceneNode();

//	static Node* child = node->removeChild( (unsigned short) 2 );
	node->setScale( scale, scale, scale );
	//node->addChild( child );
	
	navi->evaluateJS("$('HeightValue').value=height.getValue()/100.+0.5+'m'");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropBonePrev(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropBonePrev()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	Bone* bone = avatar->setPreviousBoneAsCurrent();
	std::string str = bone->getName();
	std::string temp;
	char c;
	for(int i=0; i<(int)str.length(); i++)
	{
		c = str[i];
		if (! ((c < 48 && c != 32) || c == 255 ||
			c == 208 || c == 209 ||
			c == 215 || c == 216 ||
			(c < 192 && c > 122) ||
			(c < 65 && c > 57))) 
		{
			temp += c;
		}
	}
	str = "$('BoneName').innerHTML = '" + temp;
	navi->evaluateJS(str + "'");

	AvatarEditor::getSingletonPtr()->selectType = 0;
	Quaternion q = bone->getOrientation();
	Vector3 orientation(q.getYaw().valueRadians(), q.getPitch().valueRadians(), q.getRoll().valueRadians());
	avatarUpdateSliders( bone->getPosition()+0.5, orientation/Math::TWO_PI+0.5, bone->getScale()/2 );

	//navi->evaluateJS("$('oriX').style.display='none'");
	//navi->evaluateJS("$('oriY').style.display='none'");
	//navi->evaluateJS("$('oriZ').style.display='none'");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropBoneNext(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropBoneNext()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	Bone* bone = avatar->setNextBoneAsCurrent();
	std::string str = bone->getName();
	std::string temp;
	char c;
	for(int i=0; i<(int)str.length(); i++)
	{
		c = str[i];
		if (! ((c < 48 && c != 32) || c == 255 ||
			c == 208 || c == 209 ||
			c == 215 || c == 216 ||
			(c < 192 && c > 122) ||
			(c < 65 && c > 57)	)) 
		{
			temp += c;
		}
	}
	str = "$('BoneName').innerHTML = '" + temp;
	navi->evaluateJS(str + "'");

	AvatarEditor::getSingletonPtr()->selectType = 0;
	Quaternion q = bone->getOrientation();
	Vector3 orientation(q.getYaw().valueRadians(), q.getPitch().valueRadians(), q.getRoll().valueRadians());
	avatarUpdateSliders( bone->getPosition()+0.5, orientation/Math::TWO_PI+0.5, bone->getScale()/2 );

	//navi->evaluateJS("$('oriX').style.display='none'");
	//navi->evaluateJS("$('oriY').style.display='none'");
	//navi->evaluateJS("$('oriZ').style.display='none'");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropBPPrev(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropBPPrev()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	BodyPartInstance* bp = avatar->setPreviousBodyPartAsCurrent();
	BodyPartModel* bpm = bp->getCurrentBodyPartModel();
	std::string bpName = bp->getBodyPart()->getName();

	std::string str = "$('BodyPartName').innerHTML = '";
	str += bpName;
	navi->evaluateJS(str + "'");

	str = "$('BodyPartModelName').innerHTML = '";
	if (bpm != NULL)
	{
		str += bpm->getName();
		navi->evaluateJS(str + "'");
		navi->evaluateJS("$('BodyPartEdit').disabled = 0");

		// Update sliders ...
		AvatarEditor::getSingletonPtr()->selectType = 1;
		//Vector3 pos( bpm->getPosition() );
		//Vector3 ori( bpm->getOrientatio() );
		//Vector3 scale( bpm->getScale() );
		avatarUpdateSliders( Vector3::ZERO, Vector3::ZERO, Vector3::ZERO );
	}
	else 
	{
		str += "None";
		navi->evaluateJS(str + "'");
		navi->evaluateJS("$('BodyPartEdit').disabled = 1");
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropBPNext(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropBPNext()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	BodyPartInstance* bp = avatar->setNextBodyPartAsCurrent();
	BodyPartModel* bpm = bp->getCurrentBodyPartModel();
	std::string bpName = bp->getBodyPart()->getName();

	std::string str = "$('BodyPartName').innerHTML = '";
	str += bpName;
	navi->evaluateJS(str + "'");

	str = "$('BodyPartModelName').innerHTML = '";
	if (bpm != NULL)
	{
		str += bpm->getName();
		navi->evaluateJS(str + "'");
		navi->evaluateJS("$('BodyPartEdit').disabled = 0");

		// Update sliders ...
		AvatarEditor::getSingletonPtr()->selectType = 1;
		//Vector3 pos( bpm->getPosition() );
		//Vector3 ori( bpm->getOrientatio() );
		//Vector3 scale( bpm->getScale() );
		avatarUpdateSliders( Vector3::ZERO, Vector3::ZERO, Vector3::ZERO );
	}
	else 
	{
		str += "None";
		navi->evaluateJS(str + "'");
		navi->evaluateJS("$('BodyPartEdit').disabled = 1");
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropBPMPrev(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropBPMPrev()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	BodyPartInstance* bp = avatar->getCurrentBodyPart();
	bp->setPreviousBodyPartModelAsCurrent();
	BodyPartModel* bpm = bp->getCurrentBodyPartModel();

	std::string str = "$('BodyPartModelName').innerHTML = '";
	if (bpm != NULL)
	{
		str += bpm->getName();
		navi->evaluateJS(str + "'");
		navi->evaluateJS("$('BodyPartEdit').disabled = 0");

		// Update sliders ...
		AvatarEditor::getSingletonPtr()->selectType = 1;
		//Vector3 pos( bpm->getPosition() );
		//Vector3 ori( bpm->getOrientatio() );
		//Vector3 scale( bpm->getScale() );
		//avatarUpdateSliders( Vector3::ZERO, Vector3::ZERO, Vector3::ZERO );
		navi->evaluateJS("$('sliders').style = 'display:none'");
	}
	else 
	{
		str += "None";
		navi->evaluateJS(str + "'");
		navi->evaluateJS("$('BodyPartEdit').disabled = 1");
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropBPMNext(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropBPMNext()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	BodyPartInstance* bp = avatar->getCurrentBodyPart();
	bp->setNextBodyPartModelAsCurrent();
	BodyPartModel* bpm = bp->getCurrentBodyPartModel();

	std::string str = "$('BodyPartModelName').innerHTML = '";
	if (bpm != NULL)
	{
		str += bpm->getName();
		navi->evaluateJS(str + "'");
		navi->evaluateJS("$('BodyPartEdit').disabled = 0");

		// Update sliders ...
		AvatarEditor::getSingletonPtr()->selectType = 1;
		//Vector3 pos( bpm->getPosition() );
		//Vector3 ori( bpm->getOrientatio() );
		//Vector3 scale( bpm->getScale() );
		//avatarUpdateSliders( Vector3::ZERO, Vector3::ZERO, Vector3::ZERO );
		navi->evaluateJS("$('sliders').style = 'display:none'");
	}
	else 
	{
		str += "None";
		navi->evaluateJS(str + "'");
		navi->evaluateJS("$('BodyPartEdit').disabled = 1");
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropBPMEdit(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropBPMEdit()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	AvatarEditor::getSingletonPtr()->selectType = 1;
	avatarTabberLoad(2);
	navi->evaluateJS("$('avatarTabbers').tabber.tabShow(2)");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropBPMRemove(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropBPMRemove()");
	//CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	//NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAttPrev(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropAttPrev()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	if(avatar->getCharacter()->getNumGoodies() > 0)
	{
		GoodyInstance* g = avatar->setPreviousGoodyAsCurrent();
		GoodyModel* gm = g->getCurrentGoodyModel();
		std::string gName = g->getGoody()->getName();

		std::string str = "$('AttachName').innerHTML = '";
		str += gName;
		navi->evaluateJS(str + "'");

		str = "$('AttachModelName').innerHTML = '";
		if (gm != NULL)
		{
			str += gm->getName();
			navi->evaluateJS(str + "'");
			navi->evaluateJS("$('AttachEdit').disabled = 0");

			// Update sliders ...
			AvatarEditor::getSingletonPtr()->selectType = 2;
			avatarUpdateSliders( g->getCurrentPosition()/2+0.5, g->getCurrentRotationsAngles()/180 + 0.5, Vector3::ZERO );
			navi->evaluateJS("$('sliders').style = 'display:block'");
		}
		else 
		{
			str += "None";
			navi->evaluateJS(str + "'");
			navi->evaluateJS("$('AttachEdit').disabled = 1");
		}
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAttNext(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropAttNext()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	if(avatar->getCharacter()->getNumGoodies() > 0)
	{
		GoodyInstance* g = avatar->setNextGoodyAsCurrent();
		GoodyModel* gm = g->getCurrentGoodyModel();
		std::string gName = g->getGoody()->getName();

		std::string str = "$('AttachName').innerHTML = '";
		str += gName;
		navi->evaluateJS(str + "'");

		str = "$('AttachModelName').innerHTML = '";
		if (gm != NULL)
		{
			str += gm->getName();
			navi->evaluateJS(str + "'");
			navi->evaluateJS("$('AttachEdit').disabled = 0");

			// Update sliders ...
			AvatarEditor::getSingletonPtr()->selectType = 2;
			avatarUpdateSliders( g->getCurrentPosition()/2+0.5, g->getCurrentRotationsAngles()/180 + 0.5, Vector3::ZERO );
			navi->evaluateJS("$('sliders').style = 'display:block'");
		}
		else 
		{
			str += "None";
			navi->evaluateJS(str + "'");
			navi->evaluateJS("$('AttachEdit').disabled = 1");
		}
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAttMPrev(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropAttMPrev()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	if(avatar->getCharacter()->getNumGoodies() > 0)
	{
		GoodyInstance* g = avatar->getCurrentGoody();
		g->setPreviousGoodyModelAsCurrent();
		GoodyModel* gm = g->getCurrentGoodyModel();

		std::string str = "$('AttachModelName').innerHTML = '";
		if (gm != NULL)
		{
			str += gm->getName();
			navi->evaluateJS(str + "'");
			navi->evaluateJS("$('AttachEdit').disabled = 0");

			// Update sliders ...
			//AvatarEditor::getSingletonPtr()->selectType = 2;
			//avatarUpdateSliders( g->getCurrentPosition(), g->getCurrentRotationsAngles(), Vector3::ZERO );
		}
		else 
		{
			str += "None";
			navi->evaluateJS(str + "'");
			navi->evaluateJS("$('AttachEdit').disabled = 1");
		}
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAttMNext(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropAttMNext()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	if(avatar->getCharacter()->getNumGoodies() > 0)
	{
		GoodyInstance* g = avatar->getCurrentGoody();
		g->setNextGoodyModelAsCurrent();
		GoodyModel* gm = g->getCurrentGoodyModel();

		std::string str = "$('AttachModelName').innerHTML = '";
		if (gm != NULL)
		{
			str += gm->getName();
			navi->evaluateJS(str + "'");
			navi->evaluateJS("$('AttachEdit').disabled = 0");

			// Update sliders ...
			//AvatarEditor::getSingletonPtr()->selectType = 2;
			//avatarUpdateSliders( g->getCurrentPosition(), g->getCurrentRotationsAngles(), Vector3::ZERO );
		}
		else 
		{
			str += "None";
			navi->evaluateJS(str + "'");
			navi->evaluateJS("$('AttachEdit').disabled = 1");
		}
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAttMEdit(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropAttMEdit()");
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	AvatarEditor::getSingletonPtr()->selectType = 2;
	avatarTabberLoad(2);
	navi->evaluateJS("$('avatarTabbers').tabber.tabShow(2)");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAttMRemove(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropAttMRemove()");
	//CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	//NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropSliders(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropSliders()");
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	Avatar* user = mNavigator->getUserAvatar();

	std::string slider( naviData["slider"].str().c_str() );
	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type < 0) return;

	// get the value from the slidebar position
	std::string temp = slider + ".getValue()";
	float value = atoi( navi->evaluateJS(temp).c_str() ) / 100.;

	Bone* bone = avatar->getCurrentBone();
	BodyPartInstance* body = NULL;
	GoodyInstance* goody = NULL;

	// get the selected object ( BodyPart / Goody )
	if( avatar->getCharacter()->getNumBodyParts() > 0 )
		body = avatar->getCurrentBodyPart();
	if( avatar->getCharacter()->getNumGoodies() > 0 )
		goody = avatar->getCurrentGoody();

	// enable modification on the bone properties
	if( !bone->isManuallyControlled() ) 
	{
		bone->setManuallyControlled( true );
		for(int i=0; i<user->getEntity()->getSkeleton()->getNumAnimations(); i++ )
			user->getEntity()->getSkeleton()->getAnimation( i )->destroyNodeTrack( bone->getHandle() );
	}

	Vector3 vec;
	Quaternion q;
	Radian angle;

	// apply the modification on the selection ( bone / bodyPart / goody )
	if( slider == "posX")
	{
		switch( type )
		{
		case 0: // bone
			bone->translate( value - 0.5 - bone->getPosition().x, 0, 0 );
			break;
		case 1: // bodyPart
			break;
		case 2: // goody
			goody->setCurrentXScrollPosition( value );
			break;
		}
	}
	else if( slider == "posY")
	{
		switch( type )
		{
		case 0: // bone
			bone->translate( 0, value - 0.5 - bone->getPosition().y, 0 );
			break;
		case 1: // bodyPart
			break;
		case 2: // goody
			goody->setCurrentYScrollPosition( value );
			break;
		}
	}
	else if( slider == "posZ")
	{
		switch( type )
		{
		case 0: // bone
			bone->translate( 0, 0, value - 0.5 - bone->getPosition().z );
			break;
		case 1: // bodyPart
			break;
		case 2: // goody
			goody->setCurrentZScrollPosition( value );
			break;
		}
	}
	else if( slider == "oriX")
	{
		switch( type )
		{
		case 0: // bone
			q = bone->getOrientation();
			angle = Radian( (value - 0.5)*Math::TWO_PI );
			bone->yaw( angle - q.getYaw() );
			break;
		case 1: // bodyPart
			break;
		case 2: // goody
			goody->setCurrentYawAngleScrollPosition( value );
			break;
		}
	}
	else if( slider == "oriY")
	{
		switch( type )
		{
		case 0: // bone
			q = bone->getOrientation();
			angle = Radian( (value - 0.5)*Math::TWO_PI );
			bone->pitch( angle - q.getPitch() );
			break;
		case 1: // bodyPart
			break;
		case 2: // goody
			goody->setCurrentPitchAngleScrollPosition( value );
			break;
		}
	}
	else if( slider == "oriZ")
	{
		switch( type )
		{
		case 0: // bone
			q = bone->getOrientation();
			angle = Radian( (value - 0.5)*Math::TWO_PI );
			bone->roll( angle - q.getRoll() );
			break;
		case 1: // bodyPart
			break;
		case 2: // goody
			goody->setCurrentRollAngleScrollPosition( value );
			break;
		}
	}
	else if( slider == "scaleX")
	{
		switch( type )
		{
		case 0: // bone
			vec = bone->getScale();
			vec.x = value * 2;
			bone->setScale( vec );
			break;
		case 1: // bodyPart
			break;
		case 2: // goody
			break;
		}
	}
	else if( slider == "scaleY")
	{
		switch( type )
		{
		case 0: // bone
			vec = bone->getScale();
			vec.y = value * 2;
			bone->setScale( vec );
			break;
		case 1: // bodyPart
			break;
		case 2: // goody
			break;
		}
	}
	else if( slider == "scaleZ")
	{
		switch( type )
		{
		case 0: // bone
			vec = bone->getScale();
			vec.z = value * 2;
			bone->setScale( vec );
			break;
		case 1: // bodyPart
			break;
		case 2: // goody
			break;
		}
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropReset(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropSliders()");
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();

	std::string slider( naviData["slider"].str().c_str() );
	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type < 0) return;
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarColorAmbient(const NaviData& naviData)
{
	unsigned idRGB = 0;
	std::string str, color;
	int rgb[3]; 
	str = naviData["rgb"].str();

    for (unsigned id = 0; id < str.length(); id++)
	{
		if(str[id] != ',')
			color.insert(color.end(),str[id]);
		else
		{
			rgb[idRGB++] = atoi(color.c_str());
			color = "";
		}
	}
	rgb[idRGB] = atoi(color.c_str());

	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	ModifiedMaterial* material = object->getModifiedMaterial();
	if( material != 0 )
	{
		material->useAddedColour(true);
		material->setAmbient( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
		//material->setAddedColour( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );

		if(mLockAmbientDiffuse)
			material->setDiffuse( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarColorDiffuse(const NaviData& naviData)
{
	unsigned idRGB = 0;
	std::string str, color;
	int rgb[3]; 
	str = naviData["rgb"].str();

    for (unsigned id = 0; id < str.length(); id++)
	{
		if(str[id] != ',')
			color.insert(color.end(),str[id]);
		else
		{
			rgb[idRGB++] = atoi(color.c_str());
			color = "";
		}
	}
	rgb[idRGB] = atoi(color.c_str());

	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	ModifiedMaterial* material = object->getModifiedMaterial();
	if( material != 0 )
	{
		material->setDiffuse( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
		if(mLockAmbientDiffuse)
		{
			material->useAddedColour(true);
			material->setAmbient( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
			//material->setAddedColour( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
		}
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarColorSpecular(const NaviData& naviData)
{
	unsigned idRGB = 0;
	std::string str, color;
	int rgb[3]; 
	str = naviData["rgb"].str();

    for (unsigned id = 0; id < str.length(); id++)
	{
		if(str[id] != ',')
			color.insert(color.end(),str[id]);
		else
		{
			rgb[idRGB++] = atoi(color.c_str());
			color = "";
		}
	}
	rgb[idRGB] = atoi(color.c_str());

	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	ModifiedMaterial* material = object->getModifiedMaterial();
	if( material != 0 )
		material->setSpecular( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarColorLockAmbientDiffuse(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("$('lockAmbientdiffuse').checked");
	mLockAmbientDiffuse = (value == "true")?true:false;
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarDoubleSide(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("$('doubleSide').checked");

	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	MaterialPtr mat = object->getModifiedMaterial()->getOwner();
	mat->getTechnique(0)->getPass(0)->setCullingMode( (value == "true")?CULL_NONE:CULL_CLOCKWISE );
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropShininess(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("shininess.getValue()");

	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	ModifiedMaterial* material = object->getModifiedMaterial();
	if( material != 0 )
		material->setShininess( atoi(value.c_str())*1.28 );
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropTransparency(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("transparency.getValue()");

	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	ModifiedMaterial* material = object->getModifiedMaterial();
	if( material != 0 )
	{
//        material->setSceneBlendType(SBT_TRANSPARENT_ALPHA);
        material->setAlpha( atoi(value.c_str())/100. );
//		material->getOwner()->getTechnique(0)->getPass(0)->setDepthWriteEnabled( false );
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropResetColour(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("transparency.getValue()");

	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	object->resetColour();

	// update colour sliders
	avatarTabberLoad( 2 );
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropScrollU(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("scrollU.getValue()");

	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	ModifiedMaterial* material = object->getModifiedMaterial();
	if( material != 0 )
	{
		Ogre::Vector2 UV = material->getTextureScroll();
		material->setTextureScroll(atoi(value.c_str())/100. - .5, UV.y);
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropScrollV(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("scrollV.getValue()");

	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	ModifiedMaterial* material = object->getModifiedMaterial();
	if( material != 0 )
	{
		Ogre::Vector2 UV = material->getTextureScroll();
		material->setTextureScroll(UV.x, atoi(value.c_str())/100. - .5);
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropScaleU(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("scaleU.getValue()");

	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	ModifiedMaterial* material = object->getModifiedMaterial();
	if( material != 0 )
	{
		Ogre::Vector2 UV = material->getTextureScale();
		material->setTextureScale( atoi(value.c_str())/100. + .5, UV.y );
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropScaleV(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("scaleV.getValue()");

	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	ModifiedMaterial* material = object->getModifiedMaterial();
	if( material != 0 )
	{
		Ogre::Vector2 UV = material->getTextureScale();
		material->setTextureScale( UV.x, atoi(value.c_str())/100. + .5 );
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropRotateU(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("rotateU.getValue()");

	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	ModifiedMaterial* material = object->getModifiedMaterial();

	if( material != 0 )
		material->setTextureRotate( Ogre::Radian(atoi(value.c_str())/100.*Math::TWO_PI) );
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropTextureAdd(const NaviData& naviData)
{
	char * PathTexture = FileBrowser::displayWindowForLoading( 
			"Image Files (*.png;*.bmp;*.jpg)\0*.png;*.bmp;*.jpg\0", string("") ); 
	
	if (PathTexture != NULL )
	{
		CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
		ModifiableMaterialObject* object;

		int type = AvatarEditor::getSingletonPtr()->selectType;
		if( type == 2 ) // Goody
			object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
		else //if( type <= 1 ) // BodyPart
			object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

		String TextureFilePath (PathTexture);

		//Create the new OGRE texture with the file selected :
		TexturePtr PtrTexture = TextureManager::getSingleton().load( TextureFilePath, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		//Add texture for the object (with obj->mModifiedMaterialManager)
		object->addTexture(PtrTexture);

		avatarUpdateTextures( object );
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropTextureRemove(const NaviData& naviData)
{
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;
	
	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	//get selected texture :
	if( object->getModifiableMaterialObjectBase()->isTextureModifiable() )
	{
		// remove the old texture and set the previous texture as current
		object->removeTexture( object->getCurrentTexture() );
	}

	avatarUpdateTextures( object );
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropTexturePrev(const NaviData& naviData)
{
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	object->setNextTextureAsCurrent();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropTextureNext(const NaviData& naviData)
{
	CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

	object->setPreviousTextureAsCurrent();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarUpdateTextures(ModifiableMaterialObject* pObject)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	std::string texturePath, text;
	String str;
	TexturePtr texture; 
	Ogre::Image image;

	// Go back to the main directory
	_chdir(mNavigator->getAvatarEditor()->mExecPath.c_str());
	
	// create a temporary forlder for the thumbnail textures
#ifdef WIN32
	CreateDirectory( "NaviLocal\\NaviTmpTexture", NULL );
#else
	system( "md NaviLocal\\NaviTmpTexture" );
#endif

	// update Navi interface
	text = "textTabTextures = \"";
	//if( pObject->isTextureModifiable() )
	{
		TextureVectorIterator tvIter = pObject->getModifiableMaterialObjectBase()->getTextureIterator();
		while( tvIter.hasMoreElements() )
		{
			texture = tvIter.getNext();
			texturePath = texture->getName();
			Path path(texturePath);
			size_t begin = path.getFormatedPath().find_last_of( '\\' );
			size_t end = path.getFormatedPath().find_last_of( '.' );
			std::string fileName( path.getFormatedPath(), begin+1, end-begin-1 );
			fileName += ".jpg";

			str = ResourceGroupManager::getSingleton().findGroupContainingResource(texturePath);

			//text += "<img src='file:///d:\\test.jpg";
			text += "<img src='./NaviTmpTexture/";
			text += fileName;
			text +=	"' width=128 height=128/>	";
	
			vector<std::string> files;
			SOLlistDirectoryFiles( "NaviLocal\\NaviTmpTexture\\", &files );
			vector<std::string>::iterator iter = files.begin();
			bool found = false;
			while( iter != files.end() )
			{
				if( (*iter) ==  fileName )
				{
					found = true;
					break;
				}
				iter++;
			}
			files.clear();
			if( !found )
			{
				image.load( texturePath, str);
				image.resize( 128, 128 );
				image.save( "NaviLocal\\NaviTmpTexture\\" + fileName );
			}
		}
	}
		
	text += "\"";
	navi->evaluateJS(text);

	// Go back to the main directory
	_chdir(mNavigator->getAvatarEditor()->mExecPath.c_str());
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropSound(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::avatarPropSound()");
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);
	Avatar* user = mNavigator->getUserAvatar();

    float minDist = atof(naviData["minDist"].str().c_str());
    float maxDist = atof(naviData["maxDist"].str().c_str());
    if (minDist > maxDist) minDist = maxDist;
	navi->evaluateJS("$('Sound3DMinDistValue').value=sound3DMinDist.getValue()");
	navi->evaluateJS("$('Sound3DMaxDistValue').value=sound3DMaxDist.getValue()");
    user->setVoiceDistances(minDist, maxDist);
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarUpdateSliders(Vector3 pos, Vector3 ori, Vector3 scale)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AVATARPROP]);

	navi->evaluateJS("posX.onchange = function() {}");
	navi->evaluateJS("posY.onchange = function() {}");
	navi->evaluateJS("posZ.onchange = function() {}");
	navi->evaluateJS("oriX.onchange = function() {}");
	navi->evaluateJS("oriY.onchange = function() {}");
	navi->evaluateJS("oriZ.onchange = function() {}");
	navi->evaluateJS("scaleX.onchange = function() {}");
	navi->evaluateJS("scaleY.onchange = function() {}");
	navi->evaluateJS("scaleZ.onchange = function() {}");

	// position
	pos = pos * 100;
	navi->evaluateJS("posX.setValue(" + StringConverter::toString(int(pos.x)) + ")");
	navi->evaluateJS("posY.setValue(" + StringConverter::toString(int(pos.y)) + ")");
	navi->evaluateJS("posZ.setValue(" + StringConverter::toString(int(pos.z)) + ")");

	// orientation
	ori = ori * 100;
	navi->evaluateJS("oriX.setValue(" + StringConverter::toString(int(ori.x)) + ")");
	navi->evaluateJS("oriY.setValue(" + StringConverter::toString(int(ori.y)) + ")");
	navi->evaluateJS("oriZ.setValue(" + StringConverter::toString(int(ori.z)) + ")");

	// scale
	scale = scale * 100;
	navi->evaluateJS("scaleX.setValue(" + StringConverter::toString(int(scale.x)) + ")");
	navi->evaluateJS("scaleY.setValue(" + StringConverter::toString(int(scale.y)) + ")");
	navi->evaluateJS("scaleZ.setValue(" + StringConverter::toString(int(scale.z)) + ")");

	navi->evaluateJS("posX.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'posX'}).send()}");
	navi->evaluateJS("posY.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'posY'}).send()}");
	navi->evaluateJS("posZ.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'posZ'}).send()}");
	navi->evaluateJS("oriX.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'oriX'}).send()}");
	navi->evaluateJS("oriY.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'oriY'}).send()}");
	navi->evaluateJS("oriZ.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'oriZ'}).send()}");
	navi->evaluateJS("scaleX.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'scaleX'}).send()}");
	navi->evaluateJS("scaleY.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'scaleY'}).send()}");
	navi->evaluateJS("scaleZ.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'scaleZ'}).send()}");

	// display the orientation sliders
	//navi->evaluateJS("$('oriX').style.display='block'");
	//navi->evaluateJS("$('oriY').style.display='block'");
	//navi->evaluateJS("$('oriZ').style.display='block'");
}
//-------------------------------------------------------------------------------------
#ifdef UIDEBUG
//-------------------------------------------------------------------------------------
void NavigatorGUI::debugCommand(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::debugCommand()");

    // Get message to send
    std::string cmd;
    std::string params;
    cmd = naviData["cmd"].str();
    params = naviData["params"].str();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "cmd=%s, params=%s", cmd.c_str(), params.c_str());

    // Push debug command
    DebugHelpers::debugCommands[String(cmd)] = String(params);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::navCommand(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::navCommand()");

    // Get command
    std::string cmd;
    cmd = naviData["cmd"].str();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "cmd=%s", cmd.c_str());

#ifdef DEMO_NAVI2
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_DEBUG]);
    NaviLibrary::Navi* naviDemoNavi2 = mNaviMgr->getNavi("WWW_demoNavi2Video");
    if (naviDemoNavi2 == 0) return;
    if (cmd == "back")
        naviDemoNavi2->navigateBack();
    else if (cmd == "forward")
        naviDemoNavi2->navigateForward();
    else if (cmd == "stop")
        naviDemoNavi2->navigateStop();
    else if (cmd == "go")
    {
    	std::string url = navi->evaluateJS("$('inputUrl').value");
        naviDemoNavi2->navigateTo(url);
    }
#endif
}
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