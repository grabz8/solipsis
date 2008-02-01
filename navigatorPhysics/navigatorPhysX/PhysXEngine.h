#ifndef __PhysXEngine_h__
#define __PhysXEngine_h__

#include "IPhysicsEngine.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages the PhysX engine.
*/
class PhysXEngine : public IPhysicsEngine
{
public:
    PhysXEngine() {}

    /// @copydoc IPhysicsEngine::getName
    virtual const String& getName() const;

    /// @copydoc IPhysicsEngine::init
    virtual bool init();

    /// @copydoc IPhysicsEngine::shutdown
    virtual bool shutdown();

    /// @copydoc IPhysicsEngine::createScene
    virtual IPhysicsScene* createScene();

    /// @copydoc IPhysicsEngine::destroyScene
    virtual void destroyScene(IPhysicsScene* scene);
};

} // namespace Solipsis

#endif // #ifndef __PhysXEngine_h__