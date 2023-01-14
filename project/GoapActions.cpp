#include "stdafx.h"
#include "GoapActions.h"

///////////////////////////////////////
//EXPLORE
//****
GOAP::Action_Explore::Action_Explore()
	: BaseGoapAction("exploring", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	BaseGoapAction::SetEffect("exploring", true);
	m_pWander = new Wander();
}

GOAP::Action_Explore::~Action_Explore()
{
	delete m_pWander;
	m_pWander = nullptr;
}

bool GOAP::Action_Explore::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("pSteering", m_pSteering)
		&& pBlackboard->GetData("pInterface", m_pInterface);
}

bool GOAP::Action_Explore::Execute(Elite::Blackboard* pBlackboard)
{
	m_pWander->setInterface(m_pInterface);
	m_pWander->setBlackBoard(pBlackboard);
	*m_pSteering = *m_pWander->CalculateSteering(m_AgentInfo);
	m_pSteering->AutoOrient = true;
	m_pSteering->RunMode = false;
	return false;
}
///////////////////////////////////////
//MOVETO
//****
GOAP::Action_MoveTo::Action_MoveTo()
	: BaseGoapAction("MoveTo", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	BaseGoapAction::SetEffect("targetInRange", true);
	m_pSeek = new Seek();
}

GOAP::Action_MoveTo::~Action_MoveTo()
{
	delete m_pSeek;
	m_pSeek = nullptr;
}

bool GOAP::Action_MoveTo::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("Target", m_Target)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("pSteering", m_pSteering)
		&& pBlackboard->GetData("pInterface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState);
}

bool GOAP::Action_MoveTo::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_AgentInfo.Position.DistanceSquared(m_Target) <= (m_AgentInfo.GrabRange))
	{
		m_pSteering->LinearVelocity = Elite::ZeroVector2;
		return true;
	}
	m_pSeek->SetTarget(m_Target);
	m_pSeek->setInterface(m_pInterface);
	m_pSeek->setBlackBoard(pBlackboard);
	*m_pSteering = *m_pSeek->CalculateSteering(m_AgentInfo);
	return false;
}
///////////////////////////////////////
//GRAB FOOD
//****
GOAP::Action_GrabFood::Action_GrabFood()
	:BaseGoapAction("GrabFood", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	BaseGoapAction::SetEffect("foodInInv", true);

	BaseGoapAction::SetPrecondition("targetInRange", true);
	BaseGoapAction::SetPrecondition("savedFood", true);
	BaseGoapAction::SetPrecondition("foodInInv", false);
	m_pSeek = new Seek();
}

GOAP::Action_GrabFood::~Action_GrabFood()
{
	delete m_pSeek;
	m_pSeek = nullptr;
}

bool GOAP::Action_GrabFood::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("TargetItem", m_TargetItem)
		&& pBlackboard->GetData("pInterface", m_pInterface)
		&& pBlackboard->GetData("pSteering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Food", m_pFood);
}

bool GOAP::Action_GrabFood::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pInterface->Item_Grab({}, m_TargetItem) && m_TargetItem.Type == eItemType::FOOD)
	{
		m_pInterface->Inventory_AddItem(3U, m_TargetItem);
		m_pFood->pop_back();
		*m_pWorldState = ApplyActionOnWorld(*m_pWorldState);
		return true;
	}

	return false;
}
///////////////////////////////////////
//GRAB MEDKIT
//****
GOAP::Action_GrabMedkit::Action_GrabMedkit()
	:BaseGoapAction("GrabMedKit", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	BaseGoapAction::SetPrecondition("targetInRange", true);
	BaseGoapAction::SetPrecondition("savedMedkit", true);
	BaseGoapAction::SetPrecondition("medkitInInv", false);
	BaseGoapAction::SetEffect("medkitInInv", true);
	m_pSeek = new Seek();
}

GOAP::Action_GrabMedkit::~Action_GrabMedkit()
{
	delete m_pSeek;
	m_pSeek = nullptr;
}

bool GOAP::Action_GrabMedkit::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("TargetItem", m_TargetItem)
		&& pBlackboard->GetData("pInterface", m_pInterface)
		&& pBlackboard->GetData("pSteering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("MedKits", m_pMedkits);
}

