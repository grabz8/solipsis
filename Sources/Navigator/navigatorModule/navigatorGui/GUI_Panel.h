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

#ifndef __GUI_Panel_h__
#define __GUI_Panel_h__

#include "NavigatorGui.h"

using namespace NaviLibrary;

namespace Solipsis 
{
    //! base class for all GUI panel in solipsis
    class GUI_Panel
    {
    public: 
        enum NaviState {
            NSNotCreated,   // Navi page not created
            NSCreated       // Navi page created and not visible
        };

        GUI_Panel(const std::string & panelName);
        virtual ~GUI_Panel();

        // Show the login page
        virtual bool show();
        virtual void hide();
        virtual void destroy();
        virtual bool isVisible();
        virtual void update() {        };

        virtual void windowResized(RenderWindow* rw) {};

        const std::string & getPanelName() {return mPanelName;}

        // destroy the previous page
        void destroyPreviousNavi();

        // usual function on page loaded
        void onPanelLoaded(const NaviData& naviData);

        void fitOnScreen(int & w, int & h);

    protected:
        void switchLuaNavi(bool createDestroy);
        void createNavi(const std::string &homepage, const NaviPosition &naviPosition,
            unsigned short width, unsigned short height, unsigned short zOrder = 0);
        void createNavi(const std::string &homepage,  int x, int y,
            unsigned short width, unsigned short height, unsigned short zOrder = 0);


        NaviState m_curState;
        std::string mPanelName;
        // the navi panel
        NaviLibrary::Navi* mNavi;

        // used for timeout
        unsigned long mCurrentNaviCreationDate;
    };


} // namespace Solipsis

#endif // #ifndef __GUI_Panel_h__