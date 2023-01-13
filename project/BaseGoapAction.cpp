#include "stdafx.h"
#include "BaseGoapAction.h"

BaseGoapAction::BaseGoapAction(const std::string& name, const float cost)
	:m_Cost(cost),
	m_Name(name)
{
}

BaseGoapAction::~BaseGoapAction()
{
}

bool BaseGoapAction::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
    return true;
}

bool BaseGoapAction::ConditionsMetByWorld(const WorldState& worldState)
{
    for(const auto& preconditions: m_Preconditions)
    {
	   if( worldState.getCondition(preconditions.first) != preconditions.second)
	   {
           return false;
	   }
    }
    return true;
}

WorldState BaseGoapAction::ApplyActionOnWorld(const WorldState& worldState) const
{
    WorldState tmpWorldState{ worldState };
    for(const auto& effect: m_Effects)
    {
        tmpWorldState.SetCondition(effect.first, effect.second);
    }
    return tmpWorldState;
}

bool BaseGoapAction::IsValid(Elite::Blackboard* pBlackboard)
{
    return true;
}
