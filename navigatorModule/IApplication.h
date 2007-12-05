#ifndef __IAPPLICATION_H__
#define __IAPPLICATION_H__

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

} // end namespace

#endif // __IAPPLICATION_H__
