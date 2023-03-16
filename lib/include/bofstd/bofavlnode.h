/*$T Bof/BofAvlNode.h GC 1.140 02/11/08 12:15:58 */

/*
 * File      : BofAvlNode.h
 *
 * Project   : Bof
 *
 * Package   : Bog-Include
 *
 * Company   : Sci
 *
 * Author    : Bernard HARMEL: b.harmel@gmail.com
 *
 * Purpose   : This is the definition of the BofAvlNode template
 *
 * Copyright : (C) Sci
 *
 * Version History:
 * V 1.00  Wed Sep 6 2006  BHA : Initial release
 */
#pragma once

#include <bofstd/bofstd.h>

#include <cstdint>
#include <memory.h>

BEGIN_BOF_NAMESPACE()
/*! Use mnemonic constants for valid balance-factor values*/
typedef enum
{
  LEFT_HEAVY = -1,
  BALANCED = 0,
  RIGHT_HEAVY = 1,
  AVLBALANCE_ALIGNON32BITS = 0xFFFFFFFF
} AVLBALANCE;

/*! Use mnemonic constants for indicating a change in height*/
typedef enum
{
  HEIGHT_NOCHANGE = 0,
  HEIGHT_CHANGE = 1,
  AVLHEIGHTEFFECT_ALIGNON32BITS = 0xFFFFFFFF
} AVLHEIGHTEFFECT;

typedef enum
{
  LEFTSUBTREE = 0,
  RIGHTSUBTREE,
  PARENTSUBTREE,
  MAX_SUBTREES,
  WHICHAVLSUBTREE_ALIGNON32BITS = 0xFFFFFFFF
} WHICHAVLSUBTREE;

typedef enum
{
  BOF_CMP_LESSOREQUAL = -2,         /*! less or equal than (Special Parameter for Search method)*/
  BOF_CMP_LESS = -1,         /*! less than*/
  BOF_CMP_EQUAL = 0,          /*! equal to*/
  BOF_CMP_GREATER = 1,          /*! greater than */
  BOF_CMP_GREATEROREQUAL = 2,          /*! greater or equal than (Special Parameter for Search method)*/
  BOFCMP_ALIGNON32BITS = 0xFFFFFFFF
} BOFCMP;

typedef enum
{
  BOF_TYPE_U8 = 0,
  BOF_TYPE_U16,
  BOF_TYPE_U32,
  BOF_TYPE_S8,
  BOF_TYPE_S16,
  BOF_TYPE_S32,
  BOF_TYPE_CHAR,
  BOF_TYPE_WCHAR,
  BOF_TYPE_FLOAT,
  BOF_TYPE_DOUBLE,
  BOF_TYPE_NOTHING,
  BOFTYPE_ALIGNON32BITS = 0xFFFFFFFF
} BOFTYPE;


/*! Return true if the tree is too heavy on the left side*/
inline static int32_t LEFT_IMBALANCE(int32_t _Balance_S32)
{
  return _Balance_S32 < LEFT_HEAVY;
}


/*! Return true if the tree is too heavy on the right side*/
inline static int32_t RIGHT_IMBALANCE(int32_t _Balance_S32)
{
  return _Balance_S32 > RIGHT_HEAVY;
}

template<typename KeyType>
class BofAvlTree;

/*!
 * Class "BofAvlNode" implement an AVL Tree
 */
template<typename KeyType>
class BofAvlNode
{
private:
  KeyType *mpData;                  /*! GetData field*/
  BofAvlNode<KeyType> *mpSubTree[MAX_SUBTREES]; /*! Pointers to subtrees*/
  int32_t mBalance_S32;             /*! GetBalance factor*/
private:

  // Disallow copying and assignment
  BofAvlNode(const BofAvlNode<KeyType> &) = delete;

  BofAvlNode &operator=(const BofAvlNode<KeyType> &) = delete;

public:

  // Return the opposite direction of the given index
  static WHICHAVLSUBTREE Opposite(WHICHAVLSUBTREE _Direction_E)
  {
    return (_Direction_E == LEFTSUBTREE) ? RIGHTSUBTREE : LEFTSUBTREE;
  }

  BofAvlNode();

  virtual ~BofAvlNode();

  void Clear()
  {
    mBalance_S32 = 0;
    memset(mpSubTree, 0, sizeof(mpSubTree));
    mpData = nullptr;
  }

  /*! Get this node's data*/
  KeyType *GetData() const
  {
    return mpData;
  }

  void SetData(KeyType *_pData)
  {
    mpData = _pData;
  }

