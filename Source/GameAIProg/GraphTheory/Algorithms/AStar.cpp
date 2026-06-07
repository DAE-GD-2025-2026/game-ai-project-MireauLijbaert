#include "AStar.h"
#include <queue>
#include <set>

using namespace GameAI;

AStar::AStar(Graph* const pGraph, HeuristicFunctions::Heuristic hFunction)
    : pGraph(pGraph)
    , HeuristicFunction(hFunction)
{
}

std::vector<Node*> AStar::FindPath(Node* const pStartNode, Node* const pGoalNode)
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
        currentNodeRecord = *std::min_element(openList.begin(), openList.end());

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
                [nextNode](const NodeRecord& record) { return record.pNode == nextNode; });

            if (closedIt != closedList.end())
            {
                // If old path is cheaper, skip this connection
                if (closedIt->costSoFar <= Gcost) continue;
                else closedList.erase(closedIt);
            }

            // Check if node is already in open list
            auto openIt = std::ranges::find_if(openList,
                [nextNode](const NodeRecord& record) { return record.pNode == nextNode; });

            if (openIt != openList.end())
            {
                // If old path is cheaper, skip this connection
                if (openIt->costSoFar <= Gcost) continue;
                // Remove old more expensive record
                else openList.erase(openIt);
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
    if (currentNodeRecord.pNode == pGoalNode)
    {
        while (currentNodeRecord.pNode != pStartNode)
        {
            path.push_back(currentNodeRecord.pNode);

            int previousNodeId = currentNodeRecord.pConnection->GetFromId();

            auto previousRecordIt = std::ranges::find_if(closedList,
                [previousNodeId](const NodeRecord& record)
                {
                    return record.pNode->GetId() == previousNodeId;
                });

            if (previousRecordIt == closedList.end()) break;

            currentNodeRecord = *previousRecordIt;
        }

        path.push_back(pStartNode);
        std::ranges::reverse(path);
    }
    // Path was unreachable so instead move to closest reachable spot to the endgoal
    else
    {
        // Find all reachable nodes (with BFS)
        std::vector<Node*> reachableNodes;
        std::queue<Node*> openListReachable{};
        std::set<Node*> visited{};

        openListReachable.push(pStartNode);
        visited.insert(pStartNode);

        while (!openListReachable.empty())
        {
            Node* currentNode = openListReachable.front();
            openListReachable.pop();
            reachableNodes.push_back(currentNode);
            for (auto connection : pGraph->FindConnectionsFrom(currentNode->GetId()))
            {
                auto nextNode = pGraph->GetNode(connection->GetToId()).get();
                if (!nextNode) continue;
                if (visited.find(nextNode) == visited.end())
                {
                    openListReachable.push(nextNode);
                    visited.insert(nextNode);
                }
            }
        }

        // Find the closest node to the goal node
        FVector2D goalPos = pGraph->GetNode(pGoalNode->GetId())->GetPosition();
        Node* pClosest = pStartNode;
        float closestDist2 = std::numeric_limits<float>::max();

        for (auto* node : reachableNodes)
        {
            FVector2D nodePos = pGraph->GetNode(node->GetId())->GetPosition();
            FVector2D diff = goalPos - nodePos;
            float d2 = diff.X * diff.X + diff.Y * diff.Y;
            if (d2 < closestDist2)
            {
                closestDist2 = d2;
                pClosest = node;
            }
        }
        // Rerun the pathfinding with the closest node as the goal
        UE_LOG(LogTemp, Warning, TEXT("Fallback: pClosest id = %d"), pClosest->GetId());
        UE_LOG(LogTemp, Warning, TEXT("Fallback: pStartNode id = %d"), pStartNode->GetId());
        UE_LOG(LogTemp, Warning, TEXT("Fallback: pGoalNode id = %d"), pGoalNode->GetId());
        // Rerun the pathfinding with the closest node as the goal
        path = FindPath(pStartNode, pClosest);
    }

    return path;
}

float AStar::GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const
{
    FVector2D toDestination = pGraph->GetNode(pEndNode->GetId())->GetPosition() - pGraph->GetNode(pStartNode->GetId())->GetPosition();
    return HeuristicFunction(abs(toDestination.X), abs(toDestination.Y));
}