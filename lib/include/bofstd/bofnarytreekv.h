/*
* Copyright (c) 2022-2032, Sci. All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module defines routines for creating and managing a trre structure
*
* Name:        boftree.h
* Author:      Bernard HARMEL: onbings@dscloud.me
* Revision:    1.0
*
* Rem:         https://www.codeproject.com/Articles/12822/A-Tree-Template-Class-in-C
*              https://technoteshelp.com/c-creating-a-n-ary-tree/
*              https://www.geeksforgeeks.org/list-of-nodes-of-given-n-ary-tree-with-number-of-children-in-range-0-n/
*              https://www.geeksforgeeks.org/print-n-ary-tree-graphically/
*              https://stackoverflow.com/questions/9414290/how-to-delete-a-nary-tree-each-node-has-a-parent-pointer-too-in-it
*
*
* History:
*
* V 1.00  Aug 27 2022  BHA : Initial release
*/

#pragma once

#include <bofstd/bofsystem.h>
#include <list>
#include <algorithm>
#include <string>
#include <ostream>

BEGIN_BOF_NAMESPACE()

#define  BOF_NARY_TREE_KV_LOCK(Sts)   {Sts=mNaryTreeKvParam_X.MultiThreadAware_B ? Bof_LockMutex(mTreeMtx_X):BOF_ERR_NO_ERROR;}
#define  BOF_NARY_TREE_KV_UNLOCK()    {if (mNaryTreeKvParam_X.MultiThreadAware_B) Bof_UnlockMutex(mTreeMtx_X);}
constexpr uint32_t BOF_NARY_TREE_KV_NODE_MAGIC_NUMBER = 0xBA1CD158;

struct BOFSTD_EXPORT BOF_NARY_TREE_KV_PARAM
{
  bool     MultiThreadAware_B;                                      /*! true if the object is used in a multi threaded application (use mTreeMtx_X)*/

  BOF_NARY_TREE_KV_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    MultiThreadAware_B = false;
  }
};

/*!
* Summary
* n-ary tree
*
* Description
* This class manages a n-ary tree instance
*
* See Also
* None
*/

template<typename KeyType, typename DataType>
class BofNaryTreeKv
{
private:
  class Node
  {
  private:
    const uint32_t mMagicNumber_U32 = BOF_NARY_TREE_KV_NODE_MAGIC_NUMBER;
    uint32_t            mDepth_U32;
    KeyType			        mKey;
    DataType            mValue;
    std::list<Node *>   mChildCollection;  //better than vector for iterator as it is not invalidated when we add/remove eleme

  public:
    Node(const KeyType &_rKey, const DataType &_rData);
    ~Node();

    uint32_t MagicNumber() const;
    const KeyType &Key() const;
    const DataType &Value() const;
    const std::list<Node *> &ChildCollection() const;
    BOFERR AddChild(Node *_pNode);
  };

private:
  BOF_NARY_TREE_KV_PARAM mNaryTreeKvParam_X;
  BOF_MUTEX      mTreeMtx_X;                            /*! Provide a serialized access to shared resources in a multi threaded environment*/
  BOFERR         mErrorCode_E;
  Node           *mpRoot = nullptr;		// the root node of the tree
  uint32_t            mMaxDepth_U32 = 0;
public:
  template<typename KeyType, typename DataType>
  using BofNaryTreeKvNode = typename BofNaryTreeKv<KeyType, DataType>::Node *;
  typedef void *BofNaryTreeKvNodeHandle;

  BofNaryTreeKv(const BOF_NARY_TREE_KV_PARAM &_rNaryTreeKvParam_X);
  virtual ~BofNaryTreeKv();

  BofNaryTreeKv &operator=(const BofNaryTreeKv &) = delete; // Disallow copying
  BofNaryTreeKv(const BofNaryTreeKv &) = delete;

  BOFERR LastErrorCode() const { return mErrorCode_E; }
  BOFERR LockTree();
  BOFERR UnlockTree();

