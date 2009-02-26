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

#ifndef __CTScopedMutexLock_h__
#define __CTScopedMutexLock_h__

#include <pthread.h>

namespace CommonTools {

/** This class locks a mutex on construction and release it on destruction.
 */
class ScopedMutexLock
{
public:
	/// Constructor, specify the mutex to lock/unlock
    ScopedMutexLock(pthread_mutex_t& mutex) : mMutex(mutex) { pthread_mutex_lock(&mMutex); }
	/// Destructor, mutex is unlocked
    ~ScopedMutexLock() { pthread_mutex_unlock(&mMutex); }

private:
    // dummy copy constructor and operator= to prevent copying
    ScopedMutexLock(const ScopedMutexLock&);
    ScopedMutexLock& operator=(const ScopedMutexLock&);

protected:
	pthread_mutex_t& mMutex;
};

} // namespace CommonTools

#endif // #ifndef __CTScopedMutexLock_h__
