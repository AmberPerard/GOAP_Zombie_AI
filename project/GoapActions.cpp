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
	m_pSeek = new Seek();
}

GOAP::Action_Explore::~Action_Explore()
{
	delete m_pSeek;
	m_pSeek = nullptr;
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
	m_pSeek->setInterface(m_pInterface);
	m_pSeek->setBlackBoard(pBlackboard);

	auto sizeWorld = m_pInterface->World_GetInfo().Dimensions;
	sizeWorld.y -= 200;
	sizeWorld.x -= 200;

	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_real_distribution<float> distributionY(-sizeWorld.y/2, sizeWorld.y/2);
	std::uniform_real_distribution<float> distributionX(-sizeWorld.x/2, sizeWorld.x/2);

	float randomYCoord = distributionY(generator);
	float randomXCoord = distributionX(generator);

	if(m_AgentInfo.Position.Distance(m_OldWanderPost) <= 30.f)
	{
		m_OldWanderPost = Elite::Vector2{ randomXCoord, randomYCoord };
	}
	m_pSeek->SetTarget(m_OldWanderPost);
	*m_pSteering = *m_pSeek->CalculateSteering(m_AgentInfo);
	m_pSteering->AutoOrient = true;
	m_pSteering->RunMode = false;
	return false;
}
///////////////////////////////////////
//LOOT HOUSE
//****
GOAP::Action_LootHouse::Action_LootHouse()
	: BaseGoapAction("Loothouse", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	BaseGoapAction::SetEffect("targetInRange", true);
	m_pSeek = new Seek();
}

GOAP::Action_LootHouse::~Action_LootHouse()
{
	delete m_pSeek;
	m_pSeek = nullptr;
}

bool GOAP::Action_LootHouse::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("Target", m_Target)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("pSteering", m_pSteering)
		&& pBlackboard->GetData("pInterface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState);
}

bool GOAP::Action_LootHouse::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_AgentInfo.Position.DistanceSquared(m_Target) <= (m_AgentInfo.GrabRange))
	{
		m_pSteering->LinearVelocity = Elite::ZeroVector2;
		return true;
	}
	//if (!m_TargetHouse->visitedTop)
	//{
	//	currentVisitPoint = m_TargetHouse->topPoint;
	//}else
	//{
	//	currentVisitPoint = m_TargetHouse->bottomPoint;
	//}
	m_pSeek->SetTarget(m_Target);
	m_pSeek->setInterface(m_pInterface);
	m_pSeek->setBlackBoard(pBlackboard);

	//m_pInterface->Draw_Point(currentVisitPoint, 10, { 1,0,0 });

	//if (m_AgentInfo.Position.DistanceSquared(m_TargetHouse->topPoint) <= m_AgentInfo.GrabRange * m_AgentInfo.GrabRange)
	//{
	//	currentVisitPoint = m_TargetHouse->bottomPoint;
	//	m_TargetHouse->visitedTop = true;
	//}

	//if (m_AgentInfo.Position.DistanceSquared(m_TargetHouse->bottomPoint) <= m_AgentInfo.GrabRange * m_AgentInfo.GrabRange)
	//{
	//	m_pSteering->LinearVelocity = Elite::ZeroVector2;
	//	return true;
	//}

	//m_pSeek->SetTarget(currentVisitPoint);

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
	ItemInfo* temp = new ItemInfo{};
	temp->Type = eItemType::FOOD;
	temp->ItemHash = m_TargetItem.EntityHash;
	temp->Location = m_TargetItem.Location;
	if (m_pInterface->Item_Grab(m_TargetItem, *temp))
	{
		m_pInterface->Inventory_AddItem(3U, *temp);
		m_pFood->pop_back();
		if (m_pFood->empty())
		{
			m_pWorldState->SetCondition("savedFood", false);
		}
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
		&& pBlackboard->GetData("Medkits", m_pMedkits);
}

bool GOAP::Action_GrabMedkit::Execute(Elite::Blackboard* pBlackboard)
{
	ItemInfo* temp = new ItemInfo{};
	temp->Type = eItemType::MEDKIT;
	temp->ItemHash = m_TargetItem.EntityHash;
	temp->Location = m_TargetItem.Location;
	if (m_pInterface->Item_Grab(m_TargetItem, *temp))
	{
		m_pInterface->Inventory_AddItem(2U, *temp);
		m_pMedkits->pop_back();
		if (m_pMedkits->empty())
		{
			m_pWorldState->SetCondition("savedMedkit", false);
		}
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
	ItemInfo* temp = new ItemInfo{};
	temp->Type = eItemType::PISTOL;
	temp->ItemHash = m_TargetItem.EntityHash;
	temp->Location = m_TargetItem.Location;

	if (m_pInterface->Item_Grab(m_TargetItem, *temp))
	{
		m_pInterface->Inventory_AddItem(0U, *temp);
		m_pPistol->pop_back();
		if(m_pPistol->empty())
		{
			m_pWorldState->SetCondition("savedPistol", false);
		}
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
	ItemInfo* temp = new ItemInfo{};
	temp->Type = eItemType::SHOTGUN;
	temp->ItemHash = m_TargetItem.EntityHash;
	temp->Location = m_TargetItem.Location;
	if (m_pInterface->Item_Grab(m_TargetItem, *temp))
	{
		m_pInterface->Inventory_AddItem(1U, *temp);
		m_pShotgun->pop_back();
		if (m_pShotgun->empty())
		{
			m_pWorldState->SetCondition("savedShotgun", false);
		}
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
		if (m_pGarbage->empty())
		{
			m_pWorldState->SetCondition("savedGarbage", false);
		}
		*m_pWorldState = ApplyActionOnWorld(*m_pWorldState);
		return true;
	}
	return false;
}
///////////////////////////////////////
//Consume MedKit
//****

GOAP::Action_ConsumeMedKit::Action_ConsumeMedKit()
	:BaseGoapAction("ConsumeMedKit", 2)
{
	BaseGoapAction::SetPrecondition("medkitInInv", true);
	BaseGoapAction::SetPrecondition("LowHealth", true);
	BaseGoapAction::SetEffect("LowHealth", false);
	BaseGoapAction::SetEffect("medkitInInv", false);
}


bool GOAP::Action_ConsumeMedKit::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("pInterface", m_pInterface)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("WorldState", m_pWorldState);
}

bool GOAP::Action_ConsumeMedKit::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pInterface->Inventory_UseItem(2U))
	{
		*m_pWorldState = ApplyActionOnWorld(*m_pWorldState);
		return m_pInterface->Inventory_RemoveItem(2U);
	}
	return false;
}
///////////////////////////////////////
//CONSUME FOOD
//****
GOAP::Action_ConsumeFood::Action_ConsumeFood()
	:BaseGoapAction("ConsumeFood", 2)
{
	BaseGoapAction::SetPrecondition("foodInInv", true);
	BaseGoapAction::SetPrecondition("LowFood", true);
	BaseGoapAction::SetEffect("LowFood", false);
	BaseGoapAction::SetEffect("foodtInInv", false);
}

bool GOAP::Action_ConsumeFood::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("pInterface", m_pInterface)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("WorldState", m_pWorldState);
}

bool GOAP::Action_ConsumeFood::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pInterface->Inventory_UseItem(3U))
	{
		*m_pWorldState = ApplyActionOnWorld(*m_pWorldState);
		return m_pInterface->Inventory_RemoveItem(2U);
	}
	return false;
}
