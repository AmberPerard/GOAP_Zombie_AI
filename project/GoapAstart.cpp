#include "stdafx.h"
#include "GoapAstart.h"


std::vector<BaseGoapAction*> GoapAstar::FindCurrentActions(const WorldState startState,
	const WorldState desiredState, std::vector<BaseGoapAction*> availableActions)
{
	// Feasible we'd re-use a planner, so clear out the prior results
	m_OpenList.clear();
	m_ClosedList.clear();

	GoapNode currentRecord{startState,0,GetHeuristicCost(&startState, &desiredState),0,nullptr};
	m_OpenList.push_back(std::move(currentRecord));

	while (!m_OpenList.empty())
	{
		currentRecord = PopAndClose();

		//check if current state is the goal state
		if (currentRecord.worldState == desiredState)
		{
			std::vector<BaseGoapAction*> plan;
			do
			{
				plan.emplace_back(currentRecord.previousAction);

				auto it = std::find_if(begin(m_OpenList), end(m_OpenList), [&](const GoapNode& n) { return n.id == currentRecord.parentId; });

				// Node is not on the open list, search on closed list
				if (it == end(m_OpenList))
				{
					it = std::find_if(begin(m_ClosedList), end(m_ClosedList), [&](const GoapNode& n) { return n.id == currentRecord.parentId; });
				}
				currentRecord = *it;
			} while (currentRecord.parentId != 0);

			return plan;
		}
		for (const auto& action : availableActions)
		{

			if (action->ConditionsMetByWorld(currentRecord.worldState))
			{
				WorldState outcomeWorld = action->ApplyActionOnWorld(currentRecord.worldState);

				if (IsMemberOfClosedList(outcomeWorld))
				{
					continue;
				}
				const auto totalCostSoFar = currentRecord.costSoFar + action->GetCost();
				const auto estimatedTotalCost = totalCostSoFar + GetHeuristicCost(&outcomeWorld, &desiredState);
				auto outcomeNode = IsMemberOfOpenList(outcomeWorld);
				if (outcomeNode == end(m_OpenList))
				{
					GoapNode newNode(outcomeWorld, totalCostSoFar, estimatedTotalCost, currentRecord.id, action);
					AddToOpenList(std::move(newNode));
				}
				else
				{
					if (currentRecord.costSoFar + action->GetCost() < outcomeNode->costSoFar)
					{
						outcomeNode->parentId = currentRecord.id;
						outcomeNode->costSoFar = currentRecord.costSoFar + action->GetCost();
						outcomeNode->estimatedTotalCost = GetHeuristicCost(&outcomeWorld, &desiredState);
						outcomeNode->previousAction = action;

						std::sort(begin(m_OpenList), end(m_OpenList));
					}
				}
			}
		}
	}

	// If there's nothing left to evaluate, then we have no possible path left
	throw std::runtime_error("A* planner could not find a path from start to goal");
}

bool GoapAstar::IsMemberOfClosedList(const WorldState& worldState) const
{
	return std::find_if(begin(m_ClosedList), end(m_ClosedList), [&](const GoapNode& node) { return node.worldState == worldState; }) != end(m_ClosedList);
}
std::vector<GoapNode>::iterator GoapAstar::IsMemberOfOpenList(const WorldState& worldState)
{
	return std::find_if(begin(m_OpenList), end(m_OpenList), [&](const GoapNode& node) { return node.worldState == worldState; });
}

void GoapAstar::AddToOpenList(GoapNode&& n)
{
	// insert maintaining sort order
	auto it = std::lower_bound(begin(m_OpenList), end(m_OpenList), n);
	m_OpenList.emplace(it, std::move(n));
}

int GoapAstar::GetHeuristicCost(const WorldState* startState, const WorldState* desiredState)
{
	int result{};

	for (const auto& kv : desiredState->m_Conditions)
	{
		auto it = startState->m_Conditions.find(kv.first);
		if (it == std::end(startState->m_Conditions) || it->second != kv.second)
		{
			++result;
		}
	}

	return result;
}

GoapNode& GoapAstar::PopAndClose()
{
	assert(!m_OpenList.empty());
	m_ClosedList.push_back(std::move(m_OpenList.front()));
	m_OpenList.erase(m_OpenList.begin());

	return m_ClosedList.back();
}