  bool IsNodeValid(const BofNaryTreeKvNodeHandle _NodeHandle) const;

  BOFERR SetRoot(const KeyType &_rKey, const DataType &_rData, BofNaryTreeKvNodeHandle *_pRootHandle);
  BOFERR AddChild(const BofNaryTreeKvNodeHandle _ParentHandle, const KeyType &_rKey, const DataType &_rData, BofNaryTreeKvNodeHandle *_pChildHandle);
  const std::string ToString(const BofNaryTreeKvNodeHandle _ParentHandle);    //No const (mutex)

private:
  void ClearTree(const BofNaryTreeKvNodeHandle _NodeHandle);
  void BrowseTree(const BofNaryTreeKvNodeHandle _NodeHandle) const;
  void BrowseTree(const BofNaryTreeKvNodeHandle _NodeHandle, std::ostringstream &_rToString, std::vector<bool> &_rNodeVisitedCollection, uint32_t _Depth_U32 = 0, bool _IsLast_B = false) const;
};

//--- BofNaryTreeKv<KeyType, DataType>::Node Class --------------------------------------------------------------------

template<typename KeyType, typename DataType>
BofNaryTreeKv<KeyType, DataType>::Node::Node(const KeyType &_rKey, const DataType &_rData) : mKey(_rKey), mValue(_rData)
{
  std::cout << "Create " << this << " Key '" << this->Key() << "' Value " << this->Value() << std::endl;
}

template<typename KeyType, typename DataType>
BofNaryTreeKv<KeyType, DataType>::Node::~Node()
{
  BofNaryTreeKv<KeyType, DataType>::Node *pNode;

  while (!mChildCollection.empty())
  {
    pNode = mChildCollection.front();
    std::cout << "Delete " << pNode << " Key '" << pNode->Key() << "' Value " << pNode->Value() << std::endl;
    BOF_SAFE_DELETE(pNode);
    mChildCollection.pop_front();
  }
}

template<typename KeyType, typename DataType>
uint32_t BofNaryTreeKv<KeyType, DataType>::Node::MagicNumber() const
{
  return mMagicNumber_U32;
}

template<typename KeyType, typename DataType>
const KeyType &BofNaryTreeKv<KeyType, DataType>::Node::Key() const
{
  return mKey;
}

template<typename KeyType, typename DataType>
const DataType &BofNaryTreeKv<KeyType, DataType>::Node::Value() const
{
  return mValue;
}

template<typename KeyType, typename DataType>
typename const std::list<typename BofNaryTreeKv<KeyType, DataType>::Node *> &BofNaryTreeKv<KeyType, DataType>::Node::ChildCollection() const
{
  return (mChildCollection);
}

template<typename KeyType, typename DataType>
BOFERR BofNaryTreeKv<KeyType, DataType>::Node::AddChild(Node *_pNode)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  if (_pNode)
  {
    Rts_E = BOF_ERR_DUPLICATE;

    auto It = std::find(mChildCollection.begin(), mChildCollection.end(), _pNode);
    if (It == mChildCollection.end())
    {
      mChildCollection.push_back(_pNode);
      Rts_E = BOF_ERR_NO_ERROR;
    }
  }
  return Rts_E;
}

//--- BofNaryTreeKv<KeyType, DataType>: class --------------------------------------------------------------------
template<typename KeyType, typename DataType>
BofNaryTreeKv<KeyType, DataType>::BofNaryTreeKv(const BOF_NARY_TREE_KV_PARAM &_rNaryTreeKvParam_X)
{
  mNaryTreeKvParam_X = _rNaryTreeKvParam_X;
  mpRoot = nullptr;
  mErrorCode_E = _rNaryTreeKvParam_X.MultiThreadAware_B ? Bof_CreateMutex("BofNaryTreeKv", true, true, mTreeMtx_X) : BOF_ERR_NO_ERROR;
  if (mErrorCode_E == BOF_ERR_NO_ERROR)
  {
  }
}

