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

#ifndef __IWindow_h__
#define __IWindow_h__

#include "NavigatorModule.h"

namespace Solipsis {

/** This class represents a window.
 */
class IWindow
{
public:
    /** get handle of the window
    */
    virtual void* getHandle()=0;

    /** is a fullscreen window ?
    */
    virtual bool isFullscreen()=0;

    /** get width of the window
    */
    virtual unsigned int getWidth()=0;

    /** get height of the window
    */
    virtual unsigned int getHeight()=0;

    /** set mouse exclusive mode
    */
    virtual void setMouseExclusive(bool exclusive)=0;
};

} // namespace Solipsis

#endif // #ifndef __IWindow_h__
