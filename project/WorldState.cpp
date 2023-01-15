#include "stdafx.h"
#include "WorldState.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>

#include "HelperStructExpension.h"

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
	std::vector<HouseInfoExtended>* houses;
	if (!pBlackboard->GetData("Houses", houses) || houses->empty()) return false;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("pInterface", pInterface) || pInterface == nullptr) return false;

	AgentInfo agentInfo;
	if (!pBlackboard->GetData("AgentInfo", agentInfo)) return false;

	float closest{ INFINITY };
	for (auto& house : *houses)
	{
		const float dist{ house.Center.Distance(agentInfo.Position) };
		if (!house.hasRecentlyBeenLooted && dist < closest)
		{
			closest = dist;
				house.Looted = true;
			if (!pBlackboard->ChangeData("Target", house.Center)) return false;
			if (!pBlackboard->ChangeData("TargetHouse", &house)) return false;
		}
	}

	return closest < INFINITY;
}

bool Goal_GrabFood::IsValid(Elite::Blackboard* pBlackboard) const
{
	WorldState* m_pWorldState;
	if (!pBlackboard->GetData("WorldState", m_pWorldState)) return false;
	/*if (m_pWorldState->getCondition("foodInInv")) return false;*/

	std::vector<EntityInfoExtended>* foods;
	if (!pBlackboard->GetData("Food", foods) || foods->empty() || foods == nullptr) return false;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("pInterface", pInterface) || pInterface == nullptr) return false;

	AgentInfo agentInfo;
	if (!pBlackboard->GetData("AgentInfo", agentInfo)) return false;

	if (foods->back().Location.Distance(agentInfo.Position) > 30.f) return false;

	if (!pBlackboard->ChangeData("Target", foods->back().Location)) return false;
	if (!pBlackboard->ChangeData("TargetItem", foods->back())) return false;

	return true;
}

bool Goal_GrabMedkit::IsValid(Elite::Blackboard* pBlackboard) const
{
	WorldState* m_pWorldState;
	if (!pBlackboard->GetData("WorldState", m_pWorldState)) return false;
	//if (m_pWorldState->getCondition("medkitInInv")) return false;

	std::vector<EntityInfoExtended>* medkits;
	if (!pBlackboard->GetData("Medkits", medkits) || medkits->empty() || medkits == nullptr) return false;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("pInterface", pInterface) || pInterface == nullptr) return false;

	AgentInfo agentInfo;
	if (!pBlackboard->GetData("AgentInfo", agentInfo)) return false;

	if (medkits->back().Location.Distance(agentInfo.Position) > 30.f) return false;

	if (!pBlackboard->ChangeData("Target", medkits->back().Location)) return false;
	if (!pBlackboard->ChangeData("TargetItem", medkits->back())) return false;

	return true;
}

bool Goal_GrabPistol::IsValid(Elite::Blackboard* pBlackboard) const
{
	WorldState* m_pWorldState;
	if (!pBlackboard->GetData("WorldState", m_pWorldState)) return false;
	//if (m_pWorldState->getCondition("pistolInInv")) return false;

	std::vector<EntityInfoExtended>* pistols;
	if (!pBlackboard->GetData("Pistols", pistols) || pistols->empty() || pistols == nullptr) return false;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("pInterface", pInterface) || pInterface == nullptr) return false;

	AgentInfo agentInfo;
	if (!pBlackboard->GetData("AgentInfo", agentInfo)) return false;

	if (pistols->back().Location.Distance(agentInfo.Position) > 30.f) return false;

	if (!pBlackboard->ChangeData("Target", pistols->back().Location)) return false;
	if (!pBlackboard->ChangeData("TargetItem", pistols->back())) return false;

	return true;
}

bool Goal_GrabShotgun::IsValid(Elite::Blackboard* pBlackboard) const
{
	WorldState* m_pWorldState;
	if (!pBlackboard->GetData("WorldState", m_pWorldState)) return false;
	//if (m_pWorldState->getCondition("shotgunInInv")) return false;

	std::vector<EntityInfoExtended>* shotguns;
	if (!pBlackboard->GetData("Shotguns", shotguns) || shotguns->empty() || shotguns == nullptr) return false;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("pInterface", pInterface) || pInterface == nullptr) return false;

	AgentInfo agentInfo;
	if (!pBlackboard->GetData("AgentInfo", agentInfo)) return false;

	if (shotguns->back().Location.Distance(agentInfo.Position) > 30.f) return false;

	if (!pBlackboard->ChangeData("Target", shotguns->back().Location)) return false;
	if (!pBlackboard->ChangeData("TargetItem", shotguns->back())) return false;

	return true;
}

bool Goal_DestroyGarbage::IsValid(Elite::Blackboard* pBlackboard) const
{
	WorldState* m_pWorldState;
	if (!pBlackboard->GetData("WorldState", m_pWorldState)) return false;

	std::vector<EntityInfoExtended>* garbages;
	if (!pBlackboard->GetData("Garbage", garbages) || garbages->empty() || garbages == nullptr) return false;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("pInterface", pInterface) || pInterface == nullptr) return false;


	AgentInfo agentInfo;
	if (!pBlackboard->GetData("AgentInfo", agentInfo)) return false;

	if (garbages->back().Location.Distance(agentInfo.Position) > 30.f) return false;

	if (!pBlackboard->ChangeData("Target", garbages->back().Location)) return false;
	if (!pBlackboard->ChangeData("TargetItem", garbages->back())) return false;

	return true;
}

bool Goal_Heal::IsValid(Elite::Blackboard* pBlackboard) const
{
	WorldState* m_pWorldState;
	if (!pBlackboard->GetData("WorldState", m_pWorldState)) return false;

	return m_pWorldState->getCondition("LowHealth") && m_pWorldState->getCondition("medkitInInv");
}

bool Goal_EatFood::IsValid(Elite::Blackboard* pBlackboard) const
{
	WorldState* m_pWorldState;
	if (!pBlackboard->GetData("WorldState", m_pWorldState)) return false;

	return m_pWorldState->getCondition("LowFood") && m_pWorldState->getCondition("foodInInv");
}

bool Goal_FleePurgeZone::IsValid(Elite::Blackboard* pBlackboard) const
{
	WorldState* m_pWorldState;
	if (!pBlackboard->GetData("WorldState", m_pWorldState)) return false;

	return m_pWorldState->getCondition("insidePurgezone");

}

bool Goal_ShootEnemies::IsValid(Elite::Blackboard* pBlackboard) const
{
	WorldState* m_pWorldState;
	if (!pBlackboard->GetData("WorldState", m_pWorldState)) return false;

	return m_pWorldState->getCondition("inDanger");
}

