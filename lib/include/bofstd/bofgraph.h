/*
This C++ code defines a simple directed graph implementation along with supporting data structures. Let me provide you with a breakdown of the code:

1. **`Span` Template Structure:**
   - Represents a non-owning view of a contiguous sequence of elements.
   - It has `begin()` and `end()` functions for iteration.

2. **`IdMap` Template Class:**
   - A map-like container where elements are associated with unique integer IDs.
   - Supports insertion, deletion, and lookup based on ID.
   - Uses two vectors: one for elements and another for sorted IDs for efficient search.

3. **`Graph` Template Class:**
   - Represents a directed graph with nodes and edges.
   - Nodes are associated with IDs, and each node contains information of type `NodeType`.
   - Edges are represented by the `Edge` structure.
   - The graph uses several `IdMap` instances to manage nodes, edges, and related information.
   - Provides functions to insert and erase nodes and edges, access node information, and traverse the graph using depth-first search (DFS).

4. **`dfs_traverse` Function:**
   - Performs a depth-first traversal of the graph starting from a specified node.
   - It uses a stack to keep track of nodes to visit.
   - The provided `Visitor` function is called for each visited node.

5. **General Information:**
   - The code is enclosed in the `example` namespace.
   - It uses modern C++ features, such as templates and iterators.
   - Assertions (`BOF_ASSERT`) are used for runtime checks, ensuring certain conditions hold during development.

6. **Template Metaprogramming:**
   - The code leverages template metaprogramming to handle generic types for nodes and elements.

Overall, this code provides a flexible and extensible framework for working with directed graphs and non-owning spans of elements. It's designed to be efficient and generic, allowing users to customize the types of nodes and elements in the graph.

A directed graph (or digraph) is a graph that is made up of a set of vertices (nodes) and a set of directed edges. In a directed graph, each edge has a specific direction, indicating a one-way connection between two vertices. This means that if there is an edge from vertex A to vertex B, it doesn't necessarily imply that there is an edge from B to A.

An implementation of a directed graph typically involves data structures and algorithms to manage the vertices, edges, and relationships between them. Here are some common components of a directed graph implementation:

Vertices (Nodes):

Each vertex represents a distinct element or entity in the graph.
Vertices are often associated with unique identifiers or labels.
Edges:

Directed edges connect pairs of vertices, indicating the direction of the relationship.
An edge from vertex A to vertex B is not the same as an edge from B to A.
Adjacency List or Matrix:

An adjacency list or matrix is used to represent the relationships between vertices.
Adjacency List: For each vertex, store a list of vertices to which it has outgoing edges.
Adjacency Matrix: Use a 2D matrix to represent all possible edges between vertices.
Graph Operations:

Common operations include adding and removing vertices and edges.
Traversal algorithms, such as depth-first search (DFS) and breadth-first search (BFS), are used to explore the graph.
Directed Graph Algorithms:

Directed graphs often require algorithms specific to their directed nature, such as topological sorting and finding strongly connected components.
Cyclic and Acyclic Graphs:

A directed graph may be cyclic (contains cycles) or acyclic (does not contain cycles).
Detecting cycles and managing acyclic graphs are important considerations.
Here's a simple example of a directed graph implemented in C++ using the provided code:

cpp
Copy code
#include "your_graph_library.h"

int main() {
    using namespace example;

    // Create a directed graph with integer nodes
    Graph<uint32_t> myGraph;

    // Insert nodes and edges
    uint32_t nodeA = myGraph.insert_node(1);
    uint32_t nodeB = myGraph.insert_node(2);
    uint32_t nodeC = myGraph.insert_node(3);

    myGraph.insert_edge(nodeA, nodeB);
    myGraph.insert_edge(nodeB, nodeC);

    // Perform DFS traversal
    dfs_traverse(myGraph, nodeA, [](uint32_t node) {
        // Process each visited node
        // ...
    });

    return 0;
}
This example demonstrates the basic structure of a directed graph with nodes and edges, and it performs a depth-first traversal starting from a specific node.
*/
#pragma once
#include <bofstd/bofstd.h>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <stack>
#include <stddef.h>
#include <utility>
#include <vector>
#include <mutex>

