#include "stdafx.h"
#include "Node.h"

int GoapNode::lastId = 0;

GoapNode::GoapNode(const WorldState& worldstate, const int costSoFar, const int estimatedTotalCost, const int parentId, BaseGoapAction* previousAction)
	:worldState(worldState),
	costSoFar(costSoFar),
	estimatedTotalCost(estimatedTotalCost),
	parentId(parentId)
{
	id = ++lastId;
}

bool operator<(const GoapNode& lhs, const GoapNode& rhs)
{
	return (lhs.costSoFar+lhs.estimatedTotalCost) < (rhs.costSoFar + rhs.estimatedTotalCost);
}