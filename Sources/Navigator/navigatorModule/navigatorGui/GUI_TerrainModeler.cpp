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

#include "GUI_TerrainModeler.h"


#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>
#include <CTNetSocket.h>

using namespace Solipsis;
using namespace CommonTools;

//-------------------------------------------------------------------------------------

GUI_TerrainModeler * GUI_TerrainModeler::stGUI_TerrainModeler = NULL;

GUI_TerrainModeler::GUI_TerrainModeler() : GUI_Panel("uimdlrTerrain")
{
    stGUI_TerrainModeler = this;
}

void GUI_TerrainModeler::showPanel()
{
    if (!stGUI_TerrainModeler)
    {
        new GUI_TerrainModeler();
    }

    stGUI_TerrainModeler->show();
}

/*static*/ void GUI_TerrainModeler::unloadPanel()
{
    if (!stGUI_TerrainModeler)
    {
        return;
    }

    stGUI_TerrainModeler->destroy();
}

bool GUI_TerrainModeler::show()
{

    if (m_curState == NSNotCreated)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_TerrainModeler::show()");

        // Create Navi UI modeler
        createNavi(  TopRight, 512, 512);
        mNavi->loadFile("uimdlrterrain.html");
        mNavi->setMovable(true);
        mNavi->hide();
        mNavi->setMask("uimdlrterrain.png");//Eliminate the black shadow at the margin of the menu
        //navi->setOpacity(0.75f);

        // page loaded
        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_TerrainModeler::onPageLoaded));

        mNavi->bind("MdlrTCreate", NaviDelegate(this, &GUI_TerrainModeler::onExec));
        mNavi->bind("MdlrTCancel", NaviDelegate(this, &GUI_TerrainModeler::onCancelled));

        m_curState = NSCreated;

    }
    else 
    {
        mNavi->show(true);
    }


    return true;
}

//-------------------------------------------------------------------------------------
void GUI_TerrainModeler::onPageLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_TerrainModeler::modelerSceneFromTextPageLoaded()");

    // Show Navi UI
    if (m_curState == NSCreated)
        mNavi->show(true);
}

//-------------------------------------------------------------------------------------
void GUI_TerrainModeler::onExec(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_TerrainModeler::onExec()");

    int nbOctaves = 5;

    std::string noiseScale = mNavi->evaluateJSWithResult("noiseScale.getValue()").get().toString();
    std::string msg = "noiseScale="+noiseScale;
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, msg.data());

    std::string granularity = mNavi->evaluateJSWithResult("granularity.getValue()").get().toString();
    msg = "granularity="+granularity;
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, msg.data());

    Navigator::getSingletonPtr()->createTerrain( nbOctaves, atoi( noiseScale.c_str() ), atof(granularity.c_str() ) / 100 );
    destroy();
}

//-------------------------------------------------------------------------------------
void GUI_TerrainModeler::onCancelled(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_TerrainModeler::modelerSceneFromTextSetUpCancelled()");
    destroy();
}

