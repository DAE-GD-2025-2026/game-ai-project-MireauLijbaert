#include "NavGraphPathfinding.h"

#include "AStar.h"
#include "PathSmoothing.h"
#include "VectorTypes.h"
#include "Shared/Graph/NavGraph/NavGraph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

using namespace GameAI;

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos,
	NavGraph* const pNavGraph, std::vector<FVector2D>& debugNodePositions, std::vector<NavLine>& debugPortals) 
{
	//Create the path to return
	std::vector<FVector2D> finalPath{};

	//Get the start and endTriangle
	const auto pNavMeshPoly = pNavGraph->GetNavPolygon();
	auto const& startTriangle = pNavMeshPoly->GetTriangleAtPosition(startPos, true);
	auto const& endTriangle = pNavMeshPoly->GetTriangleAtPosition(endPos, true);
	
	//We have valid start/end triangles and they are not the same
	//=> Start looking for a path
	//Copy the graph
	if (!startTriangle || !endTriangle) return finalPath;
	// if they're in the same triangle we just go directly from start to finish cuz either the nodes are the same or they can be reached in a direct line
	if (startTriangle == endTriangle)
	{
		finalPath.push_back(startPos);
		finalPath.push_back(endPos);
		
		debugNodePositions.push_back(startPos);
		debugNodePositions.push_back(endPos);

		return finalPath;
	}

	std::unique_ptr<NavGraph> pClonedGraph = pNavGraph->Clone();
	//Create Extra node for the Start Node (Agent's position)
	int startNodeId = pClonedGraph->AddNode(std::make_unique<NavGraphNode>(startPos, -1));
	// Link this node to the graph
	for (const auto& edge : startTriangle->GetEdges())
	{
		auto edgeIdx = pNavMeshPoly->FindEdgeIndex(edge);
		if (!edgeIdx.has_value()) continue;
		int nodeId = pClonedGraph->GetNodeIdFromEdgeIndex(edgeIdx.value());
		if (nodeId != Graphs::InvalidNodeId)
		{
			pClonedGraph->AddConnection(startNodeId, nodeId);
		}
	}

	//Create extra node for the endNode
	int endNodeId = pClonedGraph->AddNode(std::make_unique<NavGraphNode>(endPos, -1));
	// Link this node too
	for (const auto& edge : endTriangle->GetEdges())
	{
		auto edgeIdx = pNavMeshPoly->FindEdgeIndex(edge);
		if (!edgeIdx.has_value()) continue;
		int nodeId = pClonedGraph->GetNodeIdFromEdgeIndex(edgeIdx.value());
		if (nodeId != Graphs::InvalidNodeId)
		{
			pClonedGraph->AddConnection(endNodeId, nodeId);
		}
	}
	// Recalculate costs
	pClonedGraph->SetConnectionCostsToDistances();
	
	//Run A star on new graph
	AStar astar(pClonedGraph.get(), HeuristicFunctions::Euclidean);
	std::vector<Node*> nodePath = astar.FindPath(
		pClonedGraph->GetNode(startNodeId).get(),
		pClonedGraph->GetNode(endNodeId).get()
	);
	
	// For if there's no SSFA
	// for (Node* pNode : nodePath)
	// {
	// 	const FVector2D& pos = pNode->GetPosition();
	// 	debugNodePositions.push_back(pos); 
	// 	finalPath.push_back(pos);
	// }
	//Debug Visualisation
 
	// Extra: Run optimiser on new graph (First check if everything works without SSFA!)
	
	debugPortals = SSFA::FindPortals(nodePath, *pNavGraph->GetNavPolygon());
	finalPath = SSFA::OptimizePortals(debugPortals, *pNavGraph->GetNavPolygon());
	
	return finalPath;
}

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos, NavGraph* const pNavGraph)
{
	std::vector<FVector2D> debugNodePositions{};
	std::vector<NavLine> debugPortals{};

	return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}