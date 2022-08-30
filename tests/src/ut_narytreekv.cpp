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

TEST(BofNaryTreeKv_Test, Create)
{
	uint32_t i_U32;
	std::string ToString_S;
	BOF::BOF_NARY_TREE_KV_PARAM BofNaryTreeKvParam_X;
	BofNaryTreeKvParam_X.MultiThreadAware_B = true;
	std::unique_ptr<BOF::BofNaryTreeKv<std::string, TREE_NODE>> puBofNaryTreeKv = std::make_unique<BOF::BofNaryTreeKv<std::string, TREE_NODE>>(BofNaryTreeKvParam_X);
	BOF::BofNaryTreeKv<std::string, TREE_NODE>::BofNaryTreeKvNodeHandle RootHandle, pChildHandle[32];

	TREE_NODE Root_X(0, "Root");
	RootHandle = reinterpret_cast<BOF::BofNaryTreeKv<std::string, TREE_NODE>::BofNaryTreeKvNodeHandle>(0x12345678);
	//EXPECT_FALSE(puBofNaryTreeKv->IsNodeValid(RootHandle));
	int nv = 10;
	TREE_NODE r(0,"Root"), n1(1,"Un"), n2(2,"Deux"),
		n3(3,"Trois"), n4(4,"Quatre"), n5(5,"Cinq"),
		n6(6,"Six"), n7(7,"Sept"), n8(8,"Huit"), n9(9,"Neuf");

	RootHandle = nullptr;
	EXPECT_FALSE(puBofNaryTreeKv->IsNodeValid(RootHandle));
	EXPECT_EQ(puBofNaryTreeKv->SetRoot("/", r, &RootHandle), BOF_ERR_NO_ERROR);
	EXPECT_TRUE(puBofNaryTreeKv->IsNodeValid(RootHandle));

	// Tree Formation
	puBofNaryTreeKv->AddChild(RootHandle, "dir1", n1, &pChildHandle[0]);
	puBofNaryTreeKv->AddChild(RootHandle, "dir2", n2, &pChildHandle[1]);
	puBofNaryTreeKv->AddChild(RootHandle, "dir3", n3, &pChildHandle[2]);
	/*
	r.root.push_back(&n1);
	n1.root.push_back(&n4);
	n1.root.push_back(&n5);
	r.root.push_back(&n2);
	r.root.push_back(&n3);
	n3.root.push_back(&n6);
	n3.root.push_back(&n7);
	n7.root.push_back(&n9);
	n3.root.push_back(&n8);
	*/
	/*
	TREE_NODE Node_X;
	for (i_U32 = 0; i_U32 < 10; i_U32++)
	{
		Node_X = TREE_NODE(i_U32, "Child Layer 1");
		pChildHandle[i_U32] = nullptr;
		EXPECT_FALSE(puBofNaryTreeKv->IsNodeValid(pChildHandle[i_U32]));
		EXPECT_EQ(puBofNaryTreeKv->AddChild(RootHandle, &Node_X, &pChildHandle[i_U32]), BOF_ERR_NO_ERROR);
		EXPECT_NE(pChildHandle[i_U32], nullptr);
		EXPECT_TRUE(puBofNaryTreeKv->IsNodeValid(pChildHandle[i_U32]));
	}
	*/
	ToString_S = puBofNaryTreeKv->ToString(RootHandle);
	std::cout << ToString_S;
}