BEGIN_BOF_NAMESPACE()
struct BOF_DIR_GRAPH_PARAM
{
  bool MultiThreadAware_B;   /*! true if the object is used in a multi threaded application (use mCircularBufferMtx_X)*/

  BOF_DIR_GRAPH_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    MultiThreadAware_B = false;
  }
};
template <typename ElementType>
struct Span
{
  using iterator = ElementType *;

  template <typename Container>
  Span(Container &_Container)
      : mBegin(_Container.data()), mEnd(mBegin + _Container.size())
  {
  }
  iterator begin() const
  {
    return mBegin;
  }
  iterator end() const
  {
    return mEnd;
  }
  bool Contain(ElementType id);

private:
  iterator mBegin;
  iterator mEnd;
};

template <typename ElementType>
bool Span<ElementType>::Contain(ElementType _ElementType)
{
  return (std::find(mBegin, mEnd, _ElementType) != mEnd);
}

template <typename ElementType>
class IdMap
{
public:
  using iterator = typename std::vector<ElementType>::iterator;
  using const_iterator = typename std::vector<ElementType>::const_iterator;

  const_iterator begin() const
  {
    return mElementCollection.begin();
  }
  const_iterator end() const
  {
    return mElementCollection.end();
  }

  Span<const ElementType> Element() const
  {
    return mElementCollection;
  }

  bool Empty() const
  {
    return mSortedIdCollection.empty();
  }
  size_t Size() const
  {
    return mSortedIdCollection.size();
  }

  std::pair<iterator, bool> Insert(uint32_t _Id_U32, const ElementType &_rElement);
  std::pair<iterator, bool> Insert(uint32_t _Id_U32, ElementType &&_rrElement);
  bool Erase(uint32_t _Id_U32);
  void Clear();

  iterator Find(uint32_t _Id_U32);
  const_iterator Find(uint32_t _Id_U32) const;
  bool Contain(uint32_t _Id_U32) const;

private:
  std::vector<ElementType> mElementCollection;
  std::vector<uint32_t> mSortedIdCollection;
};

template <typename ElementType>
std::pair<typename IdMap<ElementType>::iterator, bool> IdMap<ElementType>::Insert(const uint32_t _Id_U32, const ElementType &_rElement)
{
  std::pair<typename IdMap<ElementType>::iterator, bool> Rts;

  auto LowerBound = std::lower_bound(mSortedIdCollection.begin(), mSortedIdCollection.end(), _Id_U32);
  if (LowerBound != mSortedIdCollection.end() && _Id_U32 == *LowerBound)
  {
    Rts = std::make_pair(std::next(mElementCollection.begin(), std::distance(mSortedIdCollection.begin(), LowerBound)), false);
  }
  else
  {
    auto InsertElementAt = std::next(mElementCollection.begin(), std::distance(mSortedIdCollection.begin(), LowerBound));
    mSortedIdCollection.insert(LowerBound, _Id_U32);
    Rts = std::make_pair(mElementCollection.insert(InsertElementAt, _rElement), true);
  }
  return Rts;
}

template <typename ElementType>
std::pair<typename IdMap<ElementType>::iterator, bool> IdMap<ElementType>::Insert(const uint32_t _Id_U32, ElementType &&_rrElement)
{
  std::pair<typename IdMap<ElementType>::iterator, bool> Rts;
  auto LowerBound = std::lower_bound(mSortedIdCollection.begin(), mSortedIdCollection.end(), _Id_U32);

  if (LowerBound != mSortedIdCollection.end() && _Id_U32 == *LowerBound)
  {
    Rts = std::make_pair(std::next(mElementCollection.begin(), std::distance(mSortedIdCollection.begin(), LowerBound)), false);
  }
  else
  {
    auto InsertElementAt = std::next(mElementCollection.begin(), std::distance(mSortedIdCollection.begin(), LowerBound));
    mSortedIdCollection.insert(LowerBound, _Id_U32);
    Rts = std::make_pair(mElementCollection.insert(InsertElementAt, std::move(_rrElement)), true);
  }
  return Rts;
}