  void *GetKey(uint32_t _Index_U32, BOFTYPE *_pType_E, uint32_t _MaxChar_U32, char *_pVal_c) const
  {
    if (mpData)
    {
      return mpData->GetKey(_Index_U32, _pType_E, _MaxChar_U32, _pVal_c);
    }
    else
    {
      *_pType_E = BOF_TYPE_NOTHING;
      _pVal_c[0] = 0;
      return nullptr;
    }
  }

  void GetRecord(char *_pVal_c) const
  {
    if (mpData)
    {
      return mpData->GetRecord(_pVal_c);
    }
    else
    {
      _pVal_c[0] = 0;

      // return(nullptr);
    }
  }

  BofAvlNode<KeyType> *GetParent() const
  {
    return mpSubTree[PARENTSUBTREE];
  }

  void SetParent(BofAvlNode<KeyType> *_pData)
  {
    mpSubTree[PARENTSUBTREE] = _pData;
  }

  /*!
   * Query the balance factor, it will be a value between -1 .. 1
   * where:
   * -1 => left subtree is taller than right subtree
   *  0 => left and right subtree are equal in height
   *  1 => right subtree is taller than left subtree
   */
  int32_t GetBalance() const
  {
    return mBalance_S32;
  }

  /*!
   * Get the item at the top of the left/right subtree of this
   * item (the result may be nullptr if there is no such item).
   */
  BofAvlNode *GetSubtree(WHICHAVLSUBTREE _Direction_E) const
  {
    if (_Direction_E < MAX_SUBTREES)
    {
      return mpSubTree[_Direction_E];
    }
    else
    {
      return nullptr;
    }
  }

  /*!
   * NOTE: These are all static functions instead of member functions
   *  because most of them need to modify the given tree root
   *  pointer. If these were instance member functions than
   *  that would correspond to having to modify the 'this'
   *  pointer, which is not allowed in C++. Most of the
   *  functions that are static and which take an AVL tree
   *  pointer as a parameter are static for this reason.
   */
  static BofAvlNode<KeyType> *Search(uint32_t _Index_U32, KeyType *_pKey, BofAvlNode<KeyType> *_pRoot, BOFCMP _Cmp_E);

  static BofAvlNode<KeyType> *Insert(BofAvlTree<KeyType> *_pAvlTree, KeyType *_pKey, BofAvlNode<KeyType> *&_pRoot);

  static KeyType *Delete(BofAvlTree<KeyType> *_pAvlTree, KeyType *_pKey, BofAvlNode<KeyType> *&_pRoot, BOFCMP Cmp_E);

  BOFCMP Compare(uint32_t _Index_U32, KeyType *_pKey, BOFCMP _Cmp_E) const;

  /*!
   * Return the height of this tree
   */
  int32_t GetHeight() const;

  int32_t Check(uint32_t _Index_U32, uint32_t *_pNbNode_U32) const;

private:
  static BofAvlNode<KeyType> *Insert(BofAvlTree<KeyType> *_pAvlTree, KeyType *_pKey, BofAvlNode<KeyType> *_pParent, BofAvlNode<KeyType> *&_pRoot, int32_t &Change_S32);

  static KeyType *Delete(BofAvlTree<KeyType> *_pAvlTree, KeyType *_pKey, BofAvlNode<KeyType> *&_pRoot, int32_t &Change_S32, BOFCMP _Cmp_E);

  static int32_t RotateOnce(BofAvlNode<KeyType> *&_pRoot, WHICHAVLSUBTREE _Direction_E);

  static int32_t RotateTwice(BofAvlNode<KeyType> *&_pRoot, WHICHAVLSUBTREE _Direction_E);

  static int32_t ReBalance(BofAvlNode<KeyType> *&_pRoot);
};

// Template !

/*
 * File      : BofAvlNode.cpp
 *
 * Project   : Bof
 *
 * Package   : Bog-Include
 *
 * Company   : Sci
 *
 * Author    : Bernard HARMEL: b.harmel@gmail.com
 *
 * Purpose   : This is the implementation of the BofAvlTree template
 *
 * Copyright :(C) Sci
 *
 * Version History:
 * V 1.00  Wed Sep 6 2006  BHA : Initial release
 *
 */
template<typename KeyType>
BofAvlNode<KeyType>::BofAvlNode()
{
  Clear();
}


template<typename KeyType>
BofAvlNode<KeyType>::~BofAvlNode()
{
  // No it is destroyed by the tree object !   BOF_SAFE_DELETE(mpSubTree[LEFTSUBTREE]);
  // No it is destroyed by the tree object !   BOF_SAFE_DELETE(mpSubTree[RIGHTSUBTREE]);
}


/*!
 * Look for the given key,return nullptr if not found,otherwise return the item's address.
 */
