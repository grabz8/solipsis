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

#ifndef __SoundIcon_h__
#define __SoundIcon_h__

using namespace Ogre;

namespace Solipsis {

/** This class represents an The sound icon shown above the avatar.
*/
class SoundIcon 
{
public:
    /** Enumeration denoting the status of the icon **/
    enum SoundIcon_Status {
        Invisible,
        Showed,
		Animated
    };

	SoundIcon(SceneManager* pMgr, SceneNode* pNode, const String& name, Real yPos);
	~SoundIcon();

	void setStatus(SoundIcon_Status status);
	SoundIcon_Status getStatus()
	{
		return m_status;
	}

	void animate(Real timeSinceLastFrame);

protected:
	// the billboard set
	BillboardSet* m_SoundIcon; 

	SceneNode* m_pParentNode;
	SceneManager* m_pMgr;

	SoundIcon_Status m_status;
	Real m_animationTime;
};

} // namespace Solipsis

#endif // #ifndef __SoundIcon_h__