template <typename ElementType>
bool IdMap<ElementType>::Erase(const uint32_t _Id_U32)
{
  bool Rts_B;
  auto LowerBound = std::lower_bound(mSortedIdCollection.begin(), mSortedIdCollection.end(), _Id_U32);

  if ((LowerBound == mSortedIdCollection.end()) || (_Id_U32 != *LowerBound))
  {
    Rts_B = false;
  }
  else
  {
    auto EraseElementAt = std::next(mElementCollection.begin(), std::distance(mSortedIdCollection.begin(), LowerBound));
    mSortedIdCollection.erase(LowerBound);
    mElementCollection.erase(EraseElementAt);
    Rts_B = true;
  }
  return Rts_B;
}

template <typename ElementType>
void IdMap<ElementType>::Clear()
{
  mElementCollection.clear();
  mSortedIdCollection.clear();
}

template <typename ElementType>
typename IdMap<ElementType>::iterator IdMap<ElementType>::Find(const uint32_t _Id_U32)
{
  const auto LowerBound = std::lower_bound(mSortedIdCollection.cbegin(), mSortedIdCollection.cend(), _Id_U32);
  return ((LowerBound == mSortedIdCollection.cend()) || (*LowerBound != _Id_U32)) ? mElementCollection.end() : mElementCollection.begin() + std::distance(mSortedIdCollection.cbegin(), LowerBound);
}

template <typename ElementType>
typename IdMap<ElementType>::const_iterator IdMap<ElementType>::Find(const uint32_t _Id_U32) const
{
  const auto LowerBound = std::lower_bound(mSortedIdCollection.cbegin(), mSortedIdCollection.cend(), _Id_U32);
  return ((LowerBound == mSortedIdCollection.cend()) || (*LowerBound != _Id_U32)) ? mElementCollection.cend() : std::next(mElementCollection.cbegin(), std::distance(mSortedIdCollection.cbegin(), LowerBound));
}

template <typename ElementType>
bool IdMap<ElementType>::Contain(const uint32_t _Id_U32) const
{
  bool Rts_B;

  const auto LowerBound = std::lower_bound(mSortedIdCollection.cbegin(), mSortedIdCollection.cend(), _Id_U32);
  if (LowerBound == mSortedIdCollection.cend())
  {
    Rts_B= false;
  }
  else
  {
    Rts_B = (*LowerBound == _Id_U32);
  }
  return Rts_B;
}

// a very simple directional graph
template <typename NodeType>
class BofDirGraph
{
public:
  BofDirGraph(const BOF_DIR_GRAPH_PARAM &_rBofDirGraphParam_X): mCrtId(0), mNodeCollection(), mEdgeFromNodeCollection(), mNodeNeighborCollection(), mEdgeCollection()
  {
    mBofDirGraphParam_X = _rBofDirGraphParam_X;
  }

  struct Edge
  {
    uint32_t Id_U32;
    uint32_t From_U32;
    uint32_t To_U32;

    Edge() = default;
    Edge(const uint32_t _Id_U32, const uint32_t _From_U32, const uint32_t _To_U32)
        : Id_U32(_Id_U32), From_U32(_From_U32), To_U32(_To_U32)
    {
    }
    inline uint32_t Opposite(const uint32_t _Id_U32) const
    {
      return (_Id_U32 == From_U32) ? To_U32 : From_U32;
    }
    inline bool Contain(const uint32_t _Id_U32) const
    {
      return ((_Id_U32 == From_U32) || (_Id_U32 == To_U32));
    }
  };

