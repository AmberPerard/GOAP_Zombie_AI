#pragma once
#include "BaseGoapAction.h"
#include "WorldState.h"

struct GoapNode final
{
	static int lastId;
	WorldState worldState;
	BaseGoapAction* previousAction;
	int id;
	int parentId;
	int costSoFar;
	int estimatedTotalCost;


	GoapNode(WorldState worldstate, const int costSoFar, const int estimatedTotalCost, const int parentId,
		BaseGoapAction* previousAction);
};
	bool operator<(const GoapNode& lhs, const GoapNode& rhs);
