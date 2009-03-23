/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Christophe Floutier

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

#ifndef __ProgressBar_h__
#define __ProgressBar_h__

#include "MovableText.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents an The sound icon shown above the avatar.
*/
class ProgressBar : public Ogre::MovableObject, public Ogre::Renderable
{
public:
	ProgressBar(const Ogre::String & name, Real width = 200, Real height = 20, bool applyParentScale = true);
	~ProgressBar();

	// change the material
	// default is 'Examples/ProgressBar'
	// UV coordinate must be 0 0 1 0.5 for the front bar
	// and 0.5 0 1 1 for the background
	void setMaterial(const Ogre::String & materialName)
	{
		mNeedUpdateMaterial = true;
		mMaterialName  = materialName;
		mNeedUpdateGeometry = true;
	}

	void setSize(Real width = 200, Real height = 20)
	{
		mwidth = width;
		mheight = height;
		mNeedUpdateGeometry = true;
	};


    void setAdditionalHeight(float addedHeight)
    {
        mAdditionalHeight = addedHeight;
    }

	void setProgress(Real value);
	Real getProgress() {return mProgress;}
	void showOnTop(bool show);

protected:
	// from MovableText, create the object
	void    _setupGeometry();
	void	_setupMaterial();

	// from MovableObject
	void    getWorldTransforms(Ogre::Matrix4 *xform) const;
	float   getBoundingRadius(void) const {return mRadius;};
	float   getSquaredViewDepth(const Ogre::Camera *cam) const {return 0;};
	const   Ogre::Quaternion        & getWorldOrientation(void) const;
	const   Ogre::Vector3           & getWorldPosition(void) const;
	const   Ogre::AxisAlignedBox    & getBoundingBox(void) const 
	{
		return mAABB;
	};

	const   Ogre::String            & getMovableType(void) const {static Ogre::String movTypeBar = "ProgressBar"; return movTypeBar;};

	void    _notifyCurrentCamera(Ogre::Camera *cam);
	void    _updateRenderQueue(Ogre::RenderQueue* queue);

#if (OGRE_VERSION_MAJOR >= 1 && OGRE_VERSION_MINOR >= 6)
	virtual void visitRenderables(Renderable::Visitor* visitor, bool debugRenderables);
#endif

	// from renderable
	void    getRenderOperation(Ogre::RenderOperation &op);
	const   Ogre::MaterialPtr       &getMaterial(void) const {assert(!mpMaterial.isNull());return mpMaterial;};
	const   Ogre::LightList         &getLights(void) const {return mLList;};

private:
	float                   mRadius;

	Ogre::RenderOperation   mRenderOp;
	Ogre::AxisAlignedBox    mAABB;
	Ogre::MaterialPtr		mpMaterial;
	Ogre::LightList			mLList;
	Ogre::Camera            *mpCam;

	bool                    mNeedUpdateGeometry;
	bool                    mNeedUpdateMaterial;
	bool                    mApplyParentScale;
    float                   mAdditionalHeight;

	Real mwidth, mheight;
	bool mOnTop;
	Real mProgress;
	Ogre::String mMaterialName;
};

class ProgressBarWithText
{
public:
	ProgressBarWithText(const Ogre::String & name, const Ogre::String & caption, bool applyParentScale = true);
	~ProgressBarWithText();


	void setBarMaterial(const Ogre::String & materialName) {m_Bar.setMaterial(materialName);}
	void setBarSize(Real width = 200, Real height = 20) 
    {
        m_Bar.setSize(width, height);
        m_Txt.setXpos(-width/2);

    }
	void setCaption(const Ogre::String & caption) 
	{
		m_Txt.setCaption(caption); 
		setProgress(m_Bar.getProgress());
	}

	void setFont(const Ogre::String & fontName, Real size, const ColourValue & color = ColourValue::White, unsigned int spacewidth = 2)
	{
		m_Txt.setFontName(fontName);
		m_Txt.setCharacterHeight(size);
		m_Txt.setColor(color);
		m_Txt.setSpaceWidth(spacewidth);
	}

    void setPosition(Real pos)
    {
        if (mpNodeAll)
        {
            mpNodeAll->setPosition(0,pos,0);
        }
        else
        {
            mBarPos = pos;
        }
    }

    void showRemainingTime(bool bShow)
    {
        mbShowTime = bShow;
    }

	void setTxtPosition(Real pos = 0)
	{
		m_Txt.setAdditionalHeight(pos);
	}

    void setTxtScale(float scale)
    {
        m_Txt.setScale(scale);
    }

	void setProgress(Real value);

	void attach(SceneNode *pNode);
	void detach();
    void resetTime()
    {
        beginTime = -1;
    }
	
private:
	Ogre::String mCaption; 

	ProgressBar m_Bar;
	MovableText m_Txt;
    SceneNode * mpNodeAll;
    Ogre::String mName;
    Real mBarPos;

    bool mbShowTime;
    Real beginTime;
    Real beginValue;
};




} // namespace Solipsis

#endif // #ifndef __ProgressBar_h__