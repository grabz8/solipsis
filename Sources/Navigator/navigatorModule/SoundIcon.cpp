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

SoundIcon::SoundIcon(SceneManager* pMgr, SceneNode* pNode, const String& name, Real yPos)
{
	// Sound Icon
	m_SoundIcon = pMgr->createBillboardSet(name + "_Sound_Icon", 1);
	Billboard *pBoard = m_SoundIcon->createBillboard(0,yPos,0);
	m_SoundIcon->setMaterialName("Solipsis/Sound");
	pBoard->setDimensions(0.5,0.3);
	
	m_pParentNode = pNode;
	m_pParentNode->attachObject(m_SoundIcon);
	setStatus(Invisible);
}

void SoundIcon::setStatus(SoundIcon::SoundIcon_Status status)
{
	m_status = status;
	switch(status)
	{
	case Invisible:
// 		m_SoundIcon->setVisible(false);
		m_SoundIcon->getMaterial().get()->setAmbient(1, 0, 0);
		m_SoundIcon->setVisible(true);
		break;
	case Showed:
		m_SoundIcon->getMaterial().get()->setAmbient(1, 1, 1);
		m_SoundIcon->setVisible(true);
		break;
	case Animated:
		m_SoundIcon->setVisible(true);
		m_animationTime = 0;
		break;
	}
}


SoundIcon::~SoundIcon()
{
	m_pParentNode->detachObject(m_SoundIcon);
}

void SoundIcon::animate(Real timeSinceLastFrame)
{
	if (m_status == Animated)
	{
		m_animationTime += timeSinceLastFrame;
		Real fValue = Math::Sin(m_animationTime*2*Math::PI)/2 + 0.5;
	
		m_SoundIcon->getMaterial().get()->setAmbient(fValue, 1, fValue);

	}
}