template<typename KeyType>
BofAvlNode<KeyType> *BofAvlNode<KeyType>::Search(uint32_t _Index_U32, KeyType *_pKey, BofAvlNode<KeyType> *_pRoot, BOFCMP _Cmp_E)
{
  BOFCMP CmpResult_E, PrevCmpResult_E;

  BofAvlNode<KeyType> *pPrevRoot;

  if ((_Cmp_E == BOF_CMP_LESSOREQUAL) || (_Cmp_E == BOF_CMP_GREATEROREQUAL))
  {
    // Search equal
    pPrevRoot = nullptr;
    PrevCmpResult_E = BOF_CMP_EQUAL;

    while ((_pRoot) && ((CmpResult_E = _pRoot->Compare(_Index_U32, _pKey, BOF_CMP_EQUAL)) != BOF_CMP_EQUAL))
    {
      pPrevRoot = _pRoot;
      PrevCmpResult_E = CmpResult_E;
      _pRoot = _pRoot->mpSubTree[(CmpResult_E == BOF_CMP_LESS) ? LEFTSUBTREE : RIGHTSUBTREE];
    }

    if (!_pRoot)
    {
      // Not found
      if (pPrevRoot)
      {
        if (_Cmp_E == BOF_CMP_GREATEROREQUAL)
        {
          _pRoot = (PrevCmpResult_E == BOF_CMP_LESS) ? pPrevRoot : nullptr;

          // look for node in the whole half tree

          /*
           * look for >=141: l:CmpResult_E=BOF_CMP_LESS
           * 44 (g)
           \
           \ 142 (l)
           \ /     \
           \ 46(g)    143
           */
          while (!_pRoot)
          {
            _pRoot = pPrevRoot->GetSubtree(PARENTSUBTREE);

            if (_pRoot)
            {
              pPrevRoot = _pRoot;
              CmpResult_E = _pRoot->Compare(_Index_U32, _pKey, BOF_CMP_EQUAL);
              _pRoot = (CmpResult_E == BOF_CMP_LESS) ? _pRoot : nullptr;
            }
            else
            {
              break;
            }
          }
        }
        else
        {
          _pRoot = (PrevCmpResult_E == BOF_CMP_GREATER) ? pPrevRoot : nullptr;

          // look for node in the whole half tree

          /*
           * look for <=6: l:CmpResult_E=BOF_CMP_LESS
           * 3 (g)
           \
           \ 8 (l)
           \ /    \
           \ 7 (l)    9
           */
          while (!_pRoot)
          {
            _pRoot = pPrevRoot->GetSubtree(PARENTSUBTREE);

            if (_pRoot)
            {
              pPrevRoot = _pRoot;
              CmpResult_E = _pRoot->Compare(_Index_U32, _pKey, BOF_CMP_EQUAL);
              _pRoot = (CmpResult_E == BOF_CMP_GREATER) ? _pRoot : nullptr;
            }
            else
            {
              break;
            }
          }
        }
      }
    }
  }
  else
  {
    while ((_pRoot) && ((CmpResult_E = _pRoot->Compare(_Index_U32, _pKey, _Cmp_E)) != _Cmp_E))
    {
      _pRoot = _pRoot->mpSubTree[(CmpResult_E == BOF_CMP_LESS) ? LEFTSUBTREE : RIGHTSUBTREE];
    }
  }

  return (_pRoot) ? _pRoot : nullptr;
}


/*!
 * Insert the given key,return nullptr if it was inserted,otherwise return the existing item with the same key.
 */
template<typename KeyType>
BofAvlNode<KeyType> *BofAvlNode<KeyType>::Insert(BofAvlTree<KeyType> *_pAvlTree, KeyType *_pKey, BofAvlNode<KeyType> *&_pRoot)
{
  int32_t Change_S32;

  return Insert(_pAvlTree, _pKey, nullptr, _pRoot, Change_S32);
}


/*!
 * Delete the given key from the tree. Return the corresponding node,or return nullptr if it was not found.
 */
template<typename KeyType>
KeyType *BofAvlNode<KeyType>::Delete(BofAvlTree<KeyType> *_pAvlTree, KeyType *_pKey, BofAvlNode<KeyType> *&_pRoot, BOFCMP Cmp_E)
{
  int32_t Change_S32;

  return Delete(_pAvlTree, _pKey, _pRoot, Change_S32, Cmp_E);
}


template<typename KeyType>
int32_t BofAvlNode<KeyType>::GetHeight() const
{
  int32_t LeftHeight_S32 = (mpSubTree[LEFTSUBTREE]) ? mpSubTree[LEFTSUBTREE]->GetHeight() : 0;
  int32_t RightHeight_S32 = (mpSubTree[RIGHTSUBTREE]) ? mpSubTree[RIGHTSUBTREE]->GetHeight() : 0;

  return 1 + BOF_MAX(LeftHeight_S32, RightHeight_S32);
}


