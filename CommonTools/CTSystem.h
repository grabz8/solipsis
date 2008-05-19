#ifndef __CTSystem_h__
#define __CTSystem_h__

namespace CommonTools {

/** This static class contains several helper methods above system.
 */
class System
{
public:
    /// Sleeping function
    static void sleep(unsigned long durationMs);

    /// Set mouse cursor visibility
    static void setMouseCursorVisibility(bool visible);
};

} // namespace CommonTools

#endif // #ifndef __CTSystem_h__