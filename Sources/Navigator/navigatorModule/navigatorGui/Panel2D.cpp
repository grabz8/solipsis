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

#include "Panel2D.h"
#include "Panel2DMgr.h"
//#include <NaviMouse.h>

using namespace Ogre;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Panel2D::Panel2D(const String& type, const String& name) :
    mType(type),
    mState(SWindowed),
    mOldState(SWindowed),
    mName(name),
    mMatName(name),
    mOverlay(0),
    mPanel(0),
    mTitle(0),
    mPartOverMouse(PNone),
    mButtonOverMouse(0),
    mOldButtonOverMouse(0)
{
}

//-------------------------------------------------------------------------------------
Panel2D::~Panel2D()
{
}

//-------------------------------------------------------------------------------------
bool Panel2D::create(ushort zOrder)
{
    OverlayManager& overlayManager = OverlayManager::getSingleton();

    // Create panel from templated Panel2D
    mPanel = static_cast<Panel2DOverlayElement*>(overlayManager.cloneOverlayElementFromTemplate("Solipsis/Panel2DTemplate", mName));

    // Get title element
    try {
        mTitle = mPanel->getChild(mName + "/Solipsis/Panel2DTemplateTitle");
    }
    catch (Exception e) {}

    // Create map of buttons per action
    OverlayContainer::ChildIterator childIt = mPanel->getChildIterator();
    while (childIt.hasMoreElements())
    {
        OverlayElement *ovlElt = childIt.getNext();
        if (ovlElt->getTypeName() != "SolipsisPanel2DButton") continue;
        Panel2DButtonOverlayElement *panel2DButton = static_cast<Panel2DButtonOverlayElement*>(ovlElt);
        mButtons[panel2DButton->getAction()] = panel2DButton;
    }

    // Set Material
    MaterialPtr material = (MaterialPtr)MaterialManager::getSingleton().getByName(mMatName);
    if (!material.isNull())
    {
        mMaterial = material->clone(mName + "Mtl");
        mPanel->setMaterialName(mMaterial->getName());
    }

    // Create overlay
    mOverlay = overlayManager.create(mName + "Ovl");
    mOverlay->add2D(mPanel);
    mOverlay->setZOrder(zOrder);
    mOverlay->show();

    // Set size according to original client size
    getTextureSize(mOriginalWidth, mOriginalHeight);
    resize(SOriginal);

    return true;
}

//-------------------------------------------------------------------------------------
void Panel2D::destroy()
{
    if ((mPanel == 0) || (mOverlay == 0))
        return;

    if (!mMaterial.isNull())
    {
        MaterialManager::getSingletonPtr()->remove(mMaterial->getName());
        mMaterial.setNull();
    }

    OverlayManager& overlayManager = OverlayManager::getSingleton();
    mOverlay->remove2D(mPanel);
    while (true)
    {
        OverlayContainer::ChildIterator childOvlEltIterator = mPanel->getChildIterator();
        if (!childOvlEltIterator.hasMoreElements()) break;
        OverlayElement* ovlElt = childOvlEltIterator.getNext();
        overlayManager.destroyOverlayElement(ovlElt);
    }
    overlayManager.destroyOverlayElement(mPanel);
    overlayManager.destroy(mOverlay);
}

//-------------------------------------------------------------------------------------
bool Panel2D::isFocused()
{
    return (Panel2DMgr::getSingleton().getFocusedPanel() == this);
}

//-------------------------------------------------------------------------------------
void Panel2D::getExtents(int& x, int& y, int& width, int& height)
{
    x = (int)mPanel->getLeft();
    y = (int)mPanel->getTop();
    width = (int)mPanel->getWidth();
    height = (int)mPanel->getHeight();
}

//-------------------------------------------------------------------------------------
int Panel2D::getX()
{
    return (int)mPanel->getLeft();
}

//-------------------------------------------------------------------------------------
int Panel2D::getY()
{
    return (int)mPanel->getTop();
}

//-------------------------------------------------------------------------------------
int Panel2D::getWidth()
{
    return (int)mPanel->getWidth();
}

//-------------------------------------------------------------------------------------
int Panel2D::getHeight()
{
    return (int)mPanel->getHeight();
}


//-------------------------------------------------------------------------------------
void Panel2D::getClientExtents(int& clientWidth, int& clientHeight)
{
    clientWidth = (int)mPanel->getWidth() - (int)mPanel->getLeftBorderSize() - (int)mPanel->getRightBorderSize();
    clientHeight = (int)mPanel->getHeight() - (int)mPanel->getTopBorderSize() - (int)mPanel->getBottomBorderSize();
}

//-------------------------------------------------------------------------------------
void Panel2D::move(int x, int y)
{
    mPanel->setPosition(x, y);
}

//-------------------------------------------------------------------------------------
void Panel2D::moveRelative(int deltaX, int deltaY)
{
    move((int)mPanel->getLeft() + deltaX, (int)mPanel->getTop() + deltaY);
}

