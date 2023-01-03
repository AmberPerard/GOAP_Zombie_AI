#include "stdafx.h"
#include "WorldState.h"

WorldState::WorldState(const std::string& name, int priority)
	:m_Name(name),
	m_Priority(priority)
{
}

bool WorldState::operator==(const WorldState& otherState) const
{
	return this->m_Conditions == otherState.m_Conditions;
}

void WorldState::SetCodition(const std::string& conditionName, const bool value)
{
    m_Conditions[conditionName] = value;
}

bool WorldState::getCondition(const std::string& conditionName) const
{
    return m_Conditions.at(conditionName);
}

bool WorldState::MeetsGoal(const WorldState& goal_state) const
{
    for (const auto& kv : goal_state.m_Conditions)
    {
        try
        {
            if (m_Conditions.at(kv.first) != kv.second)
            {
                return false;
            }
        }
        catch (const std::out_of_range&)
        {
            return false;
        }
    }
    return true;
}


int WorldState::DistanceTo(const WorldState& goal_state) const
{
    int result{};

    for (const auto& kv : goal_state.m_Conditions)
    {
        auto it = m_Conditions.find(kv.first);
        if (it == end(m_Conditions) || it->second != kv.second)
        {
            ++result;
        }
    }

    return result;
}