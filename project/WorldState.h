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


	friend std::ostream& operator<<(std::ostream& out, const WorldState& ws);

	void SetCodition(const std::string& conditionName, const bool value);
	bool getCondition(const std::string& conditionName) const;

	bool MeetsGoal(const WorldState& goal_state) const;
	int DistanceTo(const WorldState& goal_state) const;

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

struct Goal_Wander final : WorldState
{
public:
	Goal_Wander() : WorldState("Wander", 0)
	{
		SetCodition("wandering", true);
	}
};

struct Goal_EnterHouse final : WorldState
{
public:
	Goal_EnterHouse() : WorldState("Enter House", 100)
	{
		SetCodition("House_in_range", true);
	}
};