//-------------------------------------------------------------------------------------
void Panel2D::resize(int width, int height)
{
    mState = SWindowed;
    mPanel->setDimensions(width, height);
}

//-------------------------------------------------------------------------------------
void Panel2D::resize(State state)
{
    switch (state)
    {
    case SMaximized:
        if (mState == SMaximized)
        {
            state = mSavedState;
            move(mSavedX, mSavedY);
            resize(mSavedW, mSavedH);
            onResized();
        }
        else
        {
            mSavedState = mState;
            getExtents(mSavedX, mSavedY, mSavedW, mSavedH);
            int vpWidth = OverlayManager::getSingleton().getViewportWidth();
            int vpHeight = OverlayManager::getSingleton().getViewportHeight();
            move(0, 0);
            resize(vpWidth, vpHeight);
            onResized();
        }
        break;
    case SMinimized:
        mSavedState = mState;
        getExtents(mSavedX, mSavedY, mSavedW, mSavedH);
        resize((int)mPanel->getMinWidth(), (int)mPanel->getMinHeight());
        onResized();
        break;
    case SOriginal:
        if (mState != SOriginal)
        {
            resize(mOriginalWidth + (int)mPanel->getLeftBorderSize() + (int)mPanel->getRightBorderSize(), mOriginalHeight + (int)mPanel->getTopBorderSize() + (int)mPanel->getBottomBorderSize());
            onResized();
        }
        break;
    default: // SWindowed
        break;
    }

    mState = state;
    updateButtons();
}

//-------------------------------------------------------------------------------------
bool Panel2D::mouseMoved(const MouseEvt& evt)
{
    updateButtons();
    return false;
}

