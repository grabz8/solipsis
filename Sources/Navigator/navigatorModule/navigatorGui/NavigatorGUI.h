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

#ifndef __NavigatorGUI_h__
#define __NavigatorGUI_h__

#include <Facebook.h>
#include "NaviManager.h"
#include "World/Modeler.h"
#include "ModifiableMaterialObject.h"

using namespace NaviLibrary;

namespace Solipsis {

class Navigator;
class AvatarEditor;
class GUI_Panel;

/** This class manages all Graphical User Interfaces of the Navigator.
 */
class NavigatorGUI : public WindowEventListener
{
public:
    enum NaviState {
        NSNotCreated,   // Navi page not created
        NSCreated       // Navi page created and not visible
    };
    
    enum NaviPanel {

        FAKE,
        NAVI_COUNT
    };

 

protected:
    Navigator* mNavigator;
    NaviManager* mNaviMgr;
    static const std::string ms_NavisNames[NAVI_COUNT];

    NaviState mNavisStates[NAVI_COUNT];
    int mCurrentNavi;
    int mCurrentCtxtPanel;
    unsigned long mCurrentNaviCreationDate;
    std::string mLoginInfosText;


public:
    NavigatorGUI(Navigator* navigator);
    ~NavigatorGUI();

    // Start up GUI
    bool startup();
    // Update
    void update();

    /** See Ogre::WindowEventListener. */
    virtual void windowResized(RenderWindow* rw);

    // Mouse
    void SetMouseVisibility(bool visible);
    bool isMouseVisible();

    // Interfaces
//     void login();
    void inWorld();

    void connectionLostError();
    void connectionServerError();

protected:

    // Helpers
    NaviPanel getNaviPanel(const std::string& naviName);
    void naviToShowPageLoaded(const NaviData& naviData);
    void hidePreviousNavi();
    void destroyNavi(NaviPanel naviPanel);
 
public:
    bool setNaviVisibility(const std::string& naviName, bool show);

    ///////////////////// à supprimer à terme
    // à supprimer ? oui à terme
    void switchLuaNavi(NaviPanel naviPanel, bool createDestroy = false);

    const std::string& getNaviName(NaviPanel naviPanel);

    static NaviLibrary::Navi * getNavi(const std::string& naviName)
    {   
        return mNaviGui->mNaviMgr->getNavi(naviName);
    }

    static NaviManager* getNaviMgr()
    {   
        return mNaviGui->mNaviMgr;
    }
    
    ///////////////////////////////////////////////
    static NavigatorGUI * mNaviGui;

////////////////////////// nouvelle implémentation ////////////////////
    // a a panel to the list
    static void registerGuiPanel(GUI_Panel *pPanel);
    static void unregisterGuiPanel(GUI_Panel *pPanel);
    static void destroyAllRegisteredPanels();

    static void setCurrentPanel(GUI_Panel * pCurrentPanel)
    {
        mNaviGui->m_pCurrentPanel = pCurrentPanel;
        if (pCurrentPanel == NULL)
        {
            mNaviGui->mCurrentNaviCreationDate = 0;
        }
    }

    static void destroyCurrentPanel();

    static GUI_Panel * getCurrentPanel()
    {
        return mNaviGui->m_pCurrentPanel;
    }

    std::map<std::string, GUI_Panel *> m_panels;
    GUI_Panel * m_pCurrentPanel;
};

} // namespace Solipsis

#endif // #ifndef __NavigatorGUI_h__