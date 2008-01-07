#ifndef __PhysicsEngineManager_h__
#define __PhysicsEngineManager_h__

#include <map>
#include "NavigatorModule.h"
#include "IPhysicsEngine.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages the physics engines
 */
class NAVIGATORMODULE_EXPORT PhysicsEngineManager : public Singleton<PhysicsEngineManager>
{
public:
    /// Map of engines
    typedef std::map<String, IPhysicsEngine*> EngineMap;

protected:
    /// List of available engines
    EngineMap mEngines;
    /// Selected engine
    IPhysicsEngine* mSelected;

public:
    /** Constructor. */
    PhysicsEngineManager();
    /** Destructor. */
    virtual ~PhysicsEngineManager();

    /** Add a new engine. */
    void addEngine(IPhysicsEngine* engine);
    /** Remove an engine. */
    void removeEngine(IPhysicsEngine* engine);

    /** Get engines. */
    EngineMap& getEngines();
    /** Set current engine. */
    void selectEngine(const String& name);
    /** Get current engine. */
    IPhysicsEngine* getSelectedEngine();

    /** Override standard Singleton retrieval.
    @remarks
    Why do we do this? Well, it's because the Singleton
    implementation is in a .h file, which means it gets compiled
    into anybody who includes it. This is needed for the
    Singleton template to work, but we actually only want it
    compiled into the implementation of the class based on the
    Singleton, not all of them. If we don't change this, we get
    link errors when trying to use the Singleton-based class from
    an outside dll.
    @par
    This method just delegates to the template version anyway,
    but the implementation stays in this single compilation unit,
    preventing link errors.
    */
    static PhysicsEngineManager& getSingleton(void);
    /** Override standard Singleton retrieval.
    @remarks
    Why do we do this? Well, it's because the Singleton
    implementation is in a .h file, which means it gets compiled
    into anybody who includes it. This is needed for the
    Singleton template to work, but we actually only want it
    compiled into the implementation of the class based on the
    Singleton, not all of them. If we don't change this, we get
    link errors when trying to use the Singleton-based class from
    an outside dll.
    @par
    This method just delegates to the template version anyway,
    but the implementation stays in this single compilation unit,
    preventing link errors.
    */
    static PhysicsEngineManager* getSingletonPtr(void);
};

} // end namespace

#endif // __PhysicsEngineManager_h__
