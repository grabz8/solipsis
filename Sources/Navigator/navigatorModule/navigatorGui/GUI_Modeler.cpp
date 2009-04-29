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

#include "GUI_Modeler.h"
#include "GUI_MessageBox.h"
#include "GUI_ModelerProperties.h"
#include "GUI_ModelerSceneFromText.h"

#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>

using namespace Solipsis;
using namespace CommonTools;
//-------------------------------------------------------------------------------------

const std::string GUI_Modeler::ms_ModelerErrors[] = {
    "You have to select an object3D.",
    "This Texture is already open.",
    "File not found.",
    "Something went wrong with declarative modeling. Please re-formulate your text."
};


GUI_Modeler * GUI_Modeler::stGUI_Modeler = NULL;

GUI_Modeler::GUI_Modeler() : GUI_Panel("uimdlrmain")
{
    stGUI_Modeler = this;
    mNavigator = Navigator::getSingletonPtr();

}

/*static*/ bool GUI_Modeler::createAndShowPanel()
{
    if (!stGUI_Modeler)
    {
        new GUI_Modeler();
    }

    return stGUI_Modeler->show();
}

/*static*/ void GUI_Modeler::unload()
{
    if (!stGUI_Modeler)
    {
        return;
    } 

     stGUI_Modeler->destroy();
    return;
}  


/*static*/ bool GUI_Modeler::isPanelVisible()
{
    if (!stGUI_Modeler)
    {
        return false;
    } 

    return stGUI_Modeler->isVisible();
}  



