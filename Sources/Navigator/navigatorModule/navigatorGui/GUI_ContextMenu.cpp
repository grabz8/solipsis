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

#include "GUI_ContextMenu.h"
#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>

#include "Tools/DebugHelpers.h"
#include <OgreExternalTextureSourceManager.h>
#include "OgreTools/ExternalTextureSourceEx.h"


using namespace Solipsis;
using namespace CommonTools;
//-------------------------------------------------------------------------------------

const std::string GUI_ContextMenu::ms_NavisContexts[] = 
{
    "uictxtavatar",
    "uictxtwww",
    "uictxtswf",
    "uictxtvlc",
    "uictxtvnc"
};

GUI_ContextMenu * GUI_ContextMenu::stGUI_ContextMenu = NULL;

GUI_ContextMenu::GUI_ContextMenu() : GUI_Panel("contextMenu")
{
    m_curContext = NAVI_CTXTCOUNT;
    stGUI_ContextMenu = this;
}

bool GUI_ContextMenu::createAndShowPanel(int x, int y, NaviContext ctxtPanel, const String& params)
{
    if (!stGUI_ContextMenu)
    {
        new GUI_ContextMenu();
    }

    return stGUI_ContextMenu->show(x, y, ctxtPanel, params);
}

void GUI_ContextMenu::destroyMenu()
{
    if (!stGUI_ContextMenu) return;

    stGUI_ContextMenu->destroy();
}


void clampNaviOnScreen(int &x, int & y, int w, int h )
{
    int cx = x - w/2;
    int cy = y - h/2;

    unsigned int scrWidth, scrHeight, colourDepth;
    int left, top;
    Navigator::getSingletonPtr()->getRenderWindowPtr()->getMetrics(scrWidth, scrHeight, colourDepth, left, top);

    if (cx > ((int) scrWidth - w)) 
        cx = ((int) scrWidth - w);
    else if (cx < w)
        cx = w;

    if (cy > ((int) scrHeight - h)) 
        cy = ((int) scrHeight - h);
    else if (cy < h)
        cy = h;

    x = cx;
    y = cy;
}





bool GUI_ContextMenu::show(int x, int y, NaviContext ctxtPanel, const String& params)
{
    if (m_curState != NSNotCreated)
        destroy();

    m_curContext = ctxtPanel;
    int  naviW, naviH;

    switch (m_curContext)
    {
    case NAVI_CTXTAVATAR:
        {
            int itemW, itemH;

            itemH = itemW = 50;
            naviW = naviH = 256;

            clampNaviOnScreen(x, y, naviW, naviH);

            createNavi("", x, y,naviW, naviH);
            mNavi->setMovable(false);
            mNavi->hide();
            // no more found in awesomium
            //            mNavi->setColorKey("#010203", 0, "#000000");
            mNavi->setTransparent(true);
            mNavi->setOpacity(0.75);

            mNavi->setProperty("naviDataName", "uictxtavatarDatas");
            mNavi->setProperty("items", params);
            mNavi->setProperty("itemWidth", itemW);
            mNavi->setProperty("itemHeight", itemH);

            mNavi->loadURL("local://uictxtavatar.html");

            mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_ContextMenu::onPanelLoaded));
            mNavi->bind("contextItemSelected", NaviDelegate(this, &GUI_ContextMenu::onAvatarSelect));
        }
        break;

    case NAVI_CTXTWWW:
        {
            naviW = 256; naviH = 48;  
            clampNaviOnScreen(x, y, naviW, naviH);

            createNavi("", x, y,naviW, naviH);
            mNavi->setMovable(false);
            mNavi->hide();

            mNavi->setTransparent(true);

//            mNavi->setColorKey("#010203", 0, "#000000");
            mNavi->setOpacity(0.8);
            mNavi->setMaxUPS(8);
//            mNavi->setForceMaxUpdate(false);
            mNavi->setAutoUpdateOnFocus(true);
   
            mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_ContextMenu::onWWWPanelLoaded));
            mNavi->bind("navCommand", NaviDelegate(this, &GUI_ContextMenu::onWWWCommand));

            mNavi->setProperty("naviDataName","uictxtwwwDatas");
            mNavi->setProperty("ctxtNaviName", params);
 
            mNavi->loadURL("local://uictxtwww.html");
       }
        break;
    case NAVI_CTXTVLC:
        {
            naviW = 286;
            naviH = 64;
            clampNaviOnScreen(x, y, naviW, naviH);
            String ctxtVLCName = params;

            createNavi("", x, y,naviW, naviH);
            mNavi->setMovable(false);
            mNavi->hide();
            mNavi->setOpacity(0.8);

            mNavi->setMaxUPS(8);
            mNavi->setAutoUpdateOnFocus(true);

            mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_ContextMenu::onVLCPanelLoaded));
            mNavi->bind("vlcCommand", NaviDelegate(this, &GUI_ContextMenu::onVLCCommand));

            mNavi->setProperty("naviDataName","uictxtvlcDatas");
            mNavi->setProperty("ctxtVLCName", ctxtVLCName);

            mNavi->loadURL("local://uictxtvlc.html");
        }
        break;
    case NAVI_CTXTSWF:
        {
            naviW = 286;
            naviH = 64;
            clampNaviOnScreen(x, y, naviW, naviH);
            String ctxtSWFName = params;

            createNavi("", x, y,naviW, naviH);
            mNavi->setMovable(false);
            mNavi->hide();
            mNavi->setOpacity(0.8);

            mNavi->setMaxUPS(8);
            mNavi->setAutoUpdateOnFocus(true);

            mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_ContextMenu::onSWFPanelLoaded));
            mNavi->bind("swfCommand", NaviDelegate(this, &GUI_ContextMenu::onSWFCommand));

            mNavi->setProperty("naviDataName","uictxtswfDatas");
            mNavi->setProperty("ctxtSWFName", ctxtSWFName);

            mNavi->loadURL("local://uictxtswf.html");
       }
        break;
    }
  
    m_curState = NSCreated;
    return true;
}

