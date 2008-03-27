#ifndef __IPhysicsCharacter_h__
#define __IPhysicsCharacter_h__

#include "PeerModule.h"
#include "OgrePrerequisites.h"
#include "Ogre.h"

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
        Ogre::Vector3 position;
        /// Radius
        Ogre::Real radius;
        /// Height
        Ogre::Real height;
        /// Max offset character can step up (stair's step, ...)
        Ogre::Real stepOffset;
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
    virtual void move(Ogre::Vector3& displacement) = 0;
    /** Get current position.
    @remarks An implementation must be supplied for this method.
    */
    virtual void getPosition(Ogre::Vector3& position) = 0;
};

} // end namespace

#endif // #ifndef __IPhysicsCharacter_h__