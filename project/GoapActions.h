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

	private:
		SteeringPlugin_Output* m_pSteering;
		Seek* m_pSeek = {};
	};
}
