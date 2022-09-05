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

/*** Include files ***********************************************************/

#include <gtest/gtest.h>
#include <bofstd/bofnarytreekv.h>
 
#include <memory>
#include <iostream>

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

};

std::ostream &operator<<(std::ostream &_rOs, TREE_NODE const &_rArg)
{
	_rOs << "Val = " << _rArg.Val_U32 << ", Sts = " << _rArg.Val_S;
	return _rOs;
}

TEST(BofNaryTreeKv_Test, Create)
{
	uint32_t i_U32, HandleIndex_U32;
	std::string ToString_S;
	BOF::BOF_NARY_TREE_KV_PARAM BofNaryTreeKvParam_X;
	BofNaryTreeKvParam_X.MultiThreadAware_B = true;

	std::unique_ptr<BOF::BofNaryTreeKv<std::string, TREE_NODE>> puBofNaryTreeKv = std::make_unique<BOF::BofNaryTreeKv<std::string, TREE_NODE>>(BofNaryTreeKvParam_X);

	BOF::BofNaryTreeKv<std::string, TREE_NODE>::BofNaryTreeKvNodeHandle RootHandle, ParentHandle, pNodeHandle[32];
	TREE_NODE r(0, "Root"), n1(1, "Un"), n2(2, "Deux"),n3(3, "Trois"), n4(4, "Quatre"), n5(5, "Cinq"),n6(6, "Six"), n7(7, "Sept"), n8(8, "Huit"), n9(9, "Neuf");

	std::vector<std::string> KeyCollection;
	TREE_NODE TheNode_X;
/*
MountPoint
+--- Dir1
|    +--- FileA
|    +--- FileB
+--- Dir2
+--- Dir3
		+--- FileC
		+--- Dir4
		|    +--- FileE
		+--- FileD
*/
	HandleIndex_U32 = 0;
	RootHandle = nullptr;
	EXPECT_FALSE(puBofNaryTreeKv->IsNodeValid(RootHandle));
	EXPECT_EQ(puBofNaryTreeKv->SetRoot("MountPoint", r, &RootHandle), BOF_ERR_NO_ERROR);
	pNodeHandle[HandleIndex_U32++] = RootHandle;
	EXPECT_TRUE(puBofNaryTreeKv->IsNodeValid(RootHandle));


	EXPECT_EQ(puBofNaryTreeKv->AddChild(RootHandle, "Dir1", n1, &ParentHandle), BOF_ERR_NO_ERROR);
	pNodeHandle[HandleIndex_U32++] = ParentHandle;
	puBofNaryTreeKv->AddChild(ParentHandle, "FileA", n1, &pNodeHandle[HandleIndex_U32++]);
	puBofNaryTreeKv->AddChild(ParentHandle, "FileB", n1, &pNodeHandle[HandleIndex_U32++]);

	EXPECT_EQ(puBofNaryTreeKv->AddChild(RootHandle, "Dir2", n2, &ParentHandle), BOF_ERR_NO_ERROR);
	pNodeHandle[HandleIndex_U32++] = ParentHandle;

	EXPECT_EQ(puBofNaryTreeKv->AddChild(RootHandle, "Dir3", n3, &ParentHandle), BOF_ERR_NO_ERROR);
	pNodeHandle[HandleIndex_U32++] = ParentHandle;
	EXPECT_EQ(puBofNaryTreeKv->AddChild(ParentHandle, "FileC", n6, &pNodeHandle[HandleIndex_U32++]), BOF_ERR_NO_ERROR);
	EXPECT_EQ(puBofNaryTreeKv->AddChild(ParentHandle, "Dir4", n7, &pNodeHandle[HandleIndex_U32++]), BOF_ERR_NO_ERROR);
	EXPECT_EQ(puBofNaryTreeKv->AddChild(ParentHandle, "FileD", n8, &pNodeHandle[HandleIndex_U32++]), BOF_ERR_NO_ERROR);

	ParentHandle = pNodeHandle[HandleIndex_U32-2];
	EXPECT_EQ(puBofNaryTreeKv->AddChild(ParentHandle, "FileE", n9, &pNodeHandle[HandleIndex_U32++]), BOF_ERR_NO_ERROR);
		
	KeyCollection.clear();
	KeyCollection.push_back("MountPoint");
	KeyCollection.push_back("Dir4");
	KeyCollection.push_back("FileE");

	EXPECT_EQ(puBofNaryTreeKv->Search(KeyCollection, TheNode_X), BOF_ERR_NO_ERROR);
	BOFERR IsChildExist(const BofNaryTreeKvNodeHandle _ParentHandle, const KeyType & _rKey, BofNaryTreeKvNodeHandle * _pChildHandle, const DataType & _rData);

	ToString_S = puBofNaryTreeKv->ToString(RootHandle);
	std::cout << ToString_S;

	ParentHandle = pNodeHandle[HandleIndex_U32 - 3];
	puBofNaryTreeKv->ClearTree();

	ToString_S = puBofNaryTreeKv->ToString(RootHandle);
	std::cout << ToString_S;

}
