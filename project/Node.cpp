#include "stdafx.h"
#include "Node.h"

int GoapNode::lastId = 0;

GoapNode::GoapNode(const WorldState& worldstate, const int costSoFar, const int estimatedTotalCost, const int parentId, BaseGoapAction* previousAction)
	:m_WorldState{ m_WorldState },
	m_CostSoFar(costSoFar),
	m_EstimatedTotalCost(estimatedTotalCost),
	m_ParentId(parentId)
{
	m_Id = ++lastId;
	m_WorldState = worldstate;
	m_PreviousAction = previousAction;
}

bool operator<(const GoapNode& lhs, const GoapNode& rhs)
{
	return (lhs.m_CostSoFar+lhs.m_EstimatedTotalCost) < (rhs.m_CostSoFar + rhs.m_EstimatedTotalCost);
}