#pragma once
#include <map>
#include "Data/EBlackboard.h"


/* In the case of planning, the nodes are states
of the world, and we are searching to find a path to the goal state.The edges connecting
different states of the world are the actions that lead the state of the world to change from one
to another. */
//base world state
struct WorldState
{
public:
	WorldState(const std::string& name = "", int priority = 100);

	bool operator==(const WorldState& otherState) const;
	bool operator!=(const WorldState& other) const;

	void SetCondition(const std::string& conditionName, const bool value);
	bool getCondition(const std::string& conditionName) const;

	bool MeetsGoal(const WorldState& goal_state) const;
	int DistanceTo(const WorldState& goal_state) const;

	virtual bool IsValid(Elite::Blackboard* pBlackboard) const { return true; }

	std::string m_Name; // Name of the state
	int m_Priority; //Priority of the state
	std::map<std::string, bool> m_Conditions; 
};

inline std::ostream& operator<<(std::ostream& out, const WorldState& ws)
{
	out << "WorldState { ";
	for (const auto& kvp : ws.m_Conditions)
	{
		out << "[" << kvp.first << " = " << kvp.second << "], ";
	}
	out << "}";
	return out;
}

struct Goal_ExploreWorld final : WorldState
{
public:
	Goal_ExploreWorld() : WorldState("ExploreWorld", 0)
	{
		SetCondition("exploring", true);
	}
};

struct Goal_LootHouse final : WorldState
{
public:
	Goal_LootHouse() : WorldState("LootHouse", 100)
	{
		SetCondition("targetInRange", true);
	}
	bool IsValid(Elite::Blackboard* pBlackboard) const override;
};