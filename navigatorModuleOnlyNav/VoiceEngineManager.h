#ifndef __VoiceEngineManager_h__
#define __VoiceEngineManager_h__

#include <vector>
#include "NavigatorModule.h"
#include "IVoiceEngine.h"

namespace Solipsis {

/** This class manages the voice engines
 */
class NAVIGATORMODULE_EXPORT VoiceEngineManager : public Ogre::Singleton<VoiceEngineManager>
{
public:
    /// List of engines
    typedef std::vector<IVoiceEngine*> EngineList;

protected:
    /// List of available engines
    EngineList mEngines;
    /// Selected engine
    IVoiceEngine* mSelected;

public:
    /** Constructor. */
    VoiceEngineManager();
    /** Destructor. */
    virtual ~VoiceEngineManager();

    /** Add a new engine. */
    void addEngine(IVoiceEngine* engine);
    /** Remove an engine. */
    void removeEngine(IVoiceEngine* engine);

    /** Get engines. */
    EngineList& getEngines();
    /** Set current engine. */
    void selectEngine(const Ogre::String& name);
    /** Get current engine. */
    IVoiceEngine* getSelectedEngine();

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
    static VoiceEngineManager& getSingleton(void);
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
    static VoiceEngineManager* getSingletonPtr(void);
};

} // namespace Solipsis

#endif // #ifndef __VoiceEngineManager_h__
