#pragma once

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class AStar
	{
	public:
		AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			T_NodeType* pNode = nullptr;
			T_ConnectionType* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);

	private:
		float GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
		Heuristic m_HeuristicFunction;
	};

	template <class T_NodeType, class T_ConnectionType>
	AStar<T_NodeType, T_ConnectionType>::AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction)
		: m_pGraph(pGraph)
		, m_HeuristicFunction(hFunction)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> AStar<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pGoalNode)
	{
		std::vector<T_NodeType*> path{};
		std::vector<NodeRecord> openList{};
		std::vector<NodeRecord> closedList{};
		NodeRecord currentRecord{};

		currentRecord.pNode = pStartNode;
		currentRecord.pConnection = nullptr;
		currentRecord.costSoFar = 0.0f;
		currentRecord.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode);
		openList.push_back(currentRecord);

		while (!openList.empty())
		{
			currentRecord = *std::min_element(openList.begin(), openList.end());

			//check if current Connection lead to the end node
			if (currentRecord.pNode == pGoalNode)
			{
				closedList.push_back(currentRecord);
				break;
			}
			for (const auto& connection : m_pGraph->GetNodeConnections(currentRecord.pNode))
			{
				const auto totalCostSoFar = currentRecord.costSoFar + connection->GetCost();
				const auto estimatedTotalCost = totalCostSoFar + GetHeuristicCost(m_pGraph->GetNode(connection->GetTo()), pGoalNode);

				bool shouldSkipConnection = false;
				//check closed list if connection to that node already exists
				for (auto& closedNode : closedList)
				{
					if (closedNode.pNode == m_pGraph->GetNode(connection->GetTo()))
					{
						//if the cost is equal or lower, skip the connection
						//the equal prevents it form infinite looping and having a missing connections
						if (closedNode.costSoFar <= totalCostSoFar)
						{
							shouldSkipConnection = true;
							continue;
						}
						closedList.erase(std::remove(closedList.begin(), closedList.end(), closedNode));
					}
				}

				if (shouldSkipConnection == false)
				{
					//check open list for that connection to that node already exists
					for (const auto& openListNode : openList)
					{
						if (openListNode.pNode == m_pGraph->GetNode(connection->GetTo()))
						{
							//if the cost is equal or lower, skip the connection
							//the equal prevents it form infinite looping and having a missing connections
							if (openListNode.costSoFar <= totalCostSoFar)
							{
								shouldSkipConnection = true;
								continue;
							}
							openList.erase(std::remove(openList.begin(), openList.end(), openListNode));
						}
					}
				}
				if (shouldSkipConnection == false)
				{
					NodeRecord newNode;
					newNode.pNode = m_pGraph->GetNode(connection->GetTo());
					newNode.costSoFar = totalCostSoFar;
					newNode.estimatedTotalCost = estimatedTotalCost;
					newNode.pConnection = connection;

					openList.push_back(newNode);
				}
			}

			openList.erase(std::remove(openList.begin(), openList.end(), currentRecord));
			closedList.push_back(currentRecord);

		}
		//check if path doesn't lead to end point
		if (currentRecord.pNode != pGoalNode)
		{
			return path;
		}

		//trace back the path from the end to the beginning
		while (currentRecord.pNode != pStartNode)
		{
			auto previousNode = currentRecord.pConnection->GetFrom();
			path.push_back(currentRecord.pNode);

			for (auto& closedListRecord : closedList)
			{
				if (closedListRecord.pNode == m_pGraph->GetNode(previousNode))
				{
					currentRecord = closedListRecord;
					break;
				}

			}
		}
		path.push_back(pStartNode);
		std::reverse(path.begin(), path.end());
		return path;
	}

	template <class T_NodeType, class T_ConnectionType>
	float Elite::AStar<T_NodeType, T_ConnectionType>::GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const
	{
		const Vector2 toDestination = m_pGraph->GetNodePos(pEndNode) - m_pGraph->GetNodePos(pStartNode);
		return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
	}
}