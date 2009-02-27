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

#ifndef __ODEEngine_h__
#define __ODEEngine_h__

#include "IPhysicsEngine.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages the ODE engine.
*/
class ODEEngine : public IPhysicsEngine
{
public:
    ODEEngine() {}

    /// @copydoc IPhysicsEngine::getName
    virtual const String& getName() const;

    /// @copydoc IPhysicsEngine::init
    virtual bool init();

    /// @copydoc IPhysicsEngine::shutdown
    virtual bool shutdown();

    /// @copydoc IPhysicsEngine::createScene
    virtual IPhysicsScene* createScene();

    /// @copydoc IPhysicsEngine::destroyScene
    virtual void destroyScene(IPhysicsScene* scene);
};

} // namespace Solipsis

#endif // #ifndef __ODEEngine_h__