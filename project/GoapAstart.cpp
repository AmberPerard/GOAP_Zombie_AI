#include "stdafx.h"
#include "GoapAstart.h"

std::vector<BaseGoapAction*> GoapAstart::FindCurrentActions(const WorldState startState,
	const WorldState desiredState, std::vector<BaseGoapAction*> availableActions)
{
	std::vector<BaseGoapAction*> path{};
	std::vector<GoapNode> openList{};
	std::vector<GoapNode> closedList{};
	GoapNode currentRecord{};

	currentRecord.worldState = startState;
//	currentRecord.pConnection = nullptr;
	currentRecord.costSoFar = 0.0f;
	currentRecord.estimatedTotalCost = GetHeuristicCost(&startState, &desiredState);
	openList.push_back(currentRecord);

	while (!openList.empty())
	{
		currentRecord = *std::min_element(openList.begin(), openList.end());

		//check if current Connection lead to the end node
		if (currentRecord.worldState == desiredState)
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

int GoapAstart::GetHeuristicCost(const WorldState* startState, const WorldState* desiredState)
{
}
