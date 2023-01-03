#include "stdafx.h"
#include "BaseGoapAction.h"

BaseGoapAction::BaseGoapAction(const std::string& name, const int cost)
	:m_Cost(cost),
	m_Name(name)
{
}

BaseGoapAction::~BaseGoapAction()
{
}

bool BaseGoapAction::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
    for (const auto& preconditions : m_Preconditions)
    {
        try
        {
            //check world for precondition
        }
        catch (const std::out_of_range&)
        {
            return false;
        }
    }
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
        tmpWorldState.SetCodition(effect.first, effect.second);
    }
    return tmpWorldState;
}