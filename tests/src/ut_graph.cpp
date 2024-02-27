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
  std::unique_ptr<BOF::BofDirGraph<uint32_t>> mpuDirGraph = nullptr;

  void SetUp() override
  {
    BOF::BOF_DIR_GRAPH_PARAM BofDirGraphParam_X;
    BofDirGraphParam_X.MultiThreadAware_B = true;
    mpuDirGraph = std::make_unique<BOF::BofDirGraph<uint32_t>>(BofDirGraphParam_X);
  }
};

TEST_F(Graph_Test, CreateDestroy)
{
  uint32_t NodeId1_U32, NodeId2_U32, NodeId3_U32, EdgeId_U32;
  const uint32_t *pVal_U32;
  const std::vector<uint32_t> *pNeighbourCollection;
    
  pNeighbourCollection =mpuDirGraph->Neighbour(0);
  EXPECT_TRUE(pNeighbourCollection==nullptr);

  NodeId1_U32 = mpuDirGraph->InsertNode(42);
  EXPECT_TRUE(mpuDirGraph->NodeMap().Contain(NodeId1_U32));
  pVal_U32 = mpuDirGraph->Node(NodeId1_U32);
  EXPECT_TRUE(pVal_U32 != nullptr);

  EXPECT_FALSE(mpuDirGraph->NodeMap().Contain(NodeId1_U32+1));
  pVal_U32 = mpuDirGraph->Node(NodeId1_U32+1);
  EXPECT_TRUE(pVal_U32 == nullptr);

  NodeId2_U32 = mpuDirGraph->InsertNode(43);
  pNeighbourCollection = mpuDirGraph->Neighbour(NodeId1_U32);
  EXPECT_EQ(pNeighbourCollection->size(), 0);

  NodeId3_U32 = mpuDirGraph->InsertNode(44);

  EXPECT_EQ(mpuDirGraph->InsertEdge(0, NodeId2_U32),0);
  EXPECT_EQ(mpuDirGraph->InsertEdge(NodeId1_U32, 0),0);

  EXPECT_NE(mpuDirGraph->InsertEdge(NodeId1_U32, NodeId2_U32),0);
  pNeighbourCollection = mpuDirGraph->Neighbour(NodeId1_U32);
  EXPECT_EQ(pNeighbourCollection->size(), 1);
  EXPECT_EQ((*pNeighbourCollection)[0], NodeId2_U32);
  pNeighbourCollection = mpuDirGraph->Neighbour(NodeId2_U32);
  EXPECT_EQ(pNeighbourCollection->size(), 0);

  EdgeId_U32 = mpuDirGraph->InsertEdge(NodeId1_U32, NodeId3_U32);
  EXPECT_NE(EdgeId_U32,0);
  pNeighbourCollection = mpuDirGraph->Neighbour(NodeId1_U32);
  EXPECT_EQ(pNeighbourCollection->size(), 2);
  EXPECT_EQ((*pNeighbourCollection)[0], NodeId2_U32);
  EXPECT_EQ((*pNeighbourCollection)[1], NodeId3_U32);
  pNeighbourCollection = mpuDirGraph->Neighbour(NodeId2_U32);
  EXPECT_EQ(pNeighbourCollection->size(), 0);

  EXPECT_NE(mpuDirGraph->InsertEdge(NodeId2_U32, NodeId3_U32),0);
  pNeighbourCollection = mpuDirGraph->Neighbour(NodeId1_U32);
  EXPECT_EQ(pNeighbourCollection->size(), 2);
  EXPECT_EQ((*pNeighbourCollection)[0], NodeId2_U32);
  EXPECT_EQ((*pNeighbourCollection)[1], NodeId3_U32);
  pNeighbourCollection = mpuDirGraph->Neighbour(NodeId2_U32);
  EXPECT_EQ(pNeighbourCollection->size(), 1);
  EXPECT_EQ((*pNeighbourCollection)[0], NodeId3_U32);
  pNeighbourCollection = mpuDirGraph->Neighbour(NodeId3_U32);
  EXPECT_EQ(pNeighbourCollection->size(), 0);

  EXPECT_EQ(mpuDirGraph->NbEdgeFromNode(0), 0);
  EXPECT_EQ(mpuDirGraph->NbEdgeFromNode(NodeId1_U32), 2);
  EXPECT_EQ(mpuDirGraph->NbEdgeFromNode(NodeId2_U32), 1);
  EXPECT_EQ(mpuDirGraph->NbEdgeFromNode(NodeId3_U32), 0);

  EXPECT_FALSE(mpuDirGraph->EraseNode(0));
  EXPECT_TRUE(mpuDirGraph->EraseNode(NodeId2_U32));
  EXPECT_FALSE(mpuDirGraph->EraseNode(NodeId2_U32));
  EXPECT_EQ(mpuDirGraph->NbEdgeFromNode(NodeId1_U32), 1);
  EXPECT_EQ(mpuDirGraph->NbEdgeFromNode(NodeId2_U32), 0);
  EXPECT_EQ(mpuDirGraph->NbEdgeFromNode(NodeId3_U32), 0);

  EXPECT_FALSE(mpuDirGraph->EraseEdge(0));
  EXPECT_TRUE(mpuDirGraph->EraseEdge(EdgeId_U32));
  EXPECT_FALSE(mpuDirGraph->EraseEdge(0));
  EXPECT_EQ(mpuDirGraph->NbEdgeFromNode(NodeId1_U32), 0);
  EXPECT_EQ(mpuDirGraph->NbEdgeFromNode(NodeId2_U32), 0);
  EXPECT_EQ(mpuDirGraph->NbEdgeFromNode(NodeId3_U32), 0);

}

TEST_F(Graph_Test, InsertAndEraseNode)
{
  uint32_t NodeId_U32 = mpuDirGraph->InsertNode(42);
  ASSERT_TRUE(mpuDirGraph->NodeMap().Contain(NodeId_U32));

  mpuDirGraph->EraseNode(NodeId_U32);
  ASSERT_FALSE(mpuDirGraph->NodeMap().Contain(NodeId_U32));
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
  std::vector<uint32_t> VisitedNodeCollection;

  mpuDirGraph->InsertEdge(NodeA_U32, NodeB_U32);
  mpuDirGraph->InsertEdge(NodeB_U32, NodeC_U32);

  BOF::DfsTraverse(*mpuDirGraph, NodeA_U32, [&VisitedNodeCollection](uint32_t node) { VisitedNodeCollection.push_back(node); });

  ASSERT_EQ(VisitedNodeCollection.size(), 3);
  ASSERT_EQ(VisitedNodeCollection[0], NodeA_U32);
  ASSERT_EQ(VisitedNodeCollection[1], NodeB_U32);
  ASSERT_EQ(VisitedNodeCollection[2], NodeC_U32);
}

TEST_F(Graph_Test, FindAndContainsNode)
{
  uint32_t NodeId_U32 = mpuDirGraph->InsertNode(42);
  ASSERT_TRUE(mpuDirGraph->NodeMap().Contain(NodeId_U32));

  // Using find method to check if a node exists
  BOF::IdMap<uint32_t>::iterator iter = mpuDirGraph->NodeMap().Find(NodeId_U32);
  ASSERT_NE(iter, mpuDirGraph->NodeMap().end());

  iter = mpuDirGraph->NodeMap().Find(NodeId_U32 + 1);
  ASSERT_EQ(iter, mpuDirGraph->NodeMap().end());

  ASSERT_TRUE(mpuDirGraph->NodeMap().Contain(NodeId_U32));
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
