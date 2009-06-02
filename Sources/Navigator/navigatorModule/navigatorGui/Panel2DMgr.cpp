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

#include "Panel2DMgr.h"
#include "Panel2D.h"
#include "Panel2DWWW.h"
#include "Panel2DExtTextSrcEx.h"
#include <CTLog.h>
#include <CTMaths.h>
//#include <NaviMouse.h>

using namespace Ogre;
using namespace CommonTools;

Solipsis::Panel2DMgr *CommonTools::Singleton<Solipsis::Panel2DMgr>::ms_Singleton = 0;

namespace Solipsis {

// Default Panel2D factories
Panel2DWWWFactory panel2DWWWFactory;
Panel2DVLCFactory panel2DVLCFactory;
Panel2DVNCFactory panel2DVNCFactory;
Panel2DSWFFactory panel2DSWFFactory;

const ushort Panel2DMgr::ms_DefaultMinZOrder = 100;
const ushort Panel2DMgr::ms_DefaultMaxZOrder = 199;
const int Panel2DMgr::ms_DefaultCreationShift = 32;

struct compare { bool operator()(Panel2D* p1, Panel2D* p2) { return( *p1 > *p2); } };

//-------------------------------------------------------------------------------------
Panel2DMgr::Panel2DMgr() :
    mFocusedPanel(0),
    mMinZOrder(ms_DefaultMinZOrder),
    mMaxZOrder(ms_DefaultMaxZOrder),
    mZOrderCounter(ms_DefaultMinZOrder),
    mNextCreationPosX(ms_DefaultCreationShift), mNextCreationPosY(ms_DefaultCreationShift),
    mMoving(false),
    mSizingPart(Panel2D::PNone)
{
    registerPanel2DFactory(&panel2DWWWFactory);
    registerPanel2DFactory(&panel2DVLCFactory);
    registerPanel2DFactory(&panel2DVNCFactory);
    registerPanel2DFactory(&panel2DSWFFactory);
}

//-------------------------------------------------------------------------------------
Panel2DMgr::~Panel2DMgr()
{
    destroyAllPanels();

    unregisterPanel2DFactory(&panel2DWWWFactory);
    unregisterPanel2DFactory(&panel2DVLCFactory);
    unregisterPanel2DFactory(&panel2DVNCFactory);
    unregisterPanel2DFactory(&panel2DSWFFactory);
}

//-------------------------------------------------------------------------------------
void Panel2DMgr::initializeMouseCursors()
{
/*    NaviLibrary::NaviMouse *naviMouse = NaviLibrary::NaviMouse::GetPointer();
    if (naviMouse == 0)
        return;

    NaviLibrary::NaviCursor *newCursor;
    newCursor = naviMouse->createCursor("resizeH", 19, 19);
    newCursor->addFrame(0, "cursorResizeH.png");
    newCursor = naviMouse->createCursor("resizeV", 19, 19);
    newCursor->addFrame(0, "cursorResizeV.png");
    newCursor = naviMouse->createCursor("resize45", 19, 19);
    newCursor->addFrame(0, "cursorResize45.png");
    newCursor = naviMouse->createCursor("resize315", 19, 19);
    newCursor->addFrame(0, "cursorResize315.png");*/
}

//-------------------------------------------------------------------------------------
void Panel2DMgr::setZOrderMinMax(ushort minZOrder, ushort maxZOrder)
{
    mMinZOrder = minZOrder;
    mMaxZOrder = maxZOrder;
    recomputeZOrder();
}

//-------------------------------------------------------------------------------------
void Panel2DMgr::registerPanel2DFactory(Panel2DFactory* factory)
{
    if (factory == 0)
        return;
    mPanelFactoryMap[factory->getType()] = factory;
}

//-------------------------------------------------------------------------------------
void Panel2DMgr::unregisterPanel2DFactory(Panel2DFactory* factory)
{
    if (factory == 0)
        return;
    mPanelFactoryMap.erase(factory->getType());
}

//-------------------------------------------------------------------------------------
Panel2D* Panel2DMgr::createPanel(const String& type, const String& name)
{
    Panel2D *panel = getPanel(name);
    if (panel != 0)
        return 0;

    recomputeZOrder();
    ushort zOrder = mZOrderCounter++;

    PanelFactoryMap::const_iterator factoryIt = mPanelFactoryMap.find(type);
    if (factoryIt == mPanelFactoryMap.end())
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Panel2DMgr::createPanel() Factory for panel type %s not found to create panel %s !", type.c_str(), name.c_str());
        return 0;
    }
    Panel2DFactory *factory = factoryIt->second;
    panel = factory->createPanel2D(name);
    if (panel == 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Panel2DMgr::createPanel() Unable to create panel %s, type %s !", name.c_str(), type.c_str());
        return 0;
    }
    if (!panel->create(zOrder))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Panel2DMgr::createPanel() Unable to create panel %s, type %s !", name.c_str(), type.c_str());
        delete panel;
        return 0;
    }
    mPanelMap[name] = panel;

    // Move new panel on next creation position
    int vpWidth = OverlayManager::getSingleton().getViewportWidth();
    int vpHeight = OverlayManager::getSingleton().getViewportHeight();
    if (mNextCreationPosX > vpWidth - panel->getWidth())
        mNextCreationPosX = ms_DefaultCreationShift;
    if (mNextCreationPosY > vpHeight - panel->getHeight())
        mNextCreationPosY = ms_DefaultCreationShift;
    panel->move(mNextCreationPosX, mNextCreationPosY);
    mNextCreationPosX = ms_DefaultCreationShift + mNextCreationPosX%(vpWidth - ms_DefaultCreationShift);
    mNextCreationPosY = ms_DefaultCreationShift + mNextCreationPosY%(vpHeight - ms_DefaultCreationShift);

    return panel;
}

