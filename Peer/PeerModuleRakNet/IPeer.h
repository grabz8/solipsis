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

#ifndef __IPeer_h__
#define __IPeer_h__

#include "PeerModule.h"

namespace Solipsis {

/** This interface should be implemented if calls to render system must be synchronized.
 */
class IPeerRenderSystemLock
{
public:
    /// This method lock access to the render system
    virtual void lock() = 0;
    /// This method unlock access to the render system
    virtual void unlock() = 0;
};

/** This class represents an application managing a Solipsis Peer.
 */
class PEERMODULE_EXPORT IPeer
{
public:
    /** Creates the singleton peer application object and returns
    @remarks
	    A client should first get this object to use peer application
    @param[in] appPath The peer application path.
    @param[in] argc Arguments count.
    @param[in] argv Arguments values.
    */
    static IPeer* createPeer(const char* appPath, int argc, char** argv);

    /** Get the singleton peer application object
    @remarks
	    Returns 0 if no peer application was previously created
    */
    static IPeer* getPeer();

    /** Initialize the peer application object
    @remarks
	    Returns false if application cannot be initialized
    @param[in] renderSystemLock The optional render system locking implementation.
    */
    virtual bool initialize(IPeerRenderSystemLock* renderSystemLock = 0) = 0;

    /** A client should call this last
    @remarks
	    Returns false if application cannot be destroyed
    */
    virtual bool destroy() = 0;
};

} // namespace Solipsis

#endif // #ifndef __IPeer_h__
