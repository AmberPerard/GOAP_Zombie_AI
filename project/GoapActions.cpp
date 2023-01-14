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
	std::uniform_real_distribution<float> distributionY(-sizeWorld.y / 2, sizeWorld.y / 2);
	std::uniform_real_distribution<float> distributionX(-sizeWorld.x / 2, sizeWorld.x / 2);

	float randomYCoord = distributionY(generator);
	float randomXCoord = distributionX(generator);

	if (m_AgentInfo.Position.Distance(m_OldWanderPost) <= 30.f)
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

	* m_pSteering = *m_pSeek->CalculateSteering(m_AgentInfo);
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
	m_pFace = new Face();
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
	*m_pSteering = *m_pFace->CalculateSteering(m_AgentInfo);
	const Elite::Vector2 dir_vector = (m_TargetItem.Location - m_AgentInfo.Position).GetNormalized();

	const float target_angle = atan2f(dir_vector.y, dir_vector.x);
	const float agent_angle = m_AgentInfo.Orientation;
	const float delta_angle = target_angle - agent_angle;

	if (abs(delta_angle) <= 2.f)
	{
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
	m_pFace = new Face();
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
	*m_pSteering = *m_pFace->CalculateSteering(m_AgentInfo);
	const Elite::Vector2 dir_vector = (m_TargetItem.Location - m_AgentInfo.Position).GetNormalized();

	const float target_angle = atan2f(dir_vector.y, dir_vector.x);
	const float agent_angle = m_AgentInfo.Orientation;
	const float delta_angle = target_angle - agent_angle;

	if (abs(delta_angle) <= 2.f)
	{
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
	m_pFace = new Face();
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
	*m_pSteering = *m_pFace->CalculateSteering(m_AgentInfo);
	const Elite::Vector2 dir_vector = (m_TargetItem.Location - m_AgentInfo.Position).GetNormalized();

	const float target_angle = atan2f(dir_vector.y, dir_vector.x);
	const float agent_angle = m_AgentInfo.Orientation;
	const float delta_angle = target_angle - agent_angle;

	if (abs(delta_angle) <= 2.f)
	{
		if (m_pInterface->Item_Grab(m_TargetItem, *temp))
		{
			m_pInterface->Inventory_AddItem(0U, *temp);
			m_pPistol->pop_back();
			if (m_pPistol->empty())
			{
				m_pWorldState->SetCondition("savedPistol", false);
			}
			*m_pWorldState = ApplyActionOnWorld(*m_pWorldState);
			return true;
		}return false;
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
	m_pFace = new Face();
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
	*m_pSteering = *m_pFace->CalculateSteering(m_AgentInfo);
	const Elite::Vector2 dir_vector = (m_TargetItem.Location - m_AgentInfo.Position).GetNormalized();

	const float target_angle = atan2f(dir_vector.y, dir_vector.x);
	const float agent_angle = m_AgentInfo.Orientation;
	const float delta_angle = target_angle - agent_angle;

	if (abs(delta_angle) <= 2.f)
	{
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
	m_pFace = new Face();
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
	*m_pSteering = *m_pFace->CalculateSteering(m_AgentInfo);
	const Elite::Vector2 dir_vector = (m_pGarbage->back().Location - m_AgentInfo.Position).GetNormalized();

	const float target_angle = atan2f(dir_vector.y, dir_vector.x);
	const float agent_angle = m_AgentInfo.Orientation;
	const float delta_angle = target_angle - agent_angle;

	if (abs(delta_angle) <= 2.f)
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
	BaseGoapAction::SetEffect("foodInInv", false);
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
		return m_pInterface->Inventory_RemoveItem(3U);
	}
	return false;
}
///////////////////////////////////////
//KILL WITH SHOTGUN
//****
GOAP::Action_KillShotGun::Action_KillShotGun()
	:BaseGoapAction("KillShotGun", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	BaseGoapAction::SetPrecondition("shotgunInInv", true);
	BaseGoapAction::SetPrecondition("inDanger", true);
	BaseGoapAction::SetPrecondition("enemiesInRange", true);
	BaseGoapAction::SetEffect("enemiesInRange", false);
	BaseGoapAction::SetEffect("inDanger", false);
	m_pFace = new Face();
}

bool GOAP::Action_KillShotGun::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("pInterface", m_pInterface)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Enemies", m_Enemies);
}

bool GOAP::Action_KillShotGun::Execute(Elite::Blackboard* pBlackboard)
{
	m_pSteering->AutoOrient = false;
	m_pSteering->LinearVelocity = Elite::ZeroVector2;
	if (!m_Enemies.empty())
	{
		const Elite::Vector2 dir_vector = (m_Enemies.back().Location - m_AgentInfo.Position).GetNormalized();

		const float target_angle = atan2f(dir_vector.y, dir_vector.x);
		const float agent_angle = m_AgentInfo.Orientation;
		const float delta_angle = target_angle - agent_angle;

		if (abs(delta_angle) <= m_AngleError)
		{
			m_pSteering->AngularVelocity = 0.f;
			ItemInfo* temp{};
			m_pInterface->Inventory_GetItem(1U, *temp);
			if (m_pInterface->Weapon_GetAmmo(*temp) <= 0)
			{
				m_pInterface->Inventory_RemoveItem(1U);
				return false;
			}
			m_pInterface->Inventory_UseItem(1U);
			std::cout << "Enemy health: " << m_Enemies.back().Health << std::endl;
			if (m_Enemies.back().Health <= 1.f)
			{
				*m_pWorldState = ApplyActionOnWorld(*m_pWorldState);
				return true;
			}
		}
		m_pSteering->AngularVelocity = m_AgentInfo.MaxAngularSpeed;
		if (delta_angle < 0 || delta_angle > static_cast<float>(M_PI))
		{
			m_pSteering->AngularVelocity = -m_AgentInfo.MaxAngularSpeed;
		}
		return false;
	}return true;
}
///////////////////////////////////////
//KILL WITH PISTOL
//****
GOAP::Action_KillPistol::Action_KillPistol()
	:BaseGoapAction("KillPistol", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	BaseGoapAction::SetPrecondition("pistolInInv", true);
	BaseGoapAction::SetPrecondition("enemiesInRange", true);
	BaseGoapAction::SetPrecondition("inDanger", true);
	BaseGoapAction::SetEffect("enemiesInRange", false);
	BaseGoapAction::SetEffect("inDanger", false);
	m_pFace = new Face();
}

bool GOAP::Action_KillPistol::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("pInterface", m_pInterface)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Enemies", m_Enemies);
}

