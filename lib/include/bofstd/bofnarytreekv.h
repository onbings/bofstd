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

#include <algorithm>
#include <list>
#include <ostream>
#include <string>
#include <vector>

BEGIN_BOF_NAMESPACE()

#define BOF_NARY_TREE_KV_LOCK(Sts)                                                                                                                                                                                                                             \
  {                                                                                                                                                                                                                                                            \
    Sts = mNaryTreeKvParam_X.MultiThreadAware_B ? Bof_LockMutex(mTreeMtx_X) : BOF_ERR_NO_ERROR;                                                                                                                                                                \
  }
#define BOF_NARY_TREE_KV_UNLOCK()                                                                                                                                                                                                                              \
  {                                                                                                                                                                                                                                                            \
    if (mNaryTreeKvParam_X.MultiThreadAware_B)                                                                                                                                                                                                                 \
      Bof_UnlockMutex(mTreeMtx_X);                                                                                                                                                                                                                             \
  }
constexpr uint32_t BOF_NARY_TREE_KV_NODE_MAGIC_NUMBER = 0xBA1CD158;

struct BOFSTD_EXPORT BOF_NARY_TREE_KV_PARAM
{
  bool MultiThreadAware_B; /*! true if the object is used in a multi threaded application (use mTreeMtx_X)*/

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

template <typename KeyType, typename DataType> class BofNaryTreeKv
{
private:
  class Node
  {
  private:
    uint32_t mMagicNumber_U32 = BOF_NARY_TREE_KV_NODE_MAGIC_NUMBER;
    Node *mpParent;
    KeyType mKey;
    DataType mValue;
    std::list<Node *> mChildCollection; // better than vector for iterator as it is not invalidated when we add/remove eleme

  public:
    Node(const KeyType &_rKey, const DataType &_rData);
    ~Node();

    uint32_t MagicNumber() const;
    const KeyType &Key() const;
    const DataType &Value() const;
    BOFERR SetKey(const KeyType &);
    BOFERR SetValue(const DataType &);
    uint32_t NumberOfChild() const;
    std::list<Node *> &ChildCollection();
    BOFERR AddChild(Node *_pNode);
    Node *PopLastChild();
    bool PopAndDeleteChild(Node *_pNode);
    Node *Parent();
    void SetParent(Node *_pParent);
  };

private:
  BOF_NARY_TREE_KV_PARAM mNaryTreeKvParam_X;
  BOF_MUTEX mTreeMtx_X; /*! Provide a serialized access to shared resources in a multi threaded environment*/
  BOFERR mErrorCode_E;
  Node *mpRoot = nullptr; // the root node of the tree
  uint32_t mMaxDepth_U32 = 0;

public:
  // template<typename KeyType, typename DataType>
  // using BofNaryTreeKvNode = typename BofNaryTreeKv<KeyType, DataType>::Node *;
  typedef void *BofNaryTreeKvNodeHandle;

  BofNaryTreeKv(const BOF_NARY_TREE_KV_PARAM &_rNaryTreeKvParam_X);
  virtual ~BofNaryTreeKv();

  BofNaryTreeKv &operator=(const BofNaryTreeKv &) = delete; // Disallow copying
  BofNaryTreeKv(const BofNaryTreeKv &) = delete;

  BOFERR LastErrorCode() const
  {
    return mErrorCode_E;
  }
  BOFERR LockTree();
  BOFERR UnlockTree();

  BOFERR ClearTree(const BofNaryTreeKvNodeHandle _NodeHandle);
  BOFERR SetRoot(const KeyType &_rKey, const DataType &_rData, BofNaryTreeKvNodeHandle *_pRootHandle);
  BOFERR AddChild(const BofNaryTreeKvNodeHandle _ParentHandle, const KeyType &_rKey, const DataType &_rData, BofNaryTreeKvNodeHandle *_pChildHandle);
  BOFERR Key(BofNaryTreeKvNodeHandle _NodeHandle, KeyType &_rKey);
  BOFERR Value(BofNaryTreeKvNodeHandle _NodeHandle, DataType &_rValue);
  BOFERR SetKey(BofNaryTreeKvNodeHandle _NodeHandle, const KeyType &_rKey);
  BOFERR SetValue(BofNaryTreeKvNodeHandle _NodeHandle, const DataType &_rValue);
  BOFERR Search(const BofNaryTreeKvNodeHandle _ParentHandle, const std::vector<KeyType> &_rKeyCollection, BofNaryTreeKvNodeHandle *_pNodeHandle);
  bool IsNodeValid(const BofNaryTreeKvNodeHandle _NodeHandle) const;
  BOFERR GetNodeHandle(const BofNaryTreeKvNodeHandle _ParentHandle, const KeyType &_rKey, BofNaryTreeKvNodeHandle *_pNodeHandle);
  std::string ToString(bool _ShowExtraInfo_B, const BofNaryTreeKvNodeHandle _ParentHandle); // No const (mutex)

private:
  void BrowseTree(bool _ShowExtraInfo_B, const BofNaryTreeKvNodeHandle _NodeHandle, std::ostringstream &_rToString, std::vector<bool> &_rNodeVisitedCollection, uint32_t _Depth_U32 = 0, bool _IsLast_B = false);
};

//--- BofNaryTreeKv<KeyType, DataType>::Node Class --------------------------------------------------------------------

template <typename KeyType, typename DataType> BofNaryTreeKv<KeyType, DataType>::Node::Node(const KeyType &_rKey, const DataType &_rData) : mKey(_rKey), mValue(_rData), mpParent(nullptr)
{
  mChildCollection.clear();
  //  std::cout << "Create " << this << " Key '" << this->Key() << "' Value " << this->Value() << std::endl;
}

template <typename KeyType, typename DataType> BofNaryTreeKv<KeyType, DataType>::Node::~Node()
{
  BofNaryTreeKv<KeyType, DataType>::Node *pNode;

  //  std::cout << "         DelNode " << this << " Key '" << this->Key() << "' Value " << this->Value() << " NbChild " << mChildCollection.size() << std::endl;
  mMagicNumber_U32 = ~BOF_NARY_TREE_KV_NODE_MAGIC_NUMBER; // invalid the usage of external user handle
}

template <typename KeyType, typename DataType> uint32_t BofNaryTreeKv<KeyType, DataType>::Node::MagicNumber() const
{
  return mMagicNumber_U32;
}

template <typename KeyType, typename DataType> const KeyType &BofNaryTreeKv<KeyType, DataType>::Node::Key() const
{
  return mKey;
}

template <typename KeyType, typename DataType> const DataType &BofNaryTreeKv<KeyType, DataType>::Node::Value() const
{
  return mValue;
}

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::Node::SetKey(const KeyType &_rKey)
{
  BOFERR Rts_E = BOF_ERR_DUPLICATE;
  Node *pFather = mpParent ? mpParent : this;

  auto It = std::find_if(pFather->ChildCollection().begin(), pFather->ChildCollection().end(), [&](Node *_pItNode) { return (_pItNode->Key() == _rKey); });
  if (It == pFather->ChildCollection().end())
  {
    mKey = _rKey;
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::Node::SetValue(const DataType &_rValue)
{
  mValue = _rValue;
  return BOF_ERR_NO_ERROR;
}

template <typename KeyType, typename DataType> typename std::list<typename BofNaryTreeKv<KeyType, DataType>::Node *> &BofNaryTreeKv<KeyType, DataType>::Node::ChildCollection()
{
  return mChildCollection;
}

template <typename KeyType, typename DataType> uint32_t BofNaryTreeKv<KeyType, DataType>::Node::NumberOfChild() const
{
  return mChildCollection.size();
}

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::Node::AddChild(Node *_pNode)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if (_pNode)
  {
    Rts_E = BOF_ERR_DUPLICATE;

    auto It = std::find_if(mChildCollection.begin(), mChildCollection.end(), [_pNode](Node *_pItNode) { return (_pItNode->Key() == _pNode->Key()); });
    if (It == mChildCollection.end())
    {
      _pNode->SetParent(this);
      mChildCollection.push_back(_pNode);
      //     std::cout << "   AddChild " << _pNode << " Key '" << _pNode->Key() << "' Value " << _pNode->Value() << std::endl;
      //     std::cout << "      Container " << this << " Key '" << this->Key() << "' Value " << this->Value() << std::endl;
      if (mpParent)
      {
        //       std::cout << "      Parent " << mpParent << " Key '" << mpParent->Key() << "' Value " << mpParent->Value() << std::endl;
      }
      else
      {
        //       std::cout << "      Parent is root" << std::endl;
      }
      Rts_E = BOF_ERR_NO_ERROR;
    }
  }
  return Rts_E;
}

template <typename KeyType, typename DataType> typename BofNaryTreeKv<KeyType, DataType>::Node *BofNaryTreeKv<KeyType, DataType>::Node::PopLastChild()
{
  BofNaryTreeKv<KeyType, DataType>::Node *pRts = nullptr;

  if (!mChildCollection.empty())
  {
    pRts = mChildCollection.back();
    mChildCollection.pop_back(); // Pointer to node so no destructor called
  }
  return pRts;
}

template <typename KeyType, typename DataType> bool BofNaryTreeKv<KeyType, DataType>::Node::PopAndDeleteChild(Node *_pNode)
{
  bool Rts_B = false;

  if (_pNode)
  {
    auto It = std::find_if(mChildCollection.begin(), mChildCollection.end(), [&](Node *_pItNode) { return (_pItNode->Key() == _pNode->Key()); });
    if (It != mChildCollection.end())
    {
      mChildCollection.erase(It); // Pointer to node so no destructor called
      Rts_B = true;
    }
  }
  return Rts_B;
}
template <typename KeyType, typename DataType> typename BofNaryTreeKv<KeyType, DataType>::Node *BofNaryTreeKv<KeyType, DataType>::Node::Parent()
{
  return mpParent;
}

template <typename KeyType, typename DataType> void BofNaryTreeKv<KeyType, DataType>::Node::SetParent(BofNaryTreeKv<KeyType, DataType>::Node *_pNode)
{
  mpParent = _pNode;
}

//--- BofNaryTreeKv<KeyType, DataType>: class --------------------------------------------------------------------
template <typename KeyType, typename DataType> BofNaryTreeKv<KeyType, DataType>::BofNaryTreeKv(const BOF_NARY_TREE_KV_PARAM &_rNaryTreeKvParam_X)
{
  mNaryTreeKvParam_X = _rNaryTreeKvParam_X;
  mpRoot = nullptr;
  mErrorCode_E = _rNaryTreeKvParam_X.MultiThreadAware_B ? Bof_CreateMutex("BofNaryTreeKv", true, true, mTreeMtx_X) : BOF_ERR_NO_ERROR;
  if (mErrorCode_E == BOF_ERR_NO_ERROR)
  {
  }
}

template <typename KeyType, typename DataType> BofNaryTreeKv<KeyType, DataType>::~BofNaryTreeKv()
{
  ClearTree(mpRoot);
  Bof_DestroyMutex(mTreeMtx_X);
}

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::LockTree()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mNaryTreeKvParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_LockMutex(mTreeMtx_X);
  }
  return Rts_E;
}

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::UnlockTree()
{
  BOFERR Rts_E = BOF_ERR_BAD_TYPE;

  if (mNaryTreeKvParam_X.MultiThreadAware_B)
  {
    Rts_E = Bof_UnlockMutex(mTreeMtx_X);
  }
  return Rts_E;
}

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::SetRoot(const KeyType &_rKey, const DataType &_rData, BofNaryTreeKvNodeHandle *_pRootHandle)
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
      mpRoot->SetParent(nullptr);
    }
    else
    {
      Rts_E = BOF_ERR_ENOMEM;
    }

    *_pRootHandle = mpRoot;
  }
  return Rts_E;
}

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::AddChild(const BofNaryTreeKvNodeHandle _ParentHandle, const KeyType &_rKey, const DataType &_rData, BofNaryTreeKvNodeHandle *_pChildHandle)
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

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::Key(BofNaryTreeKvNodeHandle _NodeHandle, KeyType &_rKey)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  Node *pNode;

  if (IsNodeValid(_NodeHandle))
  {
    BOF_NARY_TREE_KV_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      pNode = reinterpret_cast<Node *>(_NodeHandle);
      _rKey = pNode->Key();
      BOF_NARY_TREE_KV_UNLOCK();
    }
  }
  return Rts_E;
}

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::Value(BofNaryTreeKvNodeHandle _NodeHandle, DataType &_rValue)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  Node *pNode;

  if (IsNodeValid(_NodeHandle))
  {
    BOF_NARY_TREE_KV_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      pNode = reinterpret_cast<Node *>(_NodeHandle);
      _rValue = pNode->Value();
      BOF_NARY_TREE_KV_UNLOCK();
    }
  }
  return Rts_E;
}

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::SetKey(BofNaryTreeKvNodeHandle _NodeHandle, const KeyType &_rKey)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  Node *pNode;

  if (IsNodeValid(_NodeHandle))
  {
    BOF_NARY_TREE_KV_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      pNode = reinterpret_cast<Node *>(_NodeHandle);
      Rts_E = pNode->SetKey(_rKey);
      BOF_NARY_TREE_KV_UNLOCK();
    }
  }
  return Rts_E;
}

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::SetValue(BofNaryTreeKvNodeHandle _NodeHandle, const DataType &_rValue)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  Node *pNode;

  if (IsNodeValid(_NodeHandle))
  {
    BOF_NARY_TREE_KV_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      pNode = reinterpret_cast<Node *>(_NodeHandle);
      Rts_E = pNode->SetValue(_rValue);
      BOF_NARY_TREE_KV_UNLOCK();
    }
  }
  return Rts_E;
}

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::Search(const BofNaryTreeKvNodeHandle _ParentHandle, const std::vector<KeyType> &_rKeyCollection, BofNaryTreeKvNodeHandle *_pNodeHandle)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  Node *pParent;

  if ((IsNodeValid(_ParentHandle)) && (_rKeyCollection.size()) && (_pNodeHandle))
  {
    *_pNodeHandle = nullptr;
    BOF_NARY_TREE_KV_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = BOF_ERR_NOT_FOUND;
      pParent = reinterpret_cast<Node *>(_ParentHandle);
      if (*(_rKeyCollection.begin()) == pParent->Key())
      {
        for (auto It = _rKeyCollection.begin() + 1; It != _rKeyCollection.end(); ++It)
        {
          Rts_E = BOF_ERR_NOT_FOUND;
          for (auto pChild : pParent->ChildCollection())
          {
            if ((*It) == pChild->Key())
            {
              Rts_E = BOF_ERR_NO_ERROR;
              *_pNodeHandle = pParent;
              pParent = pChild;
              break;
            }
          }
          if (Rts_E != BOF_ERR_NO_ERROR)
          {
            break;
          }
        }
      }
      BOF_NARY_TREE_KV_UNLOCK();
    }
  }
  return Rts_E;
}

