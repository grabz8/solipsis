#ifndef __NAVMODINAVIGATORAPP_H__
#define __NAVMODINAVIGATORAPP_H__

#include "NavigatorModule.h"

namespace NavigatorModule {

class NAVIGATORMODULE_EXPORT INavigatorApp
{
public:
    /** Creates the singleton application object and returns
    @remarks
	    A client should first get this object to use Navigator
    */
    static INavigatorApp* createNavigatorApp(const char* appPath);

    /** A client should call this last
    @remarks
	    Returns false if there are still instances that are not destroyed
    */
    virtual bool destroy()=0;

    /** This method creates a new instance of Navigator
    @remarks	
        A client can create many instances of Navigator
    */
    virtual IInstance* createInstance()=0;

    /** This cleans up the resources used by this instance
    @remarks
        This method destroys the Instance object so the
        IInstance pointer cannot be used and should be set to NULL after this call.
    */
    virtual bool destroyInstance(IInstance* instance)=0;
};

} // end namespace

#endif // __NAVMODINAVIGATORAPP_H__
