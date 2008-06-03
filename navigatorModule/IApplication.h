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

#ifndef __IApplication_h__
#define __IApplication_h__

#include "NavigatorModule.h"

namespace Solipsis {

/** This class represents an application composed of several running instances.
 */
class NAVIGATORMODULE_EXPORT IApplication
{
public:
    /** Creates the singleton application object and returns
    @remarks
	    A client should first get this object to use application
    @param[in] appPath The application path.
    @param[in] standAloneAutoCreateWindow The application run standalone into 1 auto-created window.
    @param[in] windowTitle The title of the auto-created window.
    */
    static IApplication* createApplication(const char* appPath, bool standAloneAutoCreateWindow = false, const char* windowTitle = 0);

    /** Get the singleton application object
    @remarks
	    Returns 0 if no application was previously created
    */
    static IApplication* getApplication();

    /** A client should call this last
    @remarks
	    Returns false if there are still instances that are not destroyed
    */
    virtual bool destroy() = 0;

    /** This method creates a new instance of application
    @remarks	
        A client can create many instances of application
    */
    virtual IInstance* createInstance() = 0;

    /** This cleans up the resources used by this instance
    @remarks
        This method destroys the Instance object so the
        IInstance pointer cannot be used and should be set to NULL after this call.
    @param[in] instance The instance to destroy.
    */
    virtual bool destroyInstance(IInstance* instance) = 0;
};

} // namespace Solipsis

#endif // #ifndef __IApplication_h__