bool GOAP::Action_KillPistol::Execute(Elite::Blackboard* pBlackboard)
{
	m_pSteering->AutoOrient = false;
	m_pSteering->LinearVelocity = Elite::ZeroVector2;

	if (!m_Enemies.empty())
	{

		const Elite::Vector2 dir_vector = (m_Enemies.back().Location - m_AgentInfo.Position).GetNormalized();

		const float target_angle = atan2f(dir_vector.y, dir_vector.x);
		const float agent_angle = m_AgentInfo.Orientation;
		const float delta_angle = target_angle - agent_angle;

		if (abs(delta_angle) <= m_AngleError)
		{
			m_pSteering->AngularVelocity = 0.f;
			ItemInfo* temp{};
			m_pInterface->Inventory_GetItem(0U, *temp);
			if (m_pInterface->Weapon_GetAmmo(*temp) <= 0)
			{
				m_pInterface->Inventory_RemoveItem(0U);
				return false;
			}
			m_pInterface->Inventory_UseItem(0U);
			std::cout << "Enemy health: " << m_Enemies.back().Health << std::endl;
			if (m_Enemies.back().Health <= 1.f)
			{
				*m_pWorldState = ApplyActionOnWorld(*m_pWorldState);
				return true;
			}
		}
		m_pSteering->AngularVelocity = m_AgentInfo.MaxAngularSpeed;
		if (delta_angle < 0 || delta_angle > static_cast<float>(M_PI))
		{
			m_pSteering->AngularVelocity = -m_AgentInfo.MaxAngularSpeed;
		}
		return false;
	}
	return true;
}
///////////////////////////////////////
//FLEE A PURGEZONE
//****
GOAP::Action_FleePurgezone::Action_FleePurgezone()
	:BaseGoapAction("FleePurgeZone", 1)
	, m_pSteering(new SteeringPlugin_Output())
{
	BaseGoapAction::SetPrecondition("insidePurgezone", true);
	BaseGoapAction::SetEffect("insidePurgezone", false);
	m_pFlee = new Flee();
}

bool GOAP::Action_FleePurgezone::checkProceduralPreconditions(Elite::Blackboard* pBlackboard)
{
	return BaseGoapAction::checkProceduralPreconditions(pBlackboard)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("Target", m_Target)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("WorldState", m_pWorldState);
}

bool GOAP::Action_FleePurgezone::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_AgentInfo.Position.DistanceSquared(m_Target) <= (m_AgentInfo.GrabRange))
	{
		m_pSteering->LinearVelocity = Elite::ZeroVector2;
		m_pSteering->RunMode = false;
		*m_pWorldState = ApplyActionOnWorld(*m_pWorldState);
		return true;
	}

	m_pSteering->AutoOrient = true;
	*m_pSteering = *m_pFlee->CalculateSteering(m_AgentInfo);
	m_pSteering->LinearVelocity *= m_AgentInfo.MaxLinearSpeed;


	if (m_AgentInfo.Stamina > 4.f)
		m_pSteering->RunMode = true;
	else if (m_AgentInfo.Stamina == 0.f)
		m_pSteering->RunMode = false;

	return false;
}