/*!
 * Verify this tree is a valid AVL tree, return true if it is, return false otherwise
 */
template<typename KeyType>
int32_t BofAvlNode<KeyType>::Check(uint32_t _Index_U32, uint32_t *_pNbNode_U32) const
{
  int32_t Rts_S32 = 1, LeftHeight_S32,
    RightHeight_S32,
    DiffHeight_S32;
  BOFTYPE KeyType_E;
  char pVal_c[128], pVal2_c[128];

  // First verify that subtrees are correct
  if (mpSubTree[LEFTSUBTREE])
  {
    (*_pNbNode_U32)++;
    Rts_S32 *= mpSubTree[LEFTSUBTREE]->Check(_Index_U32, _pNbNode_U32);
  }

  if (mpSubTree[RIGHTSUBTREE])
  {
    (*_pNbNode_U32)++;
    Rts_S32 *= mpSubTree[RIGHTSUBTREE]->Check(_Index_U32, _pNbNode_U32);
  }

  // Now get the height of each subtree
  LeftHeight_S32 = (mpSubTree[LEFTSUBTREE]) ? mpSubTree[LEFTSUBTREE]->GetHeight() : 0;
  RightHeight_S32 = (mpSubTree[RIGHTSUBTREE]) ? mpSubTree[RIGHTSUBTREE]->GetHeight() : 0;

  // Verify that AVL tree property is satisfied
  DiffHeight_S32 = RightHeight_S32 - LeftHeight_S32;
  GetKey(_Index_U32, &KeyType_E, sizeof(pVal_c), pVal_c);

  if ((LEFT_IMBALANCE(DiffHeight_S32)) || (RIGHT_IMBALANCE(DiffHeight_S32)))
  {
    Rts_S32 = 0;

    // BOFDBG_OUTPUT_0(DBG_DB, 0, "Height difference is %d at node %s\r\n", DiffHeight_S32, pVal_c);
  }

  // Verify that balance-factor is correct
  if (DiffHeight_S32 != mBalance_S32)
  {
    Rts_S32 = 0;

    // BOFDBG_OUTPUT_0(DBG_DB,0,"Height difference %d doesnt match balance-factor of %d at node %s\r\n", DiffHeight_S32, mBalance_S32, pVal_c);
  }

  // Verify that search-tree property is satisfied
  if ((mpSubTree[LEFTSUBTREE]) && (mpSubTree[LEFTSUBTREE]->Compare(_Index_U32, mpData, BOF_CMP_EQUAL) == BOF_CMP_LESS))
  {
    Rts_S32 = 0;
    mpSubTree[LEFTSUBTREE]->GetKey(_Index_U32, &KeyType_E, sizeof(pVal2_c), pVal2_c);

    // BOFDBG_OUTPUT_0(DBG_DB, 0, "LEFT: Node %s is *smaller* than left subtree %s\r\n", pVal_c, pVal2_c);
  }

  if ((mpSubTree[RIGHTSUBTREE]) && (mpSubTree[RIGHTSUBTREE]->Compare(_Index_U32, mpData, BOF_CMP_EQUAL) == BOF_CMP_GREATER))
  {
    Rts_S32 = 0;
    mpSubTree[RIGHTSUBTREE]->GetKey(_Index_U32, &KeyType_E, sizeof(pVal2_c), pVal2_c);

    // BOFDBG_OUTPUT_0(DBG_DB, 0, "RIGHT: Node %s is *greater* than right subtree %s\r\n", pVal_c, pVal2_c);
  }

  // Verify that parent link is valid
  if ((mpSubTree[LEFTSUBTREE]) && (mpSubTree[LEFTSUBTREE]->GetSubtree(PARENTSUBTREE))
      && (mpSubTree[LEFTSUBTREE]->GetSubtree(PARENTSUBTREE) != this))
  {
    Rts_S32 = 0;
    GetKey(_Index_U32, &KeyType_E, sizeof(pVal_c), pVal_c);
    mpSubTree[LEFTSUBTREE]->GetKey(_Index_U32, &KeyType_E, sizeof(pVal2_c), pVal2_c);

    // BOFDBG_OUTPUT_0(DBG_DB, 0, "LEFT: Node %s is not the parent of node %s\r\n", pVal_c, pVal2_c);
  }

  if ((mpSubTree[RIGHTSUBTREE]) && (mpSubTree[RIGHTSUBTREE]->GetSubtree(PARENTSUBTREE))
      && (mpSubTree[RIGHTSUBTREE]->GetSubtree(PARENTSUBTREE) != this))
  {
    Rts_S32 = 0;
    GetKey(_Index_U32, &KeyType_E, sizeof(pVal_c), pVal_c);
    mpSubTree[RIGHTSUBTREE]->GetKey(_Index_U32, &KeyType_E, sizeof(pVal2_c), pVal2_c);

    // BOFDBG_OUTPUT_0(DBG_DB, 0, "RIGHT: Node %s is not the parent of node %s\r\n", pVal_c, pVal2_c);
  }

  return Rts_S32;
}


