/*
 * Copyright (c) 2023-2033, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofgraph functions
 *
 * Name:        ut_graph.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Initial version from https://github.com/Nelarius/imnodes
 *
 * History:
 *
 * V 1.00  Feb 5 2024  BHA : Initial release
 */
#include "gtestrunner.h"
#include <bofstd/bofgraph.h>

class Graph_Test : public ::testing::Test
{
protected:
  std::unique_ptr<BOF::BofDirGraph<uint32_t>> mpuDirGraph=nullptr;

  void SetUp() override
  {
    BOF::BOF_DIR_GRAPH_PARAM BofDirGraphParam_X;
    BofDirGraphParam_X.MultiThreadAware_B = true;
    mpuDirGraph = std::make_unique<BOF::BofDirGraph<uint32_t>>(BofDirGraphParam_X);
  }
};

TEST_F(Graph_Test, InsertAndEraseNode)
{
  uint32_t node_id = mpuDirGraph->InsertNode(42);
  ASSERT_TRUE(mpuDirGraph->NodeMap().Contain(node_id));

  mpuDirGraph->EraseNode(node_id);
  ASSERT_FALSE(mpuDirGraph->NodeMap().Contain(node_id));
}

TEST_F(Graph_Test, InsertAndEraseEdge)
{
  uint32_t NodeA_U32 = mpuDirGraph->InsertNode(1);
  uint32_t NodeB_U32 = mpuDirGraph->InsertNode(2);
  uint32_t EdgeId_U32 = mpuDirGraph->InsertEdge(NodeA_U32, NodeB_U32);
  ASSERT_TRUE(mpuDirGraph->EdgeMap().Contain(EdgeId_U32));

  mpuDirGraph->EraseEdge(EdgeId_U32);
  ASSERT_FALSE(mpuDirGraph->EdgeMap().Contain(EdgeId_U32));
}

TEST_F(Graph_Test, DFSTraversal)
{
  uint32_t NodeA_U32 = mpuDirGraph->InsertNode(1);
  uint32_t NodeB_U32 = mpuDirGraph->InsertNode(2);
  uint32_t NodeC_U32 = mpuDirGraph->InsertNode(3);

  mpuDirGraph->InsertEdge(NodeA_U32, NodeB_U32);
  mpuDirGraph->InsertEdge(NodeB_U32, NodeC_U32);

  std::vector<uint32_t> visited_nodes;
  BOF::DfsTraverse(*mpuDirGraph, NodeA_U32, [&visited_nodes](uint32_t node) { visited_nodes.push_back(node); });

  ASSERT_EQ(visited_nodes.size(), 3);
  ASSERT_EQ(visited_nodes[0], NodeA_U32);
  ASSERT_EQ(visited_nodes[1], NodeB_U32);
  ASSERT_EQ(visited_nodes[2], NodeC_U32);
}

TEST_F(Graph_Test, FindAndContainsNode)
{
  uint32_t node_id = mpuDirGraph->InsertNode(42);
  ASSERT_TRUE(mpuDirGraph->NodeMap().Contain(node_id));

  // Using find method to check if a node exists
  BOF::IdMap<uint32_t>::iterator iter = mpuDirGraph->NodeMap().Find(node_id);
  ASSERT_NE(iter,  mpuDirGraph->NodeMap().end());

  iter = mpuDirGraph->NodeMap().Find(node_id+1);
  ASSERT_EQ(iter, mpuDirGraph->NodeMap().end());

  ASSERT_TRUE(mpuDirGraph->NodeMap().Contain(node_id));
}

TEST_F(Graph_Test, FindAndContainsEdge)
{
  uint32_t NodeA_U32 = mpuDirGraph->InsertNode(1);
  uint32_t NodeB_U32 = mpuDirGraph->InsertNode(2);
  uint32_t EdgeId_U32 = mpuDirGraph->InsertEdge(NodeA_U32, NodeB_U32);
  ASSERT_TRUE(mpuDirGraph->EdgeMap().Contain(EdgeId_U32));

  // Using find_edge method to check if an edge exists
  BOF::IdMap<BOF::BofDirGraph<uint32_t>::Edge>::iterator iter = mpuDirGraph->EdgeMap().Find(EdgeId_U32);
  ASSERT_NE(iter, mpuDirGraph->EdgeMap().end());

  ASSERT_TRUE(mpuDirGraph->EdgeMap().Contain(EdgeId_U32));
}

TEST_F(Graph_Test, EraseNodeWithEdges)
{
  uint32_t NodeA_U32 = mpuDirGraph->InsertNode(1);
  uint32_t NodeB_U32 = mpuDirGraph->InsertNode(2);
  uint32_t NodeC_U32 = mpuDirGraph->InsertNode(3);

  mpuDirGraph->InsertEdge(NodeA_U32, NodeB_U32);
  mpuDirGraph->InsertEdge(NodeB_U32, NodeC_U32);

  ASSERT_EQ(mpuDirGraph->NbEdgeFromNode(NodeB_U32), 1);

  mpuDirGraph->EraseNode(NodeB_U32);

  ASSERT_FALSE(mpuDirGraph->NodeMap().Contain(NodeB_U32));
  ASSERT_EQ(mpuDirGraph->NbEdgeFromNode(NodeA_U32), 0);
  ASSERT_EQ(mpuDirGraph->NbEdgeFromNode(NodeC_U32), 0);
}