template<typename KeyType, typename DataType>
BofNaryTreeKv<KeyType, DataType>::~BofNaryTreeKv()
{
  ClearTree(mpRoot);
  Bof_DestroyMutex(mTreeMtx_X);
}

template<typename KeyType, typename DataType>
BOFERR BofNaryTreeKv<KeyType, DataType>::LockTree()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mNaryTreeKvParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_LockMutex(mTreeMtx_X);
  }
  return Rts_E;
}

template<typename KeyType, typename DataType>
BOFERR BofNaryTreeKv<KeyType, DataType>::UnlockTree()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mNaryTreeKvParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_UnlockMutex(mTreeMtx_X);
  }
  return Rts_E;
}


template<typename KeyType, typename DataType>
bool BofNaryTreeKv<KeyType, DataType>::IsNodeValid(const BofNaryTreeKvNodeHandle _NodeHandle) const
{
  bool Rts_B = false;
  const Node *pNode = reinterpret_cast<Node *>(_NodeHandle);
  if (pNode)
  {
    Rts_B = (pNode->MagicNumber() == BOF_NARY_TREE_KV_NODE_MAGIC_NUMBER);
  }
  return Rts_B;
}

template<typename KeyType, typename DataType>
BOFERR BofNaryTreeKv<KeyType, DataType>::SetRoot(const KeyType &_rKey, const DataType &_rData, BofNaryTreeKvNodeHandle *_pRootHandle)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if (_pRootHandle)
  {
    Rts_E = BOF_ERR_NO_ERROR;

    if (mpRoot)
    {
      ClearTree(mpRoot);
    }
    mpRoot = new Node(_rKey, _rData);
    if (mpRoot)
    {
    }
    else
    {
      Rts_E = BOF_ERR_ENOMEM;
    }

    *_pRootHandle = mpRoot;
  }
  return Rts_E;
}

template<typename KeyType, typename DataType>
BOFERR BofNaryTreeKv<KeyType, DataType>::AddChild(const BofNaryTreeKvNodeHandle _ParentHandle, const KeyType &_rKey, const DataType &_rData, BofNaryTreeKvNodeHandle *_pChildHandle)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  Node *pNode, *pParent;

  if ((IsNodeValid(_ParentHandle)) && (_pChildHandle))
  {
    *_pChildHandle = nullptr;
    BOF_NARY_TREE_KV_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = BOF_ERR_ENOMEM;
      pParent = reinterpret_cast<Node *>(_ParentHandle);
      pNode = new Node(_rKey, _rData);
      if (pNode)
      {
        Rts_E = pParent->AddChild(pNode);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          *_pChildHandle = pNode;
        }
      }
      BOF_NARY_TREE_KV_UNLOCK();
    }
  }
  return Rts_E;
}

