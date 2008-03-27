#ifndef __TimeListener_h__
#define __TimeListener_h__

#include "Ogre.h"

namespace Solipsis {

/** An interface class defining a listener which can be used to receive
    notifications of time events.
*/
class TimeListener
{
public:
    /** Called when a frame is about to begin rendering.
        @return
            True to go ahead, false to abort rendering and drop
            out of the rendering loop.
    */
    virtual bool tick(Ogre::Real timeSinceLastTick) = 0;
};

} // namespace Solipsis

#endif // #ifndef __TimeListener_h__