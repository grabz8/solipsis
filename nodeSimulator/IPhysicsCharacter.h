#ifndef __IPhysicsCharacter_h__
#define __IPhysicsCharacter_h__

#include "PeerModule.h"
#include "OgrePrerequisites.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents a generic Physics character.
@remarks
Character is assumed centered on X and Z axis, Y origin is just under feet.
*/
class PEERMODULE_EXPORT IPhysicsCharacter
{
public:
    /// Description of 1 character
    typedef struct {
        /// Position 
        Vector3 position;
        /// Radius
        Real radius;
        /// Height
        Real height;
        /// Max offset character can step up (stair's step, ...)
        Real stepOffset;
    } Desc;
public:
    IPhysicsCharacter() {}
    virtual ~IPhysicsCharacter() {}

    /** Create.
    @remarks An implementation must be supplied for this method.
    */
    virtual void create(Desc& desc) = 0;
    /** Move.
    @remarks An implementation must be supplied for this method.
    */
    virtual void move(Vector3& displacement) = 0;
    /** Get current position.
    @remarks An implementation must be supplied for this method.
    */
    virtual void getPosition(Vector3& position) = 0;
};

} // end namespace

#endif // #ifndef __IPhysicsCharacter_h__