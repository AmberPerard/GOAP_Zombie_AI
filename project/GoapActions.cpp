#include "stdafx.h"
#include "GoapActions.h"

GOAP::Action_Explore::Action_Explore()
	: BaseGoapAction("exploring", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	SetEffect("exploring", true);
}

GOAP::Action_Explore::~Action_Explore()
{
	delete m_pWander;
	m_pWander = nullptr;
	delete m_pSteering;
	m_pSteering = nullptr;
}

bool GOAP::Action_Explore::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("Interface", m_pInterface);
}

bool GOAP::Action_Explore::Execute(Elite::Blackboard* pBlackboard)
{
	m_pSteering->AutoOrient = true;
	m_pSteering->RunMode = false;
	m_pSteering = &m_pWander->CalculateSteering(m_AgentInfo);
	return false;
}

GOAP::Action_MoveTo::Action_MoveTo()
	: BaseGoapAction("MoveTo", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	SetEffect("target_in_range", true);
}

GOAP::Action_MoveTo::~Action_MoveTo()
{
	delete m_pSeek;
	m_pSeek = nullptr;
	delete m_pSteering;
	m_pSteering = nullptr;
}

bool GOAP::Action_MoveTo::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("Target", m_Target)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("Interface", m_pInterface);
}

bool GOAP::Action_MoveTo::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_AgentInfo.Position.DistanceSquared(m_Target) <= (m_AgentInfo.GrabRange))
	{
		m_pSteering->LinearVelocity = Elite::ZeroVector2;
		return true;
	}
	m_pSeek->SetTarget(m_Target);
	m_pSteering = &m_pSeek->CalculateSteering(m_AgentInfo);
	return false;
}
