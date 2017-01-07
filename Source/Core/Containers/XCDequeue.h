#ifndef XCDEQUEUE_H
#define XCDEQUEUE_H

#include "../XCBasic.h"
#include "../XCMemory.h"
#include "../XCIterators.h"
#include "../XCAlgorithm.h"

namespace XC
{  
    template <typename T, xsize TBufferSize = 5, typename TAllocator = DefaultAllocator<T> >
    class Dequeue
    {
    public:
        template <typename TReference, typename TPointer>
        class BaseIterator
        {
        public:
            typedef BaseIterator<TReference, TPointer> Self;
            typedef BaseIterator<const T &, const T *> ConstantIterator;
            typedef BaseIterator<T &, T *> Iterator;

            // BaseIterator traits :
            typedef RandomAccessIteratorTag IteratorCategory;
            typedef T ValueType;
            typedef TPointer Pointer;
            typedef TReference Reference;
            typedef xsize SizeType;
            typedef xpointerdifference DifferenceType;

            // The map pointer :
            typedef T * * MapPointer; 

        public:
            BaseIterator() {}
            BaseIterator(const ConstantIterator & rhs) : mCurrent(rhs.mCurrent), mFirst(rhs.mCurrent), mLast(rhs.mLast), mNode(rhs.mNode) {} // use constant BaseIterator to initialize all BaseIterators
            BaseIterator(const Iterator & rhs) : mCurrent(rhs.mCurrent), mFirst(rhs.mFirst), mLast(rhs.mLast), mNode(rhs.mNode) {} // This must write in case of moti definitions
            ~BaseIterator() {}
            Self & operator = (const Self &) = default;

            // The BaseIterator feunctions :
            Reference operator * () const { return *mCurrent; }
            Pointer operator -> () const { return &(operator * ()); }
            Self operator + (xpointerdifference n) const;
            Self operator - (xpointerdifference n) const;
            xpointerdifference operator - (const Self & rhs) const; // Two BaseIterators minus together is a pointer difference.
            Reference operator [] (xpointerdifference n) const { return *(this + n); } // operator * and opeartor +
            bool operator == (const Self & rhs) const { return mCurrent == rhs.mCurrent; }
            bool operator != (const Self & rhs) const { return !(*this == rhs); }
            bool operator < (const Self & rhs) const { return mNode == rhs.mNode ? mCurrent < rhs.mCurrent : mNode < rhs.mNode; }
            bool operator > (const Self & rhs) const { return !(*this < rhs && *this == rhs); }

            Self & operator ++ ();
            Self & operator -- ();
            Self operator ++ (int);
            Self operator -- (int);
            Self & operator += (xpointerdifference n);
            Self & operator -= (xpointerdifference n) { return *this += -n; }
            
        public:
            T * mCurrent;
            T * mFirst;
            T * mLast;
            T * * mNode; // Points to the location of the whole map;

        protected:
            xsize GetBufferSize(xsize count) const { return count == 0 ? 512 : count; } // The same function DEQueue have.
            
            void SetNode(T * * newNode);

            friend class Dequeue<T, TBufferSize, TAllocator>;
        };
    
    public:
        // Basic typedefs :
        typedef T ValueType;
        typedef T * Pointer;
        typedef xsize SizeType;

        // The BaseIterators :
        typedef BaseIterator<const T &, const T *> ConstantIterator;
        typedef BaseIterator<T &, T *> Iterator;

        // The map pointer :
        typedef T * * MapPointer;

        typedef Dequeue<T, TBufferSize, TAllocator> Self;

    public:
        Dequeue() = default;
        Dequeue(const Self &) = default;
        ~Dequeue() = default;
        Self & operator = (const Self &) = default;

        ConstantIterator GetBegin() const { return ConstantIterator(mStart); }
        ConstantIterator GetEnd() const { return ConstantIterator(mFinish); }
        const T & At(xsize index) const { return mStart[index]; }
        const T & operator [] (xsize index) const { return At(index); }
        const T & GetFront() const { return *GetBegin(); }
        const T & GetBack() const { return *(GetEnd() - 1); }
        xsize GetSize() const { return xsize(mFinish - mStart); }

        Iterator GetBegin() { return mStart; }
        Iterator GetEnd() { return mFinish; }
        T & At(xsize index) { return mStart[index]; }
        T & operator [] (xsize index) { return At(index); }
        T & GetFront() { return *GetBegin(); }
        T & GetBack() { return *(GetEnd() - 1); }
        void PushBack(const T & value);
        void PopBack();
        void PushFront(const T & value);
        void PopFront();
        void Clear();
        Iterator Erase(Iterator location);
        Iterator Erase(Iterator first, Iterator last);
        Iterator Insert(Iterator location, const T & value);

        // These functions are for C++ 11 :
        ConstantIterator begin() const { return GetBegin(); }
        ConstantIterator end() const { return GetEnd(); }
        
