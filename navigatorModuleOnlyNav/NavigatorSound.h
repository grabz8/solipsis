#ifndef __NavigatorSound_h__
#define __NavigatorSound_h__

#include "fmod.hpp"
#include "fmod_errors.h"

namespace Solipsis {

/** This class manages sound into navigator (sounds, musics, voice chat).
 */
class NavigatorSound
{
protected:
    FMOD::System *mSoundSystem;

public:
    NavigatorSound();
    ~NavigatorSound();

    /// initialize sound system
    bool initialize();

    /// shutdown sound system
    void shutdown();

    /// update sound system
    void update();

private:
    /// check/log FMOD error
    bool fmodErrorCheck(FMOD_RESULT result);
};

} // namespace Solipsis

#endif // #ifndef __NavigatorVoice_h__