template <typename KeyType, typename DataType> bool BofNaryTreeKv<KeyType, DataType>::IsNodeValid(const BofNaryTreeKvNodeHandle _NodeHandle) const
{
  bool Rts_B = false;
  const Node *pNode = reinterpret_cast<Node *>(_NodeHandle);
  if (pNode)
  {
    Rts_B = (pNode->MagicNumber() == BOF_NARY_TREE_KV_NODE_MAGIC_NUMBER);
  }
  return Rts_B;
}

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::GetNodeHandle(const BofNaryTreeKvNodeHandle _ParentHandle, const KeyType &_rKey, BofNaryTreeKvNodeHandle *_pNodeHandle)
{
  BOFERR Rts_E = BOF_ERR_DONT_EXIST;
  Node *pParent;

  if ((IsNodeValid(_ParentHandle)) && (_pNodeHandle))
  {
    *_pNodeHandle = nullptr;
    BOF_NARY_TREE_KV_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = BOF_ERR_NOT_FOUND;
      pParent = reinterpret_cast<Node *>(_ParentHandle);
      for (const auto pChild : pParent->ChildCollection())
      {
        if (_rKey == pChild->Key())
        {
          Rts_E = BOF_ERR_NO_ERROR;
          *_pNodeHandle = pChild;
          break;
        }
      }
    }
    BOF_NARY_TREE_KV_UNLOCK();
  }
  return Rts_E;
}