template<typename KeyType, typename DataType>
void BofNaryTreeKv<KeyType, DataType>::BrowseTree(const BofNaryTreeKvNodeHandle _NodeHandle) const
{
  uint32_t i_U32 = 0;
  const Node *pNode;
  Node *pChildNode = nullptr;

  if (IsNodeValid(_NodeHandle))
  {
    pNode = reinterpret_cast<const Node *>(_NodeHandle);
    //std::cout << "  " << pNode->Data();
    // iterating the child of given node
    while (i_U32 < pNode->NumberOfChild())
    {
      BofNaryTreeKv<KeyType, DataType>::Node::mpParent = nullptr;
      //Node::mChildCollection;
     //pChildNode = pNode->child.at(i);
      this->BrowseTree(pChildNode);
      i_U32++;
    }
  }
}
template<typename KeyType, typename DataType>
void BofNaryTreeKv<KeyType, DataType>::ClearTree(const BofNaryTreeKvNodeHandle _NodeHandle)
{
  const Node *pStartNode = reinterpret_cast<const Node *>(_NodeHandle), *pNode;
  BOFERR Sts_E;
  uint32_t NbChild_U32;

  if (IsNodeValid(_NodeHandle))
  {
    BOF_NARY_TREE_KV_LOCK(Sts_E);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      while (pStartNode) 
      {
// if this node has any children, start by "descending" to the highest numbered child and kill that first.
          if (pStartNode->nchild--) 
          {
            pStartNode = pStartNode->child[pStartNode->nchild];
            continue;
          }
// When we arrive here, *pStartNode has no more children left, so kill it, and step up to its parent
          pNode = node->parent;
// if pStartNode->child was obtained via malloc() uncomment next line
// free (pStartNode->child);
          free(pStartNode);
          pStartNode = pNode;
        }
      }
      BOF_NARY_TREE_KV_UNLOCK();
    }
  }
}
/*
Approach: The idea is to traverse the N-ary Tree using DFS Traversal to traverse the nodes and explore its children nodes until all the nodes are visited
and then similarly, traverse the sibling nodes.

- Initialize a variable to store the current depth of the node, for the root node the depth is 0.
- Declare a boolean array to store the current exploring depths and initially mark all of them to False.
- If the current node is a root node that is the depth of the node is 0, then simply print the data of the node.
- Otherwise, Iterate over a loop from 1 to the current depth of node and store, ‘|’ and three spaces for each of the exploring depth and for non-exploring
  depth print three spaces only.
- Store the current value of the node and move the output pointer to the next line.
- If the current node is the last node of that depth then mark that depth as non-exploring.
- Similarly, explore all the child nodes with the recursive call.
*/
template<typename KeyType, typename DataType>
void BofNaryTreeKv<KeyType, DataType>::BrowseTree(const BofNaryTreeKvNodeHandle _NodeHandle, std::ostringstream &_rToString, std::vector<bool> &_rNodeVisitedCollection, uint32_t _Depth_U32 = 0, bool _IsLast_B = false) const
{
  uint32_t i_U32, Index_U32;

  const Node *pNode = reinterpret_cast<const Node *>(_NodeHandle);

  std::cout << "Node " << pNode << " val " << pNode->Key() << " Depth " << _Depth_U32 << " Visited " <<
    _rNodeVisitedCollection[0] << " " << _rNodeVisitedCollection[1] << " " << _rNodeVisitedCollection[2] << " " << _rNodeVisitedCollection[3] <<
    " " << _rNodeVisitedCollection[4] << " " << _rNodeVisitedCollection[5] << " " << _rNodeVisitedCollection[6] << " " << _rNodeVisitedCollection[7] << std::endl;
  if (pNode)
  {
    // Loop to print the depths of the current node
    for (i_U32 = 1; i_U32 < _Depth_U32; i_U32++)
    {
      std::cout << "  Loop " << i_U32 << "/" << _Depth_U32 << " Visited " << _rNodeVisitedCollection[i_U32] << " Last " << _IsLast_B << std::endl;

      // Condition when the depth is exploring
      if (_rNodeVisitedCollection[i_U32] == true)
      {
        _rToString << "|    ";
      }
      // Otherwise print the blank spaces
      else
      {
        _rToString << "    ";
      }
    }

    // Condition when the current node is the root node
    if (_Depth_U32 == 0)
    {
      _rToString << pNode->Key() << std::endl;
    }
    // Condition when the node is the last node of the exploring depth
    else if (_IsLast_B)
    {
      _rToString << "+--- " << pNode->Key() << std::endl;
      // No more childrens turn it to the non-exploring depth
      _rNodeVisitedCollection[_Depth_U32] = false;
    }
    else
    {
      _rToString << "+--- " << pNode->Key() << std::endl;
    }
    Index_U32 = 0;
    for (auto It = pNode->ChildCollection().begin(); It != pNode->ChildCollection().end(); ++It, Index_U32++)
    {
      std::cout << "  LoopRec " << Index_U32 << " Depth " << _Depth_U32 + 1 << " Index " << Index_U32 << " vs " << (pNode->ChildCollection().size() - 1) << "->Last " << (Index_U32 == (pNode->ChildCollection().size() - 1)) << std::endl;


      // Recursive call for the children nodes
      BrowseTree(*It, _rToString, _rNodeVisitedCollection, _Depth_U32 + 1, Index_U32 == (pNode->ChildCollection().size() - 1));
    }


    _rNodeVisitedCollection[_Depth_U32] = true;
    std::cout << "  NoRec Depth " << _Depth_U32 << " Visited " << _rNodeVisitedCollection[_Depth_U32] << " -> true Visited " <<
      _rNodeVisitedCollection[0] << " " << _rNodeVisitedCollection[1] << " " << _rNodeVisitedCollection[2] << " " << _rNodeVisitedCollection[3] <<
      " " << _rNodeVisitedCollection[4] << " " << _rNodeVisitedCollection[5] << " " << _rNodeVisitedCollection[6] << " " << _rNodeVisitedCollection[7] << std::endl;

  }
}


