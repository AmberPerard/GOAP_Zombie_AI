#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"
#include "GoapActions.h"

using namespace std;

//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);

	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "Alynxx";
	info.Student_FirstName = "Amber";
	info.Student_LastName = "Perard";
	info.Student_Class = "2DAE08E";

	m_pMemoryHouse = new std::vector<HouseInfo>;
	m_pMemoryEntities = new std::vector<EntityInfo>;
	m_pMemoryPistol = new std::vector<ItemInfo>;
	m_pMemoryShotGuns = new std::vector<ItemInfo>;
	m_pMemoryMedKits = new std::vector<ItemInfo>;
	m_pMemoryFood = new std::vector<ItemInfo>;
	m_pMemoryGarbage = new std::vector<ItemInfo>;

	CreateBlackboard();
	InitializeWorldState();
	AddActions();
	AddGoals();
}

//Called only once
void Plugin::DllInit()
{
	//Called when the plugin is loaded
}

//Called only once
void Plugin::DllShutdown()
{
	//Called wheb the plugin gets unloaded
}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = true; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be useful to inspect certain behaviors (Default = false)
	params.LevelFile = "GameLevel.gppl";
	params.AutoGrabClosestItem = true; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
	params.StartingDifficultyStage = 1;
	params.InfiniteStamina = false;
	params.SpawnDebugPistol = true;
	params.SpawnDebugShotgun = true;
	params.SpawnPurgeZonesOnMiddleClick = true;
	params.PrintDebugMessages = true;
	params.ShowDebugItemNames = true;
	params.Seed = 34;
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update(float dt)
{
	//Demo Event Code
	//In the end your AI should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	{
		m_CanRun = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	{
		m_AngSpeed -= Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	{
		m_AngSpeed += Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	{
		m_GrabItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	{
		m_UseItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	{
		m_RemoveItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	{
		m_CanRun = false;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Delete))
	{
		m_pInterface->RequestShutdown();
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_KP_Minus))
	{
		if (m_InventorySlot > 0)
			--m_InventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_KP_Plus))
	{
		if (m_InventorySlot < 4)
			++m_InventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Q))
	{
		ItemInfo info = {};
		m_pInterface->Inventory_GetItem(m_InventorySlot, info);
		std::cout << (int)info.Type << std::endl;
	}
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	GetEntitiesInFOV();

	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	auto agentInfo = m_pInterface->Agent_GetInfo();
	m_pBlackboard->ChangeData("Target", m_Target);
	m_pBlackboard->ChangeData("AgentInfo", agentInfo);

	m_WorldState.SetCondition("lowHealth", agentInfo.Health <= 4.f);
	m_WorldState.SetCondition("lowFood", agentInfo.Energy <= 4.f);
	m_WorldState.SetCondition("inDanger", agentInfo.WasBitten || m_WorldState.getCondition("inDanger"));

	//Use the navmesh to calculate the next navmesh point
	//auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(checkpointLocation);
	WorldState* newGoal{};
	for (const auto goal : m_pGoals)
	{
		if ((newGoal == nullptr || goal->m_Priority > newGoal->m_Priority))
		{
			newGoal = goal;
		}
	}
	if (m_CurrentGoal == nullptr || newGoal != m_CurrentGoal || empty(m_pPlan))
	{
		m_CurrentGoal = newGoal;

		std::cout << "Finding a plan for: " << m_CurrentGoal->m_Name << std::endl;
		try
		{
			m_pPlan = m_ASPlanner.FindCurrentActions(m_WorldState, *m_CurrentGoal, m_pActions);
			if (!empty(m_pPlan))
			{
				std::cout << "Found a plan: ";
				for (auto action : m_pPlan)
				{
					std::cout << action->GetName();
				}
				std::cout << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	else
	{
		if (!empty(m_pPlan))
		{
			// There are still actions in the plan, execute the first action in line
			BaseGoapAction* currentAction = m_pPlan.back();
			if (currentAction->Execute(m_pBlackboard))
			{
				std::cout << "Finished execute action: " << currentAction->GetName() << std::endl;
				m_pPlan.pop_back();
			}
		}
	}

	return *m_pSteering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
}

vector<HouseInfo> Plugin::GetHousesInFOV() const
{
	vector<HouseInfo> vHousesInFOV = {};

	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetHouseByIndex(i, hi))
		{
			vHousesInFOV.push_back(hi);
			continue;
		}

		break;
	}

	return vHousesInFOV;
}

void Plugin::GetEntitiesInFOV()
{


	EntityInfo entityInfo = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, entityInfo))
		{

			//// Check if we're not already aware of the entity
			//if (std::find(m_pMemoryEntities->begin(), m_pMemoryEntities->end(), entityInfo) == m_pMemoryEntities->end())
			//{
			m_pMemoryEntities->push_back(entityInfo);

			if (entityInfo.Type != eEntityType::ITEM) continue;

			ItemInfo item{};
			m_pInterface->Item_GetInfo(entityInfo, item);
			switch (item.Type)
			{
			case eItemType::PISTOL:
				m_pMemoryPistol->emplace_back(item);
				m_WorldState.SetCondition("savedPistol", true);
				break;
			case eItemType::SHOTGUN:
				m_pMemoryShotGuns->emplace_back(item);
				m_WorldState.SetCondition("savedShotgun", true);
				break;
			case eItemType::MEDKIT:
				m_pMemoryMedKits->emplace_back(item);
				m_WorldState.SetCondition("savedMedkit", true);
				break;
			case eItemType::FOOD:
				m_pMemoryFood->emplace_back(item);
				m_WorldState.SetCondition("savedFood", true);
				break;
			case eItemType::GARBAGE:
				m_pMemoryGarbage->emplace_back(item);
				m_WorldState.SetCondition("savedGarbage", true);
				break;
			default:
				continue;
			}
		}

		break;

	}
}

void Plugin::CreateBlackboard()
{
	m_pBlackboard = new Elite::Blackboard();
	m_pBlackboard->AddData("WorldState", &m_WorldState);
	m_pBlackboard->AddData("AgentInfo", AgentInfo{});
	m_pBlackboard->AddData("TargetItem", ItemInfo{});
	m_pBlackboard->AddData("InventorySlot", 0U);
	m_pBlackboard->AddData("Target", Elite::Vector2{});
	m_pBlackboard->AddData("Steering", m_pSteering);
	m_pBlackboard->AddData("Interface", m_pInterface);

	// Entities
	m_pBlackboard->AddData("Houses", m_pMemoryHouse);
	m_pBlackboard->AddData("Pistols", m_pMemoryPistol);
	m_pBlackboard->AddData("Shotguns", m_pMemoryShotGuns);
	m_pBlackboard->AddData("Medkits", m_pMemoryMedKits);
	m_pBlackboard->AddData("Food", m_pMemoryFood);
	m_pBlackboard->AddData("Garbage", m_pMemoryGarbage);
	m_pBlackboard->AddData("Enemies", std::vector<EnemyInfo>{});
}

void Plugin::InitializeWorldState()
{
	// Initial world state
	m_WorldState.SetCondition("insidePurgezone", false);
	m_WorldState.SetCondition("inDanger", false);
	m_WorldState.SetCondition("LowHealth", false);
	m_WorldState.SetCondition("LowFood", false);

	m_WorldState.SetCondition("enemiesInRange", false);
	m_WorldState.SetCondition("houseInRange", false);
	m_WorldState.SetCondition("targetInRange", false);
	m_WorldState.SetCondition("itemInRange", false);
	m_WorldState.SetCondition("garbageInRange", false);

	m_WorldState.SetCondition("savedPistol", false);
	m_WorldState.SetCondition("savedShotgun", false);
	m_WorldState.SetCondition("savedMedkit", false);
	m_WorldState.SetCondition("savedFood", false);
	m_WorldState.SetCondition("savedGarbage", false);
	m_WorldState.SetCondition("destroyedGarbage", false);

	m_WorldState.SetCondition("pistolInInv", false);
	m_WorldState.SetCondition("foodInInv", false);
	m_WorldState.SetCondition("shotgunInInv", false);
	m_WorldState.SetCondition("medkitInInv", false);

	m_WorldState.SetCondition("exploring", false);
}

void Plugin::AddActions()
{
	m_pActions.push_back(new GOAP::Action_Explore);
	m_pActions.push_back(new GOAP::Action_MoveTo);
}

void Plugin::AddGoals()
{
	m_pGoals.push_back(new Goal_ExploreWorld);
	m_pGoals.push_back(new Goal_LootHouse);
}