bool GUI_Modeler::show()
{
    // hide modeler Prop
   GUI_ModelerProperties::hidePanel();

    // hide declarative modeler
   GUI_ModelerProperties::hidePanel();

    if (m_curState == NSNotCreated)
    {
        // Create Navi UI modeler
        createNavi("local://uimdlrmain.html", NaviPosition(TopRight), 256, 512);

        mNavi->setMovable(true);
        mNavi->setMask("uimdlrmain.png");//Eliminate the black shadow at the margin of the menu
        mNavi->setOpacity(0.75f);
        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_Modeler::onPageLoaded));
        mNavi->bind("FileImport", NaviDelegate(this, &GUI_Modeler::modelerMainFileImport));
        mNavi->bind("FileSave", NaviDelegate(this, &GUI_Modeler::modelerMainFileSave));
        mNavi->bind("FileExit", NaviDelegate(this, &GUI_Modeler::modelerMainFileExit));
        mNavi->bind("CreatePlane", NaviDelegate(this, &GUI_Modeler::modelerMainCreatePlane)); 
        mNavi->bind("CreateBox", NaviDelegate(this, &GUI_Modeler::modelerMainCreateBox)); 
        mNavi->bind("CreateCorner", NaviDelegate(this, &GUI_Modeler::modelerMainCreateCorner)); 
        mNavi->bind("CreatePyramid", NaviDelegate(this, &GUI_Modeler::modelerMainCreatePyramid));
        mNavi->bind("CreatePrism", NaviDelegate(this, &GUI_Modeler::modelerMainCreatePrism));
        mNavi->bind("CreateCylinder", NaviDelegate(this, &GUI_Modeler::modelerMainCreateCylinder)); 
        mNavi->bind("CreateHalfCylinder", NaviDelegate(this, &GUI_Modeler::modelerMainCreateHalfCylinder)); 
        mNavi->bind("CreateCone", NaviDelegate(this, &GUI_Modeler::modelerMainCreateCone)); 
        mNavi->bind("CreateHalfCone", NaviDelegate(this, &GUI_Modeler::modelerMainCreateHalfCone)); 
        mNavi->bind("CreateSphere", NaviDelegate(this, &GUI_Modeler::modelerMainCreateSphere)); 
        mNavi->bind("CreateHalfSphere", NaviDelegate(this, &GUI_Modeler::modelerMainCreateHalfSphere)); 
        mNavi->bind("CreateTorus", NaviDelegate(this, &GUI_Modeler::modelerMainCreateTorus)); 
        mNavi->bind("CreateTube", NaviDelegate(this, &GUI_Modeler::modelerMainCreateTube)); 
        mNavi->bind("CreateRing", NaviDelegate(this, &GUI_Modeler::modelerMainCreateRing)); 
        mNavi->bind("CreateSceneFromText", NaviDelegate(this, &GUI_Modeler::modelerMainCreateSceneFromText)); 
        mNavi->bind("ActionDelete", NaviDelegate(this, &GUI_Modeler::modelerActionDelete)); 
        mNavi->bind("ActionMove", NaviDelegate(this, &GUI_Modeler::modelerActionMove)); 
        mNavi->bind("ActionRotate", NaviDelegate(this, &GUI_Modeler::modelerActionRotate)); 
        mNavi->bind("ActionScale", NaviDelegate(this, &GUI_Modeler::modelerActionScale)); 
        mNavi->bind("ActionLink", NaviDelegate(this, &GUI_Modeler::modelerActionLink)); 
        mNavi->bind("ActionProperties", NaviDelegate(this, &GUI_Modeler::modelerActionProperties)); 

        m_curState = NSCreated;
    }
    else
        mNavi->show(true);

    mNavigator->startModeling();

    Modeler *modeler = mNavigator->getModeler();
    if (modeler)
        modeler->lockSelection(false);

    return true;
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::onPageLoaded(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::naviToShowPageLoaded()");

    std::string naviName;
    naviName = naviData["naviName"].str();

    // Show Navi UI
    if (m_curState == NSCreated)
    {
        mNavi->show(true);
        mNavi->focus();
    }

    mCurrentNaviCreationDate = 0;
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::destroy()
{
    if (m_curState != NSNotCreated)
    {
        // Destroy Navi UI modeler
        mNavi->hide();
        NaviManager::Get().destroyNavi(mNavi);
        m_curState = NSNotCreated;

        mNavigator->endModeling();

        GUI_ModelerProperties::unloadPanel();
        GUI_ModelerSceneFromText::unloadPanel();

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
void GUI_Modeler::modelerMainFileImport(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainFileImport()");

    mNavigator->mdlrXMLImport();
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainFileSave(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainFileSave()");

    //modelerMainUnload();
    mNavigator->mdlrXMLSave();
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainFileExit(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainFileExit()");

    GUI_Modeler::unload();
    mNavigator->setCameraMode(mNavigator->getLastCameraMode());
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreatePlane(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreatePlane()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::PLANE);
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreateBox(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreateBox()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::BOX);
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreateCorner(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreateCorner()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::CORNER);
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreatePyramid(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreatePyramid()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::PYRAMID);
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreatePrism(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreatePrism()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::PRISM);
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreateCylinder(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreateCylinder()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::CYLINDER);
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreateHalfCylinder(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreateHalfCylinder()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::HALF_CYLINDER);
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreateCone(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreateCone()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::CONE);
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreateHalfCone(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreateHalfCone()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::HALF_CONE);
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreateSphere(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreateSphere()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::SPHERE);
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreateHalfSphere(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreateHalfSphere()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::HALF_SPHERE);
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreateTorus(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreateTorus()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::TORUS);
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreateTube(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreateTube()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::TUBE);
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerMainCreateRing(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreateRing()");
    //mNavigator->startModeling();
    mNavigator->createPrimitive(Object3D::RING);
}

//-------------------------------------------------------------------------------------

void GUI_Modeler::modelerMainCreateSceneFromText(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerMainCreateSceneFromText()");
    GUI_ModelerSceneFromText::showPanel();
    //	mNavigator->createSceneFromText( "A red ball is on a green box." );
}


//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerActionDelete(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerActionDelete()");

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
        {
            GUI_MessageBox::getMsgBox()->show(
                "Modeler error", 
                ms_ModelerErrors[ME_NOOBJECTSELECTED], 
                GUI_MessageBox::MBB_OK, 
                GUI_MessageBox::MBB_INFO);
        }
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerActionMove(const NaviData& naviData)
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
    {
        GUI_MessageBox::getMsgBox()->show(
            "Modeler error", ms_ModelerErrors[ME_NOOBJECTSELECTED], 
            GUI_MessageBox::MBB_OK, GUI_MessageBox::MBB_INFO);
    }
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerActionRotate(const NaviData& naviData)
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
    {
        GUI_MessageBox::getMsgBox()->show(
            "Modeler error", 
            ms_ModelerErrors[ME_NOOBJECTSELECTED], 
            GUI_MessageBox::MBB_OK, GUI_MessageBox::MBB_INFO);
    }
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerActionScale(const NaviData& naviData)
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
    {
        GUI_MessageBox::getMsgBox()->show(
            "Modeler error", ms_ModelerErrors[ME_NOOBJECTSELECTED], 
            GUI_MessageBox::MBB_OK, 
            GUI_MessageBox::MBB_INFO);
    }
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerActionLink(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerActionLink()");

    Modeler *modeler = mNavigator->getModeler();
    if (!modeler->isSelectionEmpty())
        modeler->lockLinkMode(true);
    else
    {
        GUI_MessageBox::getMsgBox()->show(
            "Modeler error", ms_ModelerErrors[ME_NOOBJECTSELECTED], 
            GUI_MessageBox::MBB_OK, GUI_MessageBox::MBB_INFO);
    }
}

//-------------------------------------------------------------------------------------
void GUI_Modeler::modelerActionProperties(const NaviData& naviData)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerActionProperties()");

    // Test if an Object3D has ever been created before
    Modeler *modeler = mNavigator->getModeler();
    if (!modeler->isSelectionEmpty())
    {
        // Hide the main modeler panel
        hide();

        // Show the properties modeler panel
        GUI_ModelerProperties::createAndShowPanel();
    }
    else
    {
        GUI_MessageBox::getMsgBox()->show(
            "Modeler error", 
            ms_ModelerErrors[ME_NOOBJECTSELECTED], 
            GUI_MessageBox::MBB_OK, 
            GUI_MessageBox::MBB_INFO);
    }
}
