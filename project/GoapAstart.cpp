#include "stdafx.h"
#include "GoapAstart.h"


std::vector<BaseGoapAction*> GoapAstar::FindCurrentPlan(const WorldState& startState,
	const WorldState& desiredState, std::vector<BaseGoapAction*>& availableActions)
{
	if (startState.MeetsGoal(desiredState))
	{
		std::cout << "Goal already meets current WorldState!" << std::endl;
		return std::vector<BaseGoapAction*>();
	}

	// Feasible we'd re-use a planner, so clear out the prior results
	m_OpenList.clear();
	m_ClosedList.clear();

	GoapNode currentRecord{ startState,0,GetHeuristicCost(startState, desiredState),0,nullptr };
	m_OpenList.push_back(std::move(currentRecord));

	while (!m_OpenList.empty())
	{
		GoapNode& current(PopAndClose());

		//check if current state is the goal state
		if (current.m_WorldState.MeetsGoal(desiredState))
		{
			std::vector<BaseGoapAction*> plan;
			do
			{
				plan.emplace_back(current.m_PreviousAction);

				auto it = std::find_if(begin(m_OpenList), end(m_OpenList), [&](const GoapNode& n) { return n.m_Id == current.m_ParentId; });

				// Node is not on the open list, search on closed list
				if (it == end(m_OpenList))
				{
					it = std::find_if(begin(m_ClosedList), end(m_ClosedList), [&](const GoapNode& n) { return n.m_Id == current.m_ParentId; });
				}
				current = *it;
			} while (current.m_ParentId != 0);

			return plan;
		}
		for (const auto potentialAction : availableActions)
		{
			if (potentialAction->ConditionsMetByWorld(current.m_WorldState))
			{
				WorldState outcomeWorld = potentialAction->ApplyActionOnWorld(current.m_WorldState);

				if (IsMemberOfClosedList(outcomeWorld)) continue;

				auto outcomeNode = IsMemberOfOpenList(outcomeWorld);
				auto endthingy = end(m_OpenList);
				if (outcomeNode == end(m_OpenList))
				{
					const auto totalCostSoFar = current.m_CostSoFar + int(potentialAction->GetCost());
					const auto estimatedTotalCost = totalCostSoFar + GetHeuristicCost(outcomeWorld, desiredState);
					GoapNode newNode(outcomeWorld, totalCostSoFar, estimatedTotalCost, current.m_Id, potentialAction);
					AddToOpenList(std::move(newNode));
				}
				else
				{
					if (current.m_CostSoFar + int(potentialAction->GetCost()) < outcomeNode->m_CostSoFar)
					{
						outcomeNode->m_ParentId = current.m_Id;
						outcomeNode->m_CostSoFar = current.m_CostSoFar + int(potentialAction->GetCost());
						outcomeNode->m_EstimatedTotalCost = GetHeuristicCost(outcomeWorld, desiredState);
						outcomeNode->m_PreviousAction = potentialAction;

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
	return std::find_if(begin(m_ClosedList), end(m_ClosedList), [&](const GoapNode& node) { return node.m_WorldState == worldState; }) != end(m_ClosedList);
}
std::vector<GoapNode>::iterator GoapAstar::IsMemberOfOpenList(const WorldState& worldState)
{
	return std::find_if(begin(m_OpenList), end(m_OpenList), [&](const GoapNode& n) { return n.m_WorldState == worldState; });
}

void GoapAstar::AddToOpenList(GoapNode&& n)
{
	// insert maintaining sort order
	auto it = std::lower_bound(begin(m_OpenList), end(m_OpenList), n);
	m_OpenList.emplace(it, std::move(n));
}

int GoapAstar::GetHeuristicCost(const WorldState startState, const WorldState desiredState)
{
	int result{};

	for (const auto& kv : desiredState.m_Conditions)
	{
		auto it = startState.m_Conditions.find(kv.first);
		if (it == std::end(startState.m_Conditions) || it->second != kv.second)
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

