#include "BFS.h"

#include <map>
#include <unordered_map>
#include <queue>

#include "Shared/Graph/Graph.h"

using namespace GameAI;

BFS::BFS(Graph* const pGraph)
	: pGraph(pGraph)
{
}

// TODO Breath First Search Algorithm searches for a path from the startNode to the destinationNode
std::vector<Node*> BFS::FindPath(Node* const pStartNode, Node* const pDestinationNode) const
{
	std::vector<Node*> path;
	std::queue<Node*> nodeQueue;
	std::unordered_map<int,bool> visitedNodes;
	std::unordered_map<int,int> nodeLevel;
	Node* pCurrentNode{};
	// Mark our starting node as visited, level 0 and push it into the queue
	visitedNodes[pStartNode->GetId()] = true;
	nodeLevel[pStartNode->GetId()] = 0;
	nodeQueue.push(pStartNode);
	
	// if the queue is empty all nodes are visited
	while (!nodeQueue.empty())
	{
		// Take the first node in the queue and check all it's connections, add these to the queue
		pCurrentNode = nodeQueue.front();
		nodeQueue.pop();
		auto connections{pGraph->FindConnectionsFrom(pCurrentNode->GetId())};
		for (auto connection : connections)
		{
			int connectionNodeId{connection->GetToId()};
			
			// If the node was previously visited, skip it
			if (visitedNodes[connectionNodeId] == true) continue;
			visitedNodes[connectionNodeId] = true;
			nodeLevel[connectionNodeId] = nodeLevel[pCurrentNode->GetId()]+1;
			nodeQueue.push(pGraph->GetNode(connectionNodeId).get());
			
			// Check if our connected node is the destination
			if (connection->GetToId() == pDestinationNode->GetId())
			{
				// Push the destinationNode to the start of the path and exit the loop
				path.push_back(pGraph->GetNode(connection->GetToId()).get());
				
				// clear our queue so we go out of the while loop (clearing a queue is most efficient with a swap
				std::queue<Node*> empty; // Make an empty queue
				std::swap(nodeQueue, empty); // swap it with our queue to make it empty, destructor later handles the empty queu
				
				break;
			}
		}
	}
	
	// Construct path from our nodes by backtracking
	while (pCurrentNode != pStartNode)
	{
		// Push the current node into the path
		path.push_back(pGraph->GetNode(pCurrentNode->GetId()).get());
		auto connections{pGraph->FindConnectionsFrom(pCurrentNode->GetId())};
		for (auto connection : connections)
		{
			int connectionNodeId{connection->GetToId()};
			// skip nodes that weren't visited before, our path can be made with visited nodes only
			if (visitedNodes[connectionNodeId] == false) continue;
			
			// Check if the level is lower, that means it's closer to our starting node, break out of the loop and check this nodes connections now
			if (nodeLevel[connectionNodeId] < nodeLevel[pCurrentNode->GetId()])
			{
				pCurrentNode = pGraph->GetNode(connectionNodeId).get();
				break;
			}
		}
	}
	
	// When the currentNode reached the startNode we break out of our while so we still need to add it
	path.push_back(pGraph->GetNode(pCurrentNode->GetId()).get());
	
	// The path is reversed now so we need to reverese it to get the right order
	std::ranges::reverse(path);
	
	
	return path;
}
