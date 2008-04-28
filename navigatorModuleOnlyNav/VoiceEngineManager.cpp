#include "VoiceEngineManager.h"

using namespace Solipsis;

template<> VoiceEngineManager* Singleton<VoiceEngineManager>::ms_Singleton = 0;

//-------------------------------------------------------------------------------------
VoiceEngineManager* VoiceEngineManager::getSingletonPtr(void)
{
    return ms_Singleton;
}

//-------------------------------------------------------------------------------------
VoiceEngineManager& VoiceEngineManager::getSingleton(void)
{
    assert(ms_Singleton);
    return (*ms_Singleton);
}

//-------------------------------------------------------------------------------------
VoiceEngineManager::VoiceEngineManager() :
    mSelected(0)
{
}

//-------------------------------------------------------------------------------------
VoiceEngineManager::~VoiceEngineManager()
{
}

//-------------------------------------------------------------------------------------
void VoiceEngineManager::addEngine(IVoiceEngine* engine)
{
    mEngines.push_back(engine);
}

//-------------------------------------------------------------------------------------
void VoiceEngineManager::removeEngine(IVoiceEngine* engine)
{
    // Remove only if equal to registered one, since it might overridden
    // by other plugins
    for (EngineList::iterator it=mEngines.begin(); it != mEngines.end(); ++it)
        if ((*it) == engine)
        {
            mEngines.erase(it);
            break;
        }
}

//-------------------------------------------------------------------------------------
VoiceEngineManager::EngineList& VoiceEngineManager::getEngines()
{
    return mEngines;
}

//-------------------------------------------------------------------------------------
void VoiceEngineManager::selectEngine(const String& name)
{
    mSelected = 0;
    for (EngineList::iterator it=mEngines.begin(); it != mEngines.end(); ++it)
        if ((*it)->getName().compare(name) == 0)
        {
            mSelected = (*it);
            break;
        }
}

//-------------------------------------------------------------------------------------
IVoiceEngine* VoiceEngineManager::getSelectedEngine()
{
    return mSelected;
}

//-------------------------------------------------------------------------------------
