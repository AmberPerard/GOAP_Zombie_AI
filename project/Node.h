#pragma once
#include "BaseGoapAction.h"
#include "WorldState.h"

struct GoapNode final
{
	static int lastId;

	WorldState m_WorldState;
	BaseGoapAction* m_PreviousAction;
	int m_Id;
	int m_ParentId;
	int m_CostSoFar;
	int m_EstimatedTotalCost;


	GoapNode(const WorldState& worldstate, const int costSoFar, const int estimatedTotalCost, const int parentId,
		BaseGoapAction* previousAction);
};
	bool operator<(const GoapNode& lhs, const GoapNode& rhs);