  // Element access
  NodeType &Node(uint32_t _Id_U32);
  const NodeType &Node(uint32_t _Id_U32) const;
  Span<const uint32_t> Neighbor(uint32_t _Id_U32) const;
  Span<const Edge> Edges() const;
  IdMap<NodeType> &NodeMap();
  IdMap<Edge> &EdgeMap();

  size_t NbEdgeFromNode(uint32_t _Id_U32) const;

  uint32_t InsertNode(const NodeType &node);
  void EraseNode(uint32_t _Id_U32);

  uint32_t InsertEdge(uint32_t from, uint32_t to);
  void EraseEdge(uint32_t EdgeId_U32);

private:
  void InternalEraseEdge(uint32_t EdgeId_U32);

  BOF_DIR_GRAPH_PARAM mBofDirGraphParam_X;
  mutable std::mutex mMtx;
  std::atomic<uint32_t> mCrtId;
  // These contains map to the node id
  IdMap<NodeType> mNodeCollection;
  IdMap<uint32_t> mEdgeFromNodeCollection;
  IdMap<std::vector<uint32_t>> mNodeNeighborCollection;

  // This container maps to the edge id
  IdMap<Edge> mEdgeCollection;
};

template <typename NodeType>
NodeType &BofDirGraph<NodeType>::Node(const uint32_t _Id_U32)
{
  return const_cast<NodeType &>(static_cast<const BofDirGraph *>(this)->node(_Id_U32));
}

template <typename NodeType>
const NodeType &BofDirGraph<NodeType>::Node(const uint32_t _Id_U32) const
{
  std::lock_guard<std::mutex> Lock(mMtx);
  const auto iter = mNodeCollection.find(id);
  BOF_ASSERT(iter != mNodeCollection.end());
  return *iter;
}

template <typename NodeType>
IdMap<NodeType> &BofDirGraph<NodeType>::NodeMap()
{
  return mNodeCollection;
}
template <typename NodeType>
IdMap<typename BofDirGraph<NodeType>::Edge> &BofDirGraph<NodeType>::EdgeMap()
{
  return mEdgeCollection;
}

template <typename NodeType>
Span<const uint32_t> BofDirGraph<NodeType>::Neighbor(uint32_t _Id_U32) const
{
  std::lock_guard<std::mutex> Lock(mMtx);
  const auto iter = mNodeNeighborCollection.Find(_Id_U32);
  BOF_ASSERT(iter != mNodeNeighborCollection.end());
  return *iter;
}

template <typename NodeType>
Span<const typename BofDirGraph<NodeType>::Edge> BofDirGraph<NodeType>::Edges() const
{
  return mEdgeCollection.Element();
}

template <typename NodeType>
size_t BofDirGraph<NodeType>::NbEdgeFromNode(const uint32_t _Id_U32) const
{
  std::lock_guard<std::mutex> Lock(mMtx);
  auto Rts = mEdgeFromNodeCollection.Find(_Id_U32);
  BOF_ASSERT(Rts != mEdgeFromNodeCollection.end());
  return *Rts;
}

template <typename NodeType>
uint32_t BofDirGraph<NodeType>::InsertNode(const NodeType &_rNode)
{
  std::lock_guard<std::mutex> Lock(mMtx);
  const uint32_t Rts_U32 = mCrtId++;
  BOF_ASSERT(!mNodeCollection.Contain(Rts_U32));
  mNodeCollection.Insert(Rts_U32, _rNode);
  mEdgeFromNodeCollection.Insert(Rts_U32, 0);
  mNodeNeighborCollection.Insert(Rts_U32, std::vector<uint32_t>());
  return Rts_U32;
}

