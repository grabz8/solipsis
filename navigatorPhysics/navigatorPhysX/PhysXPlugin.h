#ifndef __PhysXPlugin_h__
#define __PhysXPlugin_h__

#include "OgrePlugin.h"
#include "PhysXEngine.h"

using namespace Ogre;

namespace Solipsis
{
/** This class represents the PhysX engine plugin.
*/
class PhysXPlugin : public Plugin
{
protected:
    PhysXEngine* mEngine;

public:
    PhysXPlugin();

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

} // end namespace

#endif // #ifndef __PhysXPlugin_h__