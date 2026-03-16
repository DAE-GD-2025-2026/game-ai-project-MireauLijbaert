#include "AStar.h"

using namespace GameAI;

AStar::AStar(Graph* const pGraph, HeuristicFunctions::Heuristic hFunction)
	: pGraph(pGraph)
	, HeuristicFunction(hFunction)
{
}

std::vector<Node*>AStar::FindPath(Node* const pStartNode, Node* const pGoalNode)
{
	std::vector<Node*> path{};
	std::vector<NodeRecord> openList{};
	std::vector<NodeRecord> closedList{};
	NodeRecord currentNodeRecord{};
	NodeRecord startRecord{pStartNode, nullptr, 0, GetHeuristicCost(pStartNode, pGoalNode)};
	openList.push_back(startRecord);

	
	while (!openList.empty())
	{
		// Get Lowest nodeRecord
		currentNodeRecord = *std::min(openList.begin(), openList.end());
		
		// If node is goalnode, stop the loop
		if (currentNodeRecord.pNode == pGoalNode) break;
		// Loop over the nodes' connections
		auto currentConnections = pGraph->FindConnectionsFrom(currentNodeRecord.pNode->GetId());
		for (auto connection : currentConnections)
		{
			auto nextNode = pGraph->GetNode(connection->GetToId()).get();
			float Gcost = currentNodeRecord.costSoFar + connection->GetWeight();
			
			// Check if node is already in closed list
			auto closedIt = std::ranges::find_if(closedList,
				[nextNode](const NodeRecord& record)
				{
					return record.pNode == nextNode;
				});

			if (closedIt != closedList.end())
			{
				// If old path is cheaper, skip this connection
				if (closedIt->costSoFar <= Gcost)
				{
					continue;
				}
				else
				{
					// Remove old more expensive record
					closedList.erase(closedIt);
				}
			}
			
			// Check if node is already in open list
			auto openIt = std::ranges::find_if(openList,
				[nextNode](const NodeRecord& record)
				{
					return record.pNode == nextNode;
				});

			if (openIt != openList.end())
			{
				// If old path is cheaper, skip this connection
				if (openIt->costSoFar <= Gcost)
				{
					continue;
				}
				else
				{
					// Remove old more expensive record
					openList.erase(openIt);
				}
			}
			
			NodeRecord newNodeRecord{};
			newNodeRecord.pNode = nextNode;
			newNodeRecord.pConnection = connection;
			newNodeRecord.costSoFar = Gcost;
			newNodeRecord.estimatedTotalCost = Gcost + GetHeuristicCost(nextNode, pGoalNode);

			openList.push_back(newNodeRecord);
		}
		
		// Move current node from open to closed list
		openList.erase(std::remove(openList.begin(), openList.end(), currentNodeRecord), openList.end());
		closedList.push_back(currentNodeRecord);
	}
	
	// Reconstruct path by backtracking
	while (currentNodeRecord.pNode != pStartNode)
	{
		path.push_back(currentNodeRecord.pNode);

		int previousNodeId = currentNodeRecord.pConnection->GetFromId();

		auto previousRecordIt = std::ranges::find_if(closedList,
			[previousNodeId](const NodeRecord& record)
			{
				return record.pNode->GetId() == previousNodeId;
			});

		if (previousRecordIt == closedList.end())
			break;

		currentNodeRecord = *previousRecordIt;
	}

	path.push_back(pStartNode);
	std::reverse(path.begin(), path.end());
	
	return path;
}

float AStar::GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const
{
	FVector2D toDestination = pGraph->GetNode(pEndNode->GetId())->GetPosition() - pGraph->GetNode(pStartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(toDestination.X), abs(toDestination.Y));
}