#pragma once

#include "../SyntaxSugars/SyntaxSugars.h"
#include "../Iterators/Iterators.h"
#include "../Memories/Allocators.h"
#include "Pair.h"

XC_BEGIN_NAMESPACE_3(XC, Containers, Details)
{
    enum class RBTreeColorType : bool
    {
        Red = false, Black = true,
    };

    class RBTreeNodeBase
    {
    public:
        using ColorType = RBTreeColorType;
        using BasePointer = RBTreeNodeBase *;

    public:
        RBTreeNodeBase * GetMinimum()
        {
            RBTreeNodeBase * node = this;
            while (node->mLeft != nullptr)
            {
                node = node->mLeft;
            }

            return node;
        }

        RBTreeNodeBase * GetMaximum()
        {
            RBTreeNodeBase * node = this;
            while (node->mRight != nullptr)
            {
                node = node->mRight;
            }

            return node;
        }

    public:
        RBTreeNodeBase * mParent;
        RBTreeNodeBase * mLeft;
        RBTreeNodeBase * mRight;
        RBTreeColorType mColor;
    };

    template <typename T>
    class RBTreeNode : public RBTreeNodeBase
    {
    public:
        using LinkType = RBTreeNode<T> *;

    public:
        T mValue;
    };

    class RBTreeBaseIterator
    {
    public:
        using IteratorCategory = Iterators::BidirectionalIteratorTag;
        using DifferenceType = xptrdiff;

    public:
        void Increment()
        {
            if (mNode->mRight != nullptr)
            {
                mNode = mNode->mRight;
                while (mNode->mLeft != nullptr)
                {
                    mNode = mNode->mLeft;
                }
            }
            else
            {
                RBTreeNodeBase * parent = mNode->mParent;
                while (mNode == parent->mRight)
                {
                    mNode = parent;
                    parent = parent->mParent;
                }

                if (mNode->mRight != parent) // header
                {
                    mNode = parent;
                }
            }
        }

        void Decrement()
        {
            if (mNode->mColor == RBTreeColorType::Red && mNode->mParent->mParent == mNode)
            {
                mNode = mNode->mRight; // header 
            }
            else if (mNode->mLeft != nullptr)
            {
                RBTreeNodeBase * y = mNode->mLeft;
                while (y->mRight != nullptr)
                {
                    y = y->mRight;
                }
                mNode = y;
            }
            else
            {
                RBTreeNodeBase * y = mNode->mParent;
                while (mNode == y->mLeft)
                {
                    mNode = y;
                    y = y->mParent;
                }
                mNode = y;
            }
        }

    public:
        RBTreeNodeBase * mNode;
    };

    template <typename T, typename TReference, typename TPointer>
    class RBTreeIterator : public RBTreeBaseIterator
    {
    public:
        using ValueType = T;
        using Reference = TReference;
        using Pointer = TPointer;
        using Self = RBTreeIterator<T, TReference, TPointer>;
        using Iterator = RBTreeIterator<T, T &, T *>;
        using ConstantIterator = RBTreeIterator<T, const T &, const T *>;
        using LinkType = RBTreeNode<T> *;

    public:
        RBTreeIterator() = default;

        RBTreeIterator(RBTreeNode<T> * node)
        {
            mNode = node;
        }

        RBTreeIterator(const Iterator & iterator)
        {
            mNode = iterator.mNode;
        }

    public:
        Reference operator * () const
        {
            return mNode->mValue;
        }

        Pointer operator -> () const
        {
            return &(operator *());
        }

        Self & operator ++ ()
        {
            Increment();
            return *this;
        }

        Self & operator ++ (int)
        {
            Self ans = *this;
            ++(*this);
            return ans;
        }

        Self & operator -- ()
        {
            Decrement();
            return *this;
        }

        Self & operator -- (int)
        {
            Self ans = *this;
            --(*this);
            return ans;
        }

    public:
        RBTreeNode<T> * mNode;
    };

    template <typename TKey, typename TValue, typename TKeyOfValue, typename TCompare, typename TAllocator = XC::DefaultAllocator<TValue> >
    class RBTree // TKeyOfValue is a functor
    {
    public:
        using KeyType = TKey;
        using ValueType = TValue;
        using Pointer = TValue *;
        using ConstanctPointer = const ValueType *;
        using Reference = ValueType &;
        using ConstantReference = const ValueType &;
        using SizeType = xsize;
        using DifferenceType = xptrdiff;
        using VoidPointer = void *;
        using Node = RBTreeNode<ValueType>;
        using LinkType = Node *;
        using Iterator = RBTreeIterator<ValueType, Reference, Pointer>;
        using Self = RBTree<TKey, TValue, TKeyOfValue, TCompare, TAllocator>;

        // allocators :
        using RBTreeNodeAllocator = DefaultAllocator<Node>;

    public:
        RBTree(const TCompare & compare = Compare()) :
            mCountNodes(0), mKeyCompare(compare)
        {
            Initialize();
        }

        RBTree(const typename Self&) = delete;

        virtual ~RBTree()
        {
            Clear();
            PutNode(mHeader);
        }

        Self& operator = (const Self & rhs)
        {
            return *this;
        }

    protected:
        Node* GetNode()
        {
            return RBTreeNodeAllocator::Allocate();
        }

        Node* PutNode(Node* node)
        {
            return RBTreeNodeAllocator::Deallocate(node);
        }

        Node* CreateNode(const TValue & value)
        {
            LinkType ans = GetNode();
            Memories::Construct(&ans->mValue, value);
            return ans;
        }

        Node* CloneNode(Node* node)
        {
            Node* ans = CreateNode(node->mValue);
            ans->mColor = node->mColor;
            ans->mLeft = nullptr;
            ans->mRight = nullptr;
            return ans;
        }

        void DestroyNode(Node* node)
        {
            Memories::Destroy(&node->mValue);
            PutNode(node);
        }

        // node functions, static like
        TValue& GetValue(Node* node) const
        {
            return node->mValue;
        }

        Node* & GetLeft(Node* node) const
        {
            return node->mLeft;
        }

        Node* & GetRight(Node* node) const
        {
            return node->mRight;
        }

        TKey & GetKey(Node* node) const
        {
            return TKeyOfValue()(node->mValue);
        }

        RBTreeColorType& GetColor(Node* node) const
        {
            return node->mColor;
        }

        // global functions
        Node* & GetRoot() const
        {
            return mHeader->mParent;
        }

        Node* & GetMostLeft() const
        {
            return mHeader->mLeft;
        }

        Node* & GetMostRight() const
        {
            return mHeader->mRight;
        }

        Node* GetMaximum(Node* node) const
        {
            return node->GetMaximum();
        }

        Node* GetMinimum(Node* node) const
        {
            return node->GetMinimum();
        }

    private:
        Iterator Insert(RBTreeNodeBase * x, RBTreeNodeBase * y, const ValueType & value)
        {
            return Iterator();
        }

        void Initialize()
        {
            mHeader = GetNode();
            mHeader->mColor = RBTreeColorType::Red;
            GetRoot() = nullptr;
            GetMostLeft() = mHeader;
            GetMostRight() = mHeader;
        }

        // insert functions
        Iterator InsertEqual(const TValue& value)
        {
            Node* y = mHeader;
            Node* x = GetRoot();
            while (x != nullptr)
            {
                y = x;
                x = mKeyCompare(TKeyOfValue()(value), GetKey(x)) ? GetLeft(x) : GetRight(x);
            }
            return Insert(x, y, value);
        }

        // bool claims if it is inserted success
        Pair<Iterator, bool> InsertUnique(const TValue& value)
        {
            Node* y = mHeader;
            Node* x = GetRoot();
            bool camp = true;
            while (x != nullptr)
            {
                y = x;
                comp = mKeyCompare(TKeyOfValue()(value), GetKey(x));
                x = comp ? GetLeft(x) : GetRight(x);
            }

            Iterator j = Iterator(y);
            if (comp)
            {
                if (j == GetBegin())
                {
                    return Insert(x, y, value);
                }
                else
                {
                    --j;
                }
            }
            if (mKeyCompare(GetKey(j.mNode), TKeyOfValue()(value)))
            {
                return Pair<Iterator, bool>(Insert(x, y, value), true);
            }
            else
            {
                return Pair<Iterator, bool>(j, false);
            }
        }

        typename Iterator Insert(Node* x, Node* y, const TValue& value)
        {
            Node* z = nullptr;
            if (y == mHeader || x != nullptr || mKeyCompare(TKeyOfValue()(value), GetKey(y)))
            {
                z = CreateNode(value);
                GetLeft(y) = x;
                if (y == mHeader)
                {
                    GetRoot() = z;
                    GetMostRight() = z;
                }
                else if (y == GetMostLeft())
                {
                    GetMostLeft() = z;
                }
            }
            else
            {
                z = CreateNode(value);
                GetRight(y) = z;
                if (y == GetMostRight())
                {
                    GetMostRight() = z;
                }
            }

            GetParent(z) = y;
            GetLeft(z) = nullptr;
            GetRight(z) = nullptr;

            Rebalance(z, mHeader->mParent);
            ++mCountNodes;
            return Iterator(z);
        }

        void Rebalance(Node* x, Node* & root)
        {
            x->mColor = RBTreeColorType::Red;
            while (x != root && x->mParent->mColor == RBTreeColorType::Red)
            {
                if (x->mParent == x->mParent->mParent->mLeft)
                {
                    Node* y = x->mParent->mParent->mRight;
                    if (y != nullptr && y->mColor == RBTreeColorType::Red)
                    {
                        x->mParent->mColor = RBTreeColorType::Black;
                        y->mColor = RBTreeColorType::Black;
                        x->mParent->mParent->mColor = RBTreeColorType::Red;
                        x = x->mParent->mParent;
                    }
                    else
                    {
                        if (x == x->mParent->mRight)
                        {
                            x = x->mParent;
                            LeftRotate(x, root);
                        }
                        x->mParent->mColor = RBTreeColorType::Black;
                        x->mParent->mParent->mColor = RBTreeColorType::Red;
                        RightRotate(x->mParent->mParent, root);
                    }
                }
                else
                {
                    Node* y = x->mParent->mParent->mLeft;
                    if (y != nullptr && y->mColor == RBTreeColorType::Red)
                    {
                        x->mParent->mColor = RBTreeColorType::Black;
                        y->mColor = RBTreeColorType::Black;
                        x->mParent->mParent->mColor = RBTreeColorType::Red;
                        x = x->mParent->mParent;
                    }
                    else
                    {
                        if (x == x->mParent->mLeft)
                        {
                            x = x->mParent;
                            RightRotate(x, root);
                        }
                        x->mParent->mColor = RBTreeColorType::Black;
                        x->mParent->mParent->mColor = RBTreeColorType::Red;
                        LeftRotate(x->mParent->mParent, root);
                    }
                }
            }
            root->mColor = RBTreeColorType::Black;
        }

        void LeftRotate(Node* x, Node* & root)
        {
            Node* y = x->mRight;
            x->mRight = y->mLeft;
            if (y->mLeft != nullptr)
            {
                y->mLeft->mParent = x;
            }
            y->mParent = x->mParent;
            if (x == root)
            {
                root = y;
            }
            else if (x == x->mParent->mLeft)
            {
                x->mParent->mLeft = y;
            }
            else
            {
                x->mParent->mRight = y;
            }
            y->mLeft = x;
            x->mParent = y;

        }

        void RightRotate(Node* x, Node* root)
        {
            Node* y = x->mLeft;
            x->mLeft = y->mRight;
            if (x->mRight != nullptr)
            {
                y->mRight->mParent = x;
            }
            y->mParent = x->mParent;
            if (x == root)
            {
                root = y;
            }
            else if (x == x->mParent->mRight)
            {
                x->mParent->mRight = y;
            }
            else
            {
                x->mParent->mLeft = y;
            }
        }

    private:
        xsize mCountNodes;
        LinkType mHeader;
        TCompare mKeyCompare;
    };

} XC_END_NAMESPACE_3;

XC_BEGIN_NAMESPACE_1(XC_RBTREE_TEST)
{
    using namespace XC::Containers::Details;

    class Element
    {
    public:
        using Value = int;
        using Key = int;

        int mValue;
        int mKey;
    };

    class KeyOfValue
    {
    public:
        int operator () (const Element & value)
        {
            return value.mKey;
        }
    };

    class Compare
    {
    public:
        bool operator () (const Element& lhs, const Element& rhs) const
        {
            return lhs.mValue < rhs.mValue;
        }
    };
    XC_TEST_CASE(FSGEIWEG)
    {
        std::cout << std::endl;
        std::cout << "Begin RBTree test" << std::endl;
       
        RBTree<int, Element, KeyOfValue, Compare> tree(Compare());

        std::cout << "end RBTree test" << std::endl;
    }
};