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
#include "Panel2DMgr.h"

using namespace NaviLibrary;

namespace Solipsis {

class Navigator;
class AvatarEditor;
class GUI_Panel;

/** This class manages all Graphical User Interfaces of the Navigator.
 */
class NavigatorGUI : public WindowEventListener
{

protected:
    Navigator* mNavigator;
    NaviManager* mNaviMgr;
    Panel2DMgr* mPanel2DMgr;

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

public:
    // set 1 navi visibility
    bool setNaviVisibility(const String& naviName, bool show);

    // create a 2D panel from 1 navi, 1 VLC material or 1 SWF material
    bool createPanel2D(const String& type, const String& name);

 
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