/*!
 * Insert the given key into the given tree. Return the node if
 * it already exists. Otherwise return nullptr to indicate that
 * the key was successfully inserted.  Upon return, the "change"
 * parameter will be '1' if the tree height changed as a CmpResult_E
 * of the insertion (otherwise "change" will be 0).
 *
 */
template<typename KeyType>
BofAvlNode<KeyType> *BofAvlNode<KeyType>::Insert(BofAvlTree<KeyType> *_pAvlTree, KeyType *_pKey, BofAvlNode<KeyType> *_pParent, BofAvlNode<KeyType> *&_pRoot, int32_t &Change_S32)
{
  BofAvlNode<KeyType> *pRts = nullptr;
  int32_t Increase_S32;
  BOFCMP CmpResult_E;
  WHICHAVLSUBTREE Direction_E;

  // See if the tree is empty
  if (_pRoot == nullptr)
  {
    // Insert new node here
    // _pRoot=new BofAvlNode<KeyType>(_pAvlTree, _pKey);
    _pRoot = _pAvlTree->GetNextFreeNode();

    if (_pRoot)
    {
      _pRoot->Clear();
      _pRoot->SetData(_pKey);
      Change_S32 = HEIGHT_CHANGE;
      _pRoot->mpSubTree[PARENTSUBTREE] = _pParent;
    }
    else
    {
      pRts = (BofAvlNode<KeyType> *) 0x6996AA55; // Signal error
    }
  }
  else
  {
    // Initialize
    Increase_S32 = 0;

    // Compare items and determine which direction to search
    CmpResult_E = _pRoot->Compare(_pAvlTree->GetIndex(), _pKey, BOF_CMP_EQUAL);
    Direction_E = (CmpResult_E == BOF_CMP_LESS) ? LEFTSUBTREE : RIGHTSUBTREE;

    if (CmpResult_E != BOF_CMP_EQUAL)
    {
      // Insert into "Direction_E" subtree
      pRts = Insert(_pAvlTree, _pKey, _pRoot, _pRoot->mpSubTree[Direction_E], Change_S32);

      // if already here - dont insert !
      if (!pRts)
      {
        Increase_S32 = static_cast<int32_t>(CmpResult_E) * Change_S32;   // set balance factor increment
      }
    }
    else
    {
      // key already in tree at this node
      Increase_S32 = HEIGHT_NOCHANGE;
      pRts = _pRoot;
    }

    if (!pRts)
    {
      _pRoot->mBalance_S32 += Increase_S32;        // update balance factor

      /*
       * re-balance if needed -- height of current tree increases only if its
       * subtree height increases and the current tree needs no rotation.
       */
      Change_S32 = ((Increase_S32) && (_pRoot->mBalance_S32)) ? (1 - ReBalance(_pRoot)) : static_cast<int32_t>(HEIGHT_NOCHANGE);
    }
  }

  return pRts;
}


