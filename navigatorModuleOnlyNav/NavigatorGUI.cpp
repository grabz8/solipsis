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

#include "NavigatorGUI.h"
#include "Navigator.h"
#include "NavigatorFrameListener.h"
#include "DebugHelpers.h"
#include <OgreTimer.h>
#include <CTLog.h>
#include <CTStringHelpers.h>
#include <CTNetSocket.h>
#include <CTSystem.h>
#include <Navi.h>
#include <Modeler.h>
#include <AvatarEditor.h>
#include <CharacterManager.h>
#include <Character.h>
#include <CharacterInstance.h>
#include <VoiceEngineManager.h>
#include "Avatar.h"

#ifdef _MSC_VER
 #pragma warning (disable:4355)
#endif // _MSC_VER

using namespace Solipsis;
using namespace CommonTools;

const std::string NavigatorGUI::ms_NavisNames[] = {
    "uimsgbox",
    "uilogin",
    "uiworlds",
    "uiinfows",
    "uioptions",
    "uiauthentfb",
    "uiauthentws",
    "uimainmenu",
    "uistatusbar",
    "uichat",
    "uiabout",
    "uicommands",
    "uictxtavatar",
    "uictxtwww",
    "uictxtswf",
    "uictxtvlc",
    "uictxtvnc",
    "uimdlrmain",
    "uimdlrprop",
    "uimdlrscenefromtext",
    "uiavatarmain",
    "uiavatarprop",
#ifdef UIDEBUG
    "uidebug"
#endif
};

const std::string NavigatorGUI::ms_ModelerErrors[] = {
    "You have to select an object3D.",
    "This Texture is already open.",
    "File not found.",
	"Something went wrong with declarative modeling. Please re-formulate your text."
};

//-------------------------------------------------------------------------------------
NavigatorGUI::NavigatorGUI(Navigator* navigator) :
    mNavigator(navigator),
    mCurrentNavi(-1),
    mCurrentCtxtPanel(-1),
    mCurrentNaviCreationDate(0),
    mStatusBarDisplayDate(0),
    mLoginInfosText(""),
    mMsgBoxDisplayed(MBD_NONE),
    mFacebook(0),
    mWorldsServerEventListener(this)
{
    // Initializing Navi
    mNaviMgr = new NaviLibrary::NaviManager(mNavigator->getRenderWindowPtr(), "NaviLocal", ".");

    // Add ourself as a Window listener
    WindowEventUtilities::addWindowEventListener(mNavigator->getRenderWindowPtr(), this);

    for (int n=0;n<NAVI_COUNT;n++)
        mNavisStates[n] = NSNotCreated;
}

