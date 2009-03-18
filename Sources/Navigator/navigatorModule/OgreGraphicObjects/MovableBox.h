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

#ifndef __MovableBox_h__
#define __MovableBox_h__

using namespace Ogre;

namespace Solipsis 
{

/** This class represents an The sound icon shown above the avatar.
*/
class MovableBox : public Ogre::MovableObject, public Ogre::Renderable
{
public:
	MovableBox(const Ogre::String & name, const Vector3 & size, bool applyParentScale);
	~MovableBox();

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

	void setSize(const Vector3 & size)
	{
		mSize = size;
		mNeedUpdateGeometry = true;
	};

    void setRotateSpeed(Real rotateSpeed)
    {
        mRotateSpeed = rotateSpeed;
    }

    void animate(Real timeSinceLastFrame);
    
    void attach(SceneManager* pMgr, SceneNode *pNode);
    void detach();

    void showInnerfaces(bool show)
    {
        mInnerFaces =  show;
        mNeedUpdateGeometry = true;
    }

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

	const   Ogre::String& getMovableType(void) const {static Ogre::String movTypeBox = "Box"; return movTypeBox;};

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

	bool mNeedUpdateGeometry;
	bool mNeedUpdateMaterial;
	bool mApplyParentScale;

    bool mInnerFaces;

	Vector3 mSize;
	Real mRotateSpeed;
	Ogre::String mMaterialName;

    Real mCurTime;
};

} // namespace Solipsis

#endif // #ifndef __MovableBox_h__