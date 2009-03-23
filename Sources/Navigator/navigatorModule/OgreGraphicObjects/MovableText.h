/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author cTh see gavocanov@rambler.ru

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

/**
 * File: MovableText.h
 *
 * description: This class creates a billboarding object that display a text.
 * 
 * @author  2003 by cTh see gavocanov@rambler.ru
 * @update  2006 by barraq see nospam@barraquand.com
 * @update  2007 by independentCreations see independentCreations@gmail.com
 */

#ifndef __MovableText_H__
#define __MovableText_H__

#include <OgrePrerequisites.h>
#include <Ogre.h>

class MovableText : public Ogre::MovableObject, public Ogre::Renderable
{
public:
    enum HorizontalAlignment    {H_LEFT, H_CENTER};
    enum VerticalAlignment      {V_BELOW, V_ABOVE};

protected:
    Ogre::String            mFontName;
    Ogre::String            mName;
    Ogre::UTFString         mCaption;
    HorizontalAlignment     mHorizontalAlignment;
    VerticalAlignment       mVerticalAlignment;

    Ogre::ColourValue       mColor;
    Ogre::RenderOperation   mRenderOp;
    Ogre::AxisAlignedBox    mAABB;
    Ogre::LightList         mLList;

    unsigned int            mCharHeight;
    unsigned int            mSpaceWidth;

    bool                    mNeedUpdate;
    bool                    mUpdateColors;
    bool                    mOnTop;

    float                   mRadius;
    float                   mAdditionalHeight;
    float                   mXPos;

    Ogre::Camera            *mpCam;
    Ogre::Font              *mpFont;
    Ogre::MaterialPtr       mpMaterial;

    bool                    mApplyParentScale;
    float                   mScale;

    /******************************** public methods ******************************/
public:
    MovableText(const Ogre::String & name, const Ogre::UTFString & caption,
        bool applyParentScale = true,
        const Ogre::String & fontName = "BlueHighway", int charHeight = 32,
        const Ogre::ColourValue & color = Ogre::ColourValue::White);
    virtual ~MovableText();

    // Set settings
    void    setFontName(const Ogre::String & fontName);
    void    setCaption(const Ogre::UTFString & caption);
    void    setColor(const Ogre::ColourValue & color);
    void    setCharacterHeight(unsigned int height);
    void    setSpaceWidth(unsigned int width);
    void    setTextAlignment(const HorizontalAlignment & horizontalAlignment, const VerticalAlignment & verticalAlignment);
    void    setAdditionalHeight( float height );
    void    setXpos( float xpos);
    void    showOnTop(bool show = true);
    void    setScale(float scale);

    // Get settings
    const   Ogre::String        & getFontName() const {return mFontName;}
    const   Ogre::UTFString     & getCaption() const {return mCaption;}
    const   Ogre::ColourValue   & getColor() const {return mColor;}
    
    unsigned int                getCharacterHeight() const {return mCharHeight;}
    unsigned int                getSpaceWidth() const {return mSpaceWidth;}
    float                       getAdditionalHeight() const {return mAdditionalHeight;}
    bool                        getShowOnTop() const {return mOnTop;}
    Ogre::AxisAlignedBox        GetAABB(void) { return mAABB; }

    /******************************** protected methods and overload **************/
protected:

    float    mViewportAspectCoef;

    // from MovableText, create the object
    void    _setupGeometry();
    void    _updateColors();

    // from MovableObject
    void    getWorldTransforms(Ogre::Matrix4 *xform) const;
    float   getBoundingRadius(void) const {return mRadius;};
    float   getSquaredViewDepth(const Ogre::Camera *cam) const {return 0;};
    const   Ogre::Quaternion        & getWorldOrientation(void) const;
    const   Ogre::Vector3           & getWorldPosition(void) const;
    const   Ogre::AxisAlignedBox    & getBoundingBox(void) const {return mAABB;};
    const   Ogre::String            & getName(void) const {return mName;};
    const   Ogre::String            & getMovableType(void) const {static Ogre::String movType = "MovableText"; return movType;};

    void    _notifyCurrentCamera(Ogre::Camera *cam);
    void    _updateRenderQueue(Ogre::RenderQueue* queue);

#if (OGRE_VERSION_MAJOR >= 1 && OGRE_VERSION_MINOR >= 6)
	virtual void visitRenderables(Renderable::Visitor* visitor, bool debugRenderables);
#endif

    // from renderable
    void    getRenderOperation(Ogre::RenderOperation &op);
    const   Ogre::MaterialPtr       &getMaterial(void) const {assert(!mpMaterial.isNull());return mpMaterial;};
    const   Ogre::LightList         &getLights(void) const {return mLList;};
};

#endif /* __MovableText_H__ */