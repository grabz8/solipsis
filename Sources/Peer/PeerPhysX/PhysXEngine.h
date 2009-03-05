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

#ifndef __PhysXEngine_h__
#define __PhysXEngine_h__

#include <IPhysicsEngine.h>
#include <NxUserOutputStream.h>
#include <stdio.h>
#include <stdlib.h>

using namespace Ogre;

namespace Solipsis {

/** This class manages the PhysX engine.
*/
class PhysXEngine : public IPhysicsEngine, public NxUserOutputStream
{
private:
    /// Logging instance
    IPhysicsEngineLogger* mLogger;

public:
    PhysXEngine() {mLogger = NULL;}

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

    /// @copydoc IPhysicsEngine::setLogger
    virtual void setLogger(IPhysicsEngineLogger* logger) { mLogger = logger; }

    /// log a message
    inline void logMessage(const std::string& message) { if (mLogger != 0) mLogger->logMessage(message); }

    /// See NxUserOutputStream
    void reportError(NxErrorCode code, const char *message, const char* file, int line)
    {
        // this should be routed to the application specific error handling. If this gets hit
        // then you are in most cases using the SDK wrong and you need to debug your code!
        // however, code may  just be a warning or information.
        char reportLog[256];
        _snprintf(reportLog, sizeof(reportLog) - 1, "PhysXEngine::OutputStream::reportError() error %d, %s", code, message);
        reportLog[sizeof(reportLog) - 1] = '\0';
        logMessage(reportLog);
//        exit(1);
    }
    /// See NxUserOutputStream
    NxAssertResponse reportAssertViolation(const char *message, const char *file, int line)
    {
        // this should not get hit by a properly debugged SDK!
        char reportLog[256];
        _snprintf(reportLog, sizeof(reportLog) - 1, "PhysXEngine::OutputStream::reportAssertViolation() %s", message);
        reportLog[sizeof(reportLog) - 1] = '\0';
        logMessage(reportLog),
        assert(0);
        return NX_AR_CONTINUE;
    }
    /// See NxUserOutputStream
    void print(const char *message)
    {
        // just a information message
        char reportLog[256];
        _snprintf(reportLog, sizeof(reportLog) - 1, "PhysXEngine::OutputStream::print() %s", message);
        reportLog[sizeof(reportLog) - 1] = '\0';
        logMessage(reportLog);
    }
};

} // namespace Solipsis

#endif // #ifndef __PhysXEngine_h__