//-------------------------------------------------------------------------------------
bool GUI_ContextMenu::isContextVisible()
{
    if (!stGUI_ContextMenu || !stGUI_ContextMenu->mNavi) return false;

    return (stGUI_ContextMenu->mNavi->getVisibility());
}

//-------------------------------------------------------------------------------------
bool GUI_ContextMenu::isContextFocused()
{
    if (!stGUI_ContextMenu || !stGUI_ContextMenu->mNavi) return false;
    return (stGUI_ContextMenu->mNavi == NaviManager::Get().getFocusedNavi());
}

//-------------------------------------------------------------------------------------
void GUI_ContextMenu::destroy()
{
    if (m_curState == NSNotCreated || !mNavi) return;

    // Destroy Navi UI context
    NaviManager::Get().destroyNavi("contextMenu");

    m_curState = NSNotCreated;
    m_curContext = NAVI_CTXTCOUNT;

    mNavi = NULL; 
}

void GUI_ContextMenu::onAvatarSelect(Navi* caller, const Awesomium::JSArguments& args)
{
    Navigator::getSingletonPtr()->contextItemSelected(args[0].toString());
}


// usual function on page loaded
void GUI_ContextMenu::onPanelLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
  mNavi->show();  

}


// usual function on page loaded
void GUI_ContextMenu::onWWWPanelLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    String ctxtNaviName = args[0].toString();

/*
    no more available
    if (NaviLibrary::NaviManager::Get().getNavi(ctxtNaviName)->canNavigateBack())
    {
        mNavi->evaluateJS("$('mTbIconBack').addClass('mozToolbarBackActive')");   
    }
    
    if (NaviLibrary::NaviManager::Get().getNavi(ctxtNaviName)->canNavigateForward())
    {
        mNavi->evaluateJS("$('mTbIconFwd').addClass('mozToolbarFwdActive')");
    }    
 */
  
    String location =  NaviLibrary::NaviManager::Get().getNavi(ctxtNaviName)->getCurrentLocation();
    String JSCommand = "$('inputUrl').value = '" + location + "'";
    mNavi->evaluateJS(JSCommand);

    mNavi->show(); 
}




