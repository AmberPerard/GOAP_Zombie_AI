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
	virtual ~WorldState() = default;
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
///////////////////////////////////////
//GOAL: EXPLORE THE WORLD
//****
struct Goal_ExploreWorld final : WorldState
{
public:
	Goal_ExploreWorld() : WorldState("ExploreWorld", 0)
	{
		SetCondition("exploring", true);
	}
};
///////////////////////////////////////
//GOAL: LOOT HOUSE
//****
struct Goal_LootHouse final : WorldState
{
public:
	Goal_LootHouse() : WorldState("LootHouse", 100)
	{
		SetCondition("targetInRange", true);
	}
	bool IsValid(Elite::Blackboard* pBlackboard) const override;
};
///////////////////////////////////////
//GOAL: GARB FOOD
//****
struct Goal_GrabFood final : WorldState
{
public:
	Goal_GrabFood() : WorldState("GrabFood", 300)
	{
		SetCondition("foodInInv", true);
	}
	bool IsValid(Elite::Blackboard* pBlackboard) const override;
};
///////////////////////////////////////
//GOAL: GARB MEDKIT
//****
struct Goal_GrabMedkit final : WorldState
{
public:
	Goal_GrabMedkit() : WorldState("GrabMedkit", 250)
	{
		SetCondition("medkitInInv", true);
	}
	bool IsValid(Elite::Blackboard* pBlackboard) const override;
};
///////////////////////////////////////
//GOAL: GARB PISTOL
//****
struct Goal_GrabPistol final : WorldState
{
public:
	Goal_GrabPistol() : WorldState("GrabPistol", 200)
	{
		SetCondition("pistolInInv", true);
	}
	bool IsValid(Elite::Blackboard* pBlackboard) const override;
};
///////////////////////////////////////
//GOAL: GARB SHOTGUN
//****
struct Goal_GrabShotgun final : WorldState
{
public:
	Goal_GrabShotgun() : WorldState("GrabShotgun", 200)
	{
		SetCondition("shotgunInInv", true);
	}
	bool IsValid(Elite::Blackboard* pBlackboard) const override;
};
///////////////////////////////////////
//GOAL: DESTROY GARBAGE
//****
struct Goal_DestroyGarbage final : WorldState
{
public:
	Goal_DestroyGarbage() : WorldState("destroyGarbage", 60)
	{
		SetCondition("destroyedGarbage", true);
	}
	bool IsValid(Elite::Blackboard* pBlackboard) const override;
};
///////////////////////////////////////
//GOAL: Heal
//****
struct Goal_Heal final : WorldState
{
public:
	Goal_Heal() : WorldState("Heal", 350)
	{
		SetCondition("LowHealth", false);
	}
	bool IsValid(Elite::Blackboard* pBlackboard) const override;
};

///////////////////////////////////////
//GOAL: EatFood
//****
struct Goal_EatFood final : WorldState
{
public:
	Goal_EatFood() : WorldState("EatFood", 360)
	{
		SetCondition("LowFood", false);
	}
	bool IsValid(Elite::Blackboard* pBlackboard) const override;
};

///////////////////////////////////////
//GOAL: FleePurgeZone
//****
struct Goal_FleePurgeZone final : WorldState
{
public:
	Goal_FleePurgeZone() : WorldState("FleePurgeZone", 1050)
	{
		SetCondition("insidePurgezone", false);
		SetCondition("inDanger", false);
	}
	bool IsValid(Elite::Blackboard* pBlackboard) const override;
};

///////////////////////////////////////
//GOAL: ShootEnemies
//****
struct Goal_ShootEnemies final : WorldState
{
public:
	Goal_ShootEnemies() : WorldState("ShootEnemies", 400)
	{
		SetCondition("inDanger", false);
	}
	bool IsValid(Elite::Blackboard* pBlackboard) const override;
};