#ifndef __NAVMODIINSTANCE_H__
#define __NAVMODIINSTANCE_H__

#include "NavigatorModule.h"

namespace NavigatorModule {

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

#endif // __NAVMODIINSTANCE_H__
