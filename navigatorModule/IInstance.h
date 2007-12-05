#ifndef __IINSTANCE_H__
#define __IINSTANCE_H__

#include "NavigatorModule.h"

namespace Solipsis {

/** This class represents a running instance of the application.
 */
class IInstance
{
public:
    /** Sets the IWindow object for the rendering window that Navigator should render into
    @param w 
        The container application should create an object that implements
        the IWindow interface to return handle, width and height information
    */
    virtual bool setWindow(IWindow* w) = 0;

    /** Returns the IWindow pointer for width, height, etc.
    */
    virtual IWindow* getIWindow() = 0;

    /** Process 1 new event (Mouse, Keyboard, ...)
    */
    virtual bool processEvent(const Event& event) = 0;

    /** runs the render loop
    */
    virtual bool run() = 0;

    /** terminates the instance.
    */
    virtual void requestTerminate() = 0;
};

} // end namespace

#endif // __IINSTANCE_H__
