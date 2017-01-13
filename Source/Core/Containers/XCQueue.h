#ifndef XCQUEUE_H
#define XCQUEUE_H

#include "XCDequeue.h"

namespace XC
{
	template <typename T, typename TContainer = Dequeue<T> >
	class Queue
	{
	public:
		typedef Queue<T, TContainer> Self;
		typedef TContainer::SizeType SizeType;

	public:
		Queue() = default;
		Queue(const Self & rhs) : mContanier(rhs.mContainer) {}
		~Queue() = default;
		Self & operator = (const Self & rhs) { mContainer = rhs; }

		const T & GetFront() const { return mContainer.GetFront(); }
		SizeType GetSize() const { return mContainer.GetSize(); }
		bool IsEmpty() const { return mContainer.IsEmpty(); }
		bool operator == (const Self & rhs) const { return mContainer == rhs.mContainer; }

		T & GetFront() { return mContainer.GetFront(); }
		void Push(const T & value) { mContainer.PushBack(value); }
		void Pop() { mContainer.PopFront(); }
		void Clear() { mContainer.Clear(); }
	
	private:
		TContainer mContainer;
	};
}

#endif // XCQUEUE_H