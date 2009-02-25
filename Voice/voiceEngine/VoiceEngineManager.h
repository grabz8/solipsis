/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef __VoiceEngineManager_h__
#define __VoiceEngineManager_h__

#include "DllExport.h"

#include <vector>
#include "IVoiceEngine.h"
#include <CTSingleton.h>

namespace Solipsis {

/** This class manages the voice engines
 */
class VOICEENGINE_EXPORT VoiceEngineManager : public CommonTools::Singleton<VoiceEngineManager>
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
	void selectEngine(const std::string& name);
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
