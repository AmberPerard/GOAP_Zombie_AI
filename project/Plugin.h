#pragma once
#include "BaseGoapAction.h"
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
#include "GoapAstart.h"
#include "WorldState.h"
#include "SteeringBehaviors/SteeringBehaviors.h"

class IBaseInterface;
class IExamInterface;

class Plugin :public IExamPlugin
{
public:
	Plugin() = default;
	virtual ~Plugin() {}

	void Initialize(IBaseInterface* pInterface, PluginInfo& info) override;
	void DllInit() override;
	void DllShutdown() override;

	void InitGameDebugParams(GameDebugParams& params) override;
	void Update(float dt) override;

	SteeringPlugin_Output UpdateSteering(float dt) override;
	void Render(float dt) const override;

private:
	//Interface, used to request data from/perform actions with the AI Framework
	IExamInterface* m_pInterface = nullptr;
	std::vector<HouseInfo> GetHousesInFOV() const;
	void GetEntitiesInFOV();

	Elite::Vector2 m_Target = {};
	bool m_CanRun = false; //Demo purpose
	bool m_GrabItem = false; //Demo purpose
	bool m_UseItem = false; //Demo purpose
	bool m_RemoveItem = false; //Demo purpose
	float m_AngSpeed = 0.f; //Demo purpose

	UINT m_InventorySlot = 0;
	UINT m_ItemsInInventory = 0;

	//steering
	SteeringPlugin_Output* m_pSteering{};


	// Aquired entities
	std::vector<HouseInfo>* m_pMemoryHouse{};
	std::vector<EntityInfo>* m_pMemoryEntities{};
	std::vector<EnemyInfo> m_EnemiesInFOV{};
	std::vector<ItemInfo>* m_pMemoryPistol{};
	std::vector<ItemInfo>* m_pMemoryShotGuns{};
	std::vector<ItemInfo>* m_pMemoryMedKits{};
	std::vector<ItemInfo>* m_pMemoryFood{};
	std::vector<ItemInfo>* m_pMemoryGarbage{};

	// In current FOV
	std::vector<EntityInfo> m_EntitiesInFov{};
	std::vector<ItemInfo> m_ItemsInFov{};
	PurgeZoneInfo m_PurgeZoneInFov{};

	// GOAP
	WorldState m_WorldState{};
	std::vector<BaseGoapAction*> m_pPlan{};
	std::vector<BaseGoapAction*> m_pActions{};
	std::vector<WorldState*> m_pGoals{};
	WorldState* m_CurrentGoal{};
	GoapAstar m_ASPlanner{};

	Elite::Blackboard* m_pBlackboard{};
	void CreateBlackboard();
	void InitializeWorldState();
	void AddActions();
	void AddGoals();

	void UpdateHousesInformation(float deltaTime);
	bool FindingPath(const WorldState& worldState, const WorldState& desiredState, std::vector<BaseGoapAction*>& actions);
	bool ExecutingPlan();
	WorldState* GetHighestPriorityGoal();
};

//ENTRY
//This is the first function that is called by the host program
//The plugin returned by this function is also the plugin used by the host program
extern "C"
{
	__declspec (dllexport) IPluginBase* Register()
	{
		return new Plugin();
	}
}