//-------------------------------------------------------------------------------------
void Panel2DMgr::destroyPanel(Panel2D* panel)
{
    if (panel == 0)
        return;

    destroyPanel(panel->mName);
}

//-------------------------------------------------------------------------------------
void Panel2DMgr::destroyPanel(const String& name)
{
    PanelMap::iterator panelIt = mPanelMap.find(name);
    if (panelIt == mPanelMap.end())
        return;
    Panel2D *panel = panelIt->second;
    if (panel == mFocusedPanel)
        mFocusedPanel = 0;
    panel->destroy();
    delete panel;
    mPanelMap.erase(panelIt);
}

//-------------------------------------------------------------------------------------
void Panel2DMgr::destroyAllPanels()
{
    for (PanelMap::iterator panelIt = mPanelMap.begin(); panelIt != mPanelMap.end(); panelIt = mPanelMap.begin())
        destroyPanel(panelIt->second);
}

//-------------------------------------------------------------------------------------
Panel2D* Panel2DMgr::getPanel(const String& name)
{
    PanelMap::iterator panelIt = mPanelMap.find(name);
    if (panelIt != mPanelMap.end())
        return panelIt->second;

	return 0;
}

//-------------------------------------------------------------------------------------
Panel2D* Panel2DMgr::focusPanel(int x, int y, Panel2D* focusedPanel)
{
    defocus();

    Panel2D *panelToFocus = focusedPanel ? focusedPanel : getPanelOverMouse(x, y);

    if (panelToFocus == 0)
        return 0;

    std::vector<Panel2D*> sortedPanels;
    for (PanelMap::iterator panelIt = mPanelMap.begin(); panelIt != mPanelMap.end(); ++panelIt)
        sortedPanels.push_back(panelIt->second);
    std::sort(sortedPanels.begin(), sortedPanels.end(), compare());
    if ((sortedPanels.size() != 0) &&
        (sortedPanels.at(0) != panelToFocus))
    {
        unsigned int popIdx = 0;
        for (; popIdx < sortedPanels.size(); popIdx++)
            if (sortedPanels.at(popIdx) == panelToFocus)
                break;

        ushort highestZ = sortedPanels.at(0)->mOverlay->getZOrder();
        for (unsigned int i = 0; i < popIdx; i++)
            sortedPanels.at(i)->mOverlay->setZOrder(sortedPanels.at(i+1)->mOverlay->getZOrder());

        sortedPanels.at(popIdx)->mOverlay->setZOrder(highestZ);
    }

    mFocusedPanel = panelToFocus;
    mFocusedPanel->onFocus(true);

    return mFocusedPanel;
}

