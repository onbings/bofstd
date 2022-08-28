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
*              https://kalkicode.com/n-ary-tree-node-insertion-in-cpp
* https://www.geeksforgeeks.org/list-of-nodes-of-given-n-ary-tree-with-number-of-children-in-range-0-n/
*
*
* History:
*
* V 1.00  Aug 27 2022  BHA : Initial release
*/

#pragma once

#include <bofstd/bofsystem.h>
#include <map>

BEGIN_BOF_NAMESPACE()

#define  BOF_NARY_TREE_KV_LOCK(Sts)   {Sts=mNaryTreeKvParam_X.MultiThreadAware_B ? Bof_LockMutex(mTreeMtx_X):BOF_ERR_NO_ERROR;}
#define  BOF_NARY_TREE_KV_UNLOCK()    {if (mNaryTreeKvParam_X.MultiThreadAware_B) Bof_UnlockMutex(mTreeMtx_X);}

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

#if 1
template<typename KeyType, typename DataType>
class BofNaryTreeKv
{
private:
  class Node
  {
    std::map<KeyType, Node>	    mChildrenCollection;
    Node* mpParent;
    DataType			        mData;

  public:
    Node();
    Node(const DataType& _rData, Node* _pParent);
    ~Node();

    DataType& Data() const;
    uint32_t NumberOfChildren() const;
    const Node * Child(const KeyType & _rKey) const;
    const Node* Parent() const;
    bool  HasChildren() const;
    const Node* AddChild(const KeyType& _rKey, const Node& _rChild);
  };

private:
  BOF_NARY_TREE_KV_PARAM mNaryTreeKvParam_X;
  BOF_MUTEX      mTreeMtx_X;                            /*! Provide a serialized access to shared resources in a multi threaded environment*/
  BOFERR         mErrorCode_E;

  Node* mpRoot;		// the root node of the tree

public:
  BofNaryTreeKv(const BOF_NARY_TREE_KV_PARAM& _rNaryTreeKvParam_X);
  virtual ~BofNaryTreeKv();

  BofNaryTreeKv& operator=(const BofNaryTreeKv&) = delete; // Disallow copying
  BofNaryTreeKv(const BofNaryTreeKv&) = delete;

  BOFERR LastErrorCode() const { return mErrorCode_E; }
  BOFERR LockTree();
  BOFERR UnlockTree();

  // --- tree methods --- 
  BOFERR SetRoot(const DataType* _pRoot);
  BOFERR AddChild(const DataType* _pChild);
};


#else

template<typename DataType>
class BofNaryTreeKv
{
public:
  class Node
  {
    std::vector<Node>	    mChildrenCollection;
    Node* mpParent;
    DataType			        mData;

  public:
    Node() : mpParent(nullptr) {}
    explicit Node(DataType _Data) : mData(_Data), mpParent(nullptr) {}
    Node(DataType _Data, Node* _pParent) : mData(_Data), mpParent(_pParent) {}

    DataType& Data() { return (mData); }
    uint32_t NumberOfChildren() const { return (mChildrenCollection.size()); }
    Node& Child(uint32_t _Index_U32) { BOF_ASSERT(_Index_U32 < NumberOfChildren()); return mChildrenCollection[_Index_U32]; }
    Node* Parent() { return (mpParent); }
    void     SetParent(Node* _pParent) { mpParent = _pParent; }
    bool     HasChildren() const { return (NumberOfChildren != 0); }

    void     AddChild(Node&& _rChild) { _rChild.SetParent(this);  mChildrenCollection.push_back(_rChild); }

    ~Node() {}  // { for (uint32_t i_U32 = 0; i_U32 < NumberOfChildren(); i_U32++) { delete (mChildrenCollection[i_U32]); } }
  };
  // define a DFS iterator (in-order, most commonly used one)
  class Node_Iterator_Dfs
  {
    // dfs iterator needs a stack
    // note: the iterator needs to keep track of the 'current' node in the tree
    // so we maintain a pointer to the current node we use a stack or que for appropriate traversal type
    std::stack<Node*>  mStackOfNode;
    Node* mpCurrentNode;
    BofNaryTreeKv* mpTree;		// reference to the tree this iterator is part of

  public:
    Node_Iterator_Dfs(BofNaryTreeKv& _rTree, Node* _pStartNode) : mpTree(&_rTree), mpCurrentNode(_pStartNode) { }