template<typename KeyType>
KeyType *BofAvlNode<KeyType>::Delete(BofAvlTree<KeyType> *_pAvlTree, KeyType *_pKey, BofAvlNode<KeyType> *&_pRoot, int32_t &Change_S32, BOFCMP _Cmp_E)
{
  KeyType *pRts;
  KeyType *pSuccessor;

  BofAvlNode<KeyType> *pParent;
  int32_t Decrease_S32;
  BOFCMP CmpResult_E;
  WHICHAVLSUBTREE Direction_E;
  bool Return_B;

  // See if the tree is empty
  if (_pRoot == nullptr)
  {
    Change_S32 = HEIGHT_NOCHANGE;
    pRts = nullptr;
  }
  else
  {
    // Initialize
    Decrease_S32 = 0;

    // Compare items and determine which direction to search
    CmpResult_E = _pRoot->Compare(_pAvlTree->GetIndex(), _pKey, _Cmp_E);
    Direction_E = (CmpResult_E == BOF_CMP_LESS) ? LEFTSUBTREE : RIGHTSUBTREE;

    if (CmpResult_E != BOF_CMP_EQUAL)
    {
      // Delete from "Direction_E" subtree
      pRts = Delete(_pAvlTree, _pKey, _pRoot->mpSubTree[Direction_E], Change_S32, _Cmp_E);

      // not found - can't delete
      if (pRts)
      {
        Return_B = false;
        Decrease_S32 = static_cast<int32_t>(CmpResult_E) * Change_S32;            // set balance factor decrement
      }
      else
      {
        Return_B = true;                                    // return(nullptr)
      }
    }
    else
    {                                                       // Found key at this node
      pRts = _pRoot->mpData;                                // set return value

      /*!
       * At this point we know "CmpResult_E" is zero and "_pRoot" points to
       * the node that we need to delete.  There are three cases:
       *
       * 1) The node is a leaf.  Remove it and return.
       *
       * 2) The node is a branch(has only 1 child). Make "root"
       * (the pointer to this node) point to the child.
       *
       * 3) The node has two children. We swap items with the successor
       * of "root"(the smallest item in its right subtree) and delete
       * the successor from the right subtree of "root".  The
       * identifier "Decrease_S32" should be reset if the subtree height
       * decreased due to the deletion of the successor of "root".
       */
      if ((_pRoot->mpSubTree[LEFTSUBTREE] == nullptr) && (_pRoot->mpSubTree[RIGHTSUBTREE] == nullptr))
      {
        _pAvlTree->SetNextFreeNode(_pRoot);
        Return_B = true;

        // We have a leaf -- remove it, no parent update
        // Static allocation				BOF_SAFE_DELETE(_pRoot);
        Change_S32 = HEIGHT_CHANGE;                         // height changed from 1 to 0
        _pRoot = nullptr;
      }
      else
      {
        if ((_pRoot->mpSubTree[LEFTSUBTREE] == nullptr) || (_pRoot->mpSubTree[RIGHTSUBTREE] == nullptr))
        {
          Return_B = true;

          // We have one child -- only child becomes new root
          pParent = _pRoot->mpSubTree[PARENTSUBTREE];
          _pAvlTree->SetNextFreeNode(_pRoot);
          _pRoot = _pRoot->mpSubTree[(_pRoot->mpSubTree[RIGHTSUBTREE]) ? RIGHTSUBTREE : LEFTSUBTREE];
          Change_S32 = HEIGHT_CHANGE; // We just shortened the subtree

          // Null-out the subtree pointers so we dont recursively delete
          _pRoot->mpSubTree[LEFTSUBTREE] = nullptr;
          _pRoot->mpSubTree[RIGHTSUBTREE] = nullptr;
          _pRoot->mpSubTree[PARENTSUBTREE] = pParent;

          // Static allocation						BOF_SAFE_DELETE(pToDelete);
        }
        else
        {
          Return_B = false;

          /*
           * We have two children -- find successor and replace our current data item with that of the successor
           * 3) The node has two children. We swap items with the successor
           * of "root"(the smallest item in its right subtree) and delete
           * the successor from the right subtree of "root".  The
           * identifier "Decrease_S32" should be reset if the subtree height
           * decreased due to the deletion of the successor of "root".
           */
          pParent = _pRoot->mpSubTree[PARENTSUBTREE];
          pSuccessor = Delete(_pAvlTree, _pKey, _pRoot->mpSubTree[RIGHTSUBTREE], Decrease_S32, BOF_CMP_LESS);

          // _pAvlTree->SetNextFreeNode( _pRoot );
          _pRoot->mpData = pSuccessor;
          _pRoot->mpSubTree[PARENTSUBTREE] = pParent;
        }
      }
    }

    if (!Return_B)
    {
      _pRoot->mBalance_S32 -= Decrease_S32;                 // update balance factor

      /*!
       * Rebalance if necessary -- the height of current tree changes if one
       * of two things happens:(1) a rotation was performed which changed
       * the height of the subtree(2) the subtree height decreased and now
       * matches the height of its other subtree(so the current tree now
       * has a zero balance when it previously did not).
       * ------------------------------------------------------------------------
       * Change_S32 =(Decrease_S32) ?((_pRoot->mBalance_S32) ? balance(_pRoot) : HEIGHT_CHANGE) : HEIGHT_NOCHANGE ;
       */
      if (Decrease_S32)
      {
        if (_pRoot->mBalance_S32)
        {
          Change_S32 = ReBalance(_pRoot);                   // rebalance and see if height changed
        }
        else
        {
          Change_S32 = HEIGHT_CHANGE;                       // balanced because subtree decreased
        }
      }
      else
      {
        Change_S32 = HEIGHT_NOCHANGE;
      }
    }
  }

  return pRts;
}