        Iterator begin() { return GetBegin(); }
        Iterator end() { return GetEnd(); }

    protected:
        typedef InsideAllocator<T *, TAllocator> MapAllocator; // Allocate the whole map
        typedef InsideAllocator<T, TAllocator> DataAllocator; // Allocate element of each node

    protected:
        xsize GetBufferSize() const { return TBufferSize == 0 ? 512 : TBufferSize; }
        xsize GetInitianalMapSize() const { return xsize(8); }

        void EmptyInitialize();
        void FillInitialize(xsize count, const T & value);
        T * AllocateNode() { return DataAllocator::Allocate(GetBufferSize()); } // Allocate a node.
        void DeallocateNode(T * node) { DataAllocator::Deallocate(node, GetBufferSize()); }
        T * * AllocateMap() { return MapAllocator::Allocate(mMapSize); }
        void ReserveIfMapAtBack(xsize nodesToAdd = 1);
        void ReserveIfMapAtFront(xsize nodesToAdd = 1);
        void OutOfMemorySolve(xsize nodesToAdd, bool isAddBack);
        void ReleaseMemory();

    protected:
        T * * mMap; // Each element of the map contains a pointer of T type.
        xsize mMapSize; // The count of pointers in a map.
        Iterator mStart; // The start BaseIterator of the whole dequeue.
        Iterator mFinish; // The finish BaseIterator of the whole dequeue.
    };  

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    inline typename Dequeue<T, TBufferSize, TAllocator>::template BaseIterator<TReference, TPointer>::Self
        Dequeue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator + (xpointerdifference n) const
    {
        Self ans = *this;
        return ans += n;
    }


    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    inline typename Dequeue<T, TBufferSize, TAllocator>::template BaseIterator<TReference, TPointer>::Self
        Dequeue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator - (xpointerdifference n) const 
    {
        Self ans = *this;
        return ans += -n;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    inline xpointerdifference
        Dequeue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator - (const Self & rhs) const
    {
        return xpointerdifference((mNode - rhs.mNode) * xpointerdifference(GetBufferSize(TBufferSize))
            + (mCurrent - rhs.mCurrent) + (rhs.mLast - mLast));        
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    typename Dequeue<T, TBufferSize, TAllocator>::template BaseIterator<TReference, TPointer>::Self &
        Dequeue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator ++ ()
    {
        ++mCurrent;
        if (mCurrent == mLast)
        {
            SetNode(mNode + xpointerdifference(1));
            mCurrent = mFirst;
        }

        return *this;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    inline typename Dequeue<T, TBufferSize, TAllocator>::template BaseIterator<TReference, TPointer>::Self
        Dequeue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator ++ (int) 
    {
        Self ans = *this;
        ++(*this);
        return ans;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    inline typename Dequeue<T, TBufferSize, TAllocator>::template BaseIterator<TReference, TPointer>::Self
        Dequeue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator -- (int)
    {
        Self ans = *this;
        --(*this);
        return ans;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    typename Dequeue<T, TBufferSize, TAllocator>::template BaseIterator<TReference, TPointer>::Self &
        Dequeue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator += (xpointerdifference n)
    {
        xpointerdifference offset = (mCurrent - mFirst) + n;
        if (offset >= 0 && offset < xpointerdifference(GetBufferSize(TBufferSize)))
        {
            mCurrent += n;
        }
        else
        {
            xpointerdifference nodeOffset = offset > 0 ? 
                offset / xpointerdifference(GetBufferSize(TBufferSize)) :
                (-offset - 1) / xpointerdifference(GetBufferSize(TBufferSize)) - 1;
            SetNode(mNode + nodeOffset);
            mCurrent = mFirst + (offset - nodeOffset * xpointerdifference(GetBufferSize(TBufferSize)));
        }

        return *this;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    inline void Dequeue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::SetNode(T * * newNode)
    {
        mNode = newNode;
        mFirst = *newNode;
        mLast = mFirst + xpointerdifference(GetBufferSize);
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void Dequeue<T, TBufferSize, TAllocator>::PushBack(const T & value)
    {
        if (mFinish.mCurrent != mFinish.mLast - 1) // Should last - 1, make sure there will always be a node at the end.
        {
            Memory::Construct(mFinish.mCurrent, value);
            ++mFinish.mCurrent;
        }
        else // Should jump to next node.
        {
            ReserveIfMapAtBack();
            *(mFinish.mNode + 1) = AllocateNode();
            Memory::Construct(mFinish.mCurrent, value);
            mFinish.SetNode(mFinish.mNode + 1);
            mFinish.mCurrent = mFinish.mFirst;
        }
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void Dequeue<T, TBufferSize, TAllocator>::PopBack()
    {
        if (mFinish.mCurrent != mFinish.mFirst)
        {
            --mFinish.mCurrent;
            Memory::Destroy(mFinish.mCurrent);
        }
        else
        {
            DeallocateNode(mFinish.mFirst);
            mFinish.SetNode(mFinish.mNode - 1);
            mFinish.mCurrent = mFinish.mLast - 1;
            Memory::Destroy(mFinish.mCurrent);
        }
    }

    // PushFront is different from PushBack
    template <typename T, xsize TBufferSize, typename TAllocator>
    void Dequeue<T, TBufferSize, TAllocator>::PushFront(const T & value)
    {
        if (mStart.mCurrent != mStart.mFirst) // do not need to have more free space.
        {
            Memory::Construct(mStart.mCurrent - 1, value);
            --mStart.mCurrent;
        }
        else
        {
            ReserveIfMapAtFront();
            *(mStart.mNode - 1) = AllocateNode();
            mStart.SetNode(mStart.mNode - 1);
            mStart.mCurrent = mStart.mLast - 1;
            Memory::Construct(mStart.mCurrent, value);
        }
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void Dequeue<T, TBufferSize, TAllocator>::PopFront()
    {
        if (mStart.mCurrent != mStart.mFinish - 1)
        {
            Memory::Destroy(mStart.mCurrent);
            ++mStart.mCurrent;
        }
        else
        {
            Memory::Destroy(mStart.mCurrent);
            DeallocateNode(mStart.mFirst);
            mStart.SetNode(mStart.mNode + 1);
            mStart.mCurrent = mStart.mFirst;
        }
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void Dequeue<T, TBufferSize, TAllocator>::Clear()
    {

    }    

    template <typename T, xsize TBufferSize, typename TAllocator>
    typename Dequeue<T, TBufferSize, TAllocator>::Iterator 
        Dequeue<T, TBufferSize, TAllocator>::Erase(Iterator location)
    {

    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    typename Dequeue<T, TBufferSize, TAllocator>::Iterator 
        Dequeue<T, TBufferSize, TAllocator>::Erase(Iterator first, Iterator last)
    {

    } 

    template <typename T, xsize TBufferSize, typename TAllocator>
    typename Dequeue<T, TBufferSize, TAllocator>::Iterator
        Dequeue<T, TBufferSize, TAllocator>::Insert(Iterator location, const T & value)
    {
        
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void Dequeue<T, TBufferSize, TAllocator>::ReserveIfMapAtBack(xsize nodesToAdd)
    {
        if (nodesToAdd > mMapSize - (mFinish.mNode - mMap + 1))
        {
            OutOfMemorySolve(nodesToAdd, true);
        }
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void Dequeue<T, TBufferSize, TAllocator>::ReserveIfMapAtFront(xsize nodesToAdd)
    {
        if (nodesToAdd > mStart.mNode - mMap)
        {
            OutOfMemorySolve(nodesToAdd, false);
        }
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void Dequeue<T, TBufferSize, TAllocator>::OutOfMemorySolve(xsize nodesToAdd, bool isAddBack)
    {
        // the iterators will be changed after the function, we must update them at the end
        xpointerdifference startDiff = mStart.mCurrent - mStart.mFirst;
        xpointerdifference finishDiff = mFinish.mCurrent - mFinish.mFirst; // this two functions must be written, but the book did not write them
        
        xsize numOldNodes = mFinish.mNode - mStart.mNode + 1;
        xsize numNewNodes = numOldNodes + nodesToAdd;
        T * * newStart = nullptr;
        if (numNewNodes * 2 < mMapSize) // have enough space, just move the data to the center
        {
            newStart = mMap + (mMapSize - numNewNodes) / 2 + isAddBack ? 0 : nodesToAdd;
            if (newStart < mStart.mNode)
            {
                Memory::Copy(mStart.mNode, mFinish.mNode + 1, newStart);
            }
            else 
            {
                Memory::CopyBackward(mStart.mNode, mFinish.mNode + 1, newStart + numOldNodes);
            } 
        }
        else
        {
            xsize newMapSize = mMapSize + Algorithm::GetMax(mMapSize, nodesToAdd) + 2;
            T * * newMap = MapAllocator::Allocate(newMapSize);
            newStart = newMap + (newMapSize - numNewNodes) / 2 + isAddBack ? 0 : nodesToAdd;
            Memory::Copy(mStart.mNode, mFinish.mNode + 1, newStart);
            MapAllocator::Deallocate(mMap, mMapSize);
            mMap = newMap;
            mMapSize = newMapSize;
        }

        mStart.SetNode(newStart);
        mFinish.SetNode(newStart + numOldNodes - 1);

        // This two variables are very imporrtant, I do not know why the book do not metion them.
        mStart.mCurrent = mStart.mFirst + startDiff;
        mFinish.mCurrent = mFinish.mFirst + finishDiff;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void Dequeue<T, TBufferSize, TAllocator>::ReleaseMemory()
    {
        Clear();
    }
}

#endif // XCDEQUEUE_H
