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

#ifndef __GUI_MessageBox_h__
#define __GUI_MessageBox_h__

// #include <Facebook.h>
#include "NavigatorGui.h"
#include "MainApplication/Navigator.h"
// #include "World/Modeler.h"
// #include "ModifiableMaterialObject.h"

using namespace NaviLibrary;

namespace Solipsis {

    class GUI_Panel
    {
    public: 
        GUI_Panel(const std::string & panelName)
        {
            mPanelName = panelName;
            m_curState = NavigatorGUI::NSNotCreated;
        }

    protected:
        void switchLuaNavi(bool createDestroy);

        NavigatorGUI::NaviState m_curState;
        std::string mPanelName;
        // the navi panel
        NaviLibrary::Navi* mNavi;
    };

    /** This class manages all Graphical User Interfaces of the Navigator.
    */
    class GUI_MessageBox : public GUI_Panel
    {
    public:
        enum MsgBoxButtons {
            MBB_OK = 1,
            MBB_CANCEL = MBB_OK >> 1,
            MBB_YESNO = MBB_CANCEL >> 1
        };

        enum MsgBoxIcon {
            MBB_INFO = 0,
            MBB_QUESTION,
            MBB_ERROR,
            MBB_EXCLAMATION
        };

        enum MsgBoxDisplayed {
            MBD_NONE = 0,
            MBD_WORLDSSERVERERROR,
            MBD_WORLDSSERVERCOMPATIBILITYERROR,
            MBD_AUTHENTFBERROR,
            MBD_AUTHENTWSERROR,
            MBD_CONNECTIONLOSTERROR,
            MBD_CONNECTIONERROR
        };

    public:

        // Show a message box
        void show(const std::string& titleText, 
            const std::string& msgText, 
            MsgBoxButtons buttons, 
            MsgBoxIcon icon,
            MsgBoxDisplayed msgBoxDisplayed = MBD_NONE);

        void hide();
        bool isVisible();

        static GUI_MessageBox * getMsgBox()
        {
            if (!st_GUI_MessageBox) st_GUI_MessageBox = new GUI_MessageBox();
            return st_GUI_MessageBox;
        }

    protected:
        GUI_MessageBox() : GUI_Panel("uimsgbox")
        {
            st_GUI_MessageBox = this;
        }

        ~GUI_MessageBox();

    
        static GUI_MessageBox * st_GUI_MessageBox;

        MsgBoxDisplayed mMsgBoxDisplayed;
        std::string mMsgBoxTitleText;
        std::string mMsgBoxMsgText;
        MsgBoxButtons mMsgBoxButtons;
        MsgBoxIcon mMsgBoxIcon;

        void messageBoxPageLoaded(const NaviData& naviData);
        void messageBoxResponse(const NaviData& naviData);
    };

} // namespace Solipsis

#endif // #ifndef __GUI_MessageBox_h__