bool GOAP::Action_GrabMedkit::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pInterface->Item_Grab({}, m_TargetItem) && m_TargetItem.Type == eItemType::MEDKIT)
	{
		m_pInterface->Inventory_AddItem(2U, m_TargetItem);
		m_pMedkits->pop_back();
		*m_pWorldState = ApplyActionOnWorld(*m_pWorldState);
		return true;
	}

	return false;
}
///////////////////////////////////////
//GRAB PISTOL
//****
GOAP::Action_GrabPistol::Action_GrabPistol()
	:BaseGoapAction("GrabPistol", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	BaseGoapAction::SetPrecondition("targetInRange", true);
	BaseGoapAction::SetPrecondition("savedPistol", true);
	BaseGoapAction::SetPrecondition("pistolInInv", false);
	BaseGoapAction::SetEffect("pistolInInv", true);
	m_pSeek = new Seek();
}

GOAP::Action_GrabPistol::~Action_GrabPistol()
{
	delete m_pSeek;
	m_pSeek = nullptr;
}

bool GOAP::Action_GrabPistol::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("TargetItem", m_TargetItem)
		&& pBlackboard->GetData("pInterface", m_pInterface)
		&& pBlackboard->GetData("pSteering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Pistols", m_pPistol);
}

bool GOAP::Action_GrabPistol::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pInterface->Item_Grab({}, m_TargetItem) && m_TargetItem.Type == eItemType::PISTOL)
	{
		m_pInterface->Inventory_AddItem(0U, m_TargetItem);
		m_pPistol->pop_back();
		*m_pWorldState = ApplyActionOnWorld(*m_pWorldState);
		return true;
	}

	return false;
}
///////////////////////////////////////
//GRAB SHOTGUN
//****
GOAP::Action_GrabShotGun::Action_GrabShotGun()
	:BaseGoapAction("GrabShotGun", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	BaseGoapAction::SetPrecondition("targetInRange", true);
	BaseGoapAction::SetPrecondition("savedShotgun", true);
	BaseGoapAction::SetPrecondition("shotgunInInv", false);
	BaseGoapAction::SetEffect("shotgunInInv", true);
	m_pSeek = new Seek();
}

GOAP::Action_GrabShotGun::~Action_GrabShotGun()
{
	delete m_pSeek;
	m_pSeek = nullptr;
}

bool GOAP::Action_GrabShotGun::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("TargetItem", m_TargetItem)
		&& pBlackboard->GetData("pInterface", m_pInterface)
		&& pBlackboard->GetData("pSteering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Shotguns", m_pShotgun);
}

bool GOAP::Action_GrabShotGun::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pInterface->Item_Grab({}, m_TargetItem) && m_TargetItem.Type == eItemType::SHOTGUN)
	{
		m_pInterface->Inventory_AddItem(1U, m_TargetItem);
		m_pShotgun->pop_back();
		*m_pWorldState = ApplyActionOnWorld(*m_pWorldState);
		return true;
	}

	return false;
}
///////////////////////////////////////
//DESTROY GARBAGE
//****
GOAP::Action_DestroyGarbage::Action_DestroyGarbage()
	:BaseGoapAction("destroyGarbage", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	BaseGoapAction::SetPrecondition("targetInRange", true);
	BaseGoapAction::SetPrecondition("savedGarbage", true);
	BaseGoapAction::SetEffect("destroyedGarbage", true);
	m_pSeek = new Seek();
}

GOAP::Action_DestroyGarbage::~Action_DestroyGarbage()
{
	delete m_pSeek;
	m_pSeek = nullptr;
}

bool GOAP::Action_DestroyGarbage::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("pInterface", m_pInterface)
		&& pBlackboard->GetData("pSteering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Garbage", m_pGarbage);
}

bool GOAP::Action_DestroyGarbage::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pInterface->Item_Destroy(m_pGarbage->back()))
	{
		m_pGarbage->pop_back();
		*m_pWorldState = ApplyActionOnWorld(*m_pWorldState);
		return true;
	}
	return false;
}