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

#ifndef __SiteNode_h__
#define __SiteNode_h__

#include "Node.h"
#include "Entity.h"

namespace Solipsis {

/** This class manages 1 site node.
*/
class SiteNode : public Node
{
protected:
    /// Map of entities
    RakNetEntity::RakNetEntityMap mPresentEntities;

    /// Entity
    RakNetEntity *mEntity;

public:
    /** Constructor. */
    SiteNode();
    /** Destructor. */
    virtual ~SiteNode();

    /** Get present entities. */
    RakNetEntity::RakNetEntityMap& getPresentEntities();

    /** Get entity. */
    RakNetEntity* getEntity() { return mEntity; }
    /** Set entity. */
    void setEntity(RakNetEntity* entity) { mEntity = entity; }

    /** See Solipsis::Node. */
    virtual bool loadFromElt(TiXmlElement* nodeElt);
    /** See Solipsis::Node. */
    virtual TiXmlElement* getSavedElt();

    /** Method called when 1 entity is created. */
    virtual void onNewEntity(RakNetEntity* entity);
    /** Method called when 1 entity is destroyed. */
    virtual void onLostEntity(RakNetEntity* entity);
};

} // namespace Solipsis

#endif // #ifndef __SiteNode_h__