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

#ifndef __GUI_FromServer_h__
#define __GUI_FromServer_h__

#include "GUI_Panel.h"

using namespace NaviLibrary;

namespace Solipsis 
{

    //! base class for gui loaded from a server (like the worlds server)
    // error handling is common to
    class GUI_FromServer : public GUI_Panel , public NaviEventListener
    {
    public:
        GUI_FromServer(const std::string & panelName);

        ////// NaviEventListener Interface /////////////
        virtual void onCallback(const std::string& name, const Awesomium::JSArguments& args){}
        virtual void onLocationChange(Navi *caller, const std::string &url) {}
        virtual void onNavigateComplete(Navi *caller, const std::string &url, int responseCode);

        // update for timeout
        virtual void update();

        void serverCompatibilityError();
        void serverError();

    protected:
        Navigator * mNavigator;
    };


} // namespace Solipsis

#endif // #ifndef __GUI_FromServer_h__