template<typename KeyType, typename DataType>
const std::string BofNaryTreeKv<KeyType, DataType>::ToString(const BofNaryTreeKvNodeHandle _NodeHandle)
{
  BOFERR Sts_E;
  std::ostringstream Rts_S;
  std::vector<bool> NodeVisitedCollection(255, false);

  if (IsNodeValid(_NodeHandle))
  {
    BOF_NARY_TREE_KV_LOCK(Sts_E);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      BrowseTree(_NodeHandle, Rts_S, NodeVisitedCollection);
      BOF_NARY_TREE_KV_UNLOCK();
    }
  }
  return Rts_S.str();
}

END_BOF_NAMESPACE()




#if 0

template<typename DataType>
class BofNaryTreeKv
{
public:
  class Node
  {
    std::vector<Node>	    mChildCollection;
    Node *mpParent;
    DataType			        mData;

  public:
    Node() : mpParent(nullptr) {}
    explicit Node(DataType _Data) : mData(_Data), mpParent(nullptr) {}
    Node(DataType _Data, Node *_pParent) : mData(_Data), mpParent(_pParent) {}

    DataType &Data() { return (mData); }
    uint32_t NumberOfChild() const { return (mChildCollection.size()); }
    Node &Child(uint32_t _Index_U32) { BOF_ASSERT(_Index_U32 < NumberOfChild()); return mChildCollection[_Index_U32]; }
    Node *Parent() { return (mpParent); }
    void     SetParent(Node *_pParent) { mpParent = _pParent; }
    bool     HasChildren() const { return (NumberOfChild != 0); }

    void     AddChild(Node &&_rChild) { _rChild.SetParent(this);  mChildCollection.push_back(_rChild); }

    ~Node() {}  // { for (uint32_t i_U32 = 0; i_U32 < NumberOfChild(); i_U32++) { delete (mChildCollection[i_U32]); } }
  };
  // define a DFS iterator (in-order, most commonly used one)
  class Node_Iterator_Dfs
  {
    // dfs iterator needs a stack
    // note: the iterator needs to keep track of the 'current' node in the tree
    // so we maintain a pointer to the current node we use a stack or que for appropriate traversal type
    std::stack<Node *>  mStackOfNode;
    Node *mpCurrentNode;
    BofNaryTreeKv *mpTree;		// reference to the tree this iterator is part of

  public:
    Node_Iterator_Dfs(BofNaryTreeKv &_rTree, Node *_pStartNode) : mpTree(&_rTree), mpCurrentNode(_pStartNode) { }

    Node_Iterator_Dfs &operator ++ ()
    {
      // put the current' nodes children on the stack, and 
      // then move onto the next one by popping it off the stack and making it the current one 
      if ((mpCurrentNode) && (mpCurrentNode->NumberOfChild()))
      {
        for (uint32_t i_U32 = mpCurrentNode->NumberOfChild() - 1; ((i_U32 & 0x80000000) == 0); i_U32--)
        {
          mStackOfNode.push(&mpCurrentNode->Child(i_U32));
        }
      }

      if (!mStackOfNode.empty())
      {
        mpCurrentNode = mStackOfNode.top();
        mStackOfNode.pop();
      }
      else
      {
        *this = mpTree->end();
      }
      return (*this);
    }

