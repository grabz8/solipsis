#ifndef __IWINDOW_H__
#define __IWINDOW_H__

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

    /** get width of the window
    */
    virtual unsigned int getWidth()=0;

    /** get height of the window
    */
    virtual unsigned int getHeight()=0;
};

} // end namespace

#endif // __IWINDOW_H__
