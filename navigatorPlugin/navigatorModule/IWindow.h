#ifndef __NAVMODIWINDOW_H__
#define __NAVMODIWINDOW_H__

#include "NavigatorModule.h"

namespace NavigatorModule {

class IWindow
{
public:
    /** get handle of the window
    */
    virtual void * getHandle()=0;

    /** get width of the window
    */
    virtual unsigned int getWidth()=0;

    /** get height of the window
    */
    virtual unsigned int getHeight()=0;
};

} // end namespace

#endif // __NAVMODIWINDOW_H__