    // operator bool is a conversion functions, this allows the iterator to take part in "if else style constructs"
    operator bool() { return (mpCurrentNode != NULL); }

    const Node_Iterator_Dfs &operator = (const Node_Iterator_Dfs &_rIt)
    {
      mpTree = _rIt.mpTree;
      mpCurrentNode = _rIt.mpCurrentNode;
      while (!mStackOfNode.empty())
      {
        mStackOfNode.pop();
      }

      // and now we need to copy the stacks as well.
      std::vector <Node *> StackOfNode(_rIt.mStackOfNode.size());

      Node_Iterator_Dfs &rIt = const_cast <Node_Iterator_Dfs &> (_rIt);	// remove const-ness
      while (!rIt.mStackOfNode.empty())
      {
        StackOfNode.push_back(rIt.mStackOfNode.top());
        rIt.mStackOfNode.pop();
      }

      // now fill both stacks from the vector (traverse from back)
      if (StackOfNode.size())
      {
        for (uint32_t i_U32 = StackOfNode.size() - 1; ((i_U32 & 0x80000000) == 0); i_U32--)
        {
          rIt.mStackOfNode.push(StackOfNode[i_U32]);
          mStackOfNode.push(StackOfNode[i_U32]);
        }
      }

      return (*this);
    }


    // copy CTOR	- do via operator =
    Node_Iterator_Dfs(const Node_Iterator_Dfs &_rIt) { (*this) = _rIt; }

    // i am not going to do stack check because checking the stack is costly so just check if tree, and current and stack sizes are the same
    bool operator == (const Node_Iterator_Dfs &_rIt) const { return ((mpCurrentNode == _rIt.mpCurrentNode) && (mpTree == _rIt.mpTree) && (mStackOfNode.size() == _rIt.mStackOfNode.size())); }
    bool operator != (const Node_Iterator_Dfs &_rIt) const { return (!operator == (_rIt)); }

    DataType *operator -> () { return (mpCurrentNode ? &mpCurrentNode->Data() : nullptr); }
    DataType &operator * () { BOF_ASSERT(mpCurrentNode);	return (mpCurrentNode->Data()); }

    Node *CurrentNode() { return (mpCurrentNode); }
    // jsut in case we need to work with the internal Node class directly
    // But be sure that any node operations may potentially make the
    // iterator invalid .. and once node is given all bests are off ... since
    // the user can now add children to the nodes thus making the iterator invalid, etc.
  };

private:
  BOF_NARY_TREE_KV_PARAM mNaryTreeKvParam_X;
  BOF_MUTEX      mTreeMtx_X;                            /*! Provide a serialized access to shared resources in a multi threaded environment*/
  BOFERR         mErrorCode_E;

  Node *mpRoot;		// the root node of the tree

public:
  BofNaryTreeKv(const  BOF_NARY_TREE_KV_PARAM &_rTreeParam_X);
  virtual ~BofNaryTreeKv();

  BofNaryTreeKv &operator=(const BofNaryTreeKv &) = delete; // Disallow copying
  BofNaryTreeKv(const BofNaryTreeKv &) = delete;

  BOFERR LastErrorCode() const { return mErrorCode_E; }
  BOFERR LockTree();
  BOFERR UnlockTree();

  // --- tree methods --- 
  void SetRoot(Node *_pRoot) { if (mpRoot) delete (mpRoot);  mpRoot = _pRoot; }

  Node_Iterator_Dfs begin() { return (Node_Iterator_Dfs(*this, mpRoot)); }
  Node_Iterator_Dfs end() { return (Node_Iterator_Dfs(*this, nullptr)); }
  };
#endif

