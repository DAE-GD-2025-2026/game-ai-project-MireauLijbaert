#pragma once
#include <stack>

#include "Net/NetworkMetricsDefs.h"
#include "Shared/Graph/Graph.h"

namespace GameAI
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath final
	{
	public:
		EulerianPath(Graph* const pGraph);

		Eulerianity IsEulerian() const;
		std::vector<Node*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(const std::vector<Node*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsConnected() const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* const pGraph)
		: m_pGraph(pGraph)
	{
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{
		// TODO If the graph is not connected, there can be no Eulerian Trail
		if (!IsConnected()) return Eulerianity::notEulerian;

		// TODO Count nodes with odd degree 
		int OddNodes = 0;
		for (int index = 0; index < m_pGraph->GetNodeCount(); index++)
		{
			auto Connections = m_pGraph->FindConnectionsFrom(index);
			if (Connections.size() % 2 != 0) OddNodes++;
		}

		// TODO A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if (OddNodes > 2) return Eulerianity::notEulerian;

		// TODO A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		// TODO An Euler trail can be made, but only starting and ending in these 2 nodes
		if (OddNodes == 2 && m_pGraph->GetNodeCount() != 2) return Eulerianity::semiEulerian; 
		
		// TODO A connected graph with no odd nodes is Eulerian
		
		return Eulerianity::eulerian;
	}

	inline std::vector<Node*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		Graph graphCopy = m_pGraph->Clone();
		std::vector<Node*> Path = {};
		std::vector<Node*> Nodes = graphCopy.GetActiveNodes();
		int currentNodeId{ Graphs::InvalidNodeId };
		
		// TODO Check if there can be an Euler path
		// TODO If this graph is not eulerian, return the empty path
		if (eulerianity == Eulerianity::notEulerian) return Path;
		if (eulerianity == Eulerianity::semiEulerian)
		{
			for (int index = 0; index < m_pGraph->GetNodeCount(); index++)
			{
				// for semiEulerian we need to start and stop in an odd node, so search and set currentNode to first odd node
				auto connections = m_pGraph->FindConnectionsFrom(index);
				if (connections.size() % 2 != 0)
				{
					currentNodeId = index;
					break;
				}
			}	
		}
		// If eulerian just take the first active NodeId as any node can be used for the start;
		else if (eulerianity == Eulerianity::eulerian) currentNodeId = Nodes.at(0)->GetId();
		// TODO Start algorithm loop
		std::stack<int> nodeStack;
		// As long as the stack isn't empty or there are connections, keep looping
		while (!nodeStack.empty() || !graphCopy.FindConnectionsFrom(currentNodeId).empty())
		{
			auto connections = graphCopy.FindConnectionsFrom(currentNodeId);
			if (!connections.empty())
			{
				nodeStack.push(currentNodeId);
				int nextNodeId = connections.at(0)->GetToId();
				// Not sure if the connection is both ways or not so delete both, but check if they exist
				auto* connection = graphCopy.FindConnection(currentNodeId, nextNodeId);
				if (connection)
				{
					graphCopy.RemoveConnection(connection);
				}

				auto* reverseConnection = graphCopy.FindConnection(nextNodeId, currentNodeId);
				if (reverseConnection)
				{
					graphCopy.RemoveConnection(reverseConnection);
				}
				currentNodeId = nextNodeId;
			}
			else
			{
				// No connections, so this node has to be put on the path and we go back to the previous node
				Path.push_back(m_pGraph->GetNode(currentNodeId).get());
				if (!nodeStack.empty())
				{
					currentNodeId = nodeStack.top();
					nodeStack.pop();
				}
			}
		}
		Path.push_back(m_pGraph->GetNode(currentNodeId).get());
		std::reverse(Path.begin(), Path.end());
		return Path;
	}

	inline void EulerianPath::VisitAllNodesDFS(const std::vector<Node*>& Nodes, std::vector<bool>& visited, int startIndex ) const
	{
		// TODO Mark the visited node
		visited[startIndex] = true;
		// TODO Ask the graph for the connections from that node
		// Gets all connections from our markedNode, these connections have a from id,
		// our marked node and a to id, the connection to our marked node
		auto Connections = m_pGraph->FindConnectionsFrom(startIndex);
		// TODO recursively visit any valid connected nodes that were not visited before
		
		// TODO Tip: use an index-based for-loop to find the correct index
		for (int index = 0; index < Connections.size(); index++)
		{
			// If the index of my connection is true in the corresponding visited vector, it has already been checked, continue
			auto ToId = Connections.at(index)->GetToId();
			if (visited[ToId]) continue;
			// if the connection is false, set it to true because we have now visited it, and check all the connections of this node
			visited[ToId] = true;
			VisitAllNodesDFS(Nodes, visited, ToId);
			
		}
	}

	inline bool EulerianPath::IsConnected() const
	{
		std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		if (Nodes.size() == 0)
			return false;

		// TODO choose a starting node
		auto StartNode = Nodes.at(0);
		std::vector<bool> Visited(Nodes.size(), false);
		
		// TODO start a depth-first-search traversal from the node that has at least one connection
		VisitAllNodesDFS(Nodes, Visited, StartNode->GetId() );
		
		// TODO if a node was never visited, this graph is not connected
		for (bool visit : Visited)
		{
			if (!visit)
			{
				return false;
			}
		}
		return true;
	}
}