#ifndef __TokamakPlugin_h__
#define __TokamakPlugin_h__

#include "OgrePlugin.h"
#include "TokamakEngine.h"

using namespace Ogre;

namespace Solipsis
{
/** This class represents the Tokamak engine plugin.
*/
class TokamakPlugin : public Plugin
{
protected:
    TokamakEngine* mEngine;

public:
    TokamakPlugin();

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

#endif // #ifndef __TokamakPlugin_h__