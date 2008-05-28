#include "NavigatorGUI.h"
#include "Navigator.h"
#include "OgreHelpers.h"
#include "DebugHelpers.h"
#include "Navi.h"
#include "Modeler.h"
#include "AvatarEditor.h"
#include "Character.h"
#include "CharacterManager.h"
#include "Avatar.h"
#include "OgreExternalTextureSourceManager.h"

using namespace Solipsis;

const std::string NavigatorGUI::mNavisNames[] = {
    "uilogin",
    "uioptions",
    "uichat",
    "uicontext",
    "uimdlrmain",
    "uimdlrprop",
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
	lastTime(0.)
{
    // Initializing Navi
    mNaviMgr = new NaviLibrary::NaviManager(mNavigator->getRenderWindowPtr(), "NaviLocal", ".");

    for (int n=0;n<NAVI_COUNT;n++)
        mNavisStates[n] = NSNotCreated;
}

//-------------------------------------------------------------------------------------
NavigatorGUI::~NavigatorGUI()
{
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
    NaviCursor* defaultCursor = mouse->createCursor("default_cursor", 3, 2);
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
        OGRE_LOG("Navigator::startup() Unable to load defaultGUI.lua, error: " + String(lua_tostring(luaState, -1)));
        return false;
    }
    if (lua_pcall(luaState, 0, LUA_MULTRET, 0))
    {
        OGRE_LOG("Navigator::startup() Unable to run defaultGUI.lua, error: " + String(lua_tostring(luaState, -1)));
        return false;
    }

    return true;
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
void NavigatorGUI::login()
{
    // Hide previous Navi UI
    hidePreviousNavi();

    if (mNavisStates[NAVI_LOGIN] == NSNotCreated)
    {
        // Create Navi UI login
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(mNavisNames[NAVI_LOGIN], "local://uilogin.html", NaviPosition(Center), 400, 300);
        navi->setMovable(false);
        navi->setAutoUpdateOnFocus(true);
        navi->setMaxUPS(24);
        navi->hide();
        //navi->setMask("uilogin.png");
        navi->setOpacity(0.75f);
        navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::loginPageLoaded));
	    navi->bind("connect", NaviDelegate(this, &NavigatorGUI::connect));
	    navi->bind("options", NaviDelegate(this, &NavigatorGUI::options));
	    navi->bind("quit", NaviDelegate(this, &NavigatorGUI::quit));
		navi->bind("selectAvatar", NaviDelegate(this, &NavigatorGUI::loginSelectAvatar));
#ifdef UIDEBUG
        navi->bind("debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
#endif
        mNavisStates[NAVI_LOGIN] = NSCreated;
	}

    // Set next Navi UI
    mCurrentNavi = NAVI_LOGIN;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::inWorld()
{
    // Hide previous Navi UI
    hidePreviousNavi();

    if (mNavisStates[NAVI_CHAT] == NSNotCreated)
    {
        // Create Navi UI chat
        // Lua
        if (!mNavigator->getNavigatorLua()->call("createGUI", "%s", mNavisNames[NAVI_CHAT].c_str()))
        {
            OGRE_LOG("Navigator::inWorld() Unable to create GUI called " + mNavisNames[NAVI_CHAT]);
            return;
        }
#ifdef UIDEBUG
        mNaviMgr->getNavi(mNavisNames[NAVI_CHAT])->bind("debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
#endif
        mNavisStates[NAVI_CHAT] = NSCreated;
    }
    else
        mNaviMgr->getNavi(mNavisNames[NAVI_CHAT])->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::contextShow(int x, int y, const std::string& items)
{
    if (mNavisStates[NAVI_CONTEXT] == NSNotCreated)
    {
        // Create Navi UI context
        // Lua
        if (!mNavigator->getNavigatorLua()->call("createGUI", "%s%d%d%s", mNavisNames[NAVI_CONTEXT].c_str(), x, y, items.c_str()))
        {
            OGRE_LOG("Navigator::contextShow() Unable to create GUI called " + mNavisNames[NAVI_CONTEXT]);
            return;
        }
        mNavisStates[NAVI_CONTEXT] = NSCreated;
    }
    else
        mNaviMgr->getNavi(mNavisNames[NAVI_CONTEXT])->show(true);
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isContextVisible()
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_CONTEXT]);
    return ((navi != 0) && navi->getVisibility());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::contextHide()
{
    if (mNavisStates[NAVI_CONTEXT] != NSNotCreated)
    {
        // Destroy Navi UI context
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_CONTEXT]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[NAVI_CONTEXT] = NSNotCreated;
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainShow()
{
    if (mNavisStates[NAVI_MODELERPROP] == NSCreated)
        modelerPropHide();

    if (mNavisStates[NAVI_MODELERMAIN] == NSNotCreated)
    {
        // Create Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(mNavisNames[NAVI_MODELERMAIN], "local://uimdlrmain.html", NaviPosition(TopRight), 256, 512);
        navi->setMovable(true);
        navi->hide();
        navi->setMask("uimdlrmain.png");
        navi->setOpacity(0.75f);
        navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::naviToShowPageLoaded));
	    navi->bind("FileOpen", NaviDelegate(this, &NavigatorGUI::modelerMainFileOpen));
	    navi->bind("FileImport", NaviDelegate(this, &NavigatorGUI::modelerMainFileImport));
	    navi->bind("FileSave", NaviDelegate(this, &NavigatorGUI::modelerMainFileSave));
	    navi->bind("FileExit", NaviDelegate(this, &NavigatorGUI::modelerMainFileExit));
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

		navi->bind("ActionDelete", NaviDelegate(this, &NavigatorGUI::modelerActionDelete)); 
		navi->bind("ActionMove", NaviDelegate(this, &NavigatorGUI::modelerActionMove)); 
		navi->bind("ActionRotate", NaviDelegate(this, &NavigatorGUI::modelerActionRotate)); 
		navi->bind("ActionScale", NaviDelegate(this, &NavigatorGUI::modelerActionScale)); 
		navi->bind("ActionLink", NaviDelegate(this, &NavigatorGUI::modelerActionLink)); 
		navi->bind("ActionProperties", NaviDelegate(this, &NavigatorGUI::modelerActionProperties)); 
		navi->bind("ActionSceneSave", NaviDelegate(this, &NavigatorGUI::modelerActionSave));
  
		mNavisStates[NAVI_MODELERMAIN] = NSCreated;
    }
    else
        mNaviMgr->getNavi(mNavisNames[NAVI_MODELERMAIN])->show(true);

    mNavigator->startModeling();

    if (mNavigator->mModeler)
        mNavigator->mModeler->lockSelection(false);
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isModelerMainVisible()
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERMAIN]);
    return ((navi != 0) && navi->getVisibility());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainHide()
{
    if (!isModelerMainVisible()) return;
    mNaviMgr->getNavi(mNavisNames[NAVI_MODELERMAIN])->hide();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainUnload()
{
    if (mNavisStates[NAVI_MODELERMAIN] != NSNotCreated)
    {
        // Destroy Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERMAIN]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[NAVI_MODELERMAIN] = NSNotCreated;

		mNavigator->setState(Navigator::SInWorld);	
		mNavigator->endModeling();
		modelerPropUnload();
		modelerMainUnload();

		// Remove temporary files & folder of the thumbnails
		std::string path ( "NaviLocal\\solTmpTexture" );
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
		// Create Navi UI modeler
		NaviLibrary::Navi* navi = mNaviMgr->createNavi(mNavisNames[NAVI_MODELERPROP], "local://uimdlrprop.html", NaviPosition(TopRight), 512, 512);
		navi->setMovable(true);
		navi->hide();
		navi->setMask("uimdlrprop.png");
		navi->setOpacity(0.75f);

		// page loaded
		navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::naviToShowPageLoaded));

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
		navi->bind("MdlrApplyVLCTexture", NaviDelegate(this, &NavigatorGUI::modelerPropVLCTextureApply));
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
		mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP])->show(true);

		// Update the properties panel from the selected object datas
		modelerTabberLoad(1);
	}

	if( mNavigator->mModeler )
	{
		mNavigator->mModeler->lockSelection(true);

		if( !mNavigator->mModeler->isSelectionEmpty() )
		{
			// hide the gizmos axes
			mNavigator->mModeler->lockGizmo(false);
			mNavigator->mModeler->getSelection()->mTransformation->showGizmosMove(false);
			mNavigator->mModeler->getSelection()->mTransformation->showGizmosRotate(false);
			mNavigator->mModeler->getSelection()->mTransformation->showGizmosScale(false);
		}
	}
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isModelerPropVisible()
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
    return ((navi != 0) && navi->getVisibility());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropHide()
{
    if (!isModelerPropVisible()) return;
    mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP])->hide();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropUnload()
{
    if (mNavisStates[NAVI_MODELERPROP] != NSNotCreated)
    {
        // Destroy Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[NAVI_MODELERPROP] = NSNotCreated;

		// Update the selected objet dats from the properties panel
		modelerTabberSave();

		if( mNavigator->mModeler )
			mNavigator->mModeler->lockSelection(false);
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
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(mNavisNames[NAVI_AVATARMAIN], "local://uiavatarmain.html", NaviPosition(TopRight), 256, 512);
        navi->setMovable(true);
        navi->hide();
        navi->setMask("uiavatarmain.png");
        navi->setOpacity(0.75f);
        
        navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::naviToShowPageLoaded));
		navi->bind("AvatarPrev", NaviDelegate(this, &NavigatorGUI::avatarMainSelectPrev));
		navi->bind("AvatarNext", NaviDelegate(this, &NavigatorGUI::avatarMainSelectNext));
		navi->bind("AvatarSelected", NaviDelegate(this, &NavigatorGUI::avatarMainSelected));
	    navi->bind("FileOpen", NaviDelegate(this, &NavigatorGUI::avatarMainFileOpen));
		navi->bind("AvatarEdit", NaviDelegate(this, &NavigatorGUI::avatarMainFileEdit));
		navi->bind("FileSave", NaviDelegate(this, &NavigatorGUI::avatarMainFileSave));
        navi->bind("FileSaveAs", NaviDelegate(this, &NavigatorGUI::avatarMainFileSaveAs));
	    navi->bind("FileExit", NaviDelegate(this, &NavigatorGUI::avatarMainFileExit));

		mNavisStates[NAVI_AVATARMAIN] = NSCreated;
    }
    else
        mNaviMgr->getNavi(mNavisNames[NAVI_AVATARMAIN])->show(true);

    mNavigator->startAvatarEdit();
