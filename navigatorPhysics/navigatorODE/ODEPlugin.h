#ifndef __ODEPlugin_h__
#define __ODEPlugin_h__

#include "OgrePlugin.h"
#include "ODEEngine.h"

using namespace Ogre;

namespace Solipsis
{
/** This class represents the ODE engine plugin.
*/
class ODEPlugin : public Plugin
{
protected:
    ODEEngine* mEngine;

public:
    ODEPlugin();

    /// @copydoc Plugin::getName
    const String& getName() const;

    /// @copydoc Plugin::install
    void install();

    /// @copydoc Plugin::initialise
    void initialise();

    /// @copydoc Plugin::shutdown
    void shutdown();

    /// @copydoc Plugin::uninstall
    void uninstall();
};

} // namespace Solipsis

#endif // #ifndef __ODEPlugin_h__