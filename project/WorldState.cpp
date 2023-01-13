#include "stdafx.h"
#include "WorldState.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>

WorldState::WorldState(const std::string& name, int priority)
	:m_Name(name),
	m_Priority(priority)
{
}

bool WorldState::operator==(const WorldState& otherState) const
{
	return m_Conditions == otherState.m_Conditions;
}

bool WorldState::operator!=(const WorldState& otherState) const
{
    return !(m_Conditions == otherState.m_Conditions);
}


void WorldState::SetCondition(const std::string& conditionName, const bool value)
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

bool Goal_LootHouse::IsValid(Elite::Blackboard* pBlackboard) const
{
    std::vector<HouseInfo>* houses;
    if (!pBlackboard->GetData("Houses", houses) || houses->empty()) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

    AgentInfo agentInfo;
    if (!pBlackboard->GetData("AgentInfo", agentInfo)) return false;

    return true;
}
