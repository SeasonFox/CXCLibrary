#pragma once

#include <iostream>

#include "../Types/Types.h"
#include "../Memories/Memories.h"
#include "../Iterators/Iterators.h"
#include "../Algorithms/Algorithms.h"

XC_BEGIN_NAMESPACE_1(XC)
{  
    template <typename T, xsize TBufferSize = 5, typename TAllocator = void>
    class DEQueue
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
            typedef Iterators::RandomAccessIteratorTag IteratorCategory;
            typedef T ValueType;
            typedef TPointer Pointer;
            typedef TReference Reference;
            typedef xsize SizeType;
            typedef xptrdiff DifferenceType;

            // The map pointer :
            typedef T * * MapPointer; 

        public:
            BaseIterator() {}
            BaseIterator(const ConstantIterator & rhs) : mCurrent(rhs.mCurrent), mFirst(rhs.mCurrent), mLast(rhs.mLast), mNode(rhs.mNode) {} // use constant BaseIterator to initialize all BaseIterators
            BaseIterator(const Iterator & rhs) : mCurrent(rhs.mCurrent), mFirst(rhs.mFirst), mLast(rhs.mLast), mNode(rhs.mNode) {} // This must write in case of moti definitions
            ~BaseIterator() {}
            Self & operator = (const Self &) = default;

        public:
            Reference operator * () const { return *mCurrent; }
            Pointer operator -> () const { return &(operator * ()); }
            Self operator + (xptrdiff n) const;
            Self operator - (xptrdiff n) const;
            xptrdiff operator - (const Self & rhs) const; // Two Iterators minus together is a pointer difference.
            Reference operator [] (xptrdiff n) const { return *(this + n); } // operator * and opeartor +
            bool operator == (const Self & rhs) const { return mCurrent == rhs.mCurrent; }
            bool operator != (const Self & rhs) const { return !(*this == rhs); }
            bool operator < (const Self & rhs) const { return mNode == rhs.mNode ? mCurrent < rhs.mCurrent : mNode < rhs.mNode; }
            bool operator > (const Self & rhs) const { return !(*this < rhs && *this == rhs); }

            Self & operator ++ ();
            Self & operator -- ();
            Self operator ++ (int);
            Self operator -- (int);
            Self & operator += (xptrdiff n);
            Self & operator -= (xptrdiff n) { return *this += -n; }
            
        public:
            T * mCurrent;
            T * mFirst;
            T * mLast;
            T * * mNode; // Points to the location of the whole map;

        protected:
            xsize GetBufferSize(xsize count = TBufferSize) const { return count == 0 ? 512 : count; } // The same function DEQueue have.
            
            void SetNode(T * * newNode);

            friend class DEQueue<T, TBufferSize, TAllocator>;
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

        typedef DEQueue<T, TBufferSize, TAllocator> Self;

    public:
        DEQueue() { EmptyInitialize(); }
        DEQueue(xsize count, const T & value) { FillInitialize(count, value); }
        DEQueue(const Self & rhs) { CopyWithoutReleaseMemories(rhs); }
        ~DEQueue() { ReleaseMemories(); }
        Self & operator = (const Self &);

        ConstantIterator GetBegin() const { return ConstantIterator(mStart); }
        ConstantIterator GetEnd() const { return ConstantIterator(mFinish); }
        const T & At(xsize index) const { return mStart[index]; }
        const T & operator [] (xsize index) const { return At(index); }
        const T & GetFront() const { return *GetBegin(); }
        const T & GetBack() const { return *(GetEnd() - 1); }
        xsize GetSize() const { return xsize(mFinish - mStart); }
        bool IsEmpty() const { return GetSize() == 0; }

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
        Iterator Erase(Iterator position);
        Iterator Erase(Iterator first, Iterator last);
        Iterator Insert(Iterator position, const T & value);

        // These functions are for C++ 11 :
        ConstantIterator begin() const { return GetBegin(); }
        ConstantIterator end() const { return GetEnd(); }
        
        Iterator begin() { return GetBegin(); }
        Iterator end() { return GetEnd(); }

    protected:
        typedef InsideAllocator<T *, DefaultAllocator<T *> > MapAllocator; // Allocate the whole map
        typedef InsideAllocator<T, DefaultAllocator<T> > DataAllocator; // Allocate element of each node

    protected:
        xsize GetBufferSize() const { return TBufferSize == 0 ? 512 : TBufferSize; }
        xsize GetInitialMapSize() const { return 4; }

        void EmptyCreateMapAndNodes(); // Construct the structure of the map.
        void CreateMapAndNodes(xsize count); // Construct the structure of the map.
        void EmptyInitialize();
        void FillInitialize(xsize count, const T & value);
        T * AllocateNode() { return DataAllocator::Allocate(GetBufferSize()); } // Allocate a node.
        void DeallocateNode(T * node) { DataAllocator::Deallocate(node, GetBufferSize()); }
        T * * AllocateMap() { return MapAllocator::Allocate(mMapSize); }
        void ReserveIfMapAtBack(xsize nodesToAdd = 1);
        void ReserveIfMapAtFront(xsize nodesToAdd = 1);
        void OutOfMemoriesSolve(xsize nodesToAdd, bool isAddBack);
        void ReleaseMemories();
        void CopyWithoutReleaseMemories(const Self & other);

    protected:
        T * * mMap; // Each element of the map contains a pointer of T.
        xsize mMapSize; // The count of pointers in a map.
        Iterator mStart; // The start iterator of the whole dequeue.
        Iterator mFinish; // The finish iterator of the whole dequeue.
    };  

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    inline typename DEQueue<T, TBufferSize, TAllocator>::template BaseIterator<TReference, TPointer>::Self
        DEQueue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator + (xptrdiff n) const
    {
        Self ans = *this;
        return ans += n;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    inline typename DEQueue<T, TBufferSize, TAllocator>::template BaseIterator<TReference, TPointer>::Self
        DEQueue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator - (xptrdiff n) const 
    {
        Self ans = *this;
        return ans += -n;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    inline xptrdiff
        DEQueue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator - (const Self & rhs) const
    {
        return xptrdiff((mNode - rhs.mNode) * xptrdiff(GetBufferSize(TBufferSize))
            + (mCurrent - rhs.mCurrent) + (rhs.mLast - mLast));        
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    typename DEQueue<T, TBufferSize, TAllocator>::template BaseIterator<TReference, TPointer>::Self &
        DEQueue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator ++ ()
    {
        ++mCurrent;
        if (mCurrent == mLast)
        {
            SetNode(mNode + xptrdiff(1));
            mCurrent = mFirst;
        }

        return *this;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    typename DEQueue<T, TBufferSize, TAllocator>::template BaseIterator<TReference, TPointer>::Self &
        DEQueue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator -- ()
    {
        if (mCurrent == mFirst)
        {
            SetNode(mNode - xptrdiff(1));
            mCurrent = mLast - 1;
        }
        else
        {
            --mCurrent;
        }

        return *this;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    inline typename DEQueue<T, TBufferSize, TAllocator>::template BaseIterator<TReference, TPointer>::Self
        DEQueue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator ++ (int) 
    {
        Self ans = *this;
        ++(*this);
        return ans;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    inline typename DEQueue<T, TBufferSize, TAllocator>::template BaseIterator<TReference, TPointer>::Self
        DEQueue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator -- (int)
    {
        Self ans = *this;
        --(*this);
        return ans;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    typename DEQueue<T, TBufferSize, TAllocator>::template BaseIterator<TReference, TPointer>::Self &
        DEQueue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::operator += (xptrdiff n)
    {
        xptrdiff offset = (mCurrent - mFirst) + n;
        if (offset >= 0 && offset < xptrdiff(GetBufferSize(TBufferSize)))
        {
            mCurrent += n;
        }
        else
        {
            xptrdiff nodeOffset = offset > 0 ? 
                offset / xptrdiff(GetBufferSize(TBufferSize)) :
                (-offset - 1) / xptrdiff(GetBufferSize(TBufferSize)) - 1;
            SetNode(mNode + nodeOffset);
            mCurrent = mFirst + (offset - nodeOffset * xptrdiff(GetBufferSize(TBufferSize)));
        }

        return *this;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    template <typename TReference, typename TPointer>
    inline void DEQueue<T, TBufferSize, TAllocator>::BaseIterator<TReference, TPointer>::SetNode(T * * newNode)
    {
        mNode = newNode;
        mFirst = *newNode;
        mLast = mFirst + xptrdiff(GetBufferSize(TBufferSize));
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    inline typename DEQueue<T, TBufferSize, TAllocator>::Self &
    DEQueue<T, TBufferSize, TAllocator>::operator = (const Self & other)
    {
        ReleaseMemories();
        CopyWithoutReleaseMemories(other);
        return *this;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void DEQueue<T, TBufferSize, TAllocator>::PushBack(const T & value)
    {
        if (mFinish.mCurrent != mFinish.mLast - 1) // Should last - 1, make sure there will always be a node at the end.
        {
            Memories::Construct(mFinish.mCurrent, value);
            ++mFinish.mCurrent;
        }
        else // Should jump to next node.
        {
            ReserveIfMapAtBack();
            *(mFinish.mNode + 1) = AllocateNode();
            Memories::Construct(mFinish.mCurrent, value);
            mFinish.SetNode(mFinish.mNode + 1);
            mFinish.mCurrent = mFinish.mFirst;
        }
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void DEQueue<T, TBufferSize, TAllocator>::PopBack()
    {
        if (mFinish.mCurrent != mFinish.mFirst)
        {
            --mFinish.mCurrent;
            Memories::Destroy(mFinish.mCurrent);
        }
        else
        {
            DeallocateNode(mFinish.mFirst);
            mFinish.SetNode(mFinish.mNode - 1);
            mFinish.mCurrent = mFinish.mLast - 1;
            Memories::Destroy(mFinish.mCurrent);
        }
    }

    // PushFront is different from PushBack
    template <typename T, xsize TBufferSize, typename TAllocator>
    void DEQueue<T, TBufferSize, TAllocator>::PushFront(const T & value)
    {
        if (mStart.mCurrent != mStart.mFirst) // do not need to have more free space.
        {
            Memories::Construct(mStart.mCurrent - 1, value);
            --mStart.mCurrent;
        }
        else
        {
            ReserveIfMapAtFront();
            *(mStart.mNode - 1) = AllocateNode();
            mStart.SetNode(mStart.mNode - 1);
            mStart.mCurrent = mStart.mLast - 1;
            Memories::Construct(mStart.mCurrent, value);
        }
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void DEQueue<T, TBufferSize, TAllocator>::PopFront()
    {
        if (mStart.mCurrent != mStart.mLast - 1)
        {
            Memories::Destroy(mStart.mCurrent);
            ++mStart.mCurrent;
        }
        else
        {
            Memories::Destroy(mStart.mCurrent);
            DeallocateNode(mStart.mFirst);
            mStart.SetNode(mStart.mNode + 1);
            mStart.mCurrent = mStart.mFirst;
        }
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void DEQueue<T, TBufferSize, TAllocator>::Clear()
    {
        if (IsEmpty())
        {
            return;
        }
        
        for (T * * node = mStart.mNode + 1; node < mFinish.mNode; ++node)
        {
            Memories::Destroy(*node, *node + GetBufferSize());
            DataAllocator::Deallocate(*node, GetBufferSize());
        }

        if (mStart.mNode != mFinish.mNode)
        {
            Memories::Destroy(mStart.mCurrent, mStart.mLast);
            Memories::Destroy(mFinish.mFirst, mFinish.mCurrent);
            DataAllocator::Deallocate(mFinish.mFirst, GetBufferSize());
        }
        else
        {
            Memories::Destroy(mStart.mCurrent, mFinish.mCurrent);
        }

        mFinish = mStart; // Two iterators equal.
    }    

    template <typename T, xsize TBufferSize, typename TAllocator>
    typename DEQueue<T, TBufferSize, TAllocator>::Iterator 
        DEQueue<T, TBufferSize, TAllocator>::Erase(Iterator position)
    {
        Iterator next = position;
        ++next;
        xsize index = position - mStart;
        if (index < GetSize() / 2)
        {
            Memories::CopyBackward(mStart, position, next);
            PopFront();
        } 
        else
        {
            Memories::Copy(next, mFinish, position);
            PopBack();
        }

        return mStart + index;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    typename DEQueue<T, TBufferSize, TAllocator>::Iterator 
        DEQueue<T, TBufferSize, TAllocator>::Erase(Iterator first, Iterator last)
    {
        if (first == mStart && last == mFinish)
        {
            Clear();
            return mFinish;
        }
        else
        {
            xptrdiff n = first - last;
            xptrdiff elemsBefore = first - mStart;
            if (elemsBefore < GetSize() / 2) // Front have fewer elements, should move front.
            {
                Memories::CopyBackward(mStart, first, last);
                Iterator newStart = mStart + n;
                Memories::Destroy(mStart, newStart);
                for (T * * cur = mStart.mNode; cur < newStart.mNode; ++cur)
                {
                    DataAllocator::Deallocate(*cur, GetBufferSize());
                }
                mStart = newStart;
            }
            else // Back have    fewer elements, should move after.
            {
                Memories::Copy(last, mFinish, first);
                Iterator newFinish = mFinish - n;
                Memories::Destroy(newFinish, mFinish);
                for (T * * cur = newFinish.mNode + 1; cur <= mFinish.mNode; ++cur)
                {
                    DataAllocator::Deallocate(*cur, GetBufferSize());
                }
                mFinish = newFinish;
            }

            return mStart + elemsBefore;
        }
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    typename DEQueue<T, TBufferSize, TAllocator>::Iterator
        DEQueue<T, TBufferSize, TAllocator>::Insert(Iterator position, const T & value)
    {
        if (position == mStart)
        {
            PushFront(value);
            return mStart;
        }
        else if (position == mFinish)
        {
            PushBack(value);
            return mFinish - 1;
        }
        else
        {
            xsize index = position - mStart; // Answer index.
            Iterator ans = mStart;
            if (index < GetSize() / 2) // Front have fewer elements.
            {
                PushFront(GetFront());
                Memories::Copy(mStart + 2, mStart + index + 1, mStart + 1);
                ans = mStart + index; // must update
            }
            else // Back have fewer elements.
            {
                PushBack(GetBack());
                Memories::CopyBackward(mStart + index, mFinish - 2, mFinish - 1);
                ans = mStart + index; // must update
            }

            *ans = value;
            return ans;
        }
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void DEQueue<T, TBufferSize, TAllocator>::EmptyCreateMapAndNodes()
    {
        mMapSize = GetInitialMapSize();
        mMap = MapAllocator::Allocate(mMapSize);
        T * * nodeStart = mMap + mMapSize / 2;
        *nodeStart = AllocateNode();
        mStart.SetNode(nodeStart);
        mStart.mCurrent = mStart.mFirst;
        mFinish.SetNode(nodeStart);
        mFinish.mCurrent = mFinish.mFirst;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void DEQueue<T, TBufferSize, TAllocator>::CreateMapAndNodes(xsize count) // Count is the count of the elements of the DEQueue.
    {
        // std::cout << "CreateMapAndNodes\n";
        xsize numNodes = count / GetBufferSize() + 1; // Include finish node.
        // std::cout << numNodes << std::endl;
        mMapSize = Algorithms::GetMax(numNodes, GetInitialMapSize()) + 2; // Plus 2 because front and back free space.   
        // std::cout << mMapSize << std::endl;
        mMap = MapAllocator::Allocate(mMapSize);
        T * * nodeStart = mMap + (mMapSize - numNodes) / 2;
        T * * nodeFinish = nodeStart + numNodes - 1;
        
        // std::cout << "CreateMapAndNodes\n";
        for (T * * cur = nodeStart; cur <= nodeFinish; ++cur)
        {
            *cur = AllocateNode();
        }
        
        // std::cout << "CreateMapAndNodes\n";
        mStart.SetNode(nodeStart);
        mStart.mCurrent = mStart.mFirst;
        mFinish.SetNode(nodeFinish);
        mFinish.mCurrent = mFinish.mFirst + count % GetBufferSize();
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void DEQueue<T, TBufferSize, TAllocator>::EmptyInitialize()
    {
        EmptyCreateMapAndNodes();
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void DEQueue<T, TBufferSize, TAllocator>::FillInitialize(xsize count, const T & value)
    {
        CreateMapAndNodes(count);
        for (T * * cur = mStart.mNode; cur < mFinish.mNode; ++cur)	
        {
            Memories::UninitializedFillN(*cur, GetBufferSize(), value);
        }

        Memories::UninitializedFill(mFinish.mFirst, mFinish.mCurrent, value); // Not all the last should be initialized.
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void DEQueue<T, TBufferSize, TAllocator>::ReserveIfMapAtBack(xsize nodesToAdd)
    {
        if (nodesToAdd > mMapSize - (mFinish.mNode - mMap + 1))
        {
            OutOfMemoriesSolve(nodesToAdd, true);
        }
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void DEQueue<T, TBufferSize, TAllocator>::ReserveIfMapAtFront(xsize nodesToAdd)
    {
        if (nodesToAdd > xsize(mStart.mNode - mMap))
        {
            OutOfMemoriesSolve(nodesToAdd, false);
        }
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void DEQueue<T, TBufferSize, TAllocator>::OutOfMemoriesSolve(xsize nodesToAdd, bool isAddBack)
    {
        // the iterators will be changed after the function, we must update them at the end
        xptrdiff startDiff = mStart.mCurrent - mStart.mFirst;
        xptrdiff finishDiff = mFinish.mCurrent - mFinish.mFirst; // this two functions must be written, but the book did not write them
        
        xsize numOldNodes = mFinish.mNode - mStart.mNode + 1;
        xsize numNewNodes = numOldNodes + nodesToAdd;
        T * * newStart = nullptr;
        if (numNewNodes * 2 < mMapSize) // have enough space, just move the data to the center
        {
            newStart = mMap + (mMapSize - numNewNodes) / 2 + (isAddBack ? 0 : nodesToAdd);
            if (newStart < mStart.mNode)
            {
                Memories::Copy(mStart.mNode, mFinish.mNode + 1, newStart);
            }
            else 
            {
                Memories::CopyBackward(mStart.mNode, mFinish.mNode + 1, newStart + numOldNodes);
            } 
        }
        else
        {
            xsize newMapSize = mMapSize + Algorithms::GetMax(mMapSize, nodesToAdd) + 2;
            T * * newMap = MapAllocator::Allocate(newMapSize);
            newStart = newMap + (newMapSize - numNewNodes) / 2 + (isAddBack ? 0 : nodesToAdd);
            Memories::Copy(mStart.mNode, mFinish.mNode + 1, newStart);
            MapAllocator::Deallocate(mMap, mMapSize);
            mMap = newMap;
            mMapSize = newMapSize; // std::cout << mMapSize << std::endl;
        }

        mStart.SetNode(newStart);
        mFinish.SetNode(newStart + numOldNodes - 1);

        // This two variables are very imporrtant, I do not know why the book do not metion them.
        mStart.mCurrent = mStart.mFirst + startDiff;
        mFinish.mCurrent = mFinish.mFirst + finishDiff;
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    inline void DEQueue<T, TBufferSize, TAllocator>::ReleaseMemories()
    {
        Clear();
        DataAllocator::Deallocate(mStart.mFirst, GetBufferSize());
        MapAllocator::Deallocate(mMap, mMapSize);
    }

    template <typename T, xsize TBufferSize, typename TAllocator>
    void DEQueue<T, TBufferSize, TAllocator>::CopyWithoutReleaseMemories(const Self & other)
    {
        mMapSize = other.mMapSize;
        mMap = MapAllocator::Allocate(mMapSize);
 
        // Have not alloate, should allocate first.       
        T * * startNode = mMap + xptrdiff(other.mStart.mNode - other.mMap);
        T * * finishNode = mMap + xptrdiff(other.mFinish.mNode - other.mMap);

        // std::cout << "haha" << std::endl;
        for (T * * cur = startNode; cur <= finishNode; ++cur)
        {
            // std::cout << "new" << std::endl;
            *cur = AllocateNode();
        }

        // std::cout << "haha" << std::endl;
        // Allocated, now can initialize iterators.
        mStart.SetNode(startNode);
        mFinish.SetNode(finishNode); // std::cout << "haha" << std::endl;
        mStart.mCurrent = mStart.mFirst + xsize(other.mStart.mCurrent - other.mStart.mFirst);
        mFinish.mCurrent = mFinish.mFirst + xsize(other.mFinish.mCurrent - other.mFinish.mFirst);

        // std::cout << "copy" << std::endl;
        // Copy
        Memories::UninitializedCopy(other.mStart, other.mFinish, mStart);
    }

} XC_END_NAMESPACE_1
