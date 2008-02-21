#ifndef __IPeer_h__
#define __IPeer_h__

#include "PeerModule.h"

namespace Solipsis {

/** This class represents an application managing a Solipsis Peer.
 */
class PEERMODULE_EXPORT IPeer
{
public:
    /** Creates the singleton peer application object and returns
    @remarks
	    A client should first get this object to use peer application
    @param[in] appPath The peer application path.
    @param[in] argc Arguments count.
    @param[in] argv Arguments values.
    */
    static IPeer* createPeer(const char* appPath, int argc, char** argv);

    /** Get the singleton peer application object
    @remarks
	    Returns 0 if no peer application was previously created
    */
    static IPeer* getPeer();

    /** A client should call this last
    @remarks
	    Returns false if application cannot be destroyed
    */
    virtual bool destroy() = 0;
};

} // namespace Solipsis

#endif // #ifndef __IPeer_h__