/*!
 * Perform an XX rotation for the given direction 'X'.
 * Return 1 if the tree height changes due to rotation,
 * otherwise return 0.
 *
 * An example of a single rotation is as follows:
 * Suppose I have a tree that looks like this:
 *
 *                  c
 *                 /
 *                b
 *
 * Now I insert the item "a" and get the resulting binary tree:
 *
 *                  c
 *                 /
 *                b
 *               /
 *              a
 *
 * Now, this resulting tree violates the "AVL criteria", the left
 * subtree has a height of 2 but the right subtree has a height of 0
 * so the difference in the two heights is "2" (which is greater than
 * 1). SO what we do is perform a "single rotation" (or RR for a single
 * right rotation, or LL for a single left rotation) on the tree (by
 * rotating the "c" element down clockwise to the right) to transform
 * it into the following tree:
 *
 *                   b
 *                  / \
 *                 a   c
 *
 *
 * ex insert 0,1,2
 *
 * _Direction_E=LEFTSUBTREE
 * 0 <- _pRoot		balance 2
 \
 \ 1							  1
 \
 \ 2						  0
 \
 \ 0 <- _pOldRoot
 \
 \ 1 -> _pRoot
 \
 \ 2
 \
 \ 1 -> _pRoot
 \ / \
 \ 0   2
 \
 */
template<typename KeyType>
int32_t BofAvlNode<KeyType>::RotateOnce(BofAvlNode<KeyType> *&_pRoot, WHICHAVLSUBTREE _Direction_E)
{
  WHICHAVLSUBTREE OtherDirection_E = Opposite(_Direction_E);

  BofAvlNode<KeyType> *pOldRoot = _pRoot;
  int32_t HeightChange_S32;

  /*!
   * See if OtherDirection_E subtree is balanced. If it is,then this
   * rotation will *not* change the overall tree height.
   * Otherwise,this rotation will shorten the tree height.
   */
  HeightChange_S32 = (_pRoot->mpSubTree[OtherDirection_E]->mBalance_S32 == 0) ? HEIGHT_NOCHANGE : HEIGHT_CHANGE;

  // assign new _pRoot
  _pRoot = pOldRoot->mpSubTree[OtherDirection_E];

  // new-root exchanges it's "Direction_E" SubTree for it's parent
  _pRoot->mpSubTree[PARENTSUBTREE] = pOldRoot->mpSubTree[PARENTSUBTREE];
  pOldRoot->mpSubTree[PARENTSUBTREE] = _pRoot;

  if (_pRoot->mpSubTree[_Direction_E])
  {
    _pRoot->mpSubTree[_Direction_E]->mpSubTree[PARENTSUBTREE] = pOldRoot;
  }

  pOldRoot->mpSubTree[OtherDirection_E] = _pRoot->mpSubTree[_Direction_E];
  _pRoot->mpSubTree[_Direction_E] = pOldRoot;

  // update balances
  pOldRoot->mBalance_S32 = -((_Direction_E == LEFTSUBTREE) ? --(_pRoot->mBalance_S32) : ++(_pRoot->mBalance_S32));

  return HeightChange_S32;
}


/*!
 * Perform an XY rotation for the given direction 'X'
 * Return 1 if the tree height changes due to rotation,
 * otherwise return 0.
 *
 * An example of a "double rotation" (or RL for a double right rotation,
 * or LR for a double left rotation) is the following:  Suppose I have
 * a tree that looks like this:
 *
 *                  a
 \
 \                    c
 \
 \ Now I insert the item "b" and get the resulting binary tree:
 \
 \                  a
 \
 \                    c
 \                   /
 \                  b
 \
 \ This resulting tree also violates the "AVL criteria" so we fix it
 \ by first rotating "c" down to the right (so we get "a-b-c"), and
 \ then rotating "a" down to the left so that the tree is transformed
 \ into this:
 \
 \                  b
 \                 / \
 \                a   c
 \
 \ ex insert 1,3,2
 \ _Direction_E=LEFTSUBTREE
 \ 1 <- _pRoot		balance 2
 \
 \ 3						  1
 \ /
 \ 2                          0
 \
 \ 1 <- _pOldRoot
 \
 \ 3 -> pOldOtherDirectionSubtree
 \ /
 \ 2	->_pRoot
 \
 \
 \       2
 \      / \
 \ 1   3
 */