//-------------------------------------------------------------------------------------
Panel2D* Panel2DMgr::getFocusedPanel()
{
    return mFocusedPanel;
}

//-------------------------------------------------------------------------------------
void Panel2DMgr::defocus()
{
    if (mFocusedPanel == 0)
        return;
    mFocusedPanel->onFocus(false);
    mFocusedPanel = 0;
}

//-------------------------------------------------------------------------------------
bool Panel2DMgr::keyPressed(const KeyboardEvt& evt)
{
    if (mFocusedPanel == 0)
        return false;

    return mFocusedPanel->keyPressed(evt);
}

//-------------------------------------------------------------------------------------
bool Panel2DMgr::keyReleased(const KeyboardEvt& evt)
{
    if (mFocusedPanel == 0)
        return false;

    return mFocusedPanel->keyReleased(evt);
}

//-------------------------------------------------------------------------------------
bool Panel2DMgr::mouseMoved(const MouseEvt& evt)
{
    if (mFocusedPanel == 0)
        return false;

    bool eventHandled = false;
    if (mMoving)
    {
        mFocusedPanel->moveRelative(evt.mState.mX - mMovingSizingMouseState.mX, evt.mState.mY - mMovingSizingMouseState.mY);
        mMovingSizingMouseState = evt.mState;
        eventHandled = true;
    }
    else if (mSizingPart != Panel2D::PNone)
    {
        int vpWidth = OverlayManager::getSingleton().getViewportWidth();
        int vpHeight = OverlayManager::getSingleton().getViewportHeight();
        int x, y, w, h;
        mFocusedPanel->getExtents(x, y, w, h);
        int newX = x;
        int newY = y;
        int newW = w;
        int newH = h;
        int minW = mFocusedPanel->mPanel->getMinWidth();
        int minH = mFocusedPanel->mPanel->getMinHeight();
        switch (mSizingPart)
        {
        case Panel2D::PTopLeftBorder:
            newY = evt.mState.mY - (mMovingSizingMouseState.mY - mSizingY);
            Maths::limit<int>(newY, 0, mSizingY + mSizingH - minH);
            newH = mSizingY - newY + mSizingH;
            newX = evt.mState.mX - (mMovingSizingMouseState.mX - mSizingX);
            Maths::limit<int>(newX, 0, mSizingX + mSizingW - minW);
            newW = mSizingX - newX + mSizingW;
            break;
        case Panel2D::PTopBorder:
            newY = evt.mState.mY - (mMovingSizingMouseState.mY - mSizingY);
            Maths::limit<int>(newY, 0, mSizingY + mSizingH - minH);
            newH = mSizingY - newY + mSizingH;
            break;
        case Panel2D::PTopRightBorder:
            newY = evt.mState.mY - (mMovingSizingMouseState.mY - mSizingY);
            Maths::limit<int>(newY, 0, mSizingY + mSizingH - minH);
            newH = mSizingY - newY + mSizingH;
            newW = evt.mState.mX + (mSizingX + mSizingW - mMovingSizingMouseState.mX) - newX;
            Maths::limit<int>(newW, minW, vpWidth - newX);
            break;
        case Panel2D::PRightBorder:
            newW = evt.mState.mX + (mSizingX + mSizingW - mMovingSizingMouseState.mX) - newX;
            Maths::limit<int>(newW, minW, vpWidth - newX);
            break;
        case Panel2D::PBottomRightBorder:
            newH = evt.mState.mY + (mSizingY + mSizingH - mMovingSizingMouseState.mY) - newY;
            Maths::limit<int>(newH, minH, vpHeight - newY);
            newW = evt.mState.mX + (mSizingX + mSizingW - mMovingSizingMouseState.mX) - newX;
            Maths::limit<int>(newW, minW, vpWidth - newX);
            break;
        case Panel2D::PBottomBorder:
            newH = evt.mState.mY + (mSizingY + mSizingH - mMovingSizingMouseState.mY) - newY;
            Maths::limit<int>(newH, minH, vpHeight - newY);
            break;
        case Panel2D::PBottomLeftBorder:
            newH = evt.mState.mY + (mSizingY + mSizingH - mMovingSizingMouseState.mY) - newY;
            Maths::limit<int>(newH, minH, vpHeight - newY);
            newX = evt.mState.mX - (mMovingSizingMouseState.mX - mSizingX);
            Maths::limit<int>(newX, 0, mSizingX + mSizingW - minW);
            newW = mSizingX - newX + mSizingW;
            break;
        case Panel2D::PLeftBorder:
            newX = evt.mState.mX - (mMovingSizingMouseState.mX - mSizingX);
            Maths::limit<int>(newX, 0, mSizingX + mSizingW - minW);
            newW = mSizingX - newX + mSizingW;
            break;
        };
        if ((newX != x) || (newY != y))
            mFocusedPanel->move(newX, newY);
        if ((newW != w) || (newH != h))
        {
            mFocusedPanel->resize(newW, newH);
            mFocusedPanel->updateButtons();
        }
        eventHandled = true;
    }
    else
    {
        mFocusedPanel->computePartOverMouse(evt.mState.mX, evt.mState.mY);
        mFocusedPanel->updateMouseCursor();
        eventHandled = mFocusedPanel->mouseMoved(evt);
    }

    return eventHandled;
}

