#pragma once
#include <Exam_HelperStructs.h>
#include <IExamInterface.h>
#include <unordered_map>

#include "WorldState.h"
#include "Data/EBlackboard.h"

class BaseGoapAction
{
public:
	BaseGoapAction() = default;
	BaseGoapAction(const std::string& name, const float cost);
	virtual ~BaseGoapAction();

	virtual bool checkProceduralPreconditions(Elite::Blackboard* pBlackboard);
	virtual bool Execute(Elite::Blackboard* pBlackboard) { return true; }

	virtual void SetPrecondition(const std::string& key, const bool value) { m_Preconditions[key] = value; }
	virtual void SetEffect(const std::string& key, const bool value) { m_Effects[key] = value; }

	virtual bool ConditionsMetByWorld(const WorldState& worldState);
	virtual WorldState ApplyActionOnWorld(const WorldState& worldState) const;

	std::string GetName() { return m_Name; };

	float GetCost() const { return m_Cost; };
protected:
	float m_Cost{};
	std::string m_Name;
	Elite::Vector2 m_Target{};
	AgentInfo m_AgentInfo;
	IExamInterface* m_pInterface;
	WorldState* m_pWorldState;

	std::unordered_map<std::string, bool> m_Preconditions;
	std::unordered_map<std::string, bool> m_Effects;

};

