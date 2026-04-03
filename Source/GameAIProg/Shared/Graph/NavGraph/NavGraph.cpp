 #include "NavGraph.h"

#include <unordered_map>

#include "NavGraphNode.h"

GameAI::NavGraph::NavGraph(std::unique_ptr<TriPolygon> && NavPoly)
	: Graph{false}
	, pNavPoly{std::move(NavPoly)}
{
	CreateNavigationGraph();
}

GameAI::NavGraph::NavGraph(const NavGraph& Other)
	: Graph(false)
{
	Nodes.reserve(Other.Nodes.size());
	for (std::unique_ptr<Node> const & OtherNode : Other.Nodes)
	{
		Nodes.push_back(std::make_unique<NavGraphNode>(*dynamic_cast<NavGraphNode*>(OtherNode.get())));
	}
        
	Connections.reserve(Other.Connections.size());
	for (std::unique_ptr<Connection> const & OtherConnection : Other.Connections)
	{
		Connections.push_back(std::make_unique<Connection>(*OtherConnection.get()));
	}
}

std::unique_ptr<GameAI::NavGraph> GameAI::NavGraph::Clone() const
{
	return std::make_unique<NavGraph>(*this);
}

int GameAI::NavGraph::GetNodeIdFromEdgeIndex(int EdgeIdx) const
{
	if (EdgeIdx >= 0)
	{
		for (auto const & pNode : Nodes)
		{
			if (reinterpret_cast<NavGraphNode*>(pNode.get())->GetEdgeIdx() == EdgeIdx)
			{
				return pNode->GetId();
			}
		}
	}
	
	return Graphs::InvalidNodeId;
}

void GameAI::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigation mesh and create nodes
			// Create node here
	const auto &edges = pNavPoly->GetEdges();
	const auto &triangles = pNavPoly->GetTriangles();
	
	// Loop over all the edges in the navPoly
	for (int edgeId{0}; edgeId < static_cast<int>(edges.size()); ++edgeId)
	{
		auto const & edge = edges[edgeId];
		int triangleCount{0};
		// for each edge check all the triangles in the navpoly
		for (auto const & triangle : triangles)
		{
			// check how many triangles has this edge
			if (triangle.HasEdge(edge))
			{
				
				++triangleCount;
			}
		}
		
		// if the edge has 2 triangles it's a shared edge and a node can be made
		if (triangleCount == 2)
		{
			// Create the node in the middle of the edge
			FVector p1 = (edge.GetP1(*pNavPoly));
			FVector p2 = (edge.GetP2(*pNavPoly));
			FVector middle = (p1 + p2) * 0.5f;
			// convert to 2D cuz that's what used to add nodes
			FVector2D middle2D = FVector2D(middle.X, middle.Y);
			// add node and remember the index of the edge lining up with the node index for connections
			int nodeId = AddNode(std::make_unique<NavGraphNode>(middle2D, edgeId));
		}
	}

	
	//2. Create connections now that every node is created	
		//2 valid nodes -> 1 connection
		//3 valid nodes -> 3 connections
	// Look through all triangles
	for (const auto& triangle : triangles)
	{
		std::vector<int> validNodeIds{};
		
		// Check their edges for nodes made on them
		for (const auto& edge : triangle.GetEdges())
		{
			// Find edge index, this returns std::optional<int> so we need to check if it actually got a value, if not skip this edge
			auto edgeId = pNavPoly->FindEdgeIndex(edge);
			if (!edgeId.has_value())
				continue;

			// Get the corresponding nodeId from our edge, if we find a notde put it in validNodeId's
			int nodeId = GetNodeIdFromEdgeIndex(edgeId.value());
			if (nodeId != Graphs::InvalidNodeId)
			{
				validNodeIds.push_back(nodeId);
			}
		}

		// If we have 2 validnodeId's we make a connection between these
		if (validNodeIds.size() == 2)
		{
			AddConnection(validNodeIds[0], validNodeIds[1]);
		}
		// If we have 3 validNodeId's we make a connection between each possible pair
		else if (validNodeIds.size() == 3)
		{
			AddConnection(validNodeIds[0], validNodeIds[1]);
			AddConnection(validNodeIds[1], validNodeIds[2]);
			AddConnection(validNodeIds[0], validNodeIds[2]);
		}
	}
	
		
	//3. Set the connections cost to the actual distance
	SetConnectionCostsToDistances();
	
}
