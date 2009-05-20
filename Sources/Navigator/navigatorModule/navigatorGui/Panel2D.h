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

#ifndef __Panel2D_h__
#define __Panel2D_h__

#include <map>
#include <Ogre.h>
#include "Tools/Event.h"
#include "OgreGraphicObjects/Panel2DOverlayElement.h"
#include "OgreGraphicObjects/Panel2DButtonOverlayElement.h"

namespace Solipsis {

/** This base class manages 2D panel.
*/
class Panel2D : public KeyboardEventListener, public MouseEventListener
{
    friend class Panel2DMgr;

public:
    /// Visual state of the panel
    enum State
    {
        SWindowed = 0,
        SMaximized,
        SMinimized,
        SOriginal,
    };
    /// Differents parts of the panel
    enum Part
    {
        PNone = 0,
        PClient,
        PTitleBorder,
        PButton,
        PTopLeftBorder,
        PTopBorder,
        PTopRightBorder,
        PLeftBorder,
        PRightBorder,
        PBottomLeftBorder,
        PBottomBorder,
        PBottomRightBorder
    };

protected:
    /// Type of panel
    Ogre::String mType;
    /// Current state
    State mState;
    /// Previous state
    State mOldState;
    /// Name
    Ogre::String mName;
    /// Material name
    Ogre::String mMatName;
    /// Material resource
    Ogre::MaterialPtr mMaterial;
    /// Overlay
    Ogre::Overlay *mOverlay;
    /// Panel element
    Panel2DOverlayElement *mPanel;
    /// Title element
    OverlayElement *mTitle;
    /// Map of buttons elements
    typedef std::map<String, Panel2DButtonOverlayElement*> ButtonsMap;
    /// Buttons elements
    ButtonsMap mButtons;
    /// Which part is currently over the mouse
    Part mPartOverMouse;
    /// Which button element is currently over the mouse
    Panel2DButtonOverlayElement *mButtonOverMouse;
    /// Which button element was previously over the mouse
    Panel2DButtonOverlayElement *mOldButtonOverMouse;
    /// Original width
    int mOriginalWidth;
    /// Original height
    int mOriginalHeight;
    /// Saved state before maximizing panel
    State mSavedState;
    /// Saved X pos before maximizing panel
    int mSavedX;
    /// Saved Y pos before maximizing panel
    int mSavedY;
    /// Saved width before maximizing panel
    int mSavedW;
    /// Saved height before maximizing panel
    int mSavedH;

public:
    /// Constructor
    Panel2D(const Ogre::String& type, const Ogre::String& name);
    /// Destructor
    ~Panel2D();

    virtual bool create(Ogre::ushort zOrder);
    void destroy();

    bool isFocused();
    void getExtents(int& x, int& y, int& width, int& height);
    int getX();
    int getY();
    int getWidth();
    int getHeight();
    void getClientExtents(int& clientWidth, int& clientHeight);

    virtual void move(int x, int y);
    void moveRelative(int deltaX, int deltaY);
    virtual void resize(int width, int height);
    virtual void resize(State state);

    bool operator>(const Panel2D& panel)  const { return mOverlay->getZOrder() > panel.mOverlay->getZOrder(); };

protected:
    /** See MouseEventListener. */
    virtual bool mouseMoved(const MouseEvt& evt);
    /** See MouseEventListener. */
    virtual bool mousePressed(const MouseEvt& evt);
    /** See MouseEventListener. */
    virtual bool mouseReleased(const MouseEvt& evt);

protected:
    Part getPartOverPoint(int x, int y, Panel2DButtonOverlayElement*& button);
    void computePartOverMouse(int x, int y);
    bool isMouseOverMe(int x, int y);
    void getClientRelativePoint(int x, int y, int& clientX, int& clientY);
    virtual void getTextureSize(int& textureWidth, int& textureHeight);
    void getTextureRelativePoint(int x, int y, int& textureX, int& textureY);
    void getTextureUVRelativePoint(int x, int y, Real& textureU, Real& textureV);
    void updateButtons();
    void updateMouseCursor();

    virtual void onFocus(bool isFocused) {}
    virtual void onMoved() {}
    virtual void onResized() {}
};

/** This base factory class to create 2D panel.
*/
class Panel2DFactory
{
public:
    virtual Panel2D* createPanel2D(const Ogre::String& name) = 0;
    virtual const Ogre::String& getType() const = 0;
};

} // namespace Solipsis

#endif // #ifndef __Panel2D_h__
