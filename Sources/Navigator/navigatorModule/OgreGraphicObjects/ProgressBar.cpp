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
#include "ProgressBar.h"

#define POS_TEX_BINDING    0
#define COLOUR_BINDING     1
#if OGRE_VERSION_MAJOR >= 1
#if OGRE_VERSION_MINOR >= 4
#define OGRE_VERS_GREATER_EIHORT
#endif
#endif

using namespace Solipsis;

////////////////////// the Simple Progress Bar //////////////////////////////////////

ProgressBar::ProgressBar(const Ogre::String & name, Real width, Real height, bool applyParentScale) 
{
	mRenderOp.vertexData = NULL;
	mApplyParentScale = applyParentScale;
	mwidth = width;
	mheight = height;
	mOnTop = false;
	mProgress = 0;
    mAdditionalHeight = 0;

	mMaterialName = "Solipsis/ProgressBar";

	mNeedUpdateMaterial = true;
	mNeedUpdateGeometry = true;

	mwidth = width;

	_setupGeometry();
}
void ProgressBar::setProgress(Real value)
{
	if (value < 0)
		value = 0;
	else if (value > 1)
		value = 1;

	mProgress = value;
	mNeedUpdateGeometry = true;
}

ProgressBar::~ProgressBar()
{
	if (mRenderOp.vertexData)
		delete mRenderOp.vertexData;
}

void ProgressBar::_setupMaterial()
{
	if (!mpMaterial.isNull())
	{
		mpMaterial->unload();
	}
	
	mpMaterial = MaterialManager::getSingleton().load(mMaterialName, "Examples");
	mNeedUpdateMaterial = false;
}

void ProgressBar::_setupGeometry()
{
	if (mNeedUpdateMaterial)
		_setupMaterial();

	assert(!mpMaterial.isNull());

	unsigned int vertexCount = 12;

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

	float left = -mwidth/2;
	float right = mwidth/2;

	float top = mheight/2 + mAdditionalHeight;
	float bottom = -mheight/2 + mAdditionalHeight;
	float beginTexture = 0.55;
	float endTexture = 1;

	// each vert is (x, y, z, u, v)
	//-------------------------------------------------------------------------------------
	// First tri
	//
	// Upper left
	*pVert++ = left;
	*pVert++ = top;
	*pVert++ = 0;
	*pVert++ = 0;
	*pVert++ = beginTexture;

	// Bottom left
	*pVert++ = left;
	*pVert++ = bottom;
	*pVert++ = 0;
	*pVert++ = 0;
	*pVert++ = endTexture;

	// Top right
	*pVert++ = right;
	*pVert++ = top;
	*pVert++ = 0;
	*pVert++ = 1;
	*pVert++ = beginTexture;
	//-------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------
	// Second tri
	//
	// Top right (again)
	*pVert++ = right;
	*pVert++ = top;
	*pVert++ = 0;
	*pVert++ = 1;
	*pVert++ = beginTexture;

	// Bottom left (again)
	*pVert++ = left;
	*pVert++ = bottom;
	*pVert++ = 0;
	*pVert++ = 0;
	*pVert++ = endTexture;

	// Bottom right
	*pVert++ = right;
	*pVert++ = bottom;
	*pVert++ = 0;
	*pVert++ = 1;
	*pVert++ = endTexture;
	//-------------------------------------------------------------------------------------

	left = -mwidth/2;
	right = left + mwidth*mProgress;

	beginTexture = 0;
	endTexture = 0.5;

	// each vert is (x, y, z, u, v)
	//-------------------------------------------------------------------------------------
	// First tri
	//
	// Upper left
	*pVert++ = left;
	*pVert++ = top;
	*pVert++ = 0;
	*pVert++ = 0;
	*pVert++ = beginTexture;

	// Bottom left
	*pVert++ = left;
	*pVert++ = bottom;
	*pVert++ = 0;
	*pVert++ = 0;
	*pVert++ = endTexture;

	// Top right
	*pVert++ = right;
	*pVert++ = top;
	*pVert++ = 0;
	*pVert++ = 1;
	*pVert++ = beginTexture;
	//-------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------
	// Second tri
	//
	// Top right (again)
	*pVert++ = right;
	*pVert++ = top;
	*pVert++ = 0;
	*pVert++ = 1;
	*pVert++ = beginTexture;

	// Bottom left (again)
	*pVert++ = left;
	*pVert++ = bottom;
	*pVert++ = 0;
	*pVert++ = 0;
	*pVert++ = endTexture;

	// Bottom right
	*pVert++ = right;
	*pVert++ = bottom;
	*pVert++ = 0;
	*pVert++ = 1;
	*pVert++ = endTexture;
	//-------------------------------------------------------------------------------------

	// Unlock vertex buffer
	ptbuf->unlock();

	mAABB = Ogre::AxisAlignedBox();
	mAABB.setMaximum(mwidth/2,mheight/2, mwidth/2);
	mAABB.setMinimum(-mwidth/2,-mheight/2, -mwidth/2);

	mRadius = Ogre::Math::Sqrt((mwidth*mwidth+mheight*mheight)/4);

	mNeedUpdateGeometry = false;
}