//-------------------------------------------------------------------------------------
NavigatorGUI::~NavigatorGUI()
{
    // Remove ourself as a Window listener
    WindowEventUtilities::removeWindowEventListener(mNavigator->getRenderWindowPtr(), this);

    // Hide previous Navi UI
    hidePreviousNavi();

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
    if (luaL_loadfile(luaState, "lua\\defaultGUI.lua") != 0)
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

    // Worlds server page loaded ?
    if (((mCurrentNavi == NAVI_WORLDS) ||
         (mCurrentNavi == NAVI_AUTHENTWS) ||
         (mCurrentNavi == NAVI_INFOWS)) &&
        (mCurrentNaviCreationDate != 0) &&
        (now - mCurrentNaviCreationDate > (unsigned long)mNavigator->getWorldsServerTimeout()*1000))
        worldsServerError();
    // Status bar update
    if ((mStatusBarDisplayDate != 0) && (now - mStatusBarDisplayDate > 8*1000))
    {
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_STATUSBAR]);
        if (navi == 0) return;
        if (navi->getVisibility())
            navi->hide(true);
        mStatusBarDisplayDate = 0;
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::windowResized(RenderWindow* rw)
{
    mNaviMgr->resetAllPositions();
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
void NavigatorGUI::showMessageBox(const std::string& titleText, const std::string& msgText, MsgBoxButtons buttons, MsgBoxIcon icon)
{
    if (mNavisStates[NAVI_MSGBOX] != NSCreated)
        hideMessageBox();
    switchLuaNavi(NAVI_MSGBOX, true);
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MSGBOX]);
    navi->setModal(true);
    mMsgBoxTitleText = titleText;
    mMsgBoxMsgText = msgText;
    mMsgBoxButtons = buttons;
    mMsgBoxIcon = icon;
    navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::messageBoxPageLoaded));
    navi->bind("response", NaviDelegate(this, &NavigatorGUI::messageBoxResponse));
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::hideMessageBox()
{
    if (mNavisStates[NAVI_MSGBOX] != NSCreated) return;
    switchLuaNavi(NAVI_MSGBOX, true);
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isMessageBoxVisible()
{
    if (mNavisStates[NAVI_MSGBOX] != NSCreated) return false;
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MSGBOX]);
    return ((navi != 0) && navi->getVisibility());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::login()
{
    // Hide previous Navi UI
    hidePreviousNavi();

    // Destroy context Navi UI panel
    contextDestroy();
    destroyNavi(NAVI_CTXTAVATAR);
    destroyNavi(NAVI_CTXTWWW);
    // GILLES BEGIN
    destroyNavi(NAVI_CTXTSWF);
    // GILLES END
    destroyNavi(NAVI_CTXTVLC);
    destroyNavi(NAVI_CTXTVNC);

    // Destroy Navi UI status bar
    mStatusBarDisplayDate = 0;
    destroyNavi(NAVI_STATUSBAR);

    // Destroy Navi UI chat panel
    destroyNavi(NAVI_CHAT);

    // Destroy Navi UI about panel
    destroyNavi(NAVI_ABOUT);

    // Destroy Navi UI commands
    destroyNavi(NAVI_COMMANDS);

	// Hide the modeler panels
	modelerMainUnload();
	// Hide the avatar panels
	avatarMainUnload();

#ifdef UIDEBUG
    // Destroy UI debug
    destroyNavi(NAVI_DEBUG);
#endif

    if (mNavisStates[NAVI_LOGIN] == NSNotCreated)
    {
        // Create Navi UI login
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(ms_NavisNames[NAVI_LOGIN], "local://uilogin.html", NaviPosition(Center), 400, 300);
        navi->setMovable(false);
        navi->setAutoUpdateOnFocus(true);
        navi->setMaxUPS(24);
        navi->hide();
        navi->setMask("uilogin.png");
        navi->setOpacity(0.75f);
        navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::loginPageLoaded));
	    navi->bind("world", NaviDelegate(this, &NavigatorGUI::world));
	    navi->bind("connect", NaviDelegate(this, &NavigatorGUI::connect));
	    navi->bind("options", NaviDelegate(this, &NavigatorGUI::options));
	    navi->bind("quit", NaviDelegate(this, &NavigatorGUI::quit));
        mNavisStates[NAVI_LOGIN] = NSCreated;
	}

    // Set next Navi UI
    mCurrentNavi = NAVI_LOGIN;
    mCurrentNaviCreationDate = 0;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::inWorld()
{
    // Hide previous Navi UI
    hidePreviousNavi();

    switchLuaNavi(NAVI_MAINMENU);
#ifdef UIDEBUG
    mNaviMgr->getNavi(ms_NavisNames[NAVI_MAINMENU])->bind("debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
#endif

    // Create Navi UI status bar
    // Lua
    if (mNavisStates[NAVI_STATUSBAR] == NSNotCreated)
    {
        if (!mNavigator->getNavigatorLua()->call("createGUI", "%s", ms_NavisNames[NAVI_STATUSBAR].c_str()))
            throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create GUI called " + ms_NavisNames[NAVI_STATUSBAR], "NavigatorGUI::inWorld()"); 
        mNavisStates[NAVI_STATUSBAR] = NSCreated;
    }

    // Set next Navi UI
    mCurrentNavi = NAVI_MAINMENU;
    mCurrentNaviCreationDate = 0;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::applyLoginDatas()
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_LOGIN]);
    if (navi == 0) return;
	std::string login = navi->evaluateJS("$('inputLogin').value");
	std::string pwd = navi->evaluateJS("$('inputPwd').value");
    if ((login != mNavigator->getLogin()) || (pwd != mNavigator->getPwd()))
        mNavigator->setNodeId("");
    mNavigator->setLogin(login);
    mNavigator->setPwd(pwd);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::setStatusBarText(const std::string& statusText)
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_STATUSBAR]);
    navi->evaluateJS("$('statusbarText').innerHTML = '" + statusText + "'");
    mStatusBarDisplayDate = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
    if (!navi->getVisibility())
        navi->show(true);
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
void NavigatorGUI::contextShow(int x, int y, NaviPanel ctxtPanel, const String& params)
{
    if (mCurrentCtxtPanel != -1)
        contextHide();
    // Create Navi UI context
    // Lua
    if (!mNavigator->getNavigatorLua()->call("createGUI", "%s%d%d%s", ms_NavisNames[ctxtPanel].c_str(), x, y, params.c_str()))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorGUI::contextShow() Unable to create GUI called %s", ms_NavisNames[ctxtPanel].c_str());
        return;
    }
    mNavisStates[ctxtPanel] = NSCreated;
    mCurrentCtxtPanel = ctxtPanel;
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isContextVisible()
{
    if (mCurrentCtxtPanel == -1) return false;
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[mCurrentCtxtPanel]);
    return ((navi != 0) && navi->getVisibility());
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isContextFocused()
{
    if (mCurrentCtxtPanel == -1) return false;
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[mCurrentCtxtPanel]);
    return ((navi != 0) && (navi == mNaviMgr->getFocusedNavi()));
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::contextHide()
{
    if (mCurrentCtxtPanel == -1) return;
    if (mNavisStates[mCurrentCtxtPanel] == NSNotCreated) return;

    // Hide Navi UI context
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[mCurrentCtxtPanel]);
    navi->hide();
    mCurrentCtxtPanel = -1;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::contextDestroy()
{
    if (mCurrentCtxtPanel == -1) return;
    if (mNavisStates[mCurrentCtxtPanel] == NSNotCreated) return;

    // Destroy Navi UI context
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[mCurrentCtxtPanel]);
    mNaviMgr->destroyNavi(navi);
    mNavisStates[mCurrentCtxtPanel] = NSNotCreated;
    mCurrentCtxtPanel = -1;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainShow()
{
    if (mNavisStates[NAVI_MODELERPROP] == NSCreated)
        modelerPropHide();

	if (mNavisStates[NAVI_MODELERSCENEFROMTEXT] == NSCreated)
        modelerSceneFromTextUnload();

    if (mNavisStates[NAVI_MODELERMAIN] == NSNotCreated)
    {
        // Create Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(ms_NavisNames[NAVI_MODELERMAIN], "local://uimdlrmain.html", NaviPosition(TopRight), 256, 512);
        navi->setMovable(true);
        navi->hide();
        navi->setMask("uimdlrmain.png");//Eliminate the black shadow at the margin of the menu
        navi->setOpacity(0.75f);
        navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::naviToShowPageLoaded));
	    navi->bind("FileImport", NaviDelegate(this, &NavigatorGUI::modelerMainFileImport));
	    navi->bind("FileSave", NaviDelegate(this, &NavigatorGUI::modelerMainFileSave));
	    navi->bind("FileExit", NaviDelegate(this, &NavigatorGUI::modelerMainFileExit));
		navi->bind("CreatePlane", NaviDelegate(this, &NavigatorGUI::modelerMainCreatePlane)); 
		navi->bind("CreateBox", NaviDelegate(this, &NavigatorGUI::modelerMainCreateBox)); 
		navi->bind("CreateCorner", NaviDelegate(this, &NavigatorGUI::modelerMainCreateCorner)); 
		navi->bind("CreatePyramid", NaviDelegate(this, &NavigatorGUI::modelerMainCreatePyramid));
		navi->bind("CreatePrism", NaviDelegate(this, &NavigatorGUI::modelerMainCreatePrism));
		navi->bind("CreateCylinder", NaviDelegate(this, &NavigatorGUI::modelerMainCreateCylinder)); 
		navi->bind("CreateHalfCylinder", NaviDelegate(this, &NavigatorGUI::modelerMainCreateHalfCylinder)); 
		navi->bind("CreateCone", NaviDelegate(this, &NavigatorGUI::modelerMainCreateCone)); 
		navi->bind("CreateHalfCone", NaviDelegate(this, &NavigatorGUI::modelerMainCreateHalfCone)); 
		navi->bind("CreateSphere", NaviDelegate(this, &NavigatorGUI::modelerMainCreateSphere)); 
		navi->bind("CreateHalfSphere", NaviDelegate(this, &NavigatorGUI::modelerMainCreateHalfSphere)); 
		navi->bind("CreateTorus", NaviDelegate(this, &NavigatorGUI::modelerMainCreateTorus)); 
		navi->bind("CreateTube", NaviDelegate(this, &NavigatorGUI::modelerMainCreateTube)); 
		navi->bind("CreateRing", NaviDelegate(this, &NavigatorGUI::modelerMainCreateRing)); 
		navi->bind("CreateSceneFromText", NaviDelegate(this, &NavigatorGUI::modelerMainCreateSceneFromText)); 

		navi->bind("ActionDelete", NaviDelegate(this, &NavigatorGUI::modelerActionDelete)); 
		navi->bind("ActionMove", NaviDelegate(this, &NavigatorGUI::modelerActionMove)); 
		navi->bind("ActionRotate", NaviDelegate(this, &NavigatorGUI::modelerActionRotate)); 
		navi->bind("ActionScale", NaviDelegate(this, &NavigatorGUI::modelerActionScale)); 
		navi->bind("ActionLink", NaviDelegate(this, &NavigatorGUI::modelerActionLink)); 
		navi->bind("ActionProperties", NaviDelegate(this, &NavigatorGUI::modelerActionProperties)); 
  
		mNavisStates[NAVI_MODELERMAIN] = NSCreated;
    }
    else
        mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERMAIN])->show(true);

    mNavigator->startModeling();

    Modeler *modeler = mNavigator->getModeler();
    if (modeler)
        modeler->lockSelection(false);
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isModelerMainVisible()
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERMAIN]);
    return ((navi != 0) && navi->getVisibility());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainHide()
{
    if (!isModelerMainVisible()) return;
    mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERMAIN])->hide();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainUnload()
{
    if (mNavisStates[NAVI_MODELERMAIN] != NSNotCreated)
    {
        // Destroy Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERMAIN]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[NAVI_MODELERMAIN] = NSNotCreated;

		mNavigator->endModeling();
		modelerPropUnload();
		modelerSceneFromTextUnload();

		// Remove temporary files & folder of the thumbnails
		std::string path ( "NaviLocal\\NaviTmpTexture" );
		std::vector<std::string> fileList;

		if( SOLisDirectory( String( Modeler::getSingletonPtr()->mExecPath + "\\" + path ).c_str() ) )
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
void NavigatorGUI::modelerPropShow()
{
	if (mNavisStates[NAVI_MODELERPROP] == NSNotCreated)
	{
        // Reset the remoteMRL on local IP address with UDP
        CommonTools::NetSocket::IPAddressVector myIPAddesses;
        if (!CommonTools::NetSocket::getMyIP(myIPAddesses))
            myIPAddesses.push_back("");
        std::string firstLocalIP = myIPAddesses.front();

		// Create Navi UI modeler
		NaviLibrary::Navi* navi = mNaviMgr->createNavi(ms_NavisNames[NAVI_MODELERPROP], "local://uimdlrprop.html?localIP=" + firstLocalIP, NaviPosition(TopRight), 512, 512);
		navi->setMovable(true);
		navi->hide();
		navi->setMask("uimdlrprop.png");//Eliminate the black shadow at the margin of the menu
		navi->setOpacity(0.75f);

		// page loaded
		navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::modelerPropPageLoaded));
        navi->bind("pageClosed", NaviDelegate(this, &NavigatorGUI::modelerPropPageClosed));

		// generic function for all the binds
		//navi->bind("MdlrProperties", NaviDelegate(this, &NavigatorGUI::modelerProperties));

		// detect a changement on the properties tabber
		navi->bind("ClickOnTabber", NaviDelegate(this, &NavigatorGUI::modelerTabberChange));

		// properties
		navi->bind("MdlrObjectName", NaviDelegate(this, &NavigatorGUI::modelerPropObjectName));
		navi->bind("MdlrCreator", NaviDelegate(this, &NavigatorGUI::modelerPropCreator));
		navi->bind("MdlrOwner", NaviDelegate(this, &NavigatorGUI::modelerPropOwner));
		navi->bind("MdlrGroup", NaviDelegate(this, &NavigatorGUI::modelerPropGroup));
		navi->bind("MdlrDescription", NaviDelegate(this, &NavigatorGUI::modelerPropDescription));
		navi->bind("MdlrTags", NaviDelegate(this, &NavigatorGUI::modelerPropTags));
		navi->bind("MdlrModification", NaviDelegate(this, &NavigatorGUI::modelerPropModification));
		navi->bind("MdlrCopy", NaviDelegate(this, &NavigatorGUI::modelerPropCopy));
		// model
		navi->bind("MdlrTaperX", NaviDelegate(this, &NavigatorGUI::modelerPropTaperX));
		navi->bind("MdlrTaperY", NaviDelegate(this, &NavigatorGUI::modelerPropTaperY));
		navi->bind("MdlrTopShearX", NaviDelegate(this, &NavigatorGUI::modelerPropTopShearX));
		navi->bind("MdlrTopShearY", NaviDelegate(this, &NavigatorGUI::modelerPropTopShearY));
		navi->bind("MdlrTwistBegin", NaviDelegate(this, &NavigatorGUI::modelerPropTwistBegin));
		navi->bind("MdlrTwistEnd", NaviDelegate(this, &NavigatorGUI::modelerPropTwistEnd));
		navi->bind("MdlrDimpleBegin", NaviDelegate(this, &NavigatorGUI::modelerPropDimpleBegin));
		navi->bind("MdlrDimpleEnd", NaviDelegate(this, &NavigatorGUI::modelerPropDimpleEnd));
		navi->bind("MdlrPathBegin", NaviDelegate(this, &NavigatorGUI::modelerPropPathCutBegin));
		navi->bind("MdlrPathEnd", NaviDelegate(this, &NavigatorGUI::modelerPropPathCutEnd));
		navi->bind("MdlrHoleX", NaviDelegate(this, &NavigatorGUI::modelerPropHoleSizeX));
		navi->bind("MdlrHoleY", NaviDelegate(this, &NavigatorGUI::modelerPropHoleSizeY));
		navi->bind("MdlrHollowShape", NaviDelegate(this, &NavigatorGUI::modelerPropHollowShape));
		navi->bind("MdlrSkew", NaviDelegate(this, &NavigatorGUI::modelerPropSkew));
		navi->bind("MdlrRevolution", NaviDelegate(this, &NavigatorGUI::modelerPropRevolution));
		navi->bind("MdlrRadiusDelta", NaviDelegate(this, &NavigatorGUI::modelerPropRadiusDelta));
		navi->bind("ActionUndo", NaviDelegate(this, &NavigatorGUI::modelerActionUndo));
		// material
		navi->bind("MdlrAmbient", NaviDelegate(this, &NavigatorGUI::modelerColorAmbient));
		navi->bind("MdlrDiffuse", NaviDelegate(this, &NavigatorGUI::modelerColorDiffuse));
		navi->bind("MdlrSpecular", NaviDelegate(this, &NavigatorGUI::modelerColorSpecular));
		navi->bind("MdlrLockAmbientDiffuse", NaviDelegate(this, &NavigatorGUI::modelerColorLockAmbientDiffuse));
		navi->bind("MdlrDoubleSide", NaviDelegate(this, &NavigatorGUI::modelerDoubleSide));
		navi->bind("MdlrShininess", NaviDelegate(this, &NavigatorGUI::modelerPropShininess));
		navi->bind("MdlrTransparency", NaviDelegate(this, &NavigatorGUI::modelerPropTransparency));
		navi->bind("MdlrScrollU", NaviDelegate(this, &NavigatorGUI::modelerPropScrollU));
		navi->bind("MdlrScrollV", NaviDelegate(this, &NavigatorGUI::modelerPropScrollV));
		navi->bind("MdlrScaleU", NaviDelegate(this, &NavigatorGUI::modelerPropScaleU));
		navi->bind("MdlrScaleV", NaviDelegate(this, &NavigatorGUI::modelerPropScaleV));
		navi->bind("MdlrRotateU", NaviDelegate(this, &NavigatorGUI::modelerPropRotateU));
		navi->bind("MdlrAddTexture", NaviDelegate(this, &NavigatorGUI::modelerPropTextureAdd));
		navi->bind("MdlrRemoveTexture", NaviDelegate(this, &NavigatorGUI::modelerPropTextureRemove));
		navi->bind("MdlrApplyTexture", NaviDelegate(this, &NavigatorGUI::modelerPropTextureApply));
		navi->bind("MdlrPrevTexture", NaviDelegate(this, &NavigatorGUI::modelerPropTexturePrev));
		navi->bind("MdlrNextTexture", NaviDelegate(this, &NavigatorGUI::modelerPropTextureNext));
		navi->bind("MdlrApplyWWWTexture", NaviDelegate(this, &NavigatorGUI::modelerPropWWWTextureApply));
        navi->bind("MdlrApplySWFTexture", NaviDelegate(this, &NavigatorGUI::modelerPropSWFTextureApply));
		navi->bind("MdlrApplyVLCTexture", NaviDelegate(this, &NavigatorGUI::modelerPropVLCTextureApply));
        navi->bind("MdlrVLCMrlBrowse", NaviDelegate(this, &NavigatorGUI::modelerPropVLCMrlBrowse));
		navi->bind("MdlrApplyVNCTexture", NaviDelegate(this, &NavigatorGUI::modelerPropVNCTextureApply));
		// 3D
		navi->bind("MdlrPositionX", NaviDelegate(this, &NavigatorGUI::modelerPropPositionX));
		navi->bind("MdlrPositionY", NaviDelegate(this, &NavigatorGUI::modelerPropPositionY));
		navi->bind("MdlrPositionZ", NaviDelegate(this, &NavigatorGUI::modelerPropPositionZ));
		navi->bind("MdlrOrientationX", NaviDelegate(this, &NavigatorGUI::modelerPropOrientationX));
		navi->bind("MdlrOrientationY", NaviDelegate(this, &NavigatorGUI::modelerPropOrientationY));
		navi->bind("MdlrOrientationZ", NaviDelegate(this, &NavigatorGUI::modelerPropOrientationZ));
		navi->bind("MdlrScaleX", NaviDelegate(this, &NavigatorGUI::modelerPropScaleX));
		navi->bind("MdlrScaleY", NaviDelegate(this, &NavigatorGUI::modelerPropScaleY));
		navi->bind("MdlrScaleZ", NaviDelegate(this, &NavigatorGUI::modelerPropScaleZ));
		navi->bind("MdlrCollision", NaviDelegate(this, &NavigatorGUI::modelerPropCollision));
		navi->bind("MdlrGravity", NaviDelegate(this, &NavigatorGUI::modelerPropGravity));

		mNavisStates[NAVI_MODELERPROP] = NSCreated;
	}
	else //if(!isModelerMainVisible())
	{
		mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP])->show(true);

		// Update the properties panel from the selected object datas
		modelerTabberLoad(1);
	}

    Modeler *modeler = mNavigator->getModeler();
	if (modeler)
	{
		modeler->lockSelection(true);

		if (!modeler->isSelectionEmpty())
		{
			// hide the gizmos axes
			modeler->lockGizmo(0);
			modeler->getSelection()->mTransformation->showGizmosMove(false);
			modeler->getSelection()->mTransformation->showGizmosRotate(false);
			modeler->getSelection()->mTransformation->showGizmosScale(false);
		}

        // Goto the tab 'Model' (1 on 0..6)
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
        navi->evaluateJS("document.getElementById('myTabber').tabber.tabShow(1)");
	}
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isModelerPropVisible()
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
    return ((navi != 0) && navi->getVisibility());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropHide()
{
    if (!isModelerPropVisible()) return;
    mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP])->hide();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropUnload()
{
    if (mNavisStates[NAVI_MODELERPROP] != NSNotCreated)
    {
        // Destroy Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[NAVI_MODELERPROP] = NSNotCreated;

		// Update the selected objet dats from the properties panel
		modelerTabberSave();

        Modeler *modeler = mNavigator->getModeler();
		if (modeler)
			modeler->lockSelection(false);
    }
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
				text = navi->evaluateJS("decToHex(" + StringConverter::toString(material->getDiffus().r * 255) + ")");
				text += navi->evaluateJS("decToHex(" + StringConverter::toString(material->getDiffus().g * 255) + ")");
				text += navi->evaluateJS("decToHex(" + StringConverter::toString(material->getDiffus().b * 255) + ")");
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
//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerUpdateCommand(Object3D::Command pCommand, Object3D* pObject3D)
{
	if( mNavigator->getModeler()->updateCommand( pCommand, pObject3D ) )
	{
		modelerUpdateDeformationSliders();

		// TODO 
		//modelerAddNewDeformation( cmdOld );	// add new button to the deformation stack
		// OR ??
		//modelerAddNewDeformation( cmdNew );	// add new button to the deformation stack
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerUpdateDeformationSliders()
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	Object3D* obj = mNavigator->getModeler()->getSelected();

	// break the callback from the interface sliders
	navi->evaluateJS("taperX.onchange = function() {}");
	navi->evaluateJS("taperY.onchange = function() {}");
	navi->evaluateJS("topShearX.onchange = function() {}");
	navi->evaluateJS("topShearY.onchange = function() {}");
	navi->evaluateJS("twistBegin.onchange = function() {}");
	navi->evaluateJS("twistEnd.onchange = function() {}");
	navi->evaluateJS("dimpleBegin.onchange = function() {}");
	navi->evaluateJS("dimpleEnd.onchange = function() {}");
	navi->evaluateJS("pathCutBegin.onchange = function() {}");
	navi->evaluateJS("pathCutEnd.onchange = function() {}");
	navi->evaluateJS("holeSizeX.onchange = function() {}");
	navi->evaluateJS("holeSizeY.onchange = function() {}");
	navi->evaluateJS("skew.onchange = function() {}");
	navi->evaluateJS("revolution.onchange = function() {}");
	navi->evaluateJS("radiusDelta.onchange = function() {}");

	// properties tab
	navi->evaluateJS("taperX.setValue(" + StringConverter::toString(obj->getTaperX()*100) + ")");
	navi->evaluateJS("taperY.setValue(" + StringConverter::toString(obj->getTaperY()*100) + ")");
	navi->evaluateJS("topShearX.setValue(" + StringConverter::toString(obj->getTopShearX()*100) + ")");
	navi->evaluateJS("topShearY.setValue(" + StringConverter::toString(obj->getTopShearY()*100) + ")");
	navi->evaluateJS("twistBegin.setValue(" + StringConverter::toString(obj->getTwistBegin()*100) + ")");
	navi->evaluateJS("twistEnd.setValue(" + StringConverter::toString(obj->getTwistEnd()*100) + ")");
	navi->evaluateJS("dimpleBegin.setValue(" + StringConverter::toString(obj->getDimpleBegin()*100) + ")");
	navi->evaluateJS("dimpleEnd.setValue(" + StringConverter::toString(obj->getDimpleEnd()*100) + ")");
	navi->evaluateJS("pathCutBegin.setValue(" + StringConverter::toString(obj->getPathCutBegin()*100) + ")");
	navi->evaluateJS("pathCutEnd.setValue(" + StringConverter::toString(obj->getPathCutEnd()*100) + ")");
	navi->evaluateJS("holeSizeX.setValue(" + StringConverter::toString(obj->getHoleSizeX()*100) + ")");
	navi->evaluateJS("holeSizeY.setValue(" + StringConverter::toString(obj->getHoleSizeY()*100) + ")");

// TODO : uncomment this ->		navi->evaluateJS("document.getElementById('hollowShape').value = '1');"
	navi->evaluateJS("skew.setValue(" + StringConverter::toString(obj->getSkew()*100) + ")");
	navi->evaluateJS("revolution.setValue(" + StringConverter::toString(obj->getRevolutions()*1) + ")");
	navi->evaluateJS("radiusDelta.setValue(" + StringConverter::toString(obj->getRadiusDelta()*100) + ")");

	// link the callback from the interface sliders
	navi->evaluateJS("taperX.onchange = function() {elementClicked('MdlrTaperX')}");
	navi->evaluateJS("taperY.onchange = function() {elementClicked('MdlrTaperY')}");
	navi->evaluateJS("topShearX.onchange = function() {elementClicked('MdlrTopShearX')}");
	navi->evaluateJS("topShearY.onchange = function() {elementClicked('MdlrTopShearY')}");
	navi->evaluateJS("twistBegin.onchange = function() {elementClicked('MdlrTwistBegin')}");
	navi->evaluateJS("twistEnd.onchange = function() {elementClicked('MdlrTwistEnd')}");
	navi->evaluateJS("dimpleBegin.onchange = function() {elementClicked('MdlrDimpleBegin')}");
	navi->evaluateJS("dimpleEnd.onchange = function() {elementClicked('MdlrDimpleEnd')}");
	navi->evaluateJS("pathCutBegin.onchange = function() {elementClicked('MdlrPathBegin')}");
	navi->evaluateJS("pathCutEnd.onchange = function() {elementClicked('MdlrPathEnd')}");
	navi->evaluateJS("holeSizeX.onchange = function() {elementClicked('MdlrHoleX')}");
	navi->evaluateJS("holeSizeY.onchange = function() {elementClicked('MdlrHoleY')}");
	navi->evaluateJS("skew.onchange = function() {elementClicked('MdlrSkew')}");
	navi->evaluateJS("revolution.onchange = function() {elementClicked('MdlrRevolution')}");
	navi->evaluateJS("radiusDelta.onchange = function() {elementClicked('MdlrRadiusDelta')}");
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerUpdateTextures()
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);

    Modeler *modeler = mNavigator->getModeler();
	Object3D* obj = modeler->getSelected();
	
	std::string texturePath, text;
	String str;
	Ogre::Image image;

	// Go back to the main directory
	_chdir(modeler->mExecPath.c_str());
	
	// create a temporary forlder for the thumbnail textures
#ifdef WIN32
	CreateDirectory( "NaviLocal\\NaviTmpTexture", NULL );
#else
	system( "md NaviLocal\\NaviTmpTexture" );
#endif






/*
    // update Navi interface
    TextureExtParamsMap* textureExtParamsMap2 = obj->getMaterialManager()->getTextureExtParamsMap(obj->getMaterialManager()->getCurrentTexture());
    if (textureExtParamsMap2)
    {
        TextureExtParamsMap::iterator it = textureExtParamsMap2->find("plugin");
        if ((it->second == "vlc") || (it->second == "swf") || (it->second == "www"))
            text = "document.getElementById('imgSelec').setAttribute('style','background-image:url(__" + it->second + ".jpg)')";
        else
            text = "document.getElementById('imgSelec').setAttribute('style','background-image:url(__default_color.jpg)')";    
    }
    else // It is a picture
    {
        	texturePath = obj->getMaterialManager()->getCurrentTexture()->getName();
		    Path path(texturePath);
		    size_t begin = path.getFormatedPath().find_last_of( '\\' );
		    size_t end = path.getFormatedPath().find_last_of( '.' );
		    std::string fileName( path.getFormatedPath(), begin+1, end-begin-1 );
		    fileName += ".jpg";

		    str = ResourceGroupManager::getSingleton().findGroupContainingResource(texturePath);

		    //"<img src='file:///d:\\test.jpg";
		    text += "<img src='./solTmpTexture/";
		    text += fileName;
		    text +=	"' width=128 height=128/>";

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
            text = "document.getElementById('imgSelec').setAttribute('style','background-image:url(./NaviTmpTexture/" + fileName + ")')";    
    }
    navi->evaluateJS(text);
*/




	/*
	text = "textTabTextures = \"";
	text += "<img src='./color/blank.jpg' width=128 height=128/>	";
	text += "<img src='file:///z:/5.png' width=128 height=128/>	";
	text += "\"";
	*/

    TexturePtr texture; 
	// update Navi interface
	text = "textTabTextures = \"";

    //text += "<img src='./NaviLocal/__default_color.jpg' width=128 height=128/>";
    text += "<img src='./__default_color.jpg' width=100 height=100/> ";

	for(int t=1; t<obj->getMaterialManager()->getNbTexture(); t++)
	{
		texture = obj->getMaterialManager()->getTexture(t);
        TextureExtParamsMap* textureExtParamsMap = obj->getMaterialManager()->getTextureExtParamsMap(texture);
        if (textureExtParamsMap != 0)
        {
            //text += "<img src='./NaviLocal/";
            //text += (*textureExtParamsMap)["type"] + "_texture.jpg";
            //text +=	"' width=128 height=128/>";

            TextureExtParamsMap::iterator it = textureExtParamsMap->find("plugin");
            if ((it->second == "vlc") || (it->second == "swf") || (it->second == "www"))
            {
                //text = "document.getElementById('imgSelec').setAttribute('style','background-image:url(__" + it->second + ".jpg)')";
                //text += "<img src='./NaviLocal/__";
                text += "<img src='./__";
		        //text += (*textureExtParamsMap)["type"] + "_texture.jpg";
                text += it->second;
		        text +=	".jpg' width=100 height=100/> ";
            }
            /*
            else
            {
                //text = "document.getElementById('imgSelec').setAttribute('style','background-image:url(__default_color.jpg)')";    
                //text += "<img src='./NaviLocal/__default_color.jpg' width=128 height=128/>";
                text += "<img src='./__default_color.jpg' width=128 height=128/>";
            }
            */
        }
        else
        {
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
		    text +=	"' width=100 height=100/> ";

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
    //navi->evaluateJS("updateTexture()");
    //navi->evaluateJS("updTexture()");
    //navi->evaluateJS("document.getElementById('divTexture').innerHTML=textTabTextures;"





    // Update WWW/VLC/VNC panels
    navi->evaluateJS("resetWWWVLCVNCtabs()");
    TextureExtParamsMap* textureExtParamsMap = obj->getCurrentTextureExtParamsMap();
    if ((textureExtParamsMap != 0) && !textureExtParamsMap->empty())
    {
        String plugin = (*textureExtParamsMap)["plugin"];
        if (plugin == "www")
        {
            navi->evaluateJS("document.getElementById('MaterialWWWUrl').value = '" + (*textureExtParamsMap)["url"] + "'");
            navi->evaluateJS("document.getElementById('MaterialWWWWidth').value = '" + (*textureExtParamsMap)["width"] + "'");
            navi->evaluateJS("document.getElementById('MaterialWWWHeight').value = '" + (*textureExtParamsMap)["height"] + "'");
            navi->evaluateJS("document.getElementById('MaterialWWWFps').value = '" + (*textureExtParamsMap)["frames_per_second"] + "'");
        }
        else if (plugin == "vlc")
        {
            std::string mrl = (*textureExtParamsMap)["mrl"];
            StringHelpers::replaceSubStr(mrl, "\\", "\\\\");
            NaviUtilities::encodeURIComponent(StringHelpers::convertStringToWString((*textureExtParamsMap)["mrl"]));
            navi->evaluateJS("$('MaterialVLCMrl').value = '" + mrl + "'");
            navi->evaluateJS("$('MaterialVLCWidth').value = '" + (*textureExtParamsMap)["width"] + "'");
            navi->evaluateJS("$('MaterialVLCHeight').value = '" + (*textureExtParamsMap)["height"] + "'");
            navi->evaluateJS("$('MaterialVLCFps').value = '" + (*textureExtParamsMap)["frames_per_second"] + "'");
            navi->evaluateJS("$('MaterialVLCParams').value = '" + (*textureExtParamsMap)["vlc_params"] + "'");
            mrl = (*textureExtParamsMap)["remote_mrl"];
            StringHelpers::replaceSubStr(mrl, "\\", "\\\\");
            navi->evaluateJS("$('MaterialVLCRemoteMrl').value = '" + mrl + "'");
        }
        else if (plugin == "vnc")
        {
            String address = (*textureExtParamsMap)["address"];
            String password = (*textureExtParamsMap)["password"];
            std::string host, port;
            CommonTools::StringHelpers::getURLHostPort(address, host, port);
            String::size_type p = password.find_first_of(":");
            if (p != String::npos)
                password = password.substr(p + 1);
            navi->evaluateJS("$('MaterialVNCHost').value = '" + address + "'");
            navi->evaluateJS("$('MaterialVNCPort').value = '" + port + "'");
            navi->evaluateJS("$('MaterialVNCPwd').value = '" + password + "'");
        }
    }

	// Go back to the main directory
	_chdir(modeler->mExecPath.c_str());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerAddNewDeformation(Object3D::Command pCommand)
{
	Object3D* obj = mNavigator->getModeler()->getSelected();
	size_t nbCommands = mDeformButton.size();

	// create new name :
	// ...

	// create new window :
	// ...

	// get the button label
	std::string deformName;
	switch( pCommand)
	{
	case Object3D::TRANSLATE: deformName = "TRANSLATE"; break;
	case Object3D::ROTATE: deformName = "ROTATE"; break;
	case Object3D::SCALE: deformName = "SCLAE"; break;
	case Object3D::TAPERX:
	case Object3D::TAPERY: deformName = "TAPER"; break;
	case Object3D::TOP_SHEARX:
	case Object3D::TOP_SHEARY: deformName = "TOP_SHEAR"; break;
	case Object3D::TWIST_BEGIN:
	case Object3D::TWIST_END: deformName = "TWIST"; break;
	case Object3D::PATH_CUT_BEGIN:
	case Object3D::PATH_CUT_END: deformName = "PATH_CUT"; break;
	case Object3D::DIMPLE_BEGIN:
	case Object3D::DIMPLE_END: deformName = "DIMPLE"; break;
	case Object3D::HOLE_SIZEX:
	case Object3D::HOLE_SIZEY: 
	case Object3D::HOLLOW_SHAPE: deformName = "HOLE"; break;
	case Object3D::SKEW:
	case Object3D::REVOLUTION:
	case Object3D::RADIUS_DELTA: deformName = "SKEW"; break;
	default: deformName = "ERROR"; break;
	}

	// update property of this window :
	// ...

	// update size :
	mDeformButton.push_back( deformName );
}

/*
void NavigatorGUI::modelerProperties(const NaviData& naviData)
{
	std::string slider;
    slider = naviData["slider"].str();
}
*/
//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerTabberChange(const NaviData& naviData)
{
	unsigned tab;
	tab = atoi(naviData["tab"].str().c_str());
 	modelerTabberLoad (tab);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerTabberLoad(unsigned pTab)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);

	// get the current object3D
	Object3D* obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		ColourValue col;
		Ogre::Vector2 UV;
		std::string text;
		unsigned c,e;

		switch( pTab )
		{
		case 0:	// properties tab
            navi->evaluateJS("document.getElementById('objectUid').value = '" + obj->getEntityUID() + "'");
			navi->evaluateJS("document.getElementById('objectName').value = '" + obj->getName() + "'");
			navi->evaluateJS("document.getElementById('creator').value = '" + obj->getCreator() + "'");
			navi->evaluateJS("document.getElementById('owner').value = '" + obj->getOwner() + "'");
			navi->evaluateJS("document.getElementById('group').value = '" + obj->getGroup() + "'");
			// TODO : remplacer tous les retours chariot par \\n
			text = obj->getDesc();
			for(c=0, e=0; e<text.length(); c++,e++)
			{
				if(text[c] == '\n' || text[c] == '\r') 
				{
					text[c] = 'n';
					text.insert(c, "\\");
					c+=1;
				}
			}
			navi->evaluateJS("document.getElementById('description').value = '" + text + "'");
			// TODO : remplacer tous les retours chariot par \\n
			text = obj->getTags();
			for(c=0, e=0; e<text.length(); c++,e++)
			{
				if(text[c] == '\n' || text[c] == '\r') 
				{
					text[c] = 'n';
					text.insert(c, "\\");
					c+=1;
				}
			}
			navi->evaluateJS("document.getElementById('tags').value = '" + text + "'");
			navi->evaluateJS("document.getElementById('modification').checked = " + obj->getCanBeModified()?"true":"false");
			navi->evaluateJS("document.getElementById('copy').checked = " + obj->getCanBeCopied()?"true":"false");
			break;
		case 1:	// model
			modelerUpdateDeformationSliders();
			break;
		case 2:	// material tab
			text = navi->evaluateJS("decToHex(" + StringConverter::toString(obj->getAmbient().r * 255) + ")");
			text += navi->evaluateJS("decToHex(" + StringConverter::toString(obj->getAmbient().g * 255) + ")");
			text += navi->evaluateJS("decToHex(" + StringConverter::toString(obj->getAmbient().b * 255) + ")");
			navi->evaluateJS("$S('pAmbient').background='#" + text + "'");
			text = navi->evaluateJS("decToHex(" + StringConverter::toString(obj->getDiffus().r * 255) + ")");
			text += navi->evaluateJS("decToHex(" + StringConverter::toString(obj->getDiffus().g * 255) + ")");
			text += navi->evaluateJS("decToHex(" + StringConverter::toString(obj->getDiffus().b * 255) + ")");
			navi->evaluateJS("$S('pDiffuse').background='#" + text + "'");
			text = navi->evaluateJS("decToHex(" + StringConverter::toString(obj->getSpecular().r * 255) + ")");
			text += navi->evaluateJS("decToHex(" + StringConverter::toString(obj->getSpecular().g * 255) + ")");
			text += navi->evaluateJS("decToHex(" + StringConverter::toString(obj->getSpecular().b * 255) + ")");
			navi->evaluateJS("$S('pSpecular').background='#" + text + "'");

			navi->evaluateJS("shininess.onchange = function() {}");
			navi->evaluateJS("transparency.onchange = function() {}");
			navi->evaluateJS("scrollU.onchange = function() {}");
			navi->evaluateJS("scrollV.onchange = function() {}");
			navi->evaluateJS("scaleU.onchange = function() {}");
			navi->evaluateJS("scaleV.onchange = function() {}");
			navi->evaluateJS("rotateU.onchange = function() {}");

			navi->evaluateJS("shininess.setValue(" + StringConverter::toString(Real(obj->getShininess()/128.)*100) + ")");
			navi->evaluateJS("transparency.setValue(" + StringConverter::toString(obj->getAlpha()*100) + ")");
			UV = obj->getMaterialManager()->getTextureScroll();
			navi->evaluateJS("scrollU.setValue(" + StringConverter::toString(UV.x*100+50) + ")");
			navi->evaluateJS("scrollV.setValue(" + StringConverter::toString(UV.y*100+50) + ")");
			UV = obj->getMaterialManager()->getTextureScale();
			navi->evaluateJS("scaleU.setValue(" + StringConverter::toString(UV.x*100-50) + ")");
			navi->evaluateJS("scaleV.setValue(" + StringConverter::toString(UV.y*100-50) + ")");
			navi->evaluateJS("rotateU.setValue(" + StringConverter::toString(obj->getMaterialManager()->getTextureRotate()/Math::TWO_PI*100) + ")");

			navi->evaluateJS("shininess.onchange = function() {elementClicked('MdlrShininess')}");
			navi->evaluateJS("transparency.onchange = function() {elementClicked('MdlrTransparency')}");
			navi->evaluateJS("scrollU.onchange = function() {elementClicked('MdlrScrollU')}");
			navi->evaluateJS("scrollV.onchange = function() {elementClicked('MdlrScrollV')}");
			navi->evaluateJS("scaleU.onchange = function() {elementClicked('MdlrScaleU')}");
			navi->evaluateJS("scaleV.onchange = function() {elementClicked('MdlrScaleV')}");
			navi->evaluateJS("rotateU.onchange = function() {elementClicked('MdlrRotateU')}");
			{
				MaterialPtr mat = obj->getMaterialManager()->getModifiedMaterial()->getOwner();
				CullingMode mode = mat->getTechnique(0)->getPass(0)->getCullingMode();
				navi->evaluateJS("document.getElementById('doubleSide').checked = " + (mode == CULL_NONE)?"true":"false" );
			}
			modelerUpdateTextures();
			break;
		case 3:	// 3D tab
			navi->evaluateJS("document.getElementById('positionX').value = " + StringConverter::toString(obj->getPosition().x));
			navi->evaluateJS("document.getElementById('positionY').value = " + StringConverter::toString(obj->getPosition().y));
			navi->evaluateJS("document.getElementById('positionZ').value = " + StringConverter::toString(obj->getPosition().z));
			navi->evaluateJS("document.getElementById('orientationX').value = " + StringConverter::toString(obj->getRotate().x));
			navi->evaluateJS("document.getElementById('orientationY').value = " + StringConverter::toString(obj->getRotate().y));
			navi->evaluateJS("document.getElementById('orientationZ').value = " + StringConverter::toString(obj->getRotate().z));
			navi->evaluateJS("document.getElementById('scaleX').value = " + StringConverter::toString(obj->getScale().x));
			navi->evaluateJS("document.getElementById('scaleY').value = " + StringConverter::toString(obj->getScale().y));
			navi->evaluateJS("document.getElementById('scaleZ').value = " + StringConverter::toString(obj->getScale().z));
			navi->evaluateJS("document.getElementById('gravity').checked = " + obj->getEnableGravity()?"true":"false");
			navi->evaluateJS("document.getElementById('collision').checked = %s" + obj->getCollisionnable()?"true":"false");

			text = "document.getElementById('info').value = 'Vertex count : '+";
			text += StringConverter::toString(obj->getVertexCount());
			text += "+'\\nTriangle count : '+";
			text += StringConverter::toString(obj->getTriCount());
			text += "+'\\nPrimitives count : '+";
			text += StringConverter::toString(obj->getPrimitivesCount());
			text += "+'\\nMesh size : '+";
			text += StringConverter::toString(obj->getMeshSize().x);
			text += "+','+";
			text += StringConverter::toString(obj->getMeshSize().y);
			text += "+','+";
			text += StringConverter::toString(obj->getMeshSize().z);
			navi->evaluateJS(text);
			break;
		case 4:	// ...
			break;
		case 5:	// ...
			break;
		case 6: // ...
			break;
		}
		//mNaviMgr->Update();
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerTabberSave()
{
	/*
	std::string value;
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);

	// properties tab
	value = navi->evaluateJS("document.getElementById('objectName').value");
	value = navi->evaluateJS("document.getElementById('creator').value");
	value = navi->evaluateJS("document.getElementById('owner').value");
	value = navi->evaluateJS("document.getElementById('group').value");
	value = navi->evaluateJS("document.getElementById('description').value");
	value = navi->evaluateJS("document.getElementById('tags').value");
	value = navi->evaluateJS("document.getElementById('modification').checked");
	value = navi->evaluateJS("document.getElementById('copy').checked");
	// model
	// (none)
	// material tab
	// (none)
	// 3D tab
	value = navi->evaluateJS("document.getElementById('positionX').value");
	value = navi->evaluateJS("document.getElementById('positionY').value");
	value = navi->evaluateJS("document.getElementById('positionZ').value");
	value = navi->evaluateJS("document.getElementById('orientationX').value");
	value = navi->evaluateJS("document.getElementById('orientationX').value");
	value = navi->evaluateJS("document.getElementById('orientationZ').value");
	value = navi->evaluateJS("document.getElementById('scaleX').value");
	value = navi->evaluateJS("document.getElementById('scaleY').value");
	value = navi->evaluateJS("document.getElementById('scaleZ').checked");
	value = navi->evaluateJS("document.getElementById('gravity').checked");
	value = navi->evaluateJS("document.getElementById('collision').value");
	*/
}

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

//-------------------------------------------------------------------------------------
void NavigatorGUI::messageBoxPageLoaded(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::messageBoxPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MSGBOX]);

    navi->evaluateJS("$('titleText').innerHTML = '" + mMsgBoxTitleText + "'");
    navi->evaluateJS("$('msgText').innerHTML = '" + mMsgBoxMsgText + "'");
    navi->evaluateJS("setButtons(" + StringHelpers::toString(mMsgBoxButtons) + ")");
    navi->evaluateJS("setIcon(" + StringHelpers::toString(mMsgBoxIcon) + ")");

    // Show Navi UI message box
    if (mNavisStates[NAVI_MSGBOX] == NSCreated)
        navi->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::messageBoxResponse(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::messageBoxResponse()");

    hideMessageBox();

    switch (mMsgBoxDisplayed)
    {
    case MBD_WORLDSSERVERERROR:
    case MBD_AUTHENTFBERROR:
    case MBD_AUTHENTWSERROR:
        // Return to Navi UI login
        login();
        break;
    case MBD_WORLDSSERVERCOMPATIBILITYERROR:
        // Display the Worlds Server info page
        worldsServerInfo();
        break;
    }
    mMsgBoxDisplayed = MBD_NONE;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::loginPageLoaded(const NaviData& naviData)
{
    char txt[256];

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::loginPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_LOGIN]);

    // Set current values
    sprintf(txt, "$('inputLogin').value = '%s'", mNavigator->getLogin().c_str());
    navi->evaluateJS(txt);
    sprintf(txt, "$('inputPwd').value = '%s'", mNavigator->getPwd().c_str());
    navi->evaluateJS(txt);
    // Show/Hide password input
    sprintf(txt, "$('pwd').style.visibility = '%s'", (mNavigator->getAuthentType() == ATSolipsis) ? "visible" : "hidden");
    navi->evaluateJS(txt);
    std::string world = mNavigator->getWorldAddress();
    sprintf(txt, "$('worldText').innerHTML = '%s'", world.empty() ? "Choose a world ..." : world.c_str());
    navi->evaluateJS(txt);
    // Disable World button if no world server specified
    sprintf(txt, "$('worldButton').disabled = %s", mNavigator->getWorldsServerAddress().empty() ? "'disabled'" : "null");
    navi->evaluateJS(txt);

	// Show Navi UI login
    if (mNavisStates[NAVI_LOGIN] == NSCreated)
        navi->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::world(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::world()");

    applyLoginDatas();

    // Hide previous Navi UI
    hidePreviousNavi();

    if (mNavisStates[NAVI_WORLDS] == NSNotCreated)
    {
        // Create Navi UI worlds
        // Prepare the url to the world server uiworlds.html page
        std::string uiworldsUrl = "http://" + mNavigator->getWorldsServerAddress() + "/uiworlds.html";
        uiworldsUrl += "?navVersion=" + StringHelpers::toHexString(mNavigator->getVersion());
        std::string localWorldHost = mNavigator->getLocalWorldAddress();
        // Add the local world ?
        if (!mNavigator->getLocalWorldAddress().empty())
            uiworldsUrl += "&localWorld=" + mNavigator->getLocalWorldAddress();
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(ms_NavisNames[NAVI_WORLDS], "", NaviPosition(Center), 256, 256);
        navi->setMovable(false);
        navi->hide();
        navi->setOpacity(0.75f);
	    navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::naviToShowPageLoaded));
	    navi->bind("ok", NaviDelegate(this, &NavigatorGUI::worldOk));
	    navi->bind("cancel", NaviDelegate(this, &NavigatorGUI::worldCancel));
        // Add 1 event listener to detect network errors
        navi->addEventListener(&mWorldsServerEventListener);
        navi->navigateTo(uiworldsUrl);
        mNavisStates[NAVI_WORLDS] = NSCreated;
    }

    // Set next Navi UI
    mCurrentNavi = NAVI_WORLDS;
    mCurrentNaviCreationDate = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::WorldsServerEventListener::onNavigateComplete(Navi *caller, const std::string &url, int responseCode)
{
    if (responseCode >= 400 && responseCode < 600)
        if (responseCode == 409)
            mNavigatorGUI->worldsServerCompatibilityError();
        else
            mNavigatorGUI->worldsServerError();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::worldsServerCompatibilityError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldsServerCompatibilityError()");

    showMessageBox("Compatibility error", "Your Navigator (version " + StringHelpers::getVersionString(mNavigator->getVersion()) + ") is not compatible<br/>with this Worlds Server !<br/><br/>Upgrade your Navigator and connect again.", MBB_OK, MBB_ERROR);
    mMsgBoxDisplayed = MBD_WORLDSSERVERCOMPATIBILITYERROR;
    mCurrentNaviCreationDate = 0;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::worldsServerError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldsServerError()");

    std::string wsHost, wsPort;
    CommonTools::StringHelpers::getURLHostPort(mNavigator->getWorldsServerAddress(), wsHost, wsPort);
    showMessageBox("Network error", "Unable to connect to the Worlds Server !<br/>Check your Internet connection and configure your firewall<br/>(TCP port " + wsPort + ").", MBB_OK, MBB_ERROR);
    mMsgBoxDisplayed = MBD_WORLDSSERVERERROR;
    mCurrentNaviCreationDate = 0;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::worldOk(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldOk()");

    std::string world = naviData["world"].str();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldOk() world=%s", world.c_str());
    std::string worldHost, worldPort;
    CommonTools::StringHelpers::getURLHostPort(world, worldHost, worldPort);
    mNavigator->setWorldAddress(world);

    // extended datas associated to the world server : voice IP server, VNC server, VLC server, ...
    if (naviData.exists("voipServer"))
    {
        std::string voipServer = naviData["voipServer"].str();
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldOk() voipServer=%s", voipServer.c_str());
        mNavigator->setVoIPServerAddress(voipServer);
    }
    if (naviData.exists("vncServer"))
    {
        std::string vncServer = naviData["vncServer"].str();
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldOk() vncServer=%s", vncServer.c_str());
    }
    if (naviData.exists("vlcServer"))
    {
        std::string vlcServer = naviData["vlcServer"].str();
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldOk() vlcServer=%s", vlcServer.c_str());
    }

    // Return to Navi UI login
    login();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::worldCancel(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldCancel()");

    // Return to Navi UI login
    login();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::worldsServerInfo()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldsServerInfo()");

    // Hide previous Navi UI
    hidePreviousNavi();

    if (mNavisStates[NAVI_INFOWS] == NSNotCreated)
    {
        // Create Navi UI worlds server info
        // Prepare the url to the world server uiinfows.html page
        std::string uiinfowsUrl = "http://" + mNavigator->getWorldsServerAddress() + "/uiinfows.html";
        uiinfowsUrl += "?navVersion=" + StringHelpers::toHexString(mNavigator->getVersion());
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(ms_NavisNames[NAVI_INFOWS], "", NaviPosition(Center), 256, 256);
        navi->setMovable(false);
        navi->hide();
        navi->setOpacity(0.75f);
	    navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::naviToShowPageLoaded));
	    navi->bind("ok", NaviDelegate(this, &NavigatorGUI::worldsServerInfoOk));
        // Add 1 event listener to detect network errors
        navi->addEventListener(&mWorldsServerEventListener);
        navi->navigateTo(uiinfowsUrl);
        mNavisStates[NAVI_INFOWS] = NSCreated;
    }

    // Set next Navi UI
    mCurrentNavi = NAVI_INFOWS;
    mCurrentNaviCreationDate = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::worldsServerInfoOk(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::worldsServerInfoOk()");

    // Return to Navi UI login
    login();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::connect(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::connect()");

    applyLoginDatas();

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_LOGIN]);

    // Check
    bool validLogin = StringHelpers::isAValidLogin(mNavigator->getLogin());
    if (!validLogin)
    {
        // Malformed login
        navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid login ...'");
        return;
    }
    if (mNavigator->getWorldAddress().empty())
    {
        // Malformed login
        navi->evaluateJS("$('infosText').innerHTML = 'Select a valid world ...'");
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
            authentFacebook();
            break;
        case ATSolipsis:
            bool validPwd = StringHelpers::isAValidPassword(mNavigator->getPwd());
            if (!validPwd)
            {
                // Malformed pwd
                navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid password ...'");
                return;
            }
            authentWorldsServer(mNavigator->getPwd());
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
void NavigatorGUI::options(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::options()");

    applyLoginDatas();

    // Hide previous Navi UI
    hidePreviousNavi();

    if (mNavisStates[NAVI_OPTIONS] == NSNotCreated)
    {
        // Create Navi UI options
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(ms_NavisNames[NAVI_OPTIONS], "local://uioptions.html", NaviPosition(Center), 400, 400);
        navi->setMovable(false);
        navi->hide();
        navi->setMask("uioptions.png");
        navi->setOpacity(0.75f);
	    navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::optionsPageLoaded));
	    navi->bind("ok", NaviDelegate(this, &NavigatorGUI::optionsOk));
	    navi->bind("back", NaviDelegate(this, &NavigatorGUI::optionsBack));
        mNavisStates[NAVI_OPTIONS] = NSCreated;
    }

    // Set next Navi UI
    mCurrentNavi = NAVI_OPTIONS;
    mCurrentNaviCreationDate = 0;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::quit(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::quit()");

    mNavigator->quit();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::optionsPageLoaded(const NaviData& naviData)
{
    char txt[256];

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::optionsPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_OPTIONS]);

    // Set current values
    navi->evaluateJS("setInputState('checkboxGeneralResetDisplayConfig', null, null)");
    bool facebookAvailable = (
        !mNavigator->getFacebookApiKey().empty() &&
        !mNavigator->getFacebookSecret().empty() &&
        !mNavigator->getFacebookServer().empty() &&
        !mNavigator->getFacebookLoginUrl().empty());
    sprintf(txt, "setInputState('radioIdAuthentTypeFacebook', %s, %s)", !facebookAvailable ? "'disabled'" : "null", (mNavigator->getAuthentType() == ATFacebook) ? "'checked'" : "null");
    navi->evaluateJS(txt);
    sprintf(txt, "setInputState('radioIdAuthentTypeSolipsis', %s, %s)", mNavigator->getWorldsServerAddress().empty() ? "'disabled'" : "null", (mNavigator->getAuthentType() == ATSolipsis) ? "'checked'" : "null");
    navi->evaluateJS(txt);
    sprintf(txt, "setInputState('radioIdAuthentTypeFixed', %s, %s)", mNavigator->getFixedNodeId().empty() ? "'disabled'" : "null", (mNavigator->getAuthentType() == ATFixed) ? "'checked'" : "null");
    navi->evaluateJS(txt);
    std::string wsHost, wsPort;
    CommonTools::StringHelpers::getURLHostPort(mNavigator->getWorldsServerAddress(), wsHost, wsPort);
    sprintf(txt, "$('inputWSHost').value = '%s'", wsHost.c_str());
    navi->evaluateJS(txt);
    sprintf(txt, "$('inputWSPort').value = '%s'", wsPort.c_str());
    navi->evaluateJS(txt);
    std::string peerHost, peerPort;
    CommonTools::StringHelpers::getURLHostPort(mNavigator->getPeerAddress(), peerHost, peerPort);
    sprintf(txt, "$('inputPeerHost').value = '%s'", peerHost.c_str());
    navi->evaluateJS(txt);
    sprintf(txt, "$('inputPeerPort').value = '%s'", peerPort.c_str());
    navi->evaluateJS(txt);
    navi->evaluateJS("$('infosText').innerHTML = ''");
    std::string proxyAutoconfUrl;
    std::string proxyHttpHost;
    int proxyHttpPort;
    int proxyType;
    if (mNaviMgr->getProxyConfig(proxyType, proxyHttpHost, proxyHttpPort, proxyAutoconfUrl))
    {
        sprintf(txt, "setInputState('radioProxyTypeDirect', null, %s)", (proxyType == 0) ? "'checked'" : "null");
        navi->evaluateJS(txt);
        sprintf(txt, "setInputState('radioProxyTypeAutodetect', null, %s)", (proxyType == 4) ? "'checked'" : "null");
        navi->evaluateJS(txt);
        sprintf(txt, "setInputState('radioProxyTypeManual', null, %s)", (proxyType == 1) ? "'checked'" : "null");
        navi->evaluateJS(txt);
        sprintf(txt, "setInputState('radioProxyTypeAutoconf', null, %s)", (proxyType == 2) ? "'checked'" : "null");
        navi->evaluateJS(txt);
        sprintf(txt, "$('inputProxyHttpHost').value = '%s'", proxyHttpHost.c_str());
        navi->evaluateJS(txt);
        sprintf(txt, "$('inputProxyHttpPort').value = '%d'", proxyHttpPort);
        navi->evaluateJS(txt);
        sprintf(txt, "$('inputProxyAutoconfUrl').value = '%s'", proxyAutoconfUrl.c_str());
        navi->evaluateJS(txt);
    }
    Navigator::NavigationInterface ni=mNavigator->getNavigationInterface();
    sprintf(txt, "setInputState('radioControlKeyboardAndMouse', null, %s)", (ni==Navigator::NIMouseKeyboard) ? "'checked'" : "null");
    navi->evaluateJS(txt);
    sprintf(txt, "setInputState('radioControlWiimoteAndNunchuk', null, %s)", (ni==Navigator::NIWiimoteNunchuk) ? "'checked'" : "null");
    navi->evaluateJS(txt);
    sprintf(txt, "setInputState('radioControlWiimoteAndNunchukAndIR', null, %s)", (ni==Navigator::NIWiimoteNunchukIR) ? "'checked'" : "null");
    navi->evaluateJS(txt);

    // Show Navi UI options
    if (mNavisStates[NAVI_OPTIONS] == NSCreated)
        navi->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::optionsOk(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::optionsOk()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_OPTIONS]);

    // Get options
    bool generalResetDisplayConfig = naviData["generalResetDisplayConfig"].toBool();
    std::string radioIdAuthentType;
    radioIdAuthentType = naviData["radioIdAuthentType"].str();
	std::string wsHost;
    unsigned short wsPort;
    wsHost = naviData["wsHost"].str();
    wsPort = naviData["wsPort"].toInt();
	std::string peerHost;
    unsigned short peerPort;
    peerHost = naviData["peerHost"].str();
    peerPort = naviData["peerPort"].toInt();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "peerHost=%s, peerPort=%d", peerHost.c_str(), peerPort);
    std::string radioProxyType;
	std::string proxyHttpHost;
    std::string radioControlType;
    int proxyHttpPort;
	std::string proxyAutoconfUrl;
    radioProxyType = naviData["radioProxyType"].str();
    proxyHttpHost = naviData["proxyHttpHost"].str();
    proxyHttpPort = naviData["proxyHttpPort"].toInt();
    proxyAutoconfUrl = naviData["proxyAutoconfUrl"].str();
    radioControlType = naviData["radioControlType"].str();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "radioProxyType=%s, proxyHttpHost=%s, proxyHttpPort=%d, proxyAutoconfUrl=%s, controlType=%s", radioProxyType.c_str(), proxyHttpHost.c_str(), proxyHttpPort, proxyAutoconfUrl.c_str(), radioControlType.c_str());

    // Check
    AuthentType authentType = (AuthentType)radioIdAuthentType.c_str()[0];
    bool valid_options = true;
    if (wsHost.length() < 2)
    {
        // Bad hostname
        navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid server hostname ...'");
        valid_options = false;
    }
    if (wsPort < 0)
    {
        // Bad port
        navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid server port ...'");
        valid_options = false;
    }
    if (peerHost.length() < 2)
    {
        // Bad hostname
        navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid peer hostname ...'");
        valid_options = false;
    }
    if (peerPort < 0)
    {
        // Bad port
        navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid peer port ...'");
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
            navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid HTTP proxy Address ...'");
            valid_options = false;
        }
        if (proxyHttpPort < 0)
        {
            // Bad port
            navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid HTTP proxy Port ...'");
            valid_options = false;
        }
    }
    else // autoconf
    {
        proxyType = 2;
        if (proxyAutoconfUrl.length() == 0)
        {
            // Bad url
            navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid proxy server URL ...'");
            valid_options = false;
        }
    }

    Navigator::NavigationInterface controlType = Navigator::NIMouseKeyboard;
    if (radioControlType == "KeyboardAndMouse")
        controlType=Navigator::NIMouseKeyboard;
    else if (radioControlType == "WiimoteAndNunchuk")
        controlType=Navigator::NIWiimoteNunchuk;
    else if (radioControlType == "WiimoteAndNunchukAndIR")
        controlType=Navigator::NIWiimoteNunchukIR;

    // Valid options ?
    if (valid_options)
    {
        if (generalResetDisplayConfig)
            mNavigator->getOgreApplication()->resetDisplayConfig();
        if (authentType!= mNavigator->getAuthentType())
        {
            mNavigator->setNodeId("");
            mNavigator->setPwd("");
        }
        mNavigator->setAuthentType(authentType);
        mNavigator->setWorldsServerAddress(CommonTools::StringHelpers::getURL(wsHost, wsPort));
        mNavigator->setPeerAddress(CommonTools::StringHelpers::getURL(peerHost, peerPort));
        mNaviMgr->setProxyConfig(proxyType, proxyHttpHost, proxyHttpPort, proxyAutoconfUrl);

        mNavigator->setNavigationInterface(controlType);

        navi->evaluateJS("$('infosText').innerHTML = ''");

        // Return to Navi UI login
        login();
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::optionsBack(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::optionsBack()");

    // Return to Navi UI login
    login();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::authentFacebook()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentFacebook()");

    // Hide previous Navi UI
    hidePreviousNavi();

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
        authentFacebookError();
        return;
    }

    if (mNavisStates[NAVI_AUTHENTFB] == NSNotCreated)
    {
        // Create Navi UI authentication on Facebook
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(ms_NavisNames[NAVI_AUTHENTFB], "local://uiauthentfb.html", NaviPosition(Center), 256, 128);
        navi->setMovable(false);
        navi->hide();
        navi->setOpacity(0.75f);
	    navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::authentFacebookPageLoaded));
	    navi->bind("ok", NaviDelegate(this, &NavigatorGUI::authentFacebookOk));
	    navi->bind("cancel", NaviDelegate(this, &NavigatorGUI::authentFacebookCancel));
        mNavisStates[NAVI_AUTHENTFB] = NSCreated;
    }

    // Set next Navi UI
    mCurrentNavi = NAVI_AUTHENTFB;
    mCurrentNaviCreationDate = 0;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::authentFacebookError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentFacebookError()");

    showMessageBox("Network error", "Unable to connect to Facebook !<br/>Check your Internet connection.", MBB_OK, MBB_ERROR);
    mMsgBoxDisplayed = MBD_AUTHENTFBERROR;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::authentFacebookPageLoaded(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentFacebookPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AUTHENTFB]);

    navi->evaluateJS("$('msgText').innerHTML = 'Use browser to log on Facebook ... then press Ok'");
    // Run external Web browser on the login URL
    CommonTools::System::runExternalWebBrowser(mFacebook->getLoginUrl(mNavigator->getFacebookLoginUrl()).c_str());

	// Show Navi UI authent Facebook
    if (mNavisStates[NAVI_AUTHENTFB] == NSCreated)
        navi->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::authentFacebookOk(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentFacebookOk()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_AUTHENTFB]);

    // Session ?
    if (!mFacebook->getSession())
    {
        navi->evaluateJS("$('msgText').innerHTML = 'Unable to get session ... Are you logged ?'");
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
void NavigatorGUI::authentFacebookCancel(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentFacebookCancel()");

    // Destroy Facebook instance
    if (mFacebook != 0)
    {
        delete mFacebook;
        mFacebook = 0;
    }

    // Return to Navi UI login
    login();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::authentWorldsServer(const std::string& pwd)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentWorldsServer()");

    // Hide previous Navi UI
    hidePreviousNavi();

    if (mNavisStates[NAVI_AUTHENTWS] == NSNotCreated)
    {
        // Create Navi UI authentication on Worlds server
        // Prepare the url to the world server uiauthentws.html page
        std::string uiauthentwsUrl = "http://" + mNavigator->getWorldsServerAddress() + "/uiauthentws.html";
        uiauthentwsUrl += "?navVersion=" + StringHelpers::toHexString(mNavigator->getVersion());
        uiauthentwsUrl += "&login=" + mNavigator->getLogin() + "&pwd=" + pwd;
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(ms_NavisNames[NAVI_AUTHENTWS], "", NaviPosition(Center), 256, 128);
        navi->setMovable(false);
        navi->hide();
        navi->setOpacity(0.75f);
	    navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::naviToShowPageLoaded));
	    navi->bind("ok", NaviDelegate(this, &NavigatorGUI::authentWorldsServerOk));
        // Add 1 event listener to detect network errors
        navi->addEventListener(&mWorldsServerEventListener);
        navi->navigateTo(uiauthentwsUrl);
        mNavisStates[NAVI_AUTHENTWS] = NSCreated;
    }

    // Set next Navi UI
    mCurrentNavi = NAVI_AUTHENTWS;
    mCurrentNaviCreationDate = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::authentWorldsServerError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentWorldsServerError()");

    std::string wsHost, wsPort;
    CommonTools::StringHelpers::getURLHostPort(mNavigator->getWorldsServerAddress(), wsHost, wsPort);
    showMessageBox("Authentication error", "Authentication failed !", MBB_OK, MBB_ERROR);
    mMsgBoxDisplayed = MBD_AUTHENTWSERROR;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::authentWorldsServerOk(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentWorldsServerOk()");

    std::string result = naviData["result"].str();
    NodeId nodeId = naviData["nodeId"].str();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::authentWorldsServerOk() result=%s, nodeId=%s", result.c_str(), nodeId.c_str());
    if (nodeId.empty())
    {
        login();
        return;
    }

    mNavigator->setNodeId(XmlHelpers::convertAuthentTypeToRepr(ATSolipsis) + nodeId);
    // Call connect
    bool connected = mNavigator->connect();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainFileImport(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainFileImport()");
	
	mNavigator->mdlrXMLImport();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainFileSave(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainFileSave()");
	
	//modelerMainUnload();
	mNavigator->mdlrXMLSave();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainFileExit(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainFileExit()");

    modelerMainUnload();
    mNavigator->setCameraMode(mNavigator->getLastCameraMode());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreatePlane(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreatePlane()");
	//mNavigator->startModeling();
	mNavigator->createPlane();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateBox(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreateBox()");
	//mNavigator->startModeling();
	mNavigator->createBox();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateCorner(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreateCorner()");
	//mNavigator->startModeling();
	mNavigator->createCorner();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreatePyramid(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreatePyramid()");
	//mNavigator->startModeling();
	mNavigator->createPyramid();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreatePrism(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreatePrism()");
	//mNavigator->startModeling();
	mNavigator->createPrism();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateCylinder(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreateCylinder()");
	//mNavigator->startModeling();
	mNavigator->createCylinder();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateHalfCylinder(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreateHalfCylinder()");
	//mNavigator->startModeling();
	mNavigator->createHalfCyl();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateCone(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreateCone()");
	//mNavigator->startModeling();
	mNavigator->createCone();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateHalfCone(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreateHalfCone()");
	//mNavigator->startModeling();
	mNavigator->createHalfCone();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateSphere(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreateSphere()");
	//mNavigator->startModeling();
	mNavigator->createSphere();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateHalfSphere(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreateHalfSphere()");
	//mNavigator->startModeling();
	mNavigator->createHalfSphere();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateTorus(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreateTorus()");
	//mNavigator->startModeling();
	mNavigator->createTorus();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateTube(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreateTube()");
	//mNavigator->startModeling();
	mNavigator->createTube();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateRing(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreateRing()");
	//mNavigator->startModeling();
	mNavigator->createRing();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateSceneFromText(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerMainCreateSceneFromText()");
	modelerSceneFromTextShow();
//	mNavigator->createSceneFromText( "A red ball is on a green box." );
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionDelete(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerActionDelete()");

    Modeler *modeler = mNavigator->getModeler();
	if (modeler)
		if (!modeler->isSelectionEmpty())
        {
            //mNavigator->suppr();

            // remove the current selection
            modeler->removeSelection();
            // reset mouse picking
            mNavigator->resetMousePicking();

            // hide the gizmos axes
            modeler->getSelection()->mTransformation->showGizmosMove(false);
            modeler->getSelection()->mTransformation->showGizmosRotate(false);
            modeler->getSelection()->mTransformation->showGizmosScale(false);
			modeler->lockGizmo(0);
        }
		else
            showMessageBox("Modeler error", ms_ModelerErrors[ME_NOOBJECTSELECTED], NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionMove(const NaviData& naviData)
{
    Modeler *modeler = mNavigator->getModeler();
	if (!modeler->isSelectionEmpty())
	{
        if( modeler->isOnGizmo() == 1 )
        {
            modeler->lockGizmo( 0 );
            modeler->getSelection()->mTransformation->eventSelection();
        }
        else
        {
            modeler->lockGizmo( 1 );
            modeler->eventMove();
        }
	}
	else
        showMessageBox("Modeler error", ms_ModelerErrors[ME_NOOBJECTSELECTED], NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionRotate(const NaviData& naviData)
{
    Modeler *modeler = mNavigator->getModeler();
	if (!modeler->isSelectionEmpty())
	{
        if( modeler->isOnGizmo() == 2 )
        {
            modeler->lockGizmo( 0 );
            modeler->getSelection()->mTransformation->eventSelection();
        }
        else
        {
            modeler->lockGizmo( 2 );
            modeler->eventRotate();
        }
	}
	else
        showMessageBox("Modeler error", ms_ModelerErrors[ME_NOOBJECTSELECTED], NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionScale(const NaviData& naviData)
{
    Modeler *modeler = mNavigator->getModeler();
	if (!modeler->isSelectionEmpty())
	{
        if( modeler->isOnGizmo() == 3 )
        {
            modeler->lockGizmo( 0 );
            modeler->getSelection()->mTransformation->eventSelection();
        }
        else
        {
            modeler->lockGizmo( 3 );
            modeler->eventScale();
        }
	}
	else
        showMessageBox("Modeler error", ms_ModelerErrors[ME_NOOBJECTSELECTED], NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionLink(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerActionLink()");

    Modeler *modeler = mNavigator->getModeler();
	if (!modeler->isSelectionEmpty())
		modeler->lockLinkMode(true);
	else
        showMessageBox("Modeler error", ms_ModelerErrors[ME_NOOBJECTSELECTED], NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionProperties(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerActionProperties()");

	// Test if an Object3D has ever been created before
    Modeler *modeler = mNavigator->getModeler();
	if (!modeler->isSelectionEmpty())
		{
			// Hide the main modeler panel
			modelerMainHide();

			// Show the properties modeler panel
			modelerPropShow();
		}
	else
        showMessageBox("Modeler error", ms_ModelerErrors[ME_NOOBJECTSELECTED], NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionUndo(const NaviData& naviData)
{
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerActionUndo()");

	//mNavigator->undo();
    Modeler *modeler = mNavigator->getModeler();
	if (!modeler->isSelectionEmpty())
        modeler->getSelected()->undo();

	modelerUpdateDeformationSliders();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropPageLoaded(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerPropPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);

    // Update the properties panel from the selected object datas
    modelerTabberLoad( 0 );

    // Show Navi UI
    if (mNavisStates[NAVI_MODELERPROP] == NSCreated)
        navi->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropPageClosed(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::modelerPropPageClosed()");

    if (mNavigator->getModeler()->isSelectionLocked() && !isModelerMainVisible())
    {
        modelerPropHide();
        modelerMainShow();
    }
    else 
        modelerMainUnload();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropObjectName(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('objectName').value");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
		obj->setName(value);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropCreator(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('creator').value");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
		obj->setOwner(value.c_str());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropOwner(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('owner').value");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
		obj->setCreator(value.c_str());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropGroup(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('group').value");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
		obj->setGroup(value.c_str());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropDescription(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('description').value");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
		obj->setDesc(value.c_str());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTags(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('tags').value");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
		obj->setTags(value.c_str());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropModification(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('modification').checked");
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropCopy(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('copy').checked");
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTaperX(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("taperX.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::TAPERX, obj );
		obj->apply( Object3D::TAPERX, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTaperY(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("taperY.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::TAPERY, obj );
		obj->apply( Object3D::TAPERY, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTopShearX(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("topShearX.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::TOP_SHEARX, obj );
		obj->apply( Object3D::TOP_SHEARX, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTopShearY(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("topShearY.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::TOP_SHEARY, obj );
		obj->apply( Object3D::TOP_SHEARY, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTwistBegin(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("twistBegin.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::TWIST_BEGIN, obj );
		obj->apply( Object3D::TWIST_BEGIN, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTwistEnd(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("twistEnd.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::TWIST_END, obj );
		obj->apply( Object3D::TWIST_END, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropDimpleBegin(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("dimpleBegin.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::DIMPLE_BEGIN, obj );
		obj->apply( Object3D::DIMPLE_BEGIN, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropDimpleEnd(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("dimpleEnd.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::DIMPLE_END, obj );
		obj->apply( Object3D::DIMPLE_END, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropPathCutBegin(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("pathCutBegin.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::PATH_CUT_BEGIN, obj );
		obj->apply( Object3D::PATH_CUT_BEGIN, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropPathCutEnd(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("pathCutEnd.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::PATH_CUT_END, obj );
		obj->apply( Object3D::PATH_CUT_END, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropHoleSizeX(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("holeSizeX.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::HOLE_SIZEX, obj );
		obj->apply( Object3D::HOLE_SIZEX, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropHoleSizeY(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("holeSizeY.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::HOLE_SIZEY, obj );
		obj->apply( Object3D::HOLE_SIZEY, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropHollowShape(const NaviData& naviData)
{
	std::string value;
    value = naviData["shape"].str();

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::HOLLOW_SHAPE, obj );
		obj->apply( Object3D::HOLLOW_SHAPE, atoi(value.c_str()) );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropSkew(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("skew.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::SKEW, obj );
		obj->apply( Object3D::SKEW, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropRevolution(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("revolution.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::REVOLUTION, obj );
		obj->apply( Object3D::REVOLUTION, atoi(value.c_str())/1. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropRadiusDelta(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("radiusDelta.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::RADIUS_DELTA, obj );
		obj->apply( Object3D::RADIUS_DELTA, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerColorAmbient(const NaviData& naviData)
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

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		obj->setAmbient( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
		if(mLockAmbientDiffuse)
			obj->setDiffus( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerColorDiffuse(const NaviData& naviData)
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

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		obj->setDiffus( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
		if(mLockAmbientDiffuse)
			obj->setAmbient( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerColorSpecular(const NaviData& naviData)
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

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
		obj->setSpecular( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerColorLockAmbientDiffuse(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("$('lockAmbientdiffuse').checked");
	mLockAmbientDiffuse = (value == "true")?true:false;
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerDoubleSide(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("$('doubleSide').checked");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{	
		MaterialPtr mat = obj->getMaterialManager()->getModifiedMaterial()->getOwner();
		mat->getTechnique(0)->getPass(0)->setCullingMode( (value == "true")?CULL_NONE:CULL_CLOCKWISE );
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropShininess(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("shininess.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
		obj->setShininess( atoi(value.c_str())/1.28 );
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTransparency(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("transparency.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
        obj->setSceneBlendType(SBT_TRANSPARENT_ALPHA);
		obj->setAlpha( atoi(value.c_str())/100. );
		obj->getMaterialManager()->getModifiedMaterial()->getOwner()->getTechnique(0)->getPass(0)->setDepthWriteEnabled( false );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScrollU(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("scrollU.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		Ogre::Vector2 UV = obj->getMaterialManager()->getTextureScroll();
		obj->setTextureScroll(atoi(value.c_str())/100. - .5, UV.y);
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScrollV(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("scrollV.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		Ogre::Vector2 UV = obj->getMaterialManager()->getTextureScroll();
		obj->setTextureScroll(UV.x, atoi(value.c_str())/100. - .5);
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScaleU(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("scaleU.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		Ogre::Vector2 UV = obj->getMaterialManager()->getTextureScale();
		obj->setTextureScale( atoi(value.c_str())/100. + .5, UV.y );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScaleV(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("scaleV.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
	{
		Ogre::Vector2 UV = obj->getMaterialManager()->getTextureScale();
		obj->setTextureScale( UV.x, atoi(value.c_str())/100. + .5 );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropRotateU(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("rotateU.getValue()");

	Object3D *obj = mNavigator->getModeler()->getSelected();
	if( obj != 0 )
		obj->setTextureRotate( Ogre::Radian(atoi(value.c_str())/100.*Math::TWO_PI) );
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTextureAdd(const NaviData& naviData)
{
	char * PathTexture = FileBrowser::displayWindowForLoading( 
			"Image Files (*.png;*.bmp;*.jpg)\0*.png;*.bmp;*.jpg\0", string("") ); 
	
    Modeler *modeler = mNavigator->getModeler();
	if (PathTexture != NULL && modeler != 0)
	{
		Object3D * obj = modeler->getSelected();
		String TextureFilePath (PathTexture);

		//Create the new OGRE texture with the file selected :
		Path filePath(TextureFilePath);
        ResourceGroupManager::getSingleton().addResourceLocation(filePath.getFormatedRootPath(), "FileSystem");
        TextureExtParamsMap textureExtParamsMap;
        TexturePtr PtrTexture = modeler->loadTexture(obj->getMaterialManager(), TextureFilePath, textureExtParamsMap);

		//Test if this texture is already in the list :
		if( obj->getMaterialManager()->isPresentInList( PtrTexture ) )
		{
            showMessageBox("Modeler error", ms_ModelerErrors[ME_TEXTUREALREADYOPEN], NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);
			return;
		}

		//Add texture for the object (with obj->mModifiedMaterialManager)
		obj->addTexture(PtrTexture);

		modelerUpdateTextures();
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTextureRemove(const NaviData& naviData)
{
    Modeler *modeler = mNavigator->getModeler();
	if( modeler != 0 )
	{
		Object3D * obj = modeler->getSelected();

		//get selected texture :
		if( obj->getMaterialManager()->getNbTexture() > 1 )
		{
			TexturePtr tPtr = obj->getMaterialManager()->getCurrentTexture();
			obj->deleteTexture( tPtr );
		}

		modelerUpdateTextures();
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTextureApply(const NaviData& naviData)
{
   
    Modeler *modeler = mNavigator->getModeler();
	if( modeler != 0 )
	{
		Object3D * obj = modeler->getSelected();

		//get selected texture :
		if( obj->getMaterialManager()->getNbTexture() > 1 )
		{
			TexturePtr tPtr = obj->getMaterialManager()->getCurrentTexture();
			obj->setCurrentTexture( tPtr );
		}
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropWWWTextureApply(const NaviData& naviData)
{
    Modeler *modeler = mNavigator->getModeler();
	if( modeler != 0 )
	{
		Object3D * obj = modeler->getSelected();

        // only 1 WWW per modifiedMaterial for instance ... TODO
        ModifiedMaterialManager* modifiedMaterialManager = obj->getMaterialManager();
        TexturePtr texture;
        TextureVectorIterator tvIter = modifiedMaterialManager->getTextureIterator();
        while (tvIter.hasMoreElements())
        {
            texture = tvIter.getNext();
            TextureExtParamsMap* textureExtParamsMap = modifiedMaterialManager->getTextureExtParamsMap(texture);
            if (textureExtParamsMap == 0) continue;
            TextureExtParamsMap::const_iterator it = textureExtParamsMap->find("plugin");
            if (it == textureExtParamsMap->end()) continue;
            if (it->second == "www")
            {
                obj->deleteTexture(texture);
                break;
            }
        }

	    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	    std::string urlStr = navi->evaluateJS("$('MaterialWWWUrl').value");
	    std::string widthStr = navi->evaluateJS("$('MaterialWWWWidth').value");
	    std::string heightStr = navi->evaluateJS("$('MaterialWWWHeight').value");
	    std::string fpsStr = navi->evaluateJS("$('MaterialWWWFps').value");
        int width = atoi(widthStr.c_str());
        int height = atoi(heightStr.c_str());
        int fps = atoi(fpsStr.c_str());
        bool sound3d = true;
        float sound3dMin = 0;
        float sound3dMax = 10;

        TextureExtParamsMap textureExtParamsMap;
        textureExtParamsMap["plugin"] = "www";
        textureExtParamsMap["query_flags"] = StringConverter::toString(Navigator::QFNaviPanel);
        textureExtParamsMap["url"] = urlStr;
        textureExtParamsMap["width"] = StringConverter::toString(width);
        textureExtParamsMap["height"] = StringConverter::toString(height);
        textureExtParamsMap["frames_per_second"] = StringConverter::toString(fps);
        textureExtParamsMap["sound_params"] = (sound3d ? "3d " + StringConverter::toString(sound3dMin) + " " + StringConverter::toString(sound3dMax) : "");
        TexturePtr PtrTexture = modeler->loadTexture(obj->getMaterialManager(), "", textureExtParamsMap);

		//Test if this texture is already in the list :
		if( obj->getMaterialManager()->isPresentInList( PtrTexture ) )
		{
            showMessageBox("Modeler error", ms_ModelerErrors[ME_TEXTUREALREADYOPEN], NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);
			return;
		}

		//Add texture for the object (with obj->mModifiedMaterialManager)
		obj->addTexture(PtrTexture, textureExtParamsMap);
		obj->setCurrentTexture(PtrTexture);

        modelerUpdateTextures();
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropSWFMrlBrowse(const NaviData& naviData)
{
    std::string mrl;
    if (System::showDlgOpenFilename(mrl, "Flash Media File,(*.swf)\0*.swf\0", ""))
    {
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
        std::string mrlStr(mrl);
        StringHelpers::replaceSubStr(mrlStr, "\\", "\\\\");
        navi->evaluateJS("$('MaterialSWFMrl').value='" + mrlStr + "'");
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropSWFTextureApply(const NaviData& naviData)
{
    Modeler *modeler = mNavigator->getModeler();
	if( modeler != 0 )
	{
		Object3D * obj = modeler->getSelected();
        if( obj == 0 ) return;

        // only 1 SWF per modifiedMaterial for instance ... TODO
        ModifiedMaterialManager* modifiedMaterialManager = obj->getMaterialManager();
        TexturePtr texture;
        TextureVectorIterator tvIter = modifiedMaterialManager->getTextureIterator();
        while (tvIter.hasMoreElements())
        {
            texture = tvIter.getNext();
            TextureExtParamsMap* textureExtParamsMap = modifiedMaterialManager->getTextureExtParamsMap(texture);
            if (textureExtParamsMap == 0) continue;
            TextureExtParamsMap::const_iterator it = textureExtParamsMap->find("plugin");
            if (it == textureExtParamsMap->end()) continue;
            if (it->second == "swf")
            {
                obj->deleteTexture(texture);
                break;
            }
        }

	    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	    std::string urlStr = navi->evaluateJS("$('MaterialSWFUrl').value");
	    std::string widthStr = navi->evaluateJS("$('MaterialSWFWidth').value");
	    std::string heightStr = navi->evaluateJS("$('MaterialSWFHeight').value");
        std::string fpsStr = navi->evaluateJS("$('MaterialSWFFps').value");
        //std::string sp3dStr = navi->evaluateJS("$('MaterialSWFSP3d').checked");
        std::string spMinStr = navi->evaluateJS("$('MaterialSWFSPMin').value");
        std::string spMaxStr = navi->evaluateJS("$('MaterialSWFSPMax').value");

        int width = atoi(widthStr.c_str());
        int height = atoi(heightStr.c_str());
        int fps = atoi(fpsStr.c_str());
        bool sound3d = true;
        float sound3dMin = atof(spMinStr.c_str());
        float sound3dMax = atof(spMaxStr.c_str());

        TextureExtParamsMap textureExtParamsMap;
        textureExtParamsMap["plugin"] = "swf";
        textureExtParamsMap["query_flags"] = StringConverter::toString(Navigator::QFSWFPanel);
        textureExtParamsMap["url"] = urlStr;
        textureExtParamsMap["width"] = StringConverter::toString(width);
        textureExtParamsMap["height"] = StringConverter::toString(height);
        textureExtParamsMap["frames_per_second"] = StringConverter::toString(fps);
        textureExtParamsMap["sound_params"] = (sound3d ? "3d " + StringConverter::toString(sound3dMin) + " " + StringConverter::toString(sound3dMax) : "");

        TexturePtr PtrTexture = modeler->loadTexture(obj->getMaterialManager(), "", textureExtParamsMap);

		//Test if this texture is already in the list :
		if( obj->getMaterialManager()->isPresentInList( PtrTexture ) )
		{
            showMessageBox("Modeler error", ms_ModelerErrors[ME_TEXTUREALREADYOPEN], NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);
			return;
		}

		//Add texture for the object (with obj->mModifiedMaterialManager)
		obj->addTexture(PtrTexture, textureExtParamsMap);
		obj->setCurrentTexture(PtrTexture);

        modelerUpdateTextures();
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropVLCTextureApply(const NaviData& naviData)
{
    Modeler *modeler = mNavigator->getModeler();
	if( modeler != 0 )
	{
		Object3D * obj = modeler->getSelected();

        // only 1 VLC per modifiedMaterial for instance ... TODO
        ModifiedMaterialManager* modifiedMaterialManager = obj->getMaterialManager();
        TexturePtr texture;
        TextureVectorIterator tvIter = modifiedMaterialManager->getTextureIterator();
        while (tvIter.hasMoreElements())
        {
            texture = tvIter.getNext();
            TextureExtParamsMap* textureExtParamsMap = modifiedMaterialManager->getTextureExtParamsMap(texture);
            if (textureExtParamsMap == 0) continue;
            TextureExtParamsMap::const_iterator it = textureExtParamsMap->find("plugin");
            if (it == textureExtParamsMap->end()) continue;
            if (it->second == "vlc")
            {
                obj->deleteTexture(texture);
                break;
            }
        }

	    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	    std::string mrlStr = navi->evaluateJS("$('MaterialVLCMrl').value");
	    std::string widthStr = navi->evaluateJS("$('MaterialVLCWidth').value");
	    std::string heightStr = navi->evaluateJS("$('MaterialVLCHeight').value");
	    std::string fpsStr = navi->evaluateJS("$('MaterialVLCFps').value");
        std::string paramsStr = navi->evaluateJS("$('MaterialVLCParams').value");
        std::string remoteMrlStr = navi->evaluateJS("$('MaterialVLCRemoteMrl').value");
        std::string sp3dStr = navi->evaluateJS("$('MaterialVLCSP3d').checked");
        std::string spMinStr = navi->evaluateJS("$('MaterialVLCSPMin').value");
        std::string spMaxStr = navi->evaluateJS("$('MaterialVLCSPMax').value");
        int width = atoi(widthStr.c_str());
        int height = atoi(heightStr.c_str());
        int fps = atoi(fpsStr.c_str());
        bool sound3d = (sp3dStr == "true")?true:false;
        float sound3dMin = atof(spMinStr.c_str());
        float sound3dMax = atof(spMaxStr.c_str());

        std::string finalMrl = mrlStr;
        if (remoteMrlStr.empty() && (mrlStr.find("://") == String::npos))
        {
		    Path p(mrlStr);
            finalMrl = std::string("solTmpTexture\\") + p.getLastFileName();
            if (mrlStr.find(finalMrl) == -1) // File is not already in solTmpTexture
            {
                if (!SOLcopyFile(mrlStr.c_str(), finalMrl.c_str()))
                {
                    showMessageBox("Modeler error", "Impossible de copier le fichier dans temp", NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);                
                }
            }
        }

        TextureExtParamsMap textureExtParamsMap;
        textureExtParamsMap["plugin"] = "vlc";
        textureExtParamsMap["query_flags"] = StringConverter::toString(Navigator::QFVLCPanel);
        textureExtParamsMap["mrl"] = finalMrl;
        textureExtParamsMap["width"] = StringConverter::toString(width);
        textureExtParamsMap["height"] = StringConverter::toString(height);
        textureExtParamsMap["frames_per_second"] = StringConverter::toString(fps);
        textureExtParamsMap["vlc_params"] = paramsStr;
        textureExtParamsMap["remote_mrl"] = remoteMrlStr;
        textureExtParamsMap["sound_params"] = (sound3d ? "3d " + StringConverter::toString(sound3dMin) + " " + StringConverter::toString(sound3dMax) : "");
        TexturePtr PtrTexture = modeler->loadTexture(obj->getMaterialManager(), "", textureExtParamsMap);

		//Test if this texture is already in the list :
		if( obj->getMaterialManager()->isPresentInList( PtrTexture ) )
		{
            showMessageBox("Modeler error", ms_ModelerErrors[ME_TEXTUREALREADYOPEN], NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);
			return;
		}

		//Add texture for the object (with obj->mModifiedMaterialManager)
		obj->addTexture(PtrTexture, textureExtParamsMap);
		obj->setCurrentTexture(PtrTexture);

        modelerUpdateTextures();
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropVLCMrlBrowse(const NaviData& naviData)
{
    std::string mrl;
    if (System::showDlgOpenFilename(mrl, "Media File,(*.*)\0*.*\0", ""))
    {
	    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
        std::string mrlStr(mrl);
        StringHelpers::replaceSubStr(mrlStr, "\\", "\\\\");
        navi->evaluateJS("$('MaterialVLCMrl').innerHTML='" + mrlStr + "'");
        navi->evaluateJS("$('MaterialVLCMrl').value='" + mrlStr + "'");
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropVNCTextureApply(const NaviData& naviData)
{
    Modeler *modeler = mNavigator->getModeler();
	if( modeler != 0 )
	{
		Object3D * obj = modeler->getSelected();

        // only 1 VNC per modifiedMaterial for instance ... TODO
        ModifiedMaterialManager* modifiedMaterialManager = obj->getMaterialManager();
        TexturePtr texture;
        TextureVectorIterator tvIter = modifiedMaterialManager->getTextureIterator();
        while (tvIter.hasMoreElements())
        {
            texture = tvIter.getNext();
            TextureExtParamsMap* textureExtParamsMap = modifiedMaterialManager->getTextureExtParamsMap(texture);
            if (textureExtParamsMap == 0) continue;
            TextureExtParamsMap::const_iterator it = textureExtParamsMap->find("plugin");
            if (it == textureExtParamsMap->end()) continue;
            if (it->second == "vnc")
            {
                obj->deleteTexture(texture);
                break;
            }
        }

	    NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	    std::string hostStr = navi->evaluateJS("$('MaterialVNCHost').value");
	    std::string portStr = navi->evaluateJS("$('MaterialVNCPort').value");
	    std::string pwdStr = navi->evaluateJS("$('MaterialVNCPwd').value");
        unsigned short port = atoi(portStr.c_str());
        std::string address = "vnc://" + hostStr + ":" + StringConverter::toString(port);
        std::string password = "vncpwd:" + pwdStr;

        TextureExtParamsMap textureExtParamsMap;
        textureExtParamsMap["plugin"] = "vnc";
        textureExtParamsMap["query_flags"] = StringConverter::toString(Navigator::QFVNCPanel);
        textureExtParamsMap["address"] = address;
        textureExtParamsMap["password"] = password;
        TexturePtr PtrTexture = modeler->loadTexture(obj->getMaterialManager(), "", textureExtParamsMap);

		//Test if this texture is already in the list :
		if( obj->getMaterialManager()->isPresentInList( PtrTexture ) )
		{
            showMessageBox("Modeler error", ms_ModelerErrors[ME_TEXTUREALREADYOPEN], NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);
			return;
		}

		//Add texture for the object (with obj->mModifiedMaterialManager)
		obj->addTexture(PtrTexture, textureExtParamsMap);
		obj->setCurrentTexture(PtrTexture);

        modelerUpdateTextures();
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTexturePrev(const NaviData& naviData)
{
    Modeler *modeler = mNavigator->getModeler();
	if( modeler != 0 )
	{
		Object3D * obj = modeler->getSelected();
		obj->getMaterialManager()->setPreviousTexture();
	}
    //modelerUpdateTextures();
    modelerPropTextureApply( naviData );
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTextureNext(const NaviData& naviData)
{
    Modeler *modeler = mNavigator->getModeler();
	if( modeler != 0 )
	{
		Object3D * obj = modeler->getSelected();
		if( obj->getMaterialManager()->getNbTexture() > 1 )
			obj->getMaterialManager()->setNextTexture();
	}
    //modelerUpdateTextures();
    modelerPropTextureApply( naviData );
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropPositionX(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('positionX').value * 10000");

    Modeler *modeler = mNavigator->getModeler();
	Vector3 vec = modeler->getSelection()->getCenterPosition();
	modeler->getSelection()->moveTo(atoi(value.c_str())/10000., vec.y, vec.z);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropPositionY(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('positionY').value * 10000");

    Modeler *modeler = mNavigator->getModeler();
	Vector3 vec = modeler->getSelection()->getCenterPosition();
	modeler->getSelection()->moveTo(vec.x, atoi(value.c_str())/10000., vec.z);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropPositionZ(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('positionZ').value * 10000");

    Modeler *modeler = mNavigator->getModeler();
	Vector3 vec = modeler->getSelection()->getCenterPosition();
	modeler->getSelection()->moveTo(vec.x, vec.y, atoi(value.c_str())/10000.);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropOrientationX(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('orientationX').value * 10000");

    Modeler *modeler = mNavigator->getModeler();
	modeler->getSelection()->rotateTo(atoi(value.c_str())/10000., 0, 0);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropOrientationY(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('orientationY').value * 10000");

    Modeler *modeler = mNavigator->getModeler();
	modeler->getSelection()->rotateTo(0, atoi(value.c_str())/10000., 0);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropOrientationZ(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('orientationZ').value * 10000");

    Modeler *modeler = mNavigator->getModeler();
	modeler->getSelection()->rotateTo(0, 0, atoi(value.c_str())/10000.);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScaleX(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
    //std::string value = navi->evaluateJS("document.getElementById('scaleX').value * 10000");
    std::string valueX = navi->evaluateJS("document.getElementById('scaleX').value * 10000");
    std::string valueY = navi->evaluateJS("document.getElementById('scaleY').value * 10000");
    std::string valueZ = navi->evaluateJS("document.getElementById('scaleZ').value * 10000");

    Modeler *modeler = mNavigator->getModeler();
    //modeler->getSelection()->scaleTo(atoi(value.c_str())/10000., 1., 1. );
    modeler->getSelection()->scaleTo(atoi(valueX.c_str())/10000., atoi(valueY.c_str())/10000., atoi(valueZ.c_str())/10000.);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScaleY(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	//std::string value = navi->evaluateJS("document.getElementById('scaleY').value * 10000");
    std::string valueX = navi->evaluateJS("document.getElementById('scaleX').value * 10000");
    std::string valueY = navi->evaluateJS("document.getElementById('scaleY').value * 10000");
    std::string valueZ = navi->evaluateJS("document.getElementById('scaleZ').value * 10000");

    Modeler *modeler = mNavigator->getModeler();
	//modeler->getSelection()->scaleTo(1, atoi(value.c_str())/10000., 1);
    modeler->getSelection()->scaleTo(atoi(valueX.c_str())/10000., atoi(valueY.c_str())/10000., atoi(valueZ.c_str())/10000.);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScaleZ(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	//std::string value = navi->evaluateJS("document.getElementById('scaleZ').value * 10000");
    std::string valueX = navi->evaluateJS("document.getElementById('scaleX').value * 10000");
    std::string valueY = navi->evaluateJS("document.getElementById('scaleY').value * 10000");
    std::string valueZ = navi->evaluateJS("document.getElementById('scaleZ').value * 10000");

    Modeler *modeler = mNavigator->getModeler();
	//modeler->getSelection()->scaleTo(1, 1, atoi(value.c_str())/10000.);
    modeler->getSelection()->scaleTo(atoi(valueX.c_str())/10000., atoi(valueY.c_str())/10000., atoi(valueZ.c_str())/10000.);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropCollision(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('collision').checked");
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropGravity(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(ms_NavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('gravity').checked");
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

	std::string msg = "AvatarProp window loaded";
	MessageBox(0, msg.c_str(), "NavigatorGUI Avatar", MB_OK | MB_ICONWARNING | MB_TASKMODAL);

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
			material->setDiffus( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
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
		material->setDiffus( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
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
        material->setSceneBlendType(SBT_TRANSPARENT_ALPHA);
        material->setAlpha( atoi(value.c_str())/100. );
		material->getOwner()->getTechnique(0)->getPass(0)->setDepthWriteEnabled( false );
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
    if (mCurrentNavi != -1) {
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
    if (panel == -1) return false;
    if (mNavisStates[panel] == NSNotCreated) return false;
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(naviName);
    if (navi == 0) return false;
    if (show)
    {
        if (navi->getVisibility()) return true;
        navi->show(true);
        mCurrentNavi = panel;
    }
    else
    {
        if (!navi->getVisibility()) return true;
        navi->hide(true);
        if (mCurrentNavi == panel) mCurrentNavi = -1;
        if (mCurrentCtxtPanel == panel) mCurrentCtxtPanel = -1;
        if (mCurrentNavi == -1) mCurrentNaviCreationDate = 0;
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
			showMessageBox( "Declarative modeling error", "Current text is:<br/>'" + value + "'<br/>" + errMsg.c_str() , MBB_OK, MBB_ERROR );
		else if( warnMsg != "" )
			showMessageBox( "Declarative modeling error", "Current text is:<br/>'" + value + "'<br/>" + warnMsg.c_str() , MBB_OK, MBB_ERROR );
		else 
			showMessageBox( "Declarative modeling error", "Current text is:<br/>'" + value + "'<br/> UNKNOWN ERROR", MBB_OK, MBB_ERROR );
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