template<typename KeyType>
int32_t BofAvlNode<KeyType>::RotateTwice(BofAvlNode<KeyType> *&_pRoot, WHICHAVLSUBTREE _Direction_E)
{
  WHICHAVLSUBTREE OtherDirection_E = Opposite(_Direction_E);

  BofAvlNode<KeyType> *pOldRoot = _pRoot;
  BofAvlNode<KeyType> *pOldOtherDirectionSubtree = _pRoot->mpSubTree[OtherDirection_E];

  // assign new root
  _pRoot = pOldRoot->mpSubTree[OtherDirection_E]->mpSubTree[_Direction_E];

  // new-root exchanges it's "Direction_E" SubTree for it's grandparent
  if (_pRoot->mpSubTree[RIGHTSUBTREE])
  {
    _pRoot->mpSubTree[RIGHTSUBTREE]->mpSubTree[PARENTSUBTREE] = (_Direction_E == LEFTSUBTREE) ? pOldOtherDirectionSubtree : pOldRoot;
  }

  if (_pRoot->mpSubTree[LEFTSUBTREE])
  {
    _pRoot->mpSubTree[LEFTSUBTREE]->mpSubTree[PARENTSUBTREE] = (_Direction_E == LEFTSUBTREE) ? pOldRoot : pOldOtherDirectionSubtree;
  }

  _pRoot->mpSubTree[PARENTSUBTREE] = pOldRoot->mpSubTree[PARENTSUBTREE];
  pOldOtherDirectionSubtree->mpSubTree[PARENTSUBTREE] = _pRoot;
  pOldRoot->mpSubTree[PARENTSUBTREE] = _pRoot;

  pOldRoot->mpSubTree[OtherDirection_E] = _pRoot->mpSubTree[_Direction_E];
  _pRoot->mpSubTree[_Direction_E] = pOldRoot;

  // new-root exchanges it's "other-Direction_E" SubTree for it's parent
  pOldOtherDirectionSubtree->mpSubTree[_Direction_E] = _pRoot->mpSubTree[OtherDirection_E];
  _pRoot->mpSubTree[OtherDirection_E] = pOldOtherDirectionSubtree;

  // update balances
  _pRoot->mpSubTree[LEFTSUBTREE]->mBalance_S32 = -BOF_MAX(_pRoot->mBalance_S32, 0);
  _pRoot->mpSubTree[RIGHTSUBTREE]->mBalance_S32 = -BOF_MIN(_pRoot->mBalance_S32, 0);
  _pRoot->mBalance_S32 = 0;

  // A double rotation always shortens the overall height of the tree
  return HEIGHT_CHANGE;
}


/*!
 * Rebalance a (sub)tree if it has become imbalanced
 */
template<typename KeyType>
int32_t BofAvlNode<KeyType>::ReBalance(BofAvlNode<KeyType> *&_pRoot)
{
  int32_t HeightChange_S32 = HEIGHT_NOCHANGE;

  if (LEFT_IMBALANCE(_pRoot->mBalance_S32))
  {
    // Need a right rotation
    if (_pRoot->mpSubTree[LEFTSUBTREE]->mBalance_S32 == RIGHT_HEAVY)
    {
      // RL rotation needed
      HeightChange_S32 = RotateTwice(_pRoot, RIGHTSUBTREE);
    }
    else
    {
      // RR rotation needed
      HeightChange_S32 = RotateOnce(_pRoot, RIGHTSUBTREE);
    }
  }
  else
  {
    if (RIGHT_IMBALANCE(_pRoot->mBalance_S32))
    {
      // Need a left rotation
      if (_pRoot->mpSubTree[RIGHTSUBTREE]->mBalance_S32 == LEFT_HEAVY)
      {
        // LR rotation needed
        HeightChange_S32 = RotateTwice(_pRoot, LEFTSUBTREE);
      }
      else
      {
        // LL rotation needed
        HeightChange_S32 = RotateOnce(_pRoot, LEFTSUBTREE);
      }
    }
  }

  return HeightChange_S32;
}


/*!
 * Perform a comparison of the given key against the given
 * item using the given criteria (min, max, or equivalence
 * comparison). Returns:
 * BOF_CMP_EQUAL if the keys are equivalent
 * BOF_CMP_LESS if this key is less than the item's key
 * BOF_CMP_GREATER if this key is greater than item's key
 */
template<typename KeyType>
BOFCMP BofAvlNode<KeyType>::Compare(uint32_t _Index_U32, KeyType *_pKey, BOFCMP Cmp_E) const
{
  BOFCMP Rts_E;

  switch (Cmp_E)
  {
    default:

    case BOF_CMP_EQUAL:
    {                                  // Standard comparison
      if (mpData)
      {
        Rts_E = mpData->Compare(_Index_U32, _pKey);
      }
      else
      {
        // A delete has occured !!
        Rts_E = BOF_CMP_LESS;
      }
    }
    break;

    case BOF_CMP_LESS:
    {                                  // Find the minimal element in this tree
      Rts_E = (mpSubTree[LEFTSUBTREE] == nullptr) ? BOF_CMP_EQUAL : BOF_CMP_LESS;
    }
    break;

    case BOF_CMP_GREATER:
    {                                  // Find the maximal element in this tree
      Rts_E = (mpSubTree[RIGHTSUBTREE] == nullptr) ? BOF_CMP_EQUAL : BOF_CMP_GREATER;
    }
    break;
  }

  return Rts_E;
}
END_BOF_NAMESPACE()