    Node_Iterator_Dfs& operator ++ ()
    {
      // put the current' nodes children on the stack, and 
      // then move onto the next one by popping it off the stack and making it the current one 
      if ((mpCurrentNode) && (mpCurrentNode->NumberOfChildren()))
      {
        for (uint32_t i_U32 = mpCurrentNode->NumberOfChildren() - 1; ((i_U32 & 0x80000000) == 0); i_U32--)
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

    const Node_Iterator_Dfs& operator = (const Node_Iterator_Dfs& _rIt)
    {
      mpTree = _rIt.mpTree;
      mpCurrentNode = _rIt.mpCurrentNode;
      while (!mStackOfNode.empty())
      {
        mStackOfNode.pop();
      }

      // and now we need to copy the stacks as well.
      std::vector <Node*> StackOfNode(_rIt.mStackOfNode.size());

      Node_Iterator_Dfs& rIt = const_cast <Node_Iterator_Dfs&> (_rIt);	// remove const-ness
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
    Node_Iterator_Dfs(const Node_Iterator_Dfs& _rIt) { (*this) = _rIt; }

    // i am not going to do stack check because checking the stack is costly so just check if tree, and current and stack sizes are the same
    bool operator == (const Node_Iterator_Dfs& _rIt) const { return ((mpCurrentNode == _rIt.mpCurrentNode) && (mpTree == _rIt.mpTree) && (mStackOfNode.size() == _rIt.mStackOfNode.size())); }
    bool operator != (const Node_Iterator_Dfs& _rIt) const { return (!operator == (_rIt)); }

    DataType* operator -> () { return (mpCurrentNode ? &mpCurrentNode->Data() : nullptr); }
    DataType& operator * () { BOF_ASSERT(mpCurrentNode);	return (mpCurrentNode->Data()); }

    Node* CurrentNode() { return (mpCurrentNode); }
    // jsut in case we need to work with the internal Node class directly
    // But be sure that any node operations may potentially make the
    // iterator invalid .. and once node is given all bests are off ... since
    // the user can now add children to the nodes thus making the iterator invalid, etc.
  };

private:
  BOF_NARY_TREE_KV_PARAM mNaryTreeKvParam_X;
  BOF_MUTEX      mTreeMtx_X;                            /*! Provide a serialized access to shared resources in a multi threaded environment*/
  BOFERR         mErrorCode_E;

  Node* mpRoot;		// the root node of the tree

public:
  BofNaryTreeKv(const  BOF_NARY_TREE_KV_PARAM& _rTreeParam_X);
  virtual ~BofNaryTreeKv();

  BofNaryTreeKv& operator=(const BofNaryTreeKv&) = delete; // Disallow copying
  BofNaryTreeKv(const BofNaryTreeKv&) = delete;

  BOFERR LastErrorCode() const { return mErrorCode_E; }
  BOFERR LockTree();
  BOFERR UnlockTree();

  // --- tree methods --- 
  void SetRoot(Node* _pRoot) { if (mpRoot) delete (mpRoot);  mpRoot = _pRoot; }

  Node_Iterator_Dfs begin() { return (Node_Iterator_Dfs(*this, mpRoot)); }
  Node_Iterator_Dfs end() { return (Node_Iterator_Dfs(*this, nullptr)); }
};
#endif



template<typename KeyType, typename DataType>
BofNaryTreeKv<KeyType, DataType>::Node::Node() : mpParent(nullptr)
{
}

template<typename KeyType, typename DataType>
BofNaryTreeKv<KeyType, DataType>::Node::Node(const DataType& _rData, Node* _pParent) : mData(_rData), mpParent(_pParent)
{
}

template<typename KeyType, typename DataType>
BofNaryTreeKv<KeyType, DataType>::Node::~Node()
{
  // { for (uint32_t i_U32 = 0; i_U32 < NumberOfChildren(); i_U32++) { delete (mChildrenCollection[i_U32]); } }
}

template<typename KeyType, typename DataType>
DataType& BofNaryTreeKv<KeyType, DataType>::Node::Data() const
{
  return (mData);
}

template<typename KeyType, typename DataType>
uint32_t BofNaryTreeKv<KeyType, DataType>::Node::NumberOfChildren() const
{
  return (mChildrenCollection.size());
}


template<typename KeyType, typename DataType>
typename const BofNaryTreeKv<KeyType, DataType>::Node * BofNaryTreeKv<KeyType, DataType>::Node::Child(const KeyType & _rKey) const
{
  BOF_ERR Sts_E;

  BOF_NARY_TREE_KV_LOCK(Sts_E);
  auto It = mChildrenCollection.find(_rKey);
  BOF_NARY_TREE_KV_UNLOCK();
  return (It != mChildrenCollection.end()) &mChildrenCollection[_rKey]:nullptr;
}

template<typename KeyType, typename DataType>
typename const BofNaryTreeKv<KeyType, DataType>::Node* BofNaryTreeKv<KeyType, DataType>::Node::Parent() const
{
  return (mpParent);
}
template<typename KeyType, typename DataType>
bool BofNaryTreeKv<KeyType, DataType>::Node::HasChildren() const
{
  return (NumberOfChildren() != 0);
}

template<typename KeyType, typename DataType>
typename const BofNaryTreeKv<KeyType, DataType>::Node* BofNaryTreeKv<KeyType, DataType>::Node::AddChild(const KeyType& _rKey, const Node& _rChild)
{
  const BofNaryTreeKv<KeyType, DataType>::Node *pRts = nullptr;
  BOF_ERR Sts_E;

  BOF_NARY_TREE_KV_LOCK(Sts_E);
  auto It = mChildrenCollection.find(_rKey);
  if (It == mChildrenCollection.end())
  {
    _rChild.SetParent(this);  
    mChildrenCollection[_rKey] = _rChild;
    It = mChildrenCollection.find(_rKey);
    pRts = It;
  }
  BOF_NARY_TREE_KV_UNLOCK();
  return pRts;
}



template<typename KeyType, typename DataType>
BofNaryTreeKv<KeyType, DataType>::BofNaryTreeKv(const BOF_NARY_TREE_KV_PARAM& _rNaryTreeKvParam_X)
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
  SetRoot(nullptr);
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
BOFERR BofNaryTreeKv<KeyType, DataType>::SetRoot(const DataType* _pRoot)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;

  if (mpRoot)
  {
    BOF_SAFE_DELETE(mpRoot);  //TODO not enough
  }
  if (_pRoot)
  {
    mpRoot = new Node(*_pRoot, nullptr);
    if (!mpRoot)
    {
      Rts_E = BOF_ERR_ENOMEM;
    }
  }
  return Rts_E;
}
END_BOF_NAMESPACE()