template <typename NodeType>
void BofDirGraph<NodeType>::EraseNode(const uint32_t _Id_U32)
{
  std::lock_guard<std::mutex> Lock(mMtx);
  // first, remove any potential dangling edges
  {
    static std::vector<uint32_t> EdgeToEraseCollection;

    for (const Edge &rEdge : mEdgeCollection.Element())
    {
      if (rEdge.Contain(_Id_U32))
      {
        EdgeToEraseCollection.push_back(rEdge.Id_U32);
      }
    }

    for (const uint32_t EdgeId_U32 : EdgeToEraseCollection)
    {
      InternalEraseEdge(EdgeId_U32);
    }

    EdgeToEraseCollection.clear();
  }

  mNodeCollection.Erase(_Id_U32);
  mEdgeFromNodeCollection.Erase(_Id_U32);
  mNodeNeighborCollection.Erase(_Id_U32);
}

template <typename NodeType>
uint32_t BofDirGraph<NodeType>::InsertEdge(const uint32_t _From_U32, const uint32_t _To_U32)
{
  std::lock_guard<std::mutex> Lock(mMtx);
  const uint32_t Id_U32 = mCrtId++;
  BOF_ASSERT(!mEdgeCollection.Contain(Id_U32));
  BOF_ASSERT(mNodeCollection.Contain(_From_U32));
  BOF_ASSERT(mNodeCollection.Contain(_To_U32));
  mEdgeCollection.Insert(Id_U32, Edge(Id_U32, _From_U32, _To_U32));

  // update neighbor count
  BOF_ASSERT(mEdgeFromNodeCollection.Contain(_From_U32));
  *mEdgeFromNodeCollection.Find(_From_U32) += 1;
  // update neighbor list
  BOF_ASSERT(mNodeNeighborCollection.Contain(_From_U32));
  mNodeNeighborCollection.Find(_From_U32)->push_back(_To_U32);

  return Id_U32;
}

template <typename NodeType>
void BofDirGraph<NodeType>::EraseEdge(const uint32_t _EdgeId_U32)
{
  std::lock_guard<std::mutex> Lock(mMtx);
  InternalEraseEdge(_EdgeId_U32);
}

template <typename NodeType, typename Visitor>
void DfsTraverse(const BofDirGraph<NodeType> &_rGraph, const uint32_t _StartNode_U32, Visitor _Visitor)
{
  std::stack<uint32_t> Stack;

  Stack.push(_StartNode_U32);
  while (!Stack.empty())
  {
    const uint32_t CrtNodeId_U32 = Stack.top();
    Stack.pop();

    _Visitor(CrtNodeId_U32);

    for (const uint32_t Neighbor_U32 : _rGraph.Neighbor(CrtNodeId_U32))
    {
      Stack.push(Neighbor_U32);
    }
  }
}

template <typename NodeType>
void BofDirGraph<NodeType>::InternalEraseEdge(const uint32_t _EdgeId_U32)
{
  // This is a bit lazy, we find the pointer here, but we refind it when we erase the edge based
  // on id key.
  BOF_ASSERT(mEdgeCollection.Contain(_EdgeId_U32));
  const Edge &rEdge = *mEdgeCollection.Find(_EdgeId_U32);

  // update neighbor count
  BOF_ASSERT(mEdgeFromNodeCollection.Contain(rEdge.From_U32));
  uint32_t &rEdgeCount_U32 = *mEdgeFromNodeCollection.Find(rEdge.From_U32);
  BOF_ASSERT(rEdgeCount_U32 > 0);
  rEdgeCount_U32 -= 1;

  // update neighbor list
  {
    BOF_ASSERT(mNodeNeighborCollection.Contain(rEdge.From_U32));
    auto Neighbor = mNodeNeighborCollection.Find(rEdge.From_U32);
    auto Iter = std::find(Neighbor->begin(), Neighbor->end(), rEdge.To_U32);
    BOF_ASSERT(Iter != Neighbor->end());
    Neighbor->erase(Iter);
  }

  mEdgeCollection.Erase(_EdgeId_U32);
}
END_BOF_NAMESPACE()
