#ifndef __FModSpeexPlugin_h__
#define __FModSpeexPlugin_h__

#include "OgrePlugin.h"
#include "FModSpeexEngine.h"

using namespace Ogre;

namespace Solipsis
{
/** This class represents the FModSpeex engine plugin.
*/
class FModSpeexPlugin : public Plugin
{
protected:
    FModSpeexEngine* mEngine;

public:
    FModSpeexPlugin();

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

#endif // #ifndef __FModSpeexPlugin_h__