// usual function on page loaded
void GUI_ContextMenu::onWWWCommand(Navi* caller, const Awesomium::JSArguments& args)
{
    String ctxtNaviName = args[0].toString();
    Navi * pNaviDest = NaviLibrary::NaviManager::Get().getNavi(ctxtNaviName);
    String cmd = args[1].toString();

    if (cmd == "go") 
    {
       String url = mNavi->evaluateJSWithResult( "$('inputUrl').value").get().toString();
       pNaviDest->loadURL( url);
    }
/*    else if (cmd == "back") 
    {
        pNaviDest->navigateBack();
    }
    else if (cmd == "fwd") 
    {
        pNaviDest->navigateForward();

    }*/
    else if (cmd == "refresh") 
    {
        pNaviDest->evaluateJS( "window.location.reload(false)");
    }
  /*  else if (cmd == "stop") 
    {
        pNaviDest->navigateStop();
    }*/
    else if (cmd == "home") 
    {
        pNaviDest->loadURL( "http://www.solipsis.org" );
    }
    else if (cmd == "maximize") 
    {
        Navigator::getSingletonPtr()->getNavigatorGUI()->createPanel2D("www", ctxtNaviName);   
    }

    destroy();
}


//-------------------------------------------------------------------------------------
String extTextSrcExHandleEvt(const String & extTextSrcExPlugin, const String & mtlName, const String & extTextSrcExEvt)
{
    // Perform action
    ExternalTextureSourceManager::getSingleton().setCurrentPlugIn(extTextSrcExPlugin);
    ExternalTextureSourceEx* extTextSrcEx = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource(extTextSrcExPlugin));
    return extTextSrcEx->handleEvt(mtlName, extTextSrcExEvt);
}

// SWF callbacks 
void GUI_ContextMenu::onSWFPanelLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    String ctxtSWFName = args[0].toString(); 

    String url = extTextSrcExHandleEvt("swf", ctxtSWFName, "geturl");
    StringHelpers::replaceSubStr(url, "[\\]", "\\\\");

    mNavi->evaluateJS("$('inputUrl').value = '" + url + "'");
    String mute = extTextSrcExHandleEvt("swf", ctxtSWFName, "getmute");
    if (mute == "true")
        mNavi->evaluateJS("$('mTbIconVolume').addClass('swfToolbarVolumeOff')");
    else
        mNavi->evaluateJS("$('mTbIconVolume').removeClass('swfToolbarVolumeOff')");

    mNavi->show(); 
}

void GUI_ContextMenu::onSWFCommand(Navi* caller, const Awesomium::JSArguments& args)
{
    String ctxtSWFName = args[0].toString();
    String cmd = args[1].toString();

    if (cmd == "seturl")  
    {
        String url = mNavi->evaluateJSWithResult( "$('inputUrl').value").get().toString();
        extTextSrcExHandleEvt("swf", ctxtSWFName, "seturl?" + url);
    }
    else if (cmd == "maximize") 
    {
        Navigator::getSingletonPtr()->getNavigatorGUI()->createPanel2D("swf", ctxtSWFName);   
    }
    else 
    {
        extTextSrcExHandleEvt("swf", ctxtSWFName, cmd);
    }

    destroy();
}

// VLC callbacks 
void GUI_ContextMenu::onVLCPanelLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    String ctxtVLCName = args[0].toString(); 

    String url = extTextSrcExHandleEvt("vlc", ctxtVLCName, "getmrl");
   StringHelpers::replaceSubStr(url, "[\\]", "\\\\");

    mNavi->evaluateJS("$('inputMrl').value = '" + url + "'");
    String mute = extTextSrcExHandleEvt("vlc", ctxtVLCName, "getmute");
    if (mute == "true") 
        mNavi->evaluateJS("$('mTbIconVolume').addClass('vlcToolbarVolumeOff')");
    else
        mNavi->evaluateJS("$('mTbIconVolume').removeClass('vlcToolbarVolumeOff')");

    mNavi->show(); 

}

void GUI_ContextMenu::onVLCCommand(Navi* caller, const Awesomium::JSArguments& args)
{
    String ctxtVLCName = args[0].toString();
    String cmd = args[1].toString();

    if (cmd == "setmrl") 
    {
        String mrl = mNavi->evaluateJSWithResult( "$('inputMrl').value").get().toString();
        extTextSrcExHandleEvt("vlc", ctxtVLCName, "setmrl?" + mrl);
    }
    else if (cmd == "maximize") 
    {
        Navigator::getSingletonPtr()->getNavigatorGUI()->createPanel2D("vlc", ctxtVLCName);   
    }
    else 
    {
        extTextSrcExHandleEvt("vlc", ctxtVLCName, cmd);
    }

    destroy();
}

// VNC callbacks 
void GUI_ContextMenu::onVNCPanelLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    
}

void GUI_ContextMenu::onVNCCommand(Navi* caller, const Awesomium::JSArguments& args)
{

}