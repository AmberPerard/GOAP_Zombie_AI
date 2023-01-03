#pragma once
#include "BaseGoapAction.h"
#include "WorldState.h"

struct GoapNode final
{
	WorldState worldState;
	BaseGoapAction* previousAction;
	int costSoFar;
	int estimatedTotalCost;

};

class GoapAstart final
{
public:
	GoapAstart() = default;
	std::vector<BaseGoapAction*> FindCurrentActions(const WorldState startState, const WorldState desiredState, std::vector<BaseGoapAction*> availableActions);

private:
	int GetHeuristicCost(const WorldState* startState, const WorldState* desiredState);
};

