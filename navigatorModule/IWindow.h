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

    /** get width of the window
    */
    virtual unsigned int getWidth()=0;

    /** get height of the window
    */
    virtual unsigned int getHeight()=0;
};

} // namespace Solipsis

#endif // #ifndef __IWindow_h__
