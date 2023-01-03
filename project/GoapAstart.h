#pragma once
#include "BaseGoapAction.h"
#include "Node.h"
#include "WorldState.h"

class GoapAstar final
{
public:
	GoapAstar() = default;
	std::vector<BaseGoapAction*> FindCurrentActions(const WorldState startState, const WorldState desiredState, std::vector<BaseGoapAction*> availableActions);
private:

	bool IsMemberOfClosedList(const WorldState& worldState) const;
	std::vector<GoapNode>::iterator IsMemberOfOpenList(const WorldState& worldState);

	void AddToOpenList(GoapNode&& n);
	GoapNode& PopAndClose();

	int GetHeuristicCost(const WorldState* startState, const WorldState* desiredState);

	std::vector<GoapNode> m_OpenList{};
	std::vector<GoapNode> m_ClosedList{};
};

