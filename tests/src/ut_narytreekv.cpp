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
TEST(BofTree_Test, Create)
{
	BOF::BOF_TREE_PARAM BofTreeParam_X;
	BofTreeParam_X.MultiThreadAware_B = true;
	std::unique_ptr<BOF::BofTree<TREE_NODE>> puBofTree = std::make_unique<BOF::BofTree<TREE_NODE>>(BofTreeParam_X);

	TREE_NODE Root_X(0, "Root");
	TREE_NODE Node_X;
	uint32_t i_U32, j_U32, k_U32;
#if 0
	typedef BOF::BofTree<TREE_NODE>::Node BofTreeNode;
	BofTreeNode Root(Root_X);
	BofTreeNode Node;
	//BOF::BofTree<TREE_NODE>::Node(Root_X);
	puBofTree->SetRoot(&Root);
	for (i_U32 = 0; i_U32 < 10; i_U32++)
	{
		Node_X = TREE_NODE(i_U32, "Child Layer 1");
		BofTreeNode Child(Node_X);
		Root.AddChild(std::move(Child));
		Node = Root.Child(i_U32);
		for (j_U32 = 0; j_U32 < i_U32; j_U32++)
		{
			Node_X = TREE_NODE(j_U32, "Child Layer 2");
			BofTreeNode Child(Node_X);
			Node.AddChild(std::move(Child));
		}
	}
	i_U32 = 0;
	for (auto It = puBofTree->begin(); It != puBofTree->end(); ++It, i_U32++)
	{
		Node_X = *It;
		printf("[%d] %d: %s\n", i_U32, Node_X.Val_U32, Node_X.Val_S.c_str());
	}
#endif
}
