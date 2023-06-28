/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the tree class
 *
 * Name:        ut_tree.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  27 August 2022 16:51:15  b.harmel : Initial release
 */
#include <bofstd/bofnarytreekv.h>

#include "gtestrunner.h"

#include <iostream>
#include <memory>

struct TREE_NODE
{
  uint32_t Val_U32;
  std::string Val_S;
  TREE_NODE()
  {
    Reset();
  }
  TREE_NODE(uint32_t _Val_U32, const std::string &_rVal_S)
  {
    Val_U32 = _Val_U32;
    Val_S = _rVal_S;
  }
  void Reset()
  {
    Val_U32 = 0;
    Val_S = "";
  }
  bool operator==(const TREE_NODE &_rIt) const
  {
    return ((Val_U32 == _rIt.Val_U32) && (Val_S == _rIt.Val_S));
  }
  bool operator!=(const TREE_NODE &_rIt) const
  {
    return (!operator==(_rIt));
  }
};

std::ostream &operator<<(std::ostream &_rOs, TREE_NODE const &_rArg)
{
  _rOs << "Val = " << _rArg.Val_U32 << ", Sts = " << _rArg.Val_S;
  return _rOs;
}

TEST(NaryTreeKv_Test, Create)
{
  uint32_t i_U32, HandleIndex_U32;
  std::string ToString_S, Key_S;
  BOF::BOF_NARY_TREE_KV_PARAM BofNaryTreeKvParam_X;
  BofNaryTreeKvParam_X.MultiThreadAware_B = true;

  std::unique_ptr<BOF::BofNaryTreeKv<std::string, TREE_NODE>> puBofNaryTreeKv = std::make_unique<BOF::BofNaryTreeKv<std::string, TREE_NODE>>(BofNaryTreeKvParam_X);

  BOF::BofNaryTreeKv<std::string, TREE_NODE>::BofNaryTreeKvNodeHandle RootHandle, ParentHandle, pNodeHandle[32];
  BOF::BofNaryTreeKv<std::string, TREE_NODE>::BofNaryTreeKvNodeHandle SearchHandle1, SearchHandle2, SearchHandle3, NodeHandle;
  TREE_NODE Value_X, r(0, "Root"), n1(1, "Un"), n2(2, "Deux"), n3(3, "Trois"), n4(4, "Quatre"), n5(5, "Cinq"), n6(6, "Six"), n7(7, "Sept"), n8(8, "Huit"), n9(9, "Neuf");

  std::vector<std::string> KeyCollection;
  TREE_NODE TheNode_X;

  HandleIndex_U32 = 0;
  RootHandle = nullptr;
  EXPECT_FALSE(puBofNaryTreeKv->IsNodeValid(RootHandle));
  EXPECT_EQ(puBofNaryTreeKv->SetRoot("MountPoint", r, &RootHandle), BOF_ERR_NO_ERROR);
  pNodeHandle[HandleIndex_U32++] = RootHandle;
  EXPECT_TRUE(puBofNaryTreeKv->IsNodeValid(RootHandle));
  /*
  MountPoint [nullptr]
  */
  EXPECT_EQ(puBofNaryTreeKv->AddChild(RootHandle, "Dir1", n1, &ParentHandle), BOF_ERR_NO_ERROR);
  /*
  MountPoint [nullptr]
  +--- Dir1 [MountPoint]
  */
  pNodeHandle[HandleIndex_U32++] = ParentHandle;
  puBofNaryTreeKv->AddChild(ParentHandle, "FileA", n1, &pNodeHandle[HandleIndex_U32++]);
  /*
  MountPoint [nullptr]
  +--- Dir1 [MountPoint]
      +--- FileA [Dir1]
  */
  puBofNaryTreeKv->AddChild(ParentHandle, "FileB", n1, &pNodeHandle[HandleIndex_U32++]);
  /*
  MountPoint [nullptr]
  +--- Dir1 [MountPoint]
      +--- FileA [Dir1]
      +--- FileB [Dir1]
  */
  EXPECT_EQ(puBofNaryTreeKv->AddChild(RootHandle, "Dir2", n2, &ParentHandle), BOF_ERR_NO_ERROR);
  pNodeHandle[HandleIndex_U32++] = ParentHandle;
  /*
  MountPoint [nullptr]
  +--- Dir1 [MountPoint]
      +--- FileA [Dir1]
      +--- FileB [Dir1]
  +--- Dir2 [MountPoint]
  */
  EXPECT_EQ(puBofNaryTreeKv->AddChild(RootHandle, "Dir3", n3, &ParentHandle), BOF_ERR_NO_ERROR);
  pNodeHandle[HandleIndex_U32++] = ParentHandle;
  /*
  MountPoint [nullptr]
  +--- Dir1 [MountPoint]
      +--- FileA [Dir1]
      +--- FileB [Dir1]
  +--- Dir2 [MountPoint]
  +--- Dir3 [MountPoint]
  */
  EXPECT_EQ(puBofNaryTreeKv->AddChild(ParentHandle, "FileC", n6, &pNodeHandle[HandleIndex_U32++]), BOF_ERR_NO_ERROR);
  EXPECT_EQ(puBofNaryTreeKv->AddChild(ParentHandle, "Dir4", n7, &pNodeHandle[HandleIndex_U32++]), BOF_ERR_NO_ERROR);
  EXPECT_EQ(puBofNaryTreeKv->AddChild(ParentHandle, "FileD", n8, &pNodeHandle[HandleIndex_U32++]), BOF_ERR_NO_ERROR);
  ParentHandle = pNodeHandle[HandleIndex_U32 - 2];
  EXPECT_EQ(puBofNaryTreeKv->AddChild(ParentHandle, "FileE", n9, &pNodeHandle[HandleIndex_U32++]), BOF_ERR_NO_ERROR);
  EXPECT_EQ(puBofNaryTreeKv->AddChild(ParentHandle, "FileF", n9, &pNodeHandle[HandleIndex_U32++]), BOF_ERR_NO_ERROR);
  EXPECT_EQ(puBofNaryTreeKv->AddChild(ParentHandle, "FileG", n9, &pNodeHandle[HandleIndex_U32++]), BOF_ERR_NO_ERROR);
  /*
MountPoint [nullptr]
+--- Dir1 [MountPoint]
    +--- FileA [Dir1]
    +--- FileB [Dir1]
+--- Dir2 [MountPoint]
+--- Dir3 [MountPoint]
    +--- FileC [Dir3]
    +--- Dir4 [Dir3]
    |    +--- FileE [Dir4]
    |    +--- FileF [Dir4]
    |    +--- FileG [Dir4]
    +--- FileD [Dir3]
*/

#if 0
  ToString_S = puBofNaryTreeKv->ToString(true, RootHandle);
  std::cout << ToString_S;

  ParentHandle = pNodeHandle[HandleIndex_U32 - 3];	//Remove FileE
  ToString_S = puBofNaryTreeKv->ToString(true, ParentHandle);
  std::cout << ToString_S;
  puBofNaryTreeKv->ClearTree(ParentHandle);
  ToString_S = puBofNaryTreeKv->ToString(true, RootHandle);
  std::cout << ToString_S;

  ParentHandle = pNodeHandle[HandleIndex_U32 - 5];	//Remove Dir4
  ToString_S = puBofNaryTreeKv->ToString(true, ParentHandle);
  std::cout << ToString_S;
  puBofNaryTreeKv->ClearTree(ParentHandle);
  ToString_S = puBofNaryTreeKv->ToString(true, RootHandle);
  std::cout << ToString_S;

  //printf("%s\n", ToString_S.c_str());
  puBofNaryTreeKv->ClearTree(RootHandle);
#endif

  ToString_S = puBofNaryTreeKv->ToString(true, RootHandle);
  // std::cout << ToString_S;

  KeyCollection.clear();
  KeyCollection.push_back("MountPoint");
  KeyCollection.push_back("Dir3");
  KeyCollection.push_back("Dir4");
  KeyCollection.push_back("FileE");

  EXPECT_EQ(puBofNaryTreeKv->Search(RootHandle, KeyCollection, &SearchHandle1), BOF_ERR_NO_ERROR);
  EXPECT_TRUE(SearchHandle1 != nullptr);

  KeyCollection.clear();
  KeyCollection.push_back("Dir4");
  KeyCollection.push_back("FileE");
  ParentHandle = pNodeHandle[HandleIndex_U32 - 5]; // Dir4

  EXPECT_EQ(puBofNaryTreeKv->Search(ParentHandle, KeyCollection, &SearchHandle2), BOF_ERR_NO_ERROR);
  EXPECT_TRUE(SearchHandle1 == SearchHandle2);

  KeyCollection.clear();
  KeyCollection.push_back("MountPoint");
  KeyCollection.push_back("Dir3");
  KeyCollection.push_back("Dir4");
  KeyCollection.push_back("FileE");
  KeyCollection.push_back("FileZ");
  EXPECT_NE(puBofNaryTreeKv->Search(RootHandle, KeyCollection, &SearchHandle3), BOF_ERR_NO_ERROR);
  EXPECT_TRUE(SearchHandle1 == SearchHandle3);

  EXPECT_EQ(puBofNaryTreeKv->Key(RootHandle, Key_S), BOF_ERR_NO_ERROR);
  EXPECT_EQ(puBofNaryTreeKv->Value(RootHandle, Value_X), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Key_S.c_str(), "MountPoint");
  EXPECT_EQ(Value_X, r);

  EXPECT_EQ(puBofNaryTreeKv->GetNodeHandle(RootHandle, "Dir3", &NodeHandle), BOF_ERR_NO_ERROR);
  EXPECT_EQ(puBofNaryTreeKv->SetKey(NodeHandle, "Dir_3"), BOF_ERR_NO_ERROR);
  EXPECT_EQ(puBofNaryTreeKv->Key(NodeHandle, Key_S), BOF_ERR_NO_ERROR);
  EXPECT_NE(puBofNaryTreeKv->SetKey(NodeHandle, "Dir2"), BOF_ERR_NO_ERROR);
  EXPECT_EQ(puBofNaryTreeKv->Value(NodeHandle, Value_X), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Key_S.c_str(), "Dir_3");
  EXPECT_EQ(Value_X, n3);

  EXPECT_EQ(puBofNaryTreeKv->GetNodeHandle(NodeHandle, "Dir4", &NodeHandle), BOF_ERR_NO_ERROR);
  EXPECT_EQ(puBofNaryTreeKv->SetKey(NodeHandle, "Dir_4"), BOF_ERR_NO_ERROR);
  EXPECT_EQ(puBofNaryTreeKv->Key(NodeHandle, Key_S), BOF_ERR_NO_ERROR);
  EXPECT_EQ(puBofNaryTreeKv->Value(NodeHandle, Value_X), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Key_S.c_str(), "Dir_4");
  EXPECT_EQ(Value_X, n7);

  EXPECT_EQ(puBofNaryTreeKv->GetNodeHandle(NodeHandle, "FileE", &NodeHandle), BOF_ERR_NO_ERROR);
  EXPECT_EQ(puBofNaryTreeKv->Key(NodeHandle, Key_S), BOF_ERR_NO_ERROR);
  EXPECT_EQ(puBofNaryTreeKv->Value(NodeHandle, Value_X), BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Key_S.c_str(), "FileE");
  EXPECT_EQ(Value_X, n9);

  EXPECT_NE(puBofNaryTreeKv->GetNodeHandle(NodeHandle, "FileZ", &NodeHandle), BOF_ERR_NO_ERROR);

  ToString_S = puBofNaryTreeKv->ToString(true, RootHandle);
  // std::cout << ToString_S;

  ParentHandle = pNodeHandle[HandleIndex_U32 - 3]; // Remove FileE
  ToString_S = puBofNaryTreeKv->ToString(true, ParentHandle);
  // std::cout << ToString_S;
  puBofNaryTreeKv->ClearTree(ParentHandle);
  ToString_S = puBofNaryTreeKv->ToString(true, RootHandle);
  // std::cout << ToString_S;

  ParentHandle = pNodeHandle[HandleIndex_U32 - 5]; // Remove Dir4
  ToString_S = puBofNaryTreeKv->ToString(true, ParentHandle);
  // std::cout << ToString_S;
  puBofNaryTreeKv->ClearTree(ParentHandle);
  ToString_S = puBofNaryTreeKv->ToString(true, RootHandle);
  // std::cout << ToString_S;

  puBofNaryTreeKv->ClearTree(RootHandle);
  ToString_S = puBofNaryTreeKv->ToString(true, RootHandle);
  // std::cout << ToString_S;
}
