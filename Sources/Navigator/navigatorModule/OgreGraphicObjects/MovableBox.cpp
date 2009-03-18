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

#include "Prerequisites.h"
#include "MovableBox.h"

#define POS_TEX_BINDING    0
#define COLOUR_BINDING     1
#if OGRE_VERSION_MAJOR >= 1
#if OGRE_VERSION_MINOR >= 4
#define OGRE_VERS_GREATER_EIHORT
#endif
#endif

using namespace Solipsis;

////////////////////// the Simple Progress Bar //////////////////////////////////////

MovableBox::MovableBox(const Ogre::String & name, const Vector3 & size, bool applyParentScale = true) 
{
	mRenderOp.vertexData = NULL;
	mApplyParentScale = applyParentScale;
	mSize = size;

	mMaterialName = "Solipsis/Box";

	mNeedUpdateMaterial = true;
	mNeedUpdateGeometry = true;

	_setupGeometry();


    mCurTime = 0;
}

MovableBox::~MovableBox()
{
	if (mRenderOp.vertexData)
		delete mRenderOp.vertexData;
}


void MovableBox::_setupMaterial()
{
	if (!mpMaterial.isNull())
	{
		mpMaterial->unload();
	}
	
	mpMaterial = MaterialManager::getSingleton().load(mMaterialName, "Examples");
	mNeedUpdateMaterial = false;
}

void MovableBox::_setupGeometry()
{
	if (mNeedUpdateMaterial)
		_setupMaterial();

	assert(!mpMaterial.isNull());

	unsigned int vertexCount = 2*6*3;

	if (mRenderOp.vertexData)
	{
		if (mRenderOp.vertexData->vertexCount != vertexCount)
		{
			delete mRenderOp.vertexData;
			mRenderOp.vertexData = NULL;
		}
	}

	if (!mRenderOp.vertexData)
		mRenderOp.vertexData = new Ogre::VertexData();

	mRenderOp.indexData = 0;
	mRenderOp.vertexData->vertexStart = 0;
	mRenderOp.vertexData->vertexCount = vertexCount;
	mRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST; 
	mRenderOp.useIndexes = false; 

	Ogre::VertexDeclaration * decl = mRenderOp.vertexData->vertexDeclaration;
	Ogre::VertexBufferBinding * bind = mRenderOp.vertexData->vertexBufferBinding;
	size_t offset = 0;

	// create/bind positions/tex.ccord. buffer
	if (!decl->findElementBySemantic(Ogre::VES_POSITION))
		decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);

	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	if (!decl->findElementBySemantic(Ogre::VES_TEXTURE_COORDINATES))
		decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);

	Ogre::HardwareVertexBufferSharedPtr ptbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(POS_TEX_BINDING),
		mRenderOp.vertexData->vertexCount,
		Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	bind->setBinding(POS_TEX_BINDING, ptbuf);

	// Colours - store these in a separate buffer because they change less often
	if (!decl->findElementBySemantic(Ogre::VES_DIFFUSE))
		decl->addElement(COLOUR_BINDING, 0, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);

	Ogre::HardwareVertexBufferSharedPtr cbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(COLOUR_BINDING),
		mRenderOp.vertexData->vertexCount,
		Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	bind->setBinding(COLOUR_BINDING, cbuf);

	float *pVert = static_cast<float*>(ptbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

    float left = -mSize.x/2;
    float right = mSize.x/2;

    float top = mSize.y/2;
    float bottom = -mSize.y/2;

    float front = -mSize.z/2;
    float back = mSize.z/2;

    float beginTexture = 0;
    float endTexture = 1;

    // each vert is (x, y, z, u, v)

    // BACK
    *pVert++ = left;    *pVert++ = top;    *pVert++ = back;
    *pVert++ = 0;    *pVert++ = beginTexture;
    *pVert++ = left;    *pVert++ = bottom;    *pVert++ = back;
    *pVert++ = 0;    *pVert++ = endTexture;
    *pVert++ = right;    *pVert++ = top;    *pVert++ = back;
    *pVert++ = 1;    *pVert++ = beginTexture;
    *pVert++ = right;    *pVert++ = top;    *pVert++ = back;
    *pVert++ = 1;    *pVert++ = beginTexture;
    *pVert++ = left;    *pVert++ = bottom;    *pVert++ = back;   
    *pVert++ = 0;    *pVert++ = endTexture;
    *pVert++ = right;    *pVert++ = bottom;    *pVert++ = back;
    *pVert++ = 1;    *pVert++ = endTexture;

    // FRONT
    *pVert++ = left;    *pVert++ = top;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = beginTexture;
    *pVert++ = right;    *pVert++ = top;    *pVert++ = front;
    *pVert++ = 1;    *pVert++ = beginTexture;
    *pVert++ = left;    *pVert++ = bottom;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = endTexture;
    *pVert++ = right;    *pVert++ = top;    *pVert++ = front;
    *pVert++ = 1;    *pVert++ = beginTexture;
    *pVert++ = right;    *pVert++ = bottom;    *pVert++ = front;
    *pVert++ = 1;    *pVert++ = endTexture;
    *pVert++ = left;    *pVert++ = bottom;    *pVert++ = front;   
    *pVert++ = 0;    *pVert++ = endTexture;

    //RIGHT
    *pVert++ = right;    *pVert++ = top;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = beginTexture;
    *pVert++ = right;    *pVert++ = top;    *pVert++ = back;
    *pVert++ = 1;    *pVert++ = beginTexture;
    *pVert++ = right;    *pVert++ = bottom;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = endTexture;
    *pVert++ = right;    *pVert++ = top;    *pVert++ = back;
    *pVert++ = 1;    *pVert++ = beginTexture;
    *pVert++ = right;    *pVert++ = bottom;    *pVert++ = back;
    *pVert++ = 1;    *pVert++ = endTexture;
    *pVert++ = right;    *pVert++ = bottom;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = endTexture;

    //LEFT
    *pVert++ = left;    *pVert++ = top;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = beginTexture;
    *pVert++ = left;    *pVert++ = bottom;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = endTexture;
    *pVert++ = left;    *pVert++ = top;    *pVert++ = back;
    *pVert++ = 1;    *pVert++ = beginTexture;
    *pVert++ = left;    *pVert++ = top;    *pVert++ = back;
    *pVert++ = 1;    *pVert++ = beginTexture;
    *pVert++ = left;    *pVert++ = bottom;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = endTexture;
    *pVert++ = left;    *pVert++ = bottom;    *pVert++ = back;
    *pVert++ = 1;    *pVert++ = endTexture;

    // TOP
    *pVert++ = left;    *pVert++ = top;    *pVert++ = back;
    *pVert++ = 0;    *pVert++ = beginTexture;
    *pVert++ = right;    *pVert++ = top;    *pVert++ = back;
    *pVert++ = 1;    *pVert++ = beginTexture;
    *pVert++ = left;    *pVert++ = top;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = endTexture;
    *pVert++ = right;    *pVert++ = top;    *pVert++ = back;
    *pVert++ = 1;    *pVert++ = beginTexture;
    *pVert++ = right;    *pVert++ = top;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = endTexture;
    *pVert++ = left;    *pVert++ = top;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = endTexture;

    // Bottom
    *pVert++ = left;    *pVert++ = bottom;    *pVert++ = back;
    *pVert++ = 0;    *pVert++ = beginTexture;
    *pVert++ = left;    *pVert++ = bottom;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = endTexture;
    *pVert++ = right;    *pVert++ = bottom;    *pVert++ = back;
    *pVert++ = 1;    *pVert++ = beginTexture;
    *pVert++ = right;    *pVert++ = bottom;    *pVert++ = back;
    *pVert++ = 1;    *pVert++ = beginTexture;
    *pVert++ = left;    *pVert++ = bottom;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = endTexture;
    *pVert++ = right;    *pVert++ = bottom;    *pVert++ = front;
    *pVert++ = 0;    *pVert++ = endTexture;

	// Unlock vertex buffer
	ptbuf->unlock();

	mAABB = Ogre::AxisAlignedBox();
	mAABB.setMaximum(mSize.x/2, mSize.y/2, mSize.z/2);
	mAABB.setMinimum(-mSize.x/2, -mSize.y/2, -mSize.z/2);
    mAABB.setInfinite();
	mRadius = mSize.squaredLength();

	mNeedUpdateGeometry = false;
}

// 
// const Ogre::Quaternion & MovableBox::getWorldOrientation(void) const
// {
//     return Renderable::getWorldOrientation();
// }
// 
// const Ogre::Vector3 & MovableBox::getWorldPosition(void) const
// {
// 	return Renderable::getWorldPosition();
// }

void MovableBox::getWorldTransforms(Ogre::Matrix4 * xform) const 
{
	if (this->isVisible())
	{
        Ogre::Matrix3 rot3x3, scale3x3 = Ogre::Matrix3::IDENTITY;



		// parent node position
		Ogre::Vector3 ppos = mParentNode->_getDerivedPosition();

		// apply scale
		if (mApplyParentScale)
		{
			scale3x3[0][0] = mParentNode->_getDerivedScale().x / 2;
			scale3x3[1][1] = mParentNode->_getDerivedScale().y / 2;
			scale3x3[2][2] = mParentNode->_getDerivedScale().z / 2;
		}
		else
		{
			scale3x3[0][0] = 1;
			scale3x3[1][1] = 1;
			scale3x3[2][2] = 1;
		}

		// apply all transforms to xform       
        *xform = (rot3x3 * scale3x3);
        if (mRotateSpeed)
        {
            rot3x3.FromAxisAngle(Vector3(0,1,0), Radian(mRotateSpeed*mCurTime));
            *xform = rot3x3 * scale3x3;
        }
        else
        {
            *xform = scale3x3;
        }
        
        
        xform->setTrans(ppos);




	}
}

void MovableBox::getRenderOperation(Ogre::RenderOperation & op)
{
	if (isVisible())
	{
		if (mNeedUpdateGeometry)
			_setupGeometry();

		op = mRenderOp;
	}
}

void MovableBox::_notifyCurrentCamera(Ogre::Camera *cam)
{
	
}

void MovableBox::_updateRenderQueue(Ogre::RenderQueue* queue)
{
	if (isVisible())
	{
		if (mNeedUpdateGeometry)
			_setupGeometry();

		queue->addRenderable(this, mRenderQueueID, OGRE_RENDERABLE_DEFAULT_PRIORITY);
		//      queue->addRenderable(this, mRenderQueueID, RENDER_QUEUE_SKIES_LATE);
	}
}

#if (OGRE_VERSION_MAJOR >= 1 && OGRE_VERSION_MINOR >= 6)
void MovableBox::visitRenderables(Renderable::Visitor* visitor, bool debugRenderables)
{
	visitor->visit(this, 0, false);
}
#endif



void MovableBox::animate(Real timeSinceLastFrame)
{
   mCurTime +=  timeSinceLastFrame;


}
