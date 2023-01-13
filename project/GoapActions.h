#pragma once
#include "BaseGoapAction.h"
#include "SteeringBehaviors/SteeringBehaviors.h"

namespace GOAP
{
	class Action_Explore final : public BaseGoapAction
	{
	public:
		Action_Explore();
		~Action_Explore() override;
		bool checkProceduralPreconditions(Elite::Blackboard* pBlackboard) override;
		bool Execute(Elite::Blackboard* pBlackboard) override;
		bool IsValid(Elite::Blackboard* pBlackboard) override;

	private:
		Wander* m_pWander = {};
		SteeringPlugin_Output* m_pSteering;
	};

	class Action_MoveTo final : public BaseGoapAction
	{
	public:
		Action_MoveTo();
		~Action_MoveTo() override;
		bool checkProceduralPreconditions(Elite::Blackboard* pBlackboard) override;
		bool Execute(Elite::Blackboard* pBlackboard) override;
		bool IsValid(Elite::Blackboard* pBlackboard) override;

	private:
		SteeringPlugin_Output* m_pSteering;
		Seek* m_pSeek = {};
	};

	inline bool Action_MoveTo::IsValid(Elite::Blackboard* pBlackboard)
	{
		return BaseGoapAction::IsValid(pBlackboard)
			&& pBlackboard->GetData("Target", m_Target)
			&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
			&& pBlackboard->GetData("pSteering", m_pSteering)
			&& pBlackboard->GetData("pInterface", m_pInterface)
			&& pBlackboard->GetData("WorldState", m_pWorldState);
	}
}