template <typename KeyType, typename DataType> BOFERR BofNaryTreeKv<KeyType, DataType>::ClearTree(const BofNaryTreeKvNodeHandle _NodeHandle)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  const Node *pStartNode = reinterpret_cast<const Node *>(_NodeHandle);
  Node *pNode, *pFather, *pLast;

  if (IsNodeValid(_NodeHandle))
  {
    BOF_NARY_TREE_KV_LOCK(Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      pNode = (Node *)pStartNode;
      while (pNode)
      {
        pLast = pNode->PopLastChild();
        //        std::cout << "PopLastChild " << pNode << " Key '" << pNode->Key() << "' Value " << pNode->Value() << std::endl;

        if (pLast)
        {
          //          std::cout << "   Last " << pLast << " Key '" << pLast->Key() << "' Value " << pLast->Value() << std::endl;
          if (pLast == pStartNode)
          {
            pFather = pLast->Parent();
            if (pFather)
            {
              //              std::cout << "      PopAndDeleteChild from " << pFather << " Key '" << pFather->Key() << "' Value " << pFather->Value() << std::endl;
              pFather->PopAndDeleteChild(pLast);
            }
            //            std::cout << "      BOF_SAFE_DELETE LAST " << pLast << " Key '" << pLast->Key() << "' Value " << pLast->Value() << std::endl;
            BOF_SAFE_DELETE(pLast);
            break;
          }
          pNode = pLast;
        }
        else
        {
          pFather = pNode->Parent();
          if (pFather)
          {
            //            std::cout << "   Father " << pFather << " Key '" << pFather->Key() << "' Value " << pFather->Value() << std::endl;
          }
          else
          {
            //            std::cout << "   No Father" << std::endl;
          }
          if (pNode == pStartNode)
          {
            if (pFather)
            {
              //              std::cout << "      PopAndDeleteChild from " << pFather << " Key '" << pFather->Key() << "' Value " << pFather->Value() << std::endl;
              pFather->PopAndDeleteChild(pNode);
            }
            //            std::cout << "      BOF_SAFE_DELETE LAST " << pNode << " Key '" << pNode->Key() << "' Value " << pNode->Value() << std::endl;
            BOF_SAFE_DELETE(pNode);
            break;
          }
          //          std::cout << "      BOF_SAFE_DELETE " << pNode << " Key '" << pNode->Key() << "' Value " << pNode->Value() << std::endl;
          BOF_SAFE_DELETE(pNode);
          pNode = pFather;
        }
      }
      BOF_NARY_TREE_KV_UNLOCK();
    }
  }
  return Rts_E;
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
template <typename KeyType, typename DataType>
void BofNaryTreeKv<KeyType, DataType>::BrowseTree(bool _ShowExtraInfo_B, const BofNaryTreeKvNodeHandle _NodeHandle, std::ostringstream &_rToString, std::vector<bool> &_rNodeVisitedCollection, uint32_t _Depth_U32, bool _IsLast_B)
{
  uint32_t i_U32, Index_U32;

  Node *pNode = reinterpret_cast<Node *>(_NodeHandle);

  //  std::cout << "Node " << pNode << " val " << pNode->Key() << " Depth " << _Depth_U32 << " Visited " <<
  //    _rNodeVisitedCollection[0] << " " << _rNodeVisitedCollection[1] << " " << _rNodeVisitedCollection[2] << " " << _rNodeVisitedCollection[3] <<
  //    " " << _rNodeVisitedCollection[4] << " " << _rNodeVisitedCollection[5] << " " << _rNodeVisitedCollection[6] << " " << _rNodeVisitedCollection[7] << std::endl;
  if (pNode)
  {
    // Loop to print the depths of the current node
    for (i_U32 = 1; i_U32 < _Depth_U32; i_U32++)
    {
      //      std::cout << "  Loop " << i_U32 << "/" << _Depth_U32 << " Visited " << _rNodeVisitedCollection[i_U32] << " Last " << _IsLast_B << std::endl;

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
      _rToString << pNode->Key();
      if (_ShowExtraInfo_B)
      {
        if (pNode->Parent())
        {
          _rToString << " [" << pNode->Parent()->Key() << ':' << pNode->NumberOfChild() << ']';
        }
        else
        {
          _rToString << " [NULL]";
        }
      }
      _rToString << std::endl;
    }
    // Condition when the node is the last node of the exploring depth
    else if (_IsLast_B)
    {
      _rToString << "+--- " << pNode->Key();
      if (_ShowExtraInfo_B)
      {
        if (pNode->Parent())
        {
          _rToString << " [" << pNode->Parent()->Key() << ':' << pNode->NumberOfChild() << ']';
        }
        else
        {
          _rToString << " [NULL]";
        }
      }
      _rToString << std::endl;
      // No more childrens turn it to the non-exploring depth
      _rNodeVisitedCollection[_Depth_U32] = false;
    }
    else
    {
      _rToString << "+--- " << pNode->Key();
      if (_ShowExtraInfo_B)
      {
        if (pNode->Parent())
        {
          _rToString << " [" << pNode->Parent()->Key() << ':' << pNode->NumberOfChild() << ']';
        }
        else
        {
          _rToString << " [NULL]";
        }
      }
      _rToString << std::endl;
    }
    Index_U32 = 0;

    for (auto It = pNode->ChildCollection().begin(); It != pNode->ChildCollection().end(); ++It, Index_U32++)
    {
      //      std::cout << "  LoopRec " << Index_U32 << " Depth " << _Depth_U32 + 1 << " Index " << Index_U32 << " vs " << (pNode->ChildCollection().size() - 1) << "->Last " << (Index_U32 == (pNode->ChildCollection().size() - 1)) << std::endl;
      // Recursive call for the children nodes
      BrowseTree(_ShowExtraInfo_B, *It, _rToString, _rNodeVisitedCollection, _Depth_U32 + 1, Index_U32 == (pNode->ChildCollection().size() - 1));
    }

    _rNodeVisitedCollection[_Depth_U32] = true;
    //    std::cout << "  NoRec Depth " << _Depth_U32 << " Visited " << _rNodeVisitedCollection[_Depth_U32] << " -> true Visited " <<
    //      _rNodeVisitedCollection[0] << " " << _rNodeVisitedCollection[1] << " " << _rNodeVisitedCollection[2] << " " << _rNodeVisitedCollection[3] <<
    //      " " << _rNodeVisitedCollection[4] << " " << _rNodeVisitedCollection[5] << " " << _rNodeVisitedCollection[6] << " " << _rNodeVisitedCollection[7] << std::endl;
  }
}

template <typename KeyType, typename DataType> std::string BofNaryTreeKv<KeyType, DataType>::ToString(bool _ShowExtraInfo_B, const BofNaryTreeKvNodeHandle _NodeHandle)
{
  // return "hh";

  BOFERR Sts_E;
  std::ostringstream Rts_S;
  std::vector<bool> NodeVisitedCollection(255, false);

  if (IsNodeValid(_NodeHandle))
  {
    BOF_NARY_TREE_KV_LOCK(Sts_E);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      BrowseTree(_ShowExtraInfo_B, _NodeHandle, Rts_S, NodeVisitedCollection);
      BOF_NARY_TREE_KV_UNLOCK();
    }
  }
  return Rts_S.str();
}

END_BOF_NAMESPACE()
