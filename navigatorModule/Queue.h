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

#ifndef __Queue_h__
#define __Queue_h__

#include <vector>
#include <pthread.h>

namespace Solipsis {

/** This template class manages a synchronized queue.
 */
template <class type>
class Queue : public std::vector<type>
{
protected:
    unsigned int mHead, mTail;
    pthread_mutex_t mMutex;

public:
    Queue(unsigned int initialSize = 64) :
        mHead(0),
        mTail(0),
        mMutex(PTHREAD_MUTEX_INITIALIZER)
    {
        initialSize = std::min(initialSize, (unsigned int)8);
        reserve(initialSize);
        resize(initialSize);
    }
    virtual ~Queue() {}

    // add value at the end of the queue
    void addTail(const type& value)
    {
        pthread_mutex_lock(&mMutex);
        // empty ?
        if (_isEmpty())
        {
            mHead = 0;
            mTail = 0;
        }
        // full ?
        if (_isFull())
        {
            insert(begin() + mTail, value);
            if (mHead != 0)
                mHead += 1;
        }
        else
            (*this)[mTail] = value;
        mTail = (mTail + 1)%size();
        pthread_mutex_unlock(&mMutex);
    }

    // get the head value
    type* getHead()
    {
        pthread_mutex_lock(&mMutex);
        type* result = (_isEmpty() ? 0 : &((*this)[mHead]));
        pthread_mutex_unlock(&mMutex);

        return result;
    }

    // remove the head value
    void removeHead()
    {
        pthread_mutex_lock(&mMutex);
        if (!_isEmpty())
            mHead = (mHead + 1)%size();
        pthread_mutex_unlock(&mMutex);
    }

    // queue is empty
    bool isEmpty()
    {
        pthread_mutex_lock(&mMutex);
        bool empty = _isEmpty();
        pthread_mutex_unlock(&mMutex);

        return empty;
    }
private:
    bool _isEmpty() { return (mHead == mTail); }
    bool _isFull() { return ((size() == 0) || (mHead == ((mTail + 1)%size()))); }
};

} // namespace Solipsis

#endif // #ifndef __Queue_h__