void ProgressBar::showOnTop(bool show)
{
	if( mOnTop != show && !mpMaterial.isNull() )
	{
		mOnTop = show;
		mpMaterial->setDepthBias(!mOnTop, 0);
		mpMaterial->setDepthCheckEnabled(!mOnTop);
		mpMaterial->setDepthWriteEnabled(mOnTop);
	}
}

const Ogre::Quaternion & ProgressBar::getWorldOrientation(void) const
{
	assert(mpCam);
	return const_cast<Ogre::Quaternion&>(mpCam->getDerivedOrientation());
}

const Ogre::Vector3 & ProgressBar::getWorldPosition(void) const
{
	assert(mParentNode);
	return mParentNode->_getDerivedPosition();
}

void ProgressBar::getWorldTransforms(Ogre::Matrix4 * xform) const 
{
	if (this->isVisible() && mpCam)
	{
		Ogre::Matrix3 rot3x3, scale3x3 = Ogre::Matrix3::IDENTITY;

		// store rotation in a matrix
		mpCam->getDerivedOrientation().ToRotationMatrix(rot3x3);

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
		xform->setTrans(ppos);
	}
}

void ProgressBar::getRenderOperation(Ogre::RenderOperation & op)
{
	if (isVisible())
	{
		if (mNeedUpdateGeometry)
			_setupGeometry();

		op = mRenderOp;
	}
}

void ProgressBar::_notifyCurrentCamera(Ogre::Camera *cam)
{
	mpCam = cam;
}

void ProgressBar::_updateRenderQueue(Ogre::RenderQueue* queue)
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
void ProgressBar::visitRenderables(Renderable::Visitor* visitor, bool debugRenderables)
{
	visitor->visit(this, 0, false);
}


////////////////////// the Progress Bar with Text //////////////////////////////////////

ProgressBarWithText::ProgressBarWithText(const Ogre::String & name, const Ogre::String & caption, bool applyParentScale )
: m_Bar(name + "_Bar", 200,20, applyParentScale), m_Txt(name + "_BarText", caption, applyParentScale, "BlueHighway", 10)
{
	mCaption = caption;
	m_Txt.setTextAlignment(MovableText::H_LEFT, MovableText::V_ABOVE);
    m_Bar.showOnTop(true);
    m_Txt.showOnTop(true);
    m_Txt.setXpos(-100);
    mName = name;
    mBarPos = 0;
    mpNodeAll = NULL;
    mbShowTime = false;

    beginTime  = -1;
}

ProgressBarWithText::~ProgressBarWithText()
{
    
}

void ProgressBarWithText::setProgress(Real value)
{
    m_Bar.setProgress(value);
    if (beginTime == -1 || !mbShowTime)
    {
        beginValue = value;
        beginTime = (Real) ::GetTickCount()/1000;
        m_Txt.setCaption(mCaption + StringConverter::toString((int)(value*100)) +" %");
    }
    else
    {
        Real deltaTime = (Real) ::GetTickCount()/1000 - beginTime;
        Real deltaValue = value - beginValue;
        if (deltaValue > 0)
        {
            Real remaining = (1-value)*deltaTime/deltaValue;
            int min = Math::Floor(remaining/60);
            int sec = Math::Floor(remaining-min*60);
            char elapsedText[16];
            _snprintf(elapsedText, sizeof(elapsedText) - 1, "%d:%02d", std::min(999, min), sec);

            m_Txt.setCaption(mCaption + StringConverter::toString((int)(value*100)) +" % (" + String(elapsedText) + ")");
        }
        else
        {
            m_Txt.setCaption(mCaption + StringConverter::toString((int)(value*100)) +" %");
        }
    }
}

void ProgressBarWithText::showOnTop(bool show)
{
    m_Bar.showOnTop(show);
    m_Txt.showOnTop(show);
}

void ProgressBarWithText::attach(SceneNode *pNode)
{
    if (!mpNodeAll)
    {
        mpNodeAll = pNode->createChildSceneNode(mName + "_Node", Vector3(0,mBarPos,0));
    }
    mpNodeAll->attachObject(&m_Bar);	
    mpNodeAll->attachObject(&m_Txt);
}

void ProgressBarWithText::detach()
{
	m_Bar.detatchFromParent();
	m_Txt.detatchFromParent();
    if (mpNodeAll)
        mpNodeAll->getCreator()->destroySceneNode(mpNodeAll->getName());
}

#endif