//-------------------------------------------------------------------------------------
bool Panel2DMgr::mousePressed(const MouseEvt& evt)
{
    focusPanel(evt.mState.mX, evt.mState.mY);
    if (mFocusedPanel == 0)
        return false;

    bool eventHandled = mFocusedPanel->mousePressed(evt);

    if (evt.mState.mButtons & MBLeft)
    {
        switch (mFocusedPanel->mPartOverMouse)
        {
        case Panel2D::PButton:
            if (mFocusedPanel->mButtonOverMouse->getAction() == "close")
            {
                destroyPanel(mFocusedPanel);
                eventHandled = true;
            }
            break;
        case Panel2D::PTitleBorder:
            mMoving = true;
            mMovingSizingMouseState = evt.mState;
            eventHandled = true;
            break;
        case Panel2D::PClient:
            break;
        default:
            mSizingPart = mFocusedPanel->mPartOverMouse;
            mMovingSizingMouseState = evt.mState;
            mFocusedPanel->getExtents(mSizingX, mSizingY, mSizingW, mSizingH);
            eventHandled = true;
        }
    }

    return eventHandled;
}

//-------------------------------------------------------------------------------------
bool Panel2DMgr::mouseReleased(const MouseEvt& evt)
{
    if (mFocusedPanel == 0)
        return false;
    mFocusedPanel->computePartOverMouse(evt.mState.mX, evt.mState.mY);

    bool eventHandled = mFocusedPanel->mouseReleased(evt);

    if (mMoving)
    {
        mFocusedPanel->onMoved();
        mMoving = false;
    }
    if (mSizingPart != Panel2D::PNone)
    {
        mFocusedPanel->onResized();
        mSizingPart = Panel2D::PNone;
    }

    return eventHandled;
}

//-------------------------------------------------------------------------------------
Panel2D* Panel2DMgr::getPanelOverMouse(int x, int y)
{
    Panel2D *panel = 0;

    for (PanelMap::iterator panelIt = mPanelMap.begin(); panelIt != mPanelMap.end(); ++panelIt)
    {
        panelIt->second->computePartOverMouse(x, y);
        if (!panelIt->second->isMouseOverMe(x, y)) continue;
        if ((panel == 0) || (panel->mPanel->getZOrder() < panelIt->second->mPanel->getZOrder()))
            panel = panelIt->second;
    }

    return panel;
}

//-------------------------------------------------------------------------------------
void Panel2DMgr::recomputeZOrder()
{
    std::vector<Panel2D*> sortedPanels;
    for (PanelMap::iterator panelIt = mPanelMap.begin(); panelIt != mPanelMap.end(); ++panelIt)
        sortedPanels.push_back(panelIt->second);
    std::sort(sortedPanels.begin(), sortedPanels.end(), compare());
    unsigned int nbPanels = (unsigned int)sortedPanels.size();
    mZOrderCounter = Maths::min<unsigned int>(mMinZOrder, mMaxZOrder - nbPanels);
    for (unsigned int i = nbPanels; i > 0; i--, mZOrderCounter++)
        sortedPanels.at(i - 1)->mOverlay->setZOrder((mZOrderCounter < mMinZOrder) ? mMinZOrder : mZOrderCounter);
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
