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
#include "SoundIcon.h"

using namespace Solipsis;

SoundIcon::SoundIcon(SceneManager* pMgr, const String& name, Real yPos, Real xSize, Real ySize)
			: mCInterpolator(ColourValue(0,0.8,0,1), ColourValue(1,1,1,0.5))
{
	mMgr = pMgr;
	// Sound Icon
	mSoundIcon = pMgr->createBillboardSet("SoundIcon_BS_" + name, 1);
	mBillboard = mSoundIcon->createBillboard(0,yPos,0);
	mSoundIcon->setMaterialName("Solipsis/Sound");
	mBillboard->setDimensions(xSize, ySize);
	mParentNode = 0;

	setStatus(SNone);
}

void SoundIcon::attach(SceneNode* pNode)
{
	mParentNode = pNode;
	mParentNode->attachObject(mSoundIcon);
    mSoundIcon->setVisible(mStatus & SVisible);
}

void SoundIcon::detach()
{
	mParentNode->detachObject(mSoundIcon);
	mParentNode = 0;
}

void SoundIcon::setStatus(Status status)
{
	mStatus = status;
    mSoundIcon->setVisible(mStatus & SVisible);
    if (mStatus & SVisible)
		mBillboard->setColour(ColourValue(1, 1, 1, 0.5));
    else
		mBillboard->setColour(ColourValue(1, 0, 0, 1));
    if (mStatus & SAnimated)
    {
		mBillboard->setColour(ColourValue(1, 1, 1, 1));
		mAnimationTime = 0;
    }
}

SoundIcon::~SoundIcon()
{
	mMgr->destroyBillboardSet(mSoundIcon);
}

void SoundIcon::animate(Real timeSinceLastFrame)
{
	if (!(mStatus & SVisible) ||
        !(mStatus & SAnimated))
        return;

    mAnimationTime += timeSinceLastFrame;
	Real fValue = Math::Sin(mAnimationTime*2*Math::PI)/2 + 0.5;
	mBillboard->setColour(mCInterpolator.getResult(fValue));
}
