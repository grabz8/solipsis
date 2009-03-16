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

#ifndef __PoolEntry_h__
#define __PoolEntry_h__

namespace Solipsis 
{

#ifdef POOL

#include <list>

template<class T> 
class RefCntPoolPtr;
class Pool;

class PoolEntry
{
    template<class T>
    friend class RefCntPoolPtr;

protected:
    unsigned int mRefCount;
    pthread_mutex_t mMutex;

public:
    PoolEntry() : mMutex(PTHREAD_MUTEX_INITIALIZER) {}
    void lock() { pthread_mutex_lock(&mMutex); }
    void unlock() { pthread_mutex_unlock(&mMutex); }
    virtual Pool& getPool() const = 0;
    virtual void clear() {}
};
class Pool : public std::list<PoolEntry*>
{
protected:
    pthread_mutex_t mMutex;

public:
    Pool() : mMutex(PTHREAD_MUTEX_INITIALIZER) {}
    ~Pool() {
        lock();
        for(iterator it=begin();it!=end();++it)
            delete (*it);
        unlock();
    }
    void lock() { pthread_mutex_lock(&mMutex); }
    void unlock() { pthread_mutex_unlock(&mMutex); }
};
template<class T>
class RefCntPoolPtr
{
protected:
    T* mRep;

public:
    static RefCntPoolPtr<T> nullPtr;

protected:
    inline void addRef() {
        if (mRep == 0)
            return;
        mRep->lock();
        ++(mRep->mRefCount);
        mRep->unlock();
    }
    inline void delRef() {
        if (mRep == 0)
            return;
        Pool* pool = 0;
        T* entryToPushBack = 0;
        mRep->lock();
        if (--(mRep->mRefCount) == 0)
        {
            mRep->clear();
            pool = &(mRep->getPool());
            entryToPushBack = mRep;
            mRep->unlock();
            mRep = 0;
        }
        else mRep->unlock();
        if (entryToPushBack != 0)
        {
            pool->lock();
            pool->push_back(entryToPushBack);
            pool->unlock();
        }
    }
    virtual void swap(RefCntPoolPtr<T> &other) {
        std::swap(mRep, other.mRep);
    }
public:
    RefCntPoolPtr() : mRep(0) { allocate(); }
    template<class Y>
    explicit RefCntPoolPtr(Y* rep) : mRep(0) {
        if (rep != 0)
        {
            mRep = rep;
            mRep->mRefCount = 1;
        }
    }
    RefCntPoolPtr(const RefCntPoolPtr& other) : mRep(other.mRep) { addRef(); }
    RefCntPoolPtr& operator=(const RefCntPoolPtr& other) {
        if (mRep == other.mRep)
            return *this; 
        // Swap current data into a local copy
        // this ensures we deal with rhs and this being dependent
        RefCntPoolPtr<T> tmp(other);
        swap(tmp);
        return *this;
    }
    template<class Y>
    RefCntPoolPtr(const RefCntPoolPtr<Y>& other) : mRep(static_cast<T*>(other.getPointer())) { addRef(); }
    template<class Y>
    RefCntPoolPtr& operator=(const RefCntPoolPtr<Y>& other) {
        if (mRep == other.mRep)
            return *this; 
        // Swap current data into a local copy
        // this ensures we deal with rhs and this being dependent
        RefCntPoolPtr<T> tmp(other);
        swap(tmp);
        return *this;
    }
    virtual ~RefCntPoolPtr() { delRef(); }

    inline T& operator*() const { assert(mRep); return *mRep; }
    inline T* operator->() const { assert(mRep); return mRep; }
    inline T* get() const { return mRep; }
    inline void allocate() {
        delRef();
        Pool& pool = T::getStaticPool();
        pool.lock();
        if (pool.empty())
            mRep = new T();
        else
        {
            mRep = static_cast<T*>(pool.front());
            pool.pop_front();
        }
        pool.unlock();
        mRep->mRefCount = 1;
    }
    inline void bind(T* rep) {
        assert(mRep == 0);
        mRep = rep;
        mRep->mRefCount = 1;
    }
    inline bool unique() const {
        assert(mRep != 0);
        mRep->lock();
        bool unique = (mRep->mRefCount == 1);
        mRep->unlock();
        return unique;
    }
    inline unsigned int refCount() const {
        assert(mRep != 0);
        mRep->lock();
        unsigned int refCount = mRep->mRefCount;
        mRep->unlock();
        return refCount;
    }
    inline T* getPointer() const { return mRep; }
    inline bool isNull(void) const { return mRep == 0; }
    inline void setNull(void) { delRef(); mRep->mRefCount = 0; mRep = 0; }
};
template<class T, class U> inline bool operator==(RefCntPoolPtr<T> const& a, RefCntPoolPtr<U> const& b)
{
    return a.get() == b.get();
}
template<class T, class U> inline bool operator!=(RefCntPoolPtr<T> const& a, RefCntPoolPtr<U> const& b)
{
    return a.get() != b.get();
}
#endif


} // namespace Solipsis

#endif // #ifndef __PoolEntry_h__