/*
    if (mNavigator->mModeler)
        mNavigator->mModeler->lockSelection(false);
*/
}
//-------------------------------------------------------------------------------------
bool NavigatorGUI::isAvatarMainVisible()
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARMAIN]);
    return ((navi != 0) && navi->getVisibility());
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainHide()
{
    if (!isAvatarMainVisible()) return;
    mNaviMgr->getNavi(mNavisNames[NAVI_AVATARMAIN])->hide();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainUnload()
{
    if (mNavisStates[NAVI_AVATARMAIN] != NSNotCreated)
    {
        // Destroy Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARMAIN]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[NAVI_AVATARMAIN] = NSNotCreated;

		mNavigator->setState(Navigator::SInWorld);	
		mNavigator->endAvatarEdit();
		avatarPropUnload();
		avatarMainUnload();

		// Remove temporary files & folder of the thumbnails
		std::string path ( "NaviLocal\\solTmpTexture" );
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
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(mNavisNames[NAVI_AVATARPROP], "local://uiavatarprop.html", NaviPosition(TopRight), 512, 512);
		navi->setMovable(true);
		navi->hide();
		navi->setMask("uiavatarprop.png");
		navi->setOpacity(0.75f);

		// page loaded
		navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::naviToShowPageLoaded));

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
		navi->bind("AvatarShininess", NaviDelegate(this, &NavigatorGUI::avatarPropShininess));
		navi->bind("AvatarTransparency", NaviDelegate(this, &NavigatorGUI::avatarPropTransparency));
		navi->bind("AvatarScrollU", NaviDelegate(this, &NavigatorGUI::avatarPropScrollU));
		navi->bind("AvatarScrollV", NaviDelegate(this, &NavigatorGUI::avatarPropScrollV));
		navi->bind("AvatarScaleU", NaviDelegate(this, &NavigatorGUI::avatarPropScaleU));
		navi->bind("AvatarScaleV", NaviDelegate(this, &NavigatorGUI::avatarPropScaleV));
		navi->bind("AvatarRotateU", NaviDelegate(this, &NavigatorGUI::avatarPropRotateU));
		navi->bind("AvatarAddTexture", NaviDelegate(this, &NavigatorGUI::avatarPropTextureAdd));
		navi->bind("AvatarRemoveTexture", NaviDelegate(this, &NavigatorGUI::avatarPropTextureRemove));
		navi->bind("AvatarApplyTexture", NaviDelegate(this, &NavigatorGUI::avatarPropTextureApply));
		navi->bind("AvatarPrevTexture", NaviDelegate(this, &NavigatorGUI::avatarPropTexturePrev));
		navi->bind("AvatarNextTexture", NaviDelegate(this, &NavigatorGUI::avatarPropTextureNext));
/*		navi->bind("AvatarPositionX", NaviDelegate(this, &NavigatorGUI::modelerPropPositionX));
		navi->bind("AvatarPositionY", NaviDelegate(this, &NavigatorGUI::modelerPropPositionY));
		navi->bind("AvatarPositionZ", NaviDelegate(this, &NavigatorGUI::modelerPropPositionZ));
		navi->bind("AvatarOrientationX", NaviDelegate(this, &NavigatorGUI::modelerPropOrientationX));
		navi->bind("AvatarOrientationY", NaviDelegate(this, &NavigatorGUI::modelerPropOrientationY));
		navi->bind("AvatarOrientationZ", NaviDelegate(this, &NavigatorGUI::modelerPropOrientationZ));
		navi->bind("AvatarScaleX", NaviDelegate(this, &NavigatorGUI::modelerPropScaleX));
		navi->bind("AvatarScaleY", NaviDelegate(this, &NavigatorGUI::modelerPropScaleY));
		navi->bind("AvatarScaleZ", NaviDelegate(this, &NavigatorGUI::modelerPropScaleZ));
		navi->bind("AvatarCollision", NaviDelegate(this, &NavigatorGUI::modelerPropCollision));
		navi->bind("AvatarGravity", NaviDelegate(this, &NavigatorGUI::modelerPropGravity));
*/
        mNavisStates[NAVI_AVATARPROP] = NSCreated;
	}
	else //if(!isAvatarMainVisible())
	{
		NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
				
		// Update the properties panel from the selected object datas
		avatarTabberLoad(1);
		navi->evaluateJS("$('avatarTabbers').tabber.tabShow(1)");
		navi->show(true);
	}
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isAvatarPropVisible()
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
    return ((navi != 0) && navi->getVisibility());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropHide()
{
    if (!isAvatarPropVisible()) return;
    mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP])->hide();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropUnload()
{
    if (mNavisStates[NAVI_AVATARPROP] != NSNotCreated)
    {
        // Destroy Navi UI modeler
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
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
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	// get the current avatar
	Character* avatar = mNavigator->mAvatarEditor->getManager()->getCurrent();
	if( avatar != 0 )
	{
		ColourValue col;
		Ogre::Vector2 UV;
		std::string text;
		unsigned c,e;
		
		switch( pTab )
		{
		case 0:	// animations tab
			if(avatar->getNumAnimations() > 0)
			{
				// ...
			}
			break;
		case 1:	//  properties tab
			{
				// height
				navi->evaluateJS("height.onchange = function() {}");
				//Vector3 size = avatar->getEntity()->getBoundingBox().getSize();
				Vector3 size = avatar->getMesh()->getBounds().getSize();
				navi->evaluateJS("height.setValue(" + StringConverter::toString((size.y - 0.5) * 100) + ")");
				navi->evaluateJS("height.onchange = function() {elementClicked('AvatarHeight')}");
				navi->evaluateJS("$('HeightValue').value=height.getValue()/100.+0.5+'m'");

				// bones
				Bone* bone = avatar->getCurrentBone();
				std::string name(bone->getName());
				std::string temp("");
				for(int i=0; i<name.length(); i++)
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
				if(avatar->getNumBodyParts() > 0)
					 name = avatar->getCurrentBodyPart()->getName();
				navi->evaluateJS("$('BodyPartName').innerHTML = '" + name + "'");

				name = "None";
				if(avatar->getNumBodyParts() > 0)
					name = avatar->getCurrentBodyPart()->getCurrentBodyPartModelName();
				navi->evaluateJS("$('BodyPartModelName').innerHTML = '" + name + "'");
				if( name == "None" )
					navi->evaluateJS("$('BodyPartEdit').disabled = 1");

				// attachements / goodies
				name = "None";
				if(avatar->getNumGoodies() > 0)
					name = avatar->getCurrentGoody()->getName();
				navi->evaluateJS("$('AttachName').innerHTML = '" + name + "'");

				name = "None";
				if(avatar->getNumGoodies() > 0)
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
				int type = AvatarEditor::getSingletonPtr()->selectType;
				ModifiableMaterialObject* object;
				
				if( type == 2 ) // Goody
					object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
				else //if( type <= 1 ) // BodyPart
					object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

				ModifiedMaterial* material = object->getModifiedMaterial();

				navi->evaluateJS("shininess.onchange = function() {}");
				navi->evaluateJS("transparency.onchange = function() {}");
				navi->evaluateJS("scrollU.onchange = function() {}");
				navi->evaluateJS("scrollV.onchange = function() {}");
				navi->evaluateJS("scaleU.onchange = function() {}");
				navi->evaluateJS("scaleV.onchange = function() {}");
				navi->evaluateJS("rotateU.onchange = function() {}");

				navi->evaluateJS("$S('pAmbient').background='#'+'FFFFFF'");
				navi->evaluateJS("$S('pDiffuse').background='#'+'FFFFFF'");
				navi->evaluateJS("$S('pSpecular').background='#'+'FFFFFF'");
				navi->evaluateJS("shininess.setValue(" + StringConverter::toString(material->getShininess()*100) + ")");
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

				avatarUpdateTextures( object );
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
	if( mNavigator->mModeler->updateCommand( pCommand, pObject3D ) )
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
	char str[64];
	Object3D* obj = mNavigator->mModeler->getSelected();

	// reinitalize the sliders value
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);

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
	sprintf(str, "taperX.setValue(%f)", obj->getTaperX()*100);
	navi->evaluateJS(str);
	sprintf(str, "taperY.setValue(%f)", obj->getTaperY()*100);
	navi->evaluateJS(str);
	sprintf(str, "topShearX.setValue(%f)",obj->getTopShearX()*100);
	navi->evaluateJS(str);
	sprintf(str, "topShearY.setValue(%f)",obj->getTopShearY()*100);
	navi->evaluateJS(str);
	sprintf(str, "twistBegin.setValue(%f)",obj->getTwistBegin()*100);
	navi->evaluateJS(str);
	sprintf(str, "twistEnd.setValue(%f)",obj->getTwistEnd()*100);
	navi->evaluateJS(str);
	sprintf(str, "dimpleBegin.setValue(%f)",obj->getDimpleBegin()*100);
	navi->evaluateJS(str);
	sprintf(str, "dimpleEnd.setValue(%f)",obj->getDimpleEnd()*100);
	navi->evaluateJS(str);
	sprintf(str, "pathCutBegin.setValue(%f)",obj->getPathCutBegin()*100);
	navi->evaluateJS(str);
	sprintf(str, "pathCutEnd.setValue(%f)",obj->getPathCutEnd()*100);
	navi->evaluateJS(str);
	sprintf(str, "holeSizeX.setValue(%f)",obj->getHoleSizeX()*100);
	navi->evaluateJS(str);
	sprintf(str, "holeSizeY.setValue(%f)",obj->getHoleSizeY()*100);
	navi->evaluateJS(str);
// TODO : uncomment this ->		navi->evaluateJS("document.getElementById('hollowShape').value = '1');"
	sprintf(str, "skew.setValue(%f)",obj->getSkew()*100);
	navi->evaluateJS(str);
	sprintf(str, "revolution.setValue(%f)",obj->getRevolutions()*1.);
	navi->evaluateJS(str);
	sprintf(str, "radiusDelta.setValue(%f)",obj->getRadiusDelta()*100);
	navi->evaluateJS(str);

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
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);

	Object3D* obj = mNavigator->mModeler->getSelected();
	
	std::string texturePath, text;
	String str;
	TexturePtr texture; 
	Ogre::Image image;

	// Go back to the main directory
	_chdir(mNavigator->mModeler->mExecPath.c_str());
	
	// create a temporary forlder for the thumbnail textures
#ifdef WIN32
	CreateDirectory( "NaviLocal\\solTmpTexture", NULL );
#else
	system( "md NaviLocal\\solTmpTexture" );
#endif

	/*
	text = "textTabTextures = \"";
	text += "<img src='./color/blank.jpg' width=128 height=128/>	";
	text += "<img src='file:///z:/5.png' width=128 height=128/>	";
	text += "\"";
	*/

	// update Navi interface
	text = "textTabTextures = \"";
	for(unsigned t=1; t<obj->getMaterialManager()->getNbTexture(); t++)
	{
		texture = obj->getMaterialManager()->getTexture(t);
		texturePath = texture->getName();
		Path path(texturePath);
		size_t begin = path.getFormatedPath().find_last_of( '\\' );
		size_t end = path.getFormatedPath().find_last_of( '.' );
		std::string fileName( path.getFormatedPath(), begin+1, end-begin-1 );
		fileName += ".jpg";

		str = ResourceGroupManager::getSingleton().findGroupContainingResource(texturePath);

		//text += "<img src='file:///d:\\test.jpg";
		text += "<img src='./solTmpTexture/";
		text += fileName;
		text +=	"' width=128 height=128/>	";

		image.load( texturePath, str);
		image.resize( 128, 128 );
		image.save( "NaviLocal\\solTmpTexture\\" + fileName );
	}
	text += "\"";
	navi->evaluateJS(text);

	// Go back to the main directory
	_chdir(mNavigator->mModeler->mExecPath.c_str());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerAddNewDeformation(Object3D::Command pCommand)
{
	Object3D* obj = mNavigator->mModeler->getSelected();
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
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);

	// get the current object3D
	Object3D* obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		ColourValue col;
		Ogre::Vector2 UV;
		std::string text;
		unsigned c,e;

		switch( pTab )
		{
		case 0:	// properties tab
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
			navi->evaluateJS("shininess.onchange = function() {}");
			navi->evaluateJS("transparency.onchange = function() {}");
			navi->evaluateJS("scrollU.onchange = function() {}");
			navi->evaluateJS("scrollV.onchange = function() {}");
			navi->evaluateJS("scaleU.onchange = function() {}");
			navi->evaluateJS("scaleV.onchange = function() {}");
			navi->evaluateJS("rotateU.onchange = function() {}");

			navi->evaluateJS("$S('pAmbient').background='#'+'FFFFFF'");
			navi->evaluateJS("$S('pDiffuse').background='#'+'FFFFFF'");
			navi->evaluateJS("$S('pSpecular').background='#'+'FFFFFF'");
			navi->evaluateJS("shininess.setValue(" + StringConverter::toString(obj->getShininess()*100) + ")");
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

			modelerUpdateTextures();
			break;
		case 3:	// 3D tab
			navi->evaluateJS("document.getElementById('positionX').value = " + StringConverter::toString(obj->getPosition().x));
			navi->evaluateJS("document.getElementById('positionY').value = " + StringConverter::toString(obj->getPosition().y));
			navi->evaluateJS("document.getElementById('positionZ').value = " + StringConverter::toString(obj->getPosition().z));
			navi->evaluateJS("document.getElementById('orientationX').value = " + StringConverter::toString(obj->getOrientation().x));
			navi->evaluateJS("document.getElementById('orientationY').value = " + StringConverter::toString(obj->getOrientation().y));
			navi->evaluateJS("document.getElementById('orientationZ').value = " + StringConverter::toString(obj->getOrientation().z));
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
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);

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
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(mNavisNames[NAVI_DEBUG], "local://uidebug.html", NaviPosition(TopRight), 300, 256);
        navi->setMovable(true);
        navi->setAutoUpdateOnFocus(true);
        navi->setMaxUPS(24);
        navi->hide();
        navi->setMask("uidebug.png");
        navi->setOpacity(0.50f);
        navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::debugPageLoaded));
        navi->bind("debugRefreshTree", NaviDelegate(this, &NavigatorGUI::debugRefreshTree));
        navi->bind("debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
        navi->bind("navCommand", NaviDelegate(this, &NavigatorGUI::navCommand));
        mNavisStates[NAVI_DEBUG] = NSCreated;
        mTreeDirty = true;
    }
    else
    {
        // Hide and destroy UI debug
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_DEBUG]);
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
    OGRE_LOG("NavigatorGUI::debugRefreshUrl()");

    if (mNavisStates[NAVI_DEBUG] != NSCreated) return;

