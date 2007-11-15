#ifndef __Queue_h__
#define __Queue_h__

#include <vector>
#include <pthread.h>

namespace NavigatorModule {

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

} // end namespace

#endif // #ifndef __Queue_h__
