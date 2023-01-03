#pragma once
#include <Exam_HelperStructs.h>
#include <IExamInterface.h>
#include <unordered_map>

#include "Data/EBlackboard.h"

class BaseGoapAction
{
public:
	BaseGoapAction() = default;
	void DoReset();
	virtual void Reset();
	virtual ~BaseGoapAction();
	
	virtual bool checkProceduralPreconditions(AgentInfo agentInfo);
	virtual bool RequiresInRange();
	virtual bool Execute(Elite::Blackboard* m_pBlackboard) { return true; }
	virtual bool isDone();

	virtual void SetPrecondition(const std::string& key, const bool value) { m_Preconditions[key] = value; }
	virtual void SetEffect(const std::string& key, const bool value) { m_Effects[key] = value; }

	void SetInRange(bool inRange) { this->m_InRange = inRange; }
protected:
	float m_Cost{};
	bool m_InRange = false;
	std::string m_Name;
	Elite::Vector2 m_Target{};
	AgentInfo m_AgentInfo;
	IExamInterface* m_pInterface;

	std::unordered_map<std::string, bool> m_Preconditions;
	std::unordered_map<std::string, bool> m_Effects;
		
};