#ifdef DEMO_NAVI2
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_DEBUG]);
    NaviLibrary::Navi* naviDemoNavi2 = mNaviMgr->getNavi("demoNavi2Video");
    if (naviDemoNavi2 == 0) return;
    // Set current url
    char txt[256];
    sprintf(txt, "$('inputUrl').value = '%s'", mNavigator->demoNavi2GetUrl().c_str());
    navi->evaluateJS(txt);
    // Activate/Deactivate Back/Forward buttons
    sprintf(txt, "$('navBackButton').disabled = %s", naviDemoNavi2->canNavigateBack() ? "false" : "true");
    navi->evaluateJS(txt);
    sprintf(txt, "$('navForwardButton').disabled = %s", naviDemoNavi2->canNavigateForward() ? "false" : "true");
    navi->evaluateJS(txt);
#endif
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::debugPageLoaded(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::debugPageLoaded()");

    // Refresh url
    debugRefreshUrl();

    // Refresh tree datas
    debugRefreshTree(naviData);

    // Show Navi UI debug
    if (mNavisStates[NAVI_DEBUG] == NSCreated)
        mNaviMgr->getNavi(mNavisNames[NAVI_DEBUG])->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::debugRefreshTree(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::debugRefreshTree()");

    if (mNavisStates[NAVI_DEBUG] != NSCreated) return;
    if (!mTreeDirty) return;

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_DEBUG]);

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
void NavigatorGUI::loginPageLoaded(const NaviData& naviData)
{
    char txt[256];

    OGRE_LOG("NavigatorGUI::loginPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_LOGIN]);

    // Set current values
    sprintf(txt, "$('inputLogin').value = '%s'", mNavigator->getConnectionLogin().c_str());
    navi->evaluateJS(txt);

    // Set network config into informations text
    std::string infosText;
    switch (mNavigator->getConnectionMode())
    {
    case (Navigator::CMStartNewNode):
        sprintf(txt, "%d", mNavigator->getConnectionUdpPort());
        infosText = "Create a new node (UDP port " + String(txt) + ")";
        break;
    case (Navigator::CMExistingNode):
        sprintf(txt, "%d", mNavigator->getConnectionPort());
        infosText = "Connect to " + mNavigator->getConnectionHost() + " (port " + String(txt) + ")";
        break;
    };
    navi->evaluateJS("$('infosText').innerHTML = '" + infosText + "'");

	// Setup the avatar name list
	std::string text("");
	vector<std::string>* list = AvatarEditor::getSingletonPtr()->getManager()->getNameList();
	vector<std::string>::iterator iter = list->begin();
	int id = 0;
/*	while(iter!=list->end())
	{
		text += "<li>";
		text += (*iter).data();
		text += "</li>";
		iter++;
	}
	navi->evaluateJS("$('myComboBox').innerHTML = '" + text + "'");
//	navi->evaluateJS("initAvatarList();");

	// Select the avatar from the user.xml // avatarName
	text = "default avatar: ???";
	navi->evaluateJS("combobox.comboHead.setHTML(" + text + ")");
*/
	while(iter!=list->end())
	{
		text += "<div class='itemOut' onmouseout=this.className='itemOut' onmouseover=this.className='itemOver'><a href='#' class='lienMenu' onclick=select('";
		text += (*iter).data();
		text += "')>";
		text += (*iter).data();
		text += "</a></div>";
		iter++;
	}
	navi->evaluateJS("$('avatarSelectItem').innerHTML = \"" + text + "\"");

	// Select the avatar from the user.xml // avatarName
	//text = AvatarEditor::getSingletonPtr()->getName();
	//navi->evaluateJS("$('avatarSelectTitre').innerHTML = '" + text + "'");
	int nbItem = list->size();
	if( nbItem < 7 )
	{
		sprintf(txt, "%ipx'", nbItem*16);
		text = txt;
		navi->evaluateJS("$('avatarSelectItem').style.height = '" + text);
	}

	// Show Navi UI login
    if (mNavisStates[NAVI_LOGIN] == NSCreated)
        navi->show(true);
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::loginSelectAvatar(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::selectAvatar()");
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_LOGIN]);
	std::string item( naviData["item"].str() );
	
	//AvatarEditor::getSingletonPtr()->setCurrentByName( item );
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::connect(const NaviData& naviData)
{
    char txt[256];

    OGRE_LOG("NavigatorGUI::connect()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_LOGIN]);

    // Get login name
	std::string login;
    login = naviData["login"].str();
    OGRE_LOG("login=" + login);

    // Check
    static std::string validLoginExtrasChars = "$-_.@+!*'(),";
    bool validLogin = ((login.length() > 2) && (login.compare("null") != 0));
    for(int i=0;i<login.length();i++)
    {
        if (!validLogin)
            break;
        validLogin = ((login[i] >= '0') && (login[i] <= '9') ||
            (login[i] >= 'a') && (login[i] <= 'z') ||
            (login[i] >= 'A') && (login[i] <= 'Z') ||
            (validLoginExtrasChars.find_first_of(login[i]) != std::string::npos));
    }
    if (!validLogin)
        // Malformed login
        navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid login ...'");
    else
    {
        mNavigator->setConnectionLogin(login);
        // Valid login
        navi->evaluateJS("$('infosText').innerHTML = 'Connecting ...'");
        // Call connect
        bool connected = mNavigator->connect();
        sprintf(txt, "$('infosText').innerHTML = 'Connection %s ...'", (connected) ? "succeeded" : "failed");
        navi->evaluateJS(txt);
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::options(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::options()");

    // Hide previous Navi UI
    hidePreviousNavi();

    if (mNavisStates[NAVI_OPTIONS] == NSNotCreated)
    {
        // Create Navi UI options
        NaviLibrary::Navi* navi = mNaviMgr->createNavi(mNavisNames[NAVI_OPTIONS], "local://uioptions.html", NaviPosition(Center), 400, 400);
        navi->setMovable(false);
        navi->hide();
        navi->setMask("uioptions.png");
        navi->setOpacity(0.75f);
	    navi->bind("pageLoaded", NaviDelegate(this, &NavigatorGUI::optionsPageLoaded));
	    navi->bind("ok", NaviDelegate(this, &NavigatorGUI::optionsOk));
	    navi->bind("back", NaviDelegate(this, &NavigatorGUI::optionsBack));
#ifdef UIDEBUG
        navi->bind("debugCommand", NaviDelegate(this, &NavigatorGUI::debugCommand));
#endif
        mNavisStates[NAVI_OPTIONS] = NSCreated;
    }

    // Set next Navi UI
    mCurrentNavi = NAVI_OPTIONS;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::optionsPageLoaded(const NaviData& naviData)
{
    char txt[256];

    OGRE_LOG("NavigatorGUI::optionsPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_OPTIONS]);

    // Set current values
    if (mNavigator->getConnectionMode() == Navigator::CMStartNewNode)
        navi->evaluateJS("$('radioNewNode').checked = 'checked'");
    else
        navi->evaluateJS("$('radioExistingNode').checked = 'checked'");
    sprintf(txt, "$('inputUdpPort').value = '%d'", mNavigator->getConnectionUdpPort());
    navi->evaluateJS(txt);
    sprintf(txt, "$('inputHost').value = '%s'", mNavigator->getConnectionHost().c_str());
    navi->evaluateJS(txt);
    sprintf(txt, "$('inputPort').value = '%d'", mNavigator->getConnectionPort());
    navi->evaluateJS(txt);
    navi->evaluateJS("$('infosText').innerHTML = ''");
    std::string proxyAutoconfUrl;
    std::string proxyHttpHost;
    int proxyHttpPort;
    int proxyType;
    if (mNaviMgr->getProxyConfig(proxyType, proxyHttpHost, proxyHttpPort, proxyAutoconfUrl))
    {
        switch (proxyType)
        {
        case 0: // 0 for direct connection, no proxy
            navi->evaluateJS("$('radioProxyTypeDirect').checked = 'checked'");
            break;
        case 4: // 4 for auto-detect proxy settings
            navi->evaluateJS("$('radioProxyTypeAutodetect').checked = 'checked'");
            break;
        case 1: // 1 for manual proxy configuration
            navi->evaluateJS("$('radioProxyTypeManual').checked = 'checked'");
            break;
        default: // 2 for proxy auto-conf (PAC)
            navi->evaluateJS("$('radioProxyTypeAutoconf').checked = 'checked'");
            break;
        }
        sprintf(txt, "$('inputProxyHttpHost').value = '%s'", proxyHttpHost.c_str());
        navi->evaluateJS(txt);
        sprintf(txt, "$('inputProxyHttpPort').value = '%d'", proxyHttpPort);
        navi->evaluateJS(txt);
        sprintf(txt, "$('inputProxyAutoconfUrl').value = '%s'", proxyAutoconfUrl.c_str());
        navi->evaluateJS(txt);
    }

    // Show Navi UI options
    if (mNavisStates[NAVI_OPTIONS] == NSCreated)
        navi->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::quit(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::quit()");

    mNavigator->quit();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::optionsOk(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::optionsOk()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_OPTIONS]);

    // Get options
    std::string radioNode;
    int udpPort;
	std::string host;
    int port;
    radioNode = naviData["radioNode"].str();
    udpPort = naviData["udpPort"].toInt();
    host = naviData["host"].str();
    port = naviData["port"].toInt();
    OGRE_LOG("radioNode=" + radioNode + ", udpPort=" + StringConverter::toString(udpPort) + ", host=" + host + ", port=" + StringConverter::toString(port));
    std::string radioProxyType;
	std::string proxyHttpHost;
    int proxyHttpPort;
	std::string proxyAutoconfUrl;
    radioProxyType = naviData["radioProxyType"].str();
    proxyHttpHost = naviData["proxyHttpHost"].str();
    proxyHttpPort = naviData["proxyHttpPort"].toInt();
    proxyAutoconfUrl = naviData["proxyAutoconfUrl"].str();
    OGRE_LOG("radioProxyType=" + radioProxyType + ", proxyHttpHost=" + proxyHttpHost + ", proxyHttpPort=" + StringConverter::toString(proxyHttpPort) + ", proxyAutoconfUrl=" + proxyAutoconfUrl);

    // Check
    bool valid_options = true;
    Navigator::ConnectionMode connectionMode = (radioNode.compare("newNode") == 0) ? Navigator::CMStartNewNode : Navigator::CMExistingNode;
    if (connectionMode == Navigator::CMStartNewNode)
    {
        if (udpPort < 0)
        {
            // Bad UDP port
            navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid UDP Port ...'");
            valid_options = false;
        }
    }
    else
    {
        if (host.length() < 2)
         {
           // Bad hostname
            navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid hostname ...'");
            valid_options = false;
        }
        if (port < 0)
        {
            // Bad port
            navi->evaluateJS("$('infosText').innerHTML = 'Enter a valid Port ...'");
            valid_options = false;
        }
    }
    int proxyType;
    if (radioProxyType.compare("direct") == 0)
    {
        proxyType = 0;
        proxyAutoconfUrl = "";
        proxyHttpHost = "";
        proxyHttpPort = 0;
    }
    else if (radioProxyType.compare("autodetect") == 0)
    {
        proxyType = 4;
        proxyAutoconfUrl = "";
        proxyHttpHost = "";
        proxyHttpPort = 0;
    }
    else if (radioProxyType.compare("manual") == 0)
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

    // Valid options ?
    if (valid_options)
    {
        mNavigator->setConnectionMode(connectionMode);
        mNavigator->setConnectionUdpPort(udpPort);
        mNavigator->setConnectionHost(host);
        mNavigator->setConnectionPort(port);
        navi->evaluateJS("$('infosText').innerHTML = ''");

        mNaviMgr->setProxyConfig(proxyType, proxyHttpHost, proxyHttpPort, proxyAutoconfUrl);

        // Return to Navi UI login
        login();
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::optionsBack(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::optionsBack()");

    // Return to Navi UI login
    login();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::chatPageLoaded(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::chatPageLoaded()");

    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_CHAT]);

    // Set current values
    navi->evaluateJS("$('inputChat').value = ''");

    // Show Navi UI chat
    if (mNavisStates[NAVI_CHAT] == NSCreated)
        navi->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainFileOpen(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainFileOpen()");
	
    //modelerMainUnload();
	mNavigator->mdlrXMLLoad();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainFileImport(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainFileImport()");
	
	mNavigator->mdlrXMLImport();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainFileSave(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainFileSave()");
	
	//modelerMainUnload();
	mNavigator->mdlrXMLSave();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainFileExit(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainFileExit()");
	
    modelerMainUnload();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateBox(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateBox()");
	//mNavigator->startModeling();
	mNavigator->createBox();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateCorner(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateCorner()");
	//mNavigator->startModeling();
	mNavigator->createCorner();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreatePyramid(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreatePyramid()");
	//mNavigator->startModeling();
	mNavigator->createPyramid();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreatePrism(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreatePrism()");
	//mNavigator->startModeling();
	mNavigator->createPrism();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateCylinder(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateCylinder()");
	//mNavigator->startModeling();
	mNavigator->createCylinder();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateHalfCylinder(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateHalfCylinder()");
	//mNavigator->startModeling();
	mNavigator->createHalfCyl();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateCone(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateCone()");
	//mNavigator->startModeling();
	mNavigator->createCone();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateHalfCone(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateHalfCone()");
	//mNavigator->startModeling();
	mNavigator->createHalfCone();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateSphere(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateSphere()");
	//mNavigator->startModeling();
	mNavigator->createSphere();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateHalfSphere(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateHalfSphere()");
	//mNavigator->startModeling();
	mNavigator->createHalfSphere();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateTorus(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateTorus()");
	//mNavigator->startModeling();
	mNavigator->createTorus();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateTube(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateTube()");
	//mNavigator->startModeling();
	mNavigator->createTube();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerMainCreateRing(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::modelerMainCreateRing()");
	//mNavigator->startModeling();
	mNavigator->createRing();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionDelete(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::modelerActionDelete()");

	if( mNavigator->mModeler )
		if( !mNavigator->mModeler->isSelectionEmpty() )
        {
            //mNavigator->suppr();

            // remove the current selection
            mNavigator->mModeler->removeSelection();

            // hide the gizmos axes
            mNavigator->mModeler->getSelection()->mTransformation->showGizmosMove(false);
            mNavigator->mModeler->getSelection()->mTransformation->showGizmosRotate(false);
            mNavigator->mModeler->getSelection()->mTransformation->showGizmosScale(false);
        }
		else
#ifdef WIN32
			MessageBox(NULL,"You have to select an object3D","Information",MB_OK | MB_ICONINFORMATION); 
#else
			std::cerr << " You have to select an object3D " << std::endl;
#endif
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionMove(const NaviData& naviData)
{
	if( !mNavigator->mModeler->isSelectionEmpty() )
		{
			static bool active = false;

			if (!active)
			{
				active = true;
				mNavigator->mModeler->eventMove();
				mNavigator->mModeler->lockGizmo(active);
			}
			else
			{
				active = false;
				mNavigator->mModeler->lockGizmo(active);
				mNavigator->mModeler->getSelection()->mTransformation->eventSelection();
			}
		}
	else
#ifdef WIN32
		MessageBox(NULL,"You have to select an object3D","Information",MB_OK | MB_ICONINFORMATION); 
#else
		std::cerr << " You have to select an object3D " << std::endl;
#endif
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionRotate(const NaviData& naviData)
{
	if( !mNavigator->mModeler->isSelectionEmpty() )
	{
		static bool active = false;

		if (!active)
		{
			active = true;
			mNavigator->mModeler->eventRotate();
			mNavigator->mModeler->lockGizmo(active);
		}
		else
		{
			active = false;
			mNavigator->mModeler->lockGizmo(active);
			mNavigator->mModeler->getSelection()->mTransformation->eventSelection();
		}
	}
	else
#ifdef WIN32
		MessageBox(NULL,"You have to select an object3D","Information",MB_OK | MB_ICONINFORMATION); 
#else
		std::cerr << " You have to select an object3D " << std::endl;
#endif
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionScale(const NaviData& naviData)
{
	if( !mNavigator->mModeler->isSelectionEmpty() )
	{
		static bool active = false;

		if (!active)
		{
			active = true;
			mNavigator->mModeler->eventScale();
			mNavigator->mModeler->lockGizmo(active);
		}
		else
		{
			active = false;
			mNavigator->mModeler->lockGizmo(active);
			mNavigator->mModeler->getSelection()->mTransformation->eventSelection();
		}
	}
	else
#ifdef WIN32
		MessageBox(NULL,"You have to select an object3D","Information",MB_OK | MB_ICONINFORMATION); 
#else
		std::cerr << " You have to select an object3D " << std::endl;
#endif
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionLink(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::modelerActionLink()");

	if( !mNavigator->mModeler->isSelectionEmpty() )
		mNavigator->mModeler->lockLinkMode(true);
	else
#ifdef WIN32
		MessageBox(NULL,"You have to select an object3D","Information",MB_OK | MB_ICONINFORMATION); 
#else
		std::cerr << " You have to select an object3D " << std::endl;
#endif
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionProperties(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::modelerActionProperties()");

	// Test if an Object3D has ever been created before
	if( !mNavigator->mModeler->isSelectionEmpty() )
		{
			// Hide the main modeler panel
			modelerMainHide();

			// Show the properties modeler panel
			modelerPropShow();
		}
	else
#ifdef WIN32
		MessageBox(NULL,"You have to select an object3D","Information",MB_OK | MB_ICONINFORMATION); 
#else
		std::cerr << " You have to select an object3D " << std::endl;
#endif
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionUndo(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::modelerActionUndo()");

	//mNavigator->undo();
    if( !mNavigator->mModeler->isSelectionEmpty() )
        mNavigator->mModeler->getSelected()->undo();

	modelerUpdateDeformationSliders();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerActionSave(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::modelerActionSave()");

	std::string path = "..\\..\\..\\..\\Media\\cache\\";
	path += mNavigator->getOgrePeerManager()->getXmlObjectFilename();
	mNavigator->mdlrXMLSave(true, path.c_str() );
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropObjectName(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('objectName').value");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
		obj->setName(value);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropCreator(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('creator').value");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
		obj->setOwner(value.c_str());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropOwner(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('owner').value");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
		obj->setCreator(value.c_str());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropGroup(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('group').value");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
		obj->setGroup(value.c_str());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropDescription(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('description').value");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
		obj->setDesc(value.c_str());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTags(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('tags').value");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
		obj->setTags(value.c_str());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropModification(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('modification').checked");
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropCopy(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('copy').checked");
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTaperX(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("taperX.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::TAPERX, obj );
		obj->apply( Object3D::Command::TAPERX, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTaperY(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("taperY.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::TAPERY, obj );
		obj->apply( Object3D::Command::TAPERY, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTopShearX(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("topShearX.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::TOP_SHEARX, obj );
		obj->apply( Object3D::Command::TOP_SHEARX, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTopShearY(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("topShearY.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::TOP_SHEARY, obj );
		obj->apply( Object3D::Command::TOP_SHEARY, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTwistBegin(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("twistBegin.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::TWIST_BEGIN, obj );
		obj->apply( Object3D::Command::TWIST_BEGIN, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTwistEnd(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("twistEnd.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::TWIST_END, obj );
		obj->apply( Object3D::Command::TWIST_END, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropDimpleBegin(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("dimpleBegin.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::DIMPLE_BEGIN, obj );
		obj->apply( Object3D::Command::DIMPLE_BEGIN, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropDimpleEnd(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("dimpleEnd.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::DIMPLE_END, obj );
		obj->apply( Object3D::Command::DIMPLE_END, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropPathCutBegin(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("pathCutBegin.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::PATH_CUT_BEGIN, obj );
		obj->apply( Object3D::Command::PATH_CUT_BEGIN, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropPathCutEnd(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("pathCutEnd.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::PATH_CUT_END, obj );
		obj->apply( Object3D::Command::PATH_CUT_END, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropHoleSizeX(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("holeSizeX.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::HOLE_SIZEX, obj );
		obj->apply( Object3D::Command::HOLE_SIZEX, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropHoleSizeY(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("holeSizeY.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::HOLE_SIZEY, obj );
		obj->apply( Object3D::Command::HOLE_SIZEY, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropHollowShape(const NaviData& naviData)
{
	std::string value;
    value = naviData["shape"].str();

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::HOLLOW_SHAPE, obj );
		obj->apply( Object3D::Command::HOLLOW_SHAPE, atoi(value.c_str()) );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropSkew(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("skew.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::SKEW, obj );
		obj->apply( Object3D::Command::SKEW, atoi(value.c_str())/100. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropRevolution(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("revolution.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::REVOLUTION, obj );
		obj->apply( Object3D::Command::REVOLUTION, atoi(value.c_str())/1. );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropRadiusDelta(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("radiusDelta.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		modelerUpdateCommand( Object3D::RADIUS_DELTA, obj );
		obj->apply( Object3D::Command::RADIUS_DELTA, atoi(value.c_str())/100. );
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

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		obj->setAmbiant( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
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

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		obj->setDiffus( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
		if(mLockAmbientDiffuse)
			obj->setAmbiant( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
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

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
		obj->setSpecular( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerColorLockAmbientDiffuse(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("$('lockAmbientdiffuse').checked");
	mLockAmbientDiffuse = (value == "true")?true:false;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropShininess(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("shininess.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
		obj->setShininess( atoi(value.c_str())/100. );
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTransparency(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("transparency.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
		obj->setAlpha( atoi(value.c_str())/100. );
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScrollU(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("scrollU.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		Ogre::Vector2 UV = obj->getMaterialManager()->getTextureScroll();
		obj->setTextureScroll(atoi(value.c_str())/100. - .5, UV.y);
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScrollV(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("scrollV.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		Ogre::Vector2 UV = obj->getMaterialManager()->getTextureScroll();
		obj->setTextureScroll(UV.x, atoi(value.c_str())/100. - .5);
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScaleU(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("scaleU.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		Ogre::Vector2 UV = obj->getMaterialManager()->getTextureScale();
		obj->setTextureScale( atoi(value.c_str())/100. + .5, UV.y );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScaleV(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("scaleV.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
	{
		Ogre::Vector2 UV = obj->getMaterialManager()->getTextureScale();
		obj->setTextureScale( UV.x, atoi(value.c_str())/100. + .5 );
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropRotateU(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("rotateU.getValue()");

	Object3D *obj = mNavigator->mModeler->getSelected();
	if( obj != 0 )
		obj->setTextureRotate( Ogre::Radian(atoi(value.c_str())/100.*Math::TWO_PI) );
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTextureAdd(const NaviData& naviData)
{
	char * PathTexture = FileBrowser::displayWindowForLoading( 
			"Image Files (*.png;*.bmp;*.jpg)\0*.png;*.bmp;*.jpg\0", string("") ); 
	
	if (PathTexture != NULL && mNavigator->mModeler != 0)
	{
		Object3D * obj = mNavigator->mModeler->getSelected();
		String TextureFilePath (PathTexture);

		//Create the new OGRE texture with the file selected :
		TexturePtr PtrTexture = TextureManager::getSingleton().load( TextureFilePath, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		//Test if this texture is already in the list :
		if( obj->getMaterialManager()->isPresentInList( PtrTexture ) )
		{
			MessageBox(NULL,"This Texture is already open","Error",MB_OK|MB_ICONEXCLAMATION);
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
	if( mNavigator->mModeler != 0 )
	{
		Object3D * obj = mNavigator->mModeler->getSelected();

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
	if( mNavigator->mModeler != 0 )
	{
		Object3D * obj = mNavigator->mModeler->getSelected();

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
	if( mNavigator->mModeler != 0 )
	{
		Object3D * obj = mNavigator->mModeler->getSelected();

	    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	    std::string urlStr = navi->evaluateJS("document.getElementById('MaterialWWWUrl').value");
	    std::string widthStr = navi->evaluateJS("document.getElementById('MaterialWWWWidth').value");
	    std::string heightStr = navi->evaluateJS("document.getElementById('MaterialWWWHeight').value");
	    std::string fpsStr = navi->evaluateJS("document.getElementById('MaterialWWWFps').value");
        int width = atoi(widthStr.c_str());
        int height = atoi(heightStr.c_str());
        int fps = atoi(fpsStr.c_str());

        Entity* objEntity = obj->getEntity();
        String mtlName = "WWW_" + objEntity->getName();
        NaviLibrary::Navi* naviWWWTexture = NaviLibrary::NaviManager::Get().createNaviMaterial(mtlName, urlStr, width, height);
        naviWWWTexture->show(true);
        naviWWWTexture->setMaxUPS(fps);
        naviWWWTexture->setForceMaxUpdate(fps != 0);
        naviWWWTexture->setOpacity(1.0f);
        objEntity->setMaterialName(naviWWWTexture->getMaterialName());
        objEntity->setQueryFlags(Navigator::QFNaviPanel);
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropVLCTextureApply(const NaviData& naviData)
{
	if( mNavigator->mModeler != 0 )
	{
		Object3D * obj = mNavigator->mModeler->getSelected();

	    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	    std::string mrlStr = navi->evaluateJS("document.getElementById('MaterialVLCMrl').value");
	    std::string widthStr = navi->evaluateJS("document.getElementById('MaterialVLCWidth').value");
	    std::string heightStr = navi->evaluateJS("document.getElementById('MaterialVLCHeight').value");
	    std::string fpsStr = navi->evaluateJS("document.getElementById('MaterialVLCFps').value");
        std::string paramsStr = navi->evaluateJS("document.getElementById('MaterialVLCParams').value");
        int width = atoi(widthStr.c_str());
        int height = atoi(heightStr.c_str());
        int fps = atoi(fpsStr.c_str());

        Entity* objEntity = obj->getEntity();
        String mtlName = "VLC_" + objEntity->getName();
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vlc");
        ExternalTextureSource* vlcExtTextSrc = ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vlc");
        vlcExtTextSrc->setParameter("mrl", mrlStr);
        vlcExtTextSrc->setParameter("width", StringConverter::toString(width));
        vlcExtTextSrc->setParameter("height", StringConverter::toString(height));
        vlcExtTextSrc->setParameter("frames_per_second", StringConverter::toString(fps));
        vlcExtTextSrc->setParameter("vlc_params", paramsStr);
        MaterialManager::getSingleton().create(mtlName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        vlcExtTextSrc->createDefinedTexture(mtlName);
        objEntity->setMaterialName(mtlName);
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropVNCTextureApply(const NaviData& naviData)
{
	if( mNavigator->mModeler != 0 )
	{
		Object3D * obj = mNavigator->mModeler->getSelected();

	    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	    std::string hostStr = navi->evaluateJS("document.getElementById('MaterialVNCHost').value");
	    std::string portStr = navi->evaluateJS("document.getElementById('MaterialVNCPort').value");
	    std::string pwdStr = navi->evaluateJS("document.getElementById('MaterialVNCPwd').value");
        int port = atoi(portStr.c_str());

        Entity* objEntity = obj->getEntity();
        String mtlName = "VNC_" + objEntity->getName();
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
        ExternalTextureSource* vncExtTextSrc = ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc");
        vncExtTextSrc->setParameter("address", "vnc://" + hostStr + ":" + StringConverter::toString(port));
        vncExtTextSrc->setParameter("password", "vncpwd:" + pwdStr);
        MaterialManager::getSingleton().create(mtlName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        vncExtTextSrc->createDefinedTexture(mtlName);
        objEntity->setMaterialName(mtlName);
        objEntity->setQueryFlags(Navigator::QFVNCPanel);
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTexturePrev(const NaviData& naviData)
{
	if( mNavigator->mModeler != 0 )
	{
		Object3D * obj = mNavigator->mModeler->getSelected();
		obj->getMaterialManager()->setPreviousTexture();
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropTextureNext(const NaviData& naviData)
{
	if( mNavigator->mModeler != 0 )
	{
		Object3D * obj = mNavigator->mModeler->getSelected();
		if( obj->getMaterialManager()->getNbTexture() > 1 )
			obj->getMaterialManager()->setNextTexture();
	}
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropPositionX(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('positionX').value * 10000");

	Vector3 vec = mNavigator->mModeler->getSelection()->getCenterPosition();
	mNavigator->mModeler->getSelection()->moveTo(atoi(value.c_str())/10000., vec.y, vec.z);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropPositionY(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('positionY').value * 10000");

	Vector3 vec = mNavigator->mModeler->getSelection()->getCenterPosition();
	mNavigator->mModeler->getSelection()->moveTo(vec.x, atoi(value.c_str())/10000., vec.z);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropPositionZ(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('positionZ').value * 10000");

	Vector3 vec = mNavigator->mModeler->getSelection()->getCenterPosition();
	mNavigator->mModeler->getSelection()->moveTo(vec.x, vec.y, atoi(value.c_str())/10000.);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropOrientationX(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('orientationX').value * 10000");

	mNavigator->mModeler->getSelection()->rotateTo(atoi(value.c_str())/10000., 0, 0);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropOrientationY(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('orientationY').value * 10000");

	mNavigator->mModeler->getSelection()->rotateTo(0, atoi(value.c_str())/10000., 0);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropOrientationZ(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('orientationZ').value * 10000");

	mNavigator->mModeler->getSelection()->rotateTo(0, 0, atoi(value.c_str())/10000.);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScaleX(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('scaleX').value * 10000");

	mNavigator->mModeler->getSelection()->scaleTo(atoi(value.c_str())/10000., 1, 1);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScaleY(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('scaleY').value * 10000");

	mNavigator->mModeler->getSelection()->scaleTo(1, atoi(value.c_str())/10000., 1);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropScaleZ(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('scaleZ').value * 10000");

	mNavigator->mModeler->getSelection()->scaleTo(1, 1, atoi(value.c_str())/10000.);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropCollision(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('collision').checked");
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::modelerPropGravity(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_MODELERPROP]);
	std::string value = navi->evaluateJS("document.getElementById('gravity').checked");
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainFileOpen(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::avatarMainFileOpen()");
    mNavigator->avatarXMLLoad();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainFileEdit(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::avatarMainFileEdit()");

	// Hide the main modeler panel
	avatarMainHide();
	// Show the properties modeler panel
	avatarPropShow();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainFileSave(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::avatarMainFileSave()");
    mNavigator->avatarXMLSave();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainFileSaveAs(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::avatarMainFileSaveAs()");
    mNavigator->avatarXMLSaveAs();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainFileExit(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::avatarMainFileExit()");
    avatarMainUnload();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainSelectPrev(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::avatarMainSelectPrev()");
	AvatarEditor::getSingletonPtr()->setPrevAsCurrent();
	mNavigator->getUserAvatar()->setEntity( AvatarEditor::getSingletonPtr()->getEntity() );
	mNavigator->getUserAvatar()->setState(Avatar::SIdle);

	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARMAIN]);
	std::string text( AvatarEditor::getSingletonPtr()->getName() );
	navi->evaluateJS("$('AvatarName').innerHTML = '<p>Name : <b>" + text + "</b></p>'");
	navi->evaluateJS("$('avatarSelectTitre').innerHTML = '" + text + "'");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainSelectNext(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::avatarMainSelectNext()");
	AvatarEditor::getSingletonPtr()->setNextAsCurrent();
	mNavigator->getUserAvatar()->setEntity( AvatarEditor::getSingletonPtr()->getEntity() );
    mNavigator->getUserAvatar()->setState(Avatar::SIdle);

	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARMAIN]);
	std::string text( AvatarEditor::getSingletonPtr()->getName() );
	navi->evaluateJS("$('AvatarName').innerHTML = '<p>Name : <b>" + text + "</b></p>'");
	navi->evaluateJS("$('avatarSelectTitre').innerHTML = '" + text + "'");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarMainSelected(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::avatarMainSelected()");
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARMAIN]);
	std::string item( naviData["item"].str() );
	
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	if( avatar->getName() != item )
	{
		AvatarEditor::getSingletonPtr()->setCurrentByName( item );

		mNavigator->getUserAvatar()->setEntity( AvatarEditor::getSingletonPtr()->getEntity() );
		mNavigator->getUserAvatar()->setState(Avatar::SIdle);

		NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARMAIN]);
		std::string text( AvatarEditor::getSingletonPtr()->getName() );
		navi->evaluateJS("$('AvatarName').innerHTML = '<p>Name : <b>" + text + "</b></p>'");
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAnimPlayPause(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::avatarPropAnimPlayPause()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	Avatar* user = mNavigator->getUserAvatar();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

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
		user->setState(Avatar::State(avatar->getCurrentAnimation()+1));
		user->startAnimation(user->getEntity()->getSkeleton()->getAnimation(avatar->getCurrentAnimation()+1)->getName());
	}
	

	char text[64];
	sprintf(text, "'<p>Animation %i//%i<br/><b>%s</b></p>'",
		avatar->getCurrentAnimation()+1,
		avatar->getNumAnimations(),
		user->getEntity()->getSkeleton()->getAnimation(avatar->getCurrentAnimation())->getName()
		);
	navi->evaluateJS(String("$('AnimName').innerHTML = ")+ text);
	
	// ...
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAnimStop(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::avatarPropAnimStop()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	Avatar* user = mNavigator->getUserAvatar();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	navi->evaluateJS(std::string("$('AnimPlayPause').value = 'Play'"));
	navi->evaluateJS(std::string("$('AnimTime').style = 'display: none'"));
	user->setState(Avatar::State(0));
	user->stopAnimation();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAnimNext(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::avatarPropAnimNext()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	Avatar* user = mNavigator->getUserAvatar();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	unsigned int numAnim = avatar->getNumAnimations();
	unsigned int current = avatar->getCurrentAnimation();
	if(++current >= numAnim) current = 0;
	//avatar->setCurrentAnimation(current);
	
	std::string text( user->getEntity()->getSkeleton()->getAnimation(current)->getName() );
	//user->stopAnimation();
	//user->setState(Avatar::State(current+1));
	//user->startAnimation( text );

	navi->evaluateJS("$('animationSelectTitre').innerHTML = '" + text + "'");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAnimPrev(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::avatarPropAnimPrev()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	Avatar* user = mNavigator->getUserAvatar();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	unsigned int numAnim = avatar->getNumAnimations();
	int current = avatar->getCurrentAnimation();
	if(--current < 0) current = numAnim-1;
	//avatar->setCurrentAnimation(current);

	std::string text( user->getEntity()->getSkeleton()->getAnimation(current)->getName() );
	//user->stopAnimation();
	//user->setState(Avatar::State(current+1));
	//user->startAnimation( text );

	navi->evaluateJS("$('animationSelectTitre').innerHTML = '" + text + "'");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropHeight(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::avatarPropAnimPrev()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	float height = atoi(navi->evaluateJS("height.getValue()").data()) / 100. + 0.5;
	float scale = height / avatar->getEntity()->getBoundingBox().getSize().y;
	SceneNode* node = avatar->getNode();

//	static Node* child = node->removeChild( (unsigned short) 2 );
	node->setScale( scale, scale, scale );
	//node->addChild( child );
	
	navi->evaluateJS("$('HeightValue').value=height.getValue()/100.+0.5+'m'");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropBonePrev(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::avatarPropBonePrev()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	Bone* bone = avatar->setPreviousBoneAsCurrent();
	std::string str = bone->getName();
	std::string temp;
	char c;
	for(int i=0; i<str.length(); i++)
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
	OGRE_LOG("NavigatorGUI::avatarPropBoneNext()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	Bone* bone = avatar->setNextBoneAsCurrent();
	std::string str = bone->getName();
	std::string temp;
	char c;
	for(int i=0; i<str.length(); i++)
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
	OGRE_LOG("NavigatorGUI::avatarPropBPPrev()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	BodyPart* bp = avatar->setPreviousBodyPartAsCurrent();
	BodyPartModel* bpm = bp->getCurrentBodyPartModel();
	std::string bpName = bp->getName();

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
	OGRE_LOG("NavigatorGUI::avatarPropBPNext()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	BodyPart* bp = avatar->setNextBodyPartAsCurrent();
	BodyPartModel* bpm = bp->getCurrentBodyPartModel();
	std::string bpName = bp->getName();

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
	OGRE_LOG("NavigatorGUI::avatarPropBPMPrev()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	BodyPart* bp = avatar->getCurrentBodyPart();
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
	OGRE_LOG("NavigatorGUI::avatarPropBPMNext()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	BodyPart* bp = avatar->getCurrentBodyPart();
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
	OGRE_LOG("NavigatorGUI::avatarPropBPMEdit()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	AvatarEditor::getSingletonPtr()->selectType = 1;
	avatarTabberLoad(2);
	navi->evaluateJS("$('avatarTabbers').tabber.tabShow(2)");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropBPMRemove(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::avatarPropBPMRemove()");
	//Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	//NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAttPrev(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::avatarPropAttPrev()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	if(avatar->getNumGoodies() > 0)
	{
		Goody* g = avatar->setPreviousGoodyAsCurrent();
		GoodyModel* gm = g->getCurrentGoodyModel();
		std::string gName = g->getName();

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
	OGRE_LOG("NavigatorGUI::avatarPropAttNext()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	if(avatar->getNumGoodies() > 0)
	{
		Goody* g = avatar->setNextGoodyAsCurrent();
		GoodyModel* gm = g->getCurrentGoodyModel();
		std::string gName = g->getName();

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
	OGRE_LOG("NavigatorGUI::avatarPropAttMPrev()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	if(avatar->getNumGoodies() > 0)
	{
		Goody* g = avatar->getCurrentGoody();
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
	OGRE_LOG("NavigatorGUI::avatarPropAttMNext()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	if(avatar->getNumGoodies() > 0)
	{
		Goody* g = avatar->getCurrentGoody();
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
	OGRE_LOG("NavigatorGUI::avatarPropAttMEdit()");
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	AvatarEditor::getSingletonPtr()->selectType = 2;
	avatarTabberLoad(2);
	navi->evaluateJS("$('avatarTabbers').tabber.tabShow(2)");
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropAttMRemove(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::avatarPropAttMRemove()");
	//Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	//NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropSliders(const NaviData& naviData)
{
	OGRE_LOG("NavigatorGUI::avatarPropSliders()");
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	Avatar* user = mNavigator->getUserAvatar();

	std::string slider( naviData["slider"].str().c_str() );
	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type < 0) return;

	// get the value from the slidebar position
	std::string temp = slider + ".getValue()";
	float value = atoi( navi->evaluateJS(temp).c_str() ) / 100.;

	Bone* bone = avatar->getCurrentBone();
	BodyPart* body = NULL;
	Goody* goody = NULL;

	// get the selected object ( BodyPart / Goody )
	if( avatar->getNumBodyParts() > 0 )
		body = avatar->getCurrentBodyPart();
	if( avatar->getNumGoodies() > 0 )
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
	OGRE_LOG("NavigatorGUI::avatarPropSliders()");
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();

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

	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

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

	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

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

	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

	ModifiedMaterial* material = object->getModifiedMaterial();
	if( material != 0 )
		material->setSpecular( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarColorLockAmbientDiffuse(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("$('lockAmbientdiffuse').checked");
	mLockAmbientDiffuse = (value == "true")?true:false;
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropShininess(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("shininess.getValue()");

	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

	ModifiedMaterial* material = object->getModifiedMaterial();
	if( material != 0 )
		material->setShininess( atoi(value.c_str())/100. );
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropTransparency(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("transparency.getValue()");

	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

	ModifiedMaterial* material = object->getModifiedMaterial();
	if( material != 0 )
		material->setAlpha( atoi(value.c_str())/100. );
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropScrollU(const NaviData& naviData)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("scrollU.getValue()");

	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

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
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("scrollV.getValue()");

	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

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
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("scaleU.getValue()");

	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

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
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("scaleV.getValue()");

	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

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
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
	std::string value = navi->evaluateJS("rotateU.getValue()");

	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

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
		Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
		ModifiableMaterialObject* object;

		int type = AvatarEditor::getSingletonPtr()->selectType;
		if( type == 2 ) // Goody
			object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
		else //if( type <= 1 ) // BodyPart
			object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

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
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	ModifiableMaterialObject* object;
	
	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();
/*
	//get selected texture :
	if( obj->getMaterialManager()->getNbTexture() > 1 )
	{
		TexturePtr tPtr = obj->getMaterialManager()->getCurrentTexture();
		obj->deleteTexture( tPtr );
	}

	avatarUpdateTextures( object );
*/
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropTextureApply(const NaviData& naviData)
{
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	BodyPartModel* bpm = avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

	//get selected texture :
	//if( bpm->isTextureModifiable() )
	//if( obj->getMaterialManager()->getNbTexture() > 1 )
	{
		//TexturePtr tPtr = obj->getMaterialManager()->getCurrentTexture();
		//obj->setCurrentTexture( tPtr );
	}
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropTexturePrev(const NaviData& naviData)
{
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

	object->setNextTextureAsCurrent();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarPropTextureNext(const NaviData& naviData)
{
	Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
	ModifiableMaterialObject* object;

	int type = AvatarEditor::getSingletonPtr()->selectType;
	if( type == 2 ) // Goody
		object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModel();
	else //if( type <= 1 ) // BodyPart
		object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModel();

	object->setPreviousTextureAsCurrent();
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarUpdateTextures(ModifiableMaterialObject* pObject)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);

	std::string texturePath, text;
	String str;
	TexturePtr texture; 
	Ogre::Image image;

	// Go back to the main directory
	_chdir(mNavigator->mAvatarEditor->mExecPath.c_str());
	
	// create a temporary forlder for the thumbnail textures
#ifdef WIN32
	CreateDirectory( "NaviLocal\\solTmpTexture", NULL );
#else
	system( "md NaviLocal\\solTmpTexture" );
#endif

	// update Navi interface
	text = "textTabTextures = \"";
	//if( pObject->isTextureModifiable() )
	{
		TextureVectorIterator tvIter = pObject->getTextureIterator();
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
			text += "<img src='./solTmpTexture/";
			text += fileName;
			text +=	"' width=128 height=128/>	";
	
			vector<std::string> files;
			SOLlistDirectoryFiles( "NaviLocal\\solTmpTexture\\", &files );
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
				image.save( "NaviLocal\\solTmpTexture\\" + fileName );
			}
		}
	}
		
	text += "\"";
	navi->evaluateJS(text);

	// Go back to the main directory
	_chdir(mNavigator->mAvatarEditor->mExecPath.c_str());
}
//-------------------------------------------------------------------------------------
void NavigatorGUI::avatarUpdateSliders(Vector3 pos, Vector3 ori, Vector3 scale)
{
	NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_AVATARPROP]);
	char str[48];

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
	sprintf(str, "posX.setValue(%i)", (int)pos.x);
	navi->evaluateJS(str);
	sprintf(str, "posY.setValue(%i)", (int)pos.y);
	navi->evaluateJS(str);
	sprintf(str, "posZ.setValue(%i)", (int)pos.z);
	navi->evaluateJS(str);

	// orientation
	ori = ori * 100;
	sprintf(str, "oriX.setValue(%i)", (int)ori.x);
	navi->evaluateJS(str);					   
	sprintf(str, "oriY.setValue(%i)", (int)ori.y);
	navi->evaluateJS(str);					   
	sprintf(str, "oriZ.setValue(%i)", (int)ori.z);
	navi->evaluateJS(str);

	// scale
	scale = scale * 100;
	sprintf(str, "scaleX.setValue(%i)", (int)scale.x);
	navi->evaluateJS(str);					   
	sprintf(str, "scaleY.setValue(%i)", (int)scale.y);
	navi->evaluateJS(str);					   
	sprintf(str, "scaleZ.setValue(%i)", (int)scale.z);
	navi->evaluateJS(str);

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
    OGRE_LOG("NavigatorGUI::debugCommand()");

    // Get message to send
    std::string cmd;
    std::string params;
    cmd = naviData["cmd"].str();
    params = naviData["params"].str();
    OGRE_LOG("cmd=" + cmd + ", params=" + params);

    // Push debug command
    DebugHelpers::debugCommands[String(cmd)] = String(params);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::navCommand(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::navCommand()");

    // Get command
    std::string cmd;
    cmd = naviData["cmd"].str();
    OGRE_LOG("cmd=" + cmd);

#ifdef DEMO_NAVI2
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[NAVI_DEBUG]);
    NaviLibrary::Navi* naviDemoNavi2 = mNaviMgr->getNavi("demoNavi2Video");
    if (naviDemoNavi2 == 0) return;
    if (cmd.compare("back") == 0)
        naviDemoNavi2->navigateBack();
    else if (cmd.compare("forward") == 0)
        naviDemoNavi2->navigateForward();
    else if (cmd.compare("stop") == 0)
        naviDemoNavi2->navigateStop();
    else if (cmd.compare("go") == 0)
    {
    	std::string url = navi->evaluateJS("document.getElementById('inputUrl').value");
        naviDemoNavi2->navigateTo(url);
    }
#endif
}
#endif
//-------------------------------------------------------------------------------------
NavigatorGUI::NaviPanel NavigatorGUI::getNaviPanel(const std::string& naviName)
{
    for (int n=0; n < NAVI_COUNT; ++n)
        if (mNavisNames[n].compare(naviName) == 0) return (NaviPanel)n;

    return (NaviPanel)-1;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::naviToShowPageLoaded(const NaviData& naviData)
{
    OGRE_LOG("NavigatorGUI::naviToShowPageLoaded()");

    std::string naviName;
    naviName = naviData["naviName"].str();
    NaviPanel naviPanel = getNaviPanel(naviName);
    OGRE_LOG("naviName=" + naviName + ", naviPanel=" + StringConverter::toString(naviPanel));

    // Update the properties panel from the selected object datas
	if (naviPanel == NAVI_AVATARMAIN)
	{
		NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[naviPanel]);
		Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
		Avatar* user = mNavigator->getUserAvatar();

		std::string text("$('AvatarName').innerHTML = '<p>Name : <b>");
		text += avatar->getName();
		text += "</b></p>'";
		navi->evaluateJS(text.data());

		// Setup the avatar name list
		navi->evaluateJS("$('avatarSelectTitre').innerHTML = '" + AvatarEditor::getSingletonPtr()->getName() + "'");
		text = "";
		vector<std::string>* list = AvatarEditor::getSingletonPtr()->getManager()->getNameList();
		vector<std::string>::iterator iter = list->begin();
		int id = 0;
		while(iter!=list->end())
		{
			text += "<div class='itemOut' onmouseout=this.className='itemOut' onmouseover=this.className='itemOver'><a href='#' class='lienMenu' onclick=select('";
			text += (*iter).data();
			text += "')>";
			text += (*iter).data();
			text += "</a></div>";
			iter++;
		}
		navi->evaluateJS("$('avatarSelectItem').innerHTML = \"" + text + "\"");

		// Select the avatar from the user.xml // avatarName
		char txt[6];
		int nbItem = list->size();
		if( nbItem < 7 )
		{
			sprintf(txt, "%ipx'", nbItem*16);
			text = txt;
			navi->evaluateJS("$('avatarSelectItem').style.height = '" + text);
		}
		list->clear();
	}
	else if (naviPanel == NAVI_AVATARPROP)
	{
		NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[naviPanel]);
		Character* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrent();
		Avatar* user = mNavigator->getUserAvatar();

		// Setup the animation list
		vector<std::string> list;
		for(int i=0; i<avatar->getNumAnimations(); i++)
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

		avatarTabberLoad(1);
		navi->evaluateJS("$('avatarTabbers').tabber.tabShow(1)");

		char txt[6];
		int nbItem = list.size();
		if( nbItem < 7 )
		{
			sprintf(txt, "%ipx'", nbItem*16);
			text = txt;
			navi->evaluateJS("$('animationSelectItem').style.height = '" + text);
		}
		list.clear();
	}
    else if (naviPanel == NAVI_MODELERPROP)
        modelerTabberLoad(0);
	

    // Show Navi UI
    if (mNavisStates[naviPanel] == NSCreated)
        mNaviMgr->getNavi(mNavisNames[naviPanel])->show(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::hidePreviousNavi()
{
    // Hide previous Navi UI
    if (mCurrentNavi != -1) {
        NaviLibrary::Navi* navi = mNaviMgr->getNavi(mNavisNames[mCurrentNavi]);
        navi->hide();
        mNaviMgr->destroyNavi(navi);
        mNavisStates[mCurrentNavi] = NSNotCreated;
        mCurrentNavi = -1;
    }
}

//-------------------------------------------------------------------------------------