//-------------------------------------------------------------------------------------
bool Panel2D::mousePressed(const MouseEvt& evt)
{
    if (mPartOverMouse == PButton)
    {
        if (mButtonOverMouse->getAction() == "maximize")
        {
            resize(SMaximized);
            return true;
        }
        else if (mButtonOverMouse->getAction() == "minimize")
        {
            resize(SMinimized);
            return true;
        }
        else if (mButtonOverMouse->getAction() == "original")
        {
            resize(SOriginal);
            return true;
        }
        else if (mButtonOverMouse->getAction() == "ratio11")
        {
            mButtonOverMouse->setEventState(!mButtonOverMouse->getEventState());
            updateButtons();
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------
bool Panel2D::mouseReleased(const MouseEvt& evt)
{
    return false;
}

//-------------------------------------------------------------------------------------
Panel2D::Part Panel2D::getPartOverPoint(int x, int y, Panel2DButtonOverlayElement*& button)
{
    int vpWidth = OverlayManager::getSingleton().getViewportWidth();
    int vpHeight = OverlayManager::getSingleton().getViewportHeight();
    int l = (int)(mPanel->_getDerivedLeft()*vpWidth);
    int t = (int)(mPanel->_getDerivedTop()*vpHeight);
    int w = (int)mPanel->getWidth();
    int h = (int)mPanel->getHeight();
    int titleb = (int)mPanel->getTitleBorderSize();
    int tb = (int)mPanel->getTopBorderSize();
    int bb = (int)mPanel->getBottomBorderSize();
    int lb = (int)mPanel->getLeftBorderSize();
    int rb = (int)mPanel->getRightBorderSize();
    button = 0;
    // Out of panel ?
    if ((y < t) || (t + h <= y))
        return PNone;
    if ((x < l) || (l + w <= x))
        return PNone;
    // Button ?
    for (ButtonsMap::const_iterator btnIt = mButtons.begin(); btnIt != mButtons.end(); ++btnIt)
    {
        Panel2DButtonOverlayElement *btn = btnIt->second;
        if (btn->getCurrentAction().empty())
            continue;
        int bl = (int)(btn->_getDerivedLeft()*vpWidth);
        int bt = (int)(btn->_getDerivedTop()*vpHeight);
        if ((bt <= y) && (y < bt + (int)btn->getHeight()) &&
            (bl <= x) && (x < bl + (int)btn->getWidth()))
        {
            button = btn;
            return PButton;
        }
    }
    // Top parts ?
    if (y < t + tb - titleb)
    {
        if (x < l + lb)
            return PTopLeftBorder;
        if (x < l + w - rb)
            return PTopBorder;
        return PTopRightBorder;
    }
    // Title parts ?
    if (y < t + tb)
    {
        if (x < l + lb)
            return PLeftBorder;
        if (x < l + w - rb)
            return PTitleBorder;
        return PRightBorder;
    }
    // Client parts ?
    if (y < t + h - bb)
    {
        if (x < l + lb)
            return PLeftBorder;
        if (x < l + w - rb)
            return PClient;
        return PRightBorder;
    }
    // Bottom parts ?
    if (y < t + h)
    {
        if (x < l + lb)
            return PBottomLeftBorder;
        if (x < l + w - rb)
            return PBottomBorder;
        return PBottomRightBorder;
    }
    return PNone;
}

//-------------------------------------------------------------------------------------
void Panel2D::computePartOverMouse(int x, int y)
{
    mPartOverMouse = getPartOverPoint(x, y, mButtonOverMouse);
}

//-------------------------------------------------------------------------------------
bool Panel2D::isMouseOverMe(int x, int y)
{
    return (mPartOverMouse != PNone);
}

//-------------------------------------------------------------------------------------
void Panel2D::getClientRelativePoint(int x, int y, int& clientX, int& clientY)
{
    int clientWidth, clientHeight;
    getClientExtents(clientWidth, clientHeight);

    clientX = x - (int)mPanel->getLeft() - (int)mPanel->getLeftBorderSize();
    clientX = (clientX < 0) ? 0 : (clientX > clientWidth - 1 ? clientWidth - 1 : clientX);
    clientY = y - (int)mPanel->getTop() - (int)mPanel->getTopBorderSize();
    clientY = (clientY < 0) ? 0 : (clientY > clientHeight - 1 ? clientHeight - 1 : clientY);
}

//-------------------------------------------------------------------------------------
void Panel2D::getTextureSize(int& textureWidth, int& textureHeight)
{
    textureWidth = 100;
    textureHeight = 100;
    MaterialPtr material = MaterialManager::getSingleton().getByName(mMatName);
    if (material.isNull())
        return;
    Technique *technique0 = material->getTechnique(0);
    if (technique0 == 0)
        return;
    Pass *pass0 = technique0->getPass(0);
    if (pass0 == 0)
        return;
    TextureUnitState *textureUnitState0 = pass0->getTextureUnitState(0);
    if (textureUnitState0 == 0)
        return;
    std::pair<size_t, size_t> textWH = textureUnitState0->getTextureDimensions(0);
    textureWidth = (int)textWH.first;
    textureHeight = (int)textWH.second;
}

//-------------------------------------------------------------------------------------
void Panel2D::getTextureRelativePoint(int x, int y, int& textureX, int& textureY)
{
    int clientWidth, clientHeight;
    getClientExtents(clientWidth, clientHeight);
    int clientX, clientY;
    getClientRelativePoint(x, y, clientX, clientY);
    int textureWidth, textureHeight;
    getTextureSize(textureWidth, textureHeight);
    textureX = (clientX*textureWidth)/clientWidth;
    textureY = (clientY*textureHeight)/clientHeight;
}

//-------------------------------------------------------------------------------------
void Panel2D::getTextureUVRelativePoint(int x, int y, Real& textureU, Real& textureV)
{
    int clientWidth, clientHeight;
    getClientExtents(clientWidth, clientHeight);
    int clientX, clientY;
    getClientRelativePoint(x, y, clientX, clientY);
    int textureWidth, textureHeight;
    getTextureSize(textureWidth, textureHeight);
    textureU = (Real)clientX/(Real)clientWidth;
    textureV = (Real)clientY/(Real)clientHeight;
}

//-------------------------------------------------------------------------------------
void Panel2D::updateButtons()
{
    ButtonsMap::const_iterator btnIt;

    // maximize and original event buttons
    if (mState != mOldState)
    {
        btnIt = mButtons.find("maximize");
        if (btnIt != mButtons.end())
            btnIt->second->setEventState(mState == SMaximized);
        btnIt = mButtons.find("original");
        if (btnIt != mButtons.end())
            btnIt->second->setEventState(mState == SOriginal);
        mOldState = mState;
    }

    // buttons hover
    if (mButtonOverMouse != mOldButtonOverMouse)
    {
        if (mOldButtonOverMouse != 0)
            mOldButtonOverMouse->setHoverState(false);
        if (mButtonOverMouse != 0)
            mButtonOverMouse->setHoverState(true);
        mOldButtonOverMouse = mButtonOverMouse;
    }
}

//-------------------------------------------------------------------------------------
void Panel2D::updateMouseCursor()
{
/*    NaviLibrary::NaviMouse *naviMouse = NaviLibrary::NaviMouse::GetPointer();
    if (naviMouse == 0)
        return;

    switch (mPartOverMouse)
    {
    case PLeftBorder:
    case PRightBorder:
        naviMouse->activateCursor("resizeH");
        break;
    case PTopBorder:
    case PBottomBorder:
        naviMouse->activateCursor("resizeV");
        break;
    case PTopRightBorder:
    case PBottomLeftBorder:
        naviMouse->activateCursor("resize45");
        break;
    case PTopLeftBorder:
    case PBottomRightBorder:
        naviMouse->activateCursor("resize315");
        break;
    default:
        naviMouse->activateCursor("default");
        break;
    }*/
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
