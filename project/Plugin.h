#pragma once
#include "BaseGoapAction.h"
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
#include "GoapAstart.h"
#include "HelperStructExpension.h"
#include "WorldState.h"

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
	void GetEntitiesInFOV();
	void GetNewHousesInFOV(float deltaTime);
	void GetEnemiesInFOV();
	bool CheckIfInisdePurgeZone();


	Elite::Vector2 m_Target = {};

	//steering
	SteeringPlugin_Output* m_pSteering{};


	// Aquired entities
	std::vector<HouseInfoExtended>* m_pMemoryHouse{};
	std::vector<EntityInfoExtended>* m_pMemoryEntities{};
	std::vector<EnemyInfo> m_EnemiesInFOV{};
	std::vector<EntityInfoExtended>* m_pMemoryPistol{};
	std::vector<EntityInfoExtended>* m_pMemoryShotGuns{};
	std::vector<EntityInfoExtended>* m_pMemoryMedKits{};
	std::vector<EntityInfoExtended>* m_pMemoryFood{};
	std::vector<EntityInfoExtended>* m_pMemoryGarbage{};

	// In current FOV
	std::vector<EntityInfoExtended> m_EntitiesInFov{};
	std::vector<ItemInfoExtended> m_ItemsInFov{};
	PurgeZoneInfo m_PurgeZoneInFov{};

	// GOAP
	WorldState m_WorldState{};
	std::vector<BaseGoapAction*> m_pPlan{};
	std::vector<BaseGoapAction*> m_pActions{};
	std::vector<WorldState*> m_pGoals{};
	WorldState* m_CurrentGoal{};
	GoapAstar m_ASPlanner{};

	//timer
	float m_DeltaTime = 0.f;
	float m_TotalElapsedTime = 0.f;

	Elite::Blackboard* m_pBlackboard{};
	void CreateBlackboard();
	void InitializeWorldState();
	void AddActions();
	void AddGoals();

	bool FindingPath(const WorldState& worldState, const WorldState& desiredState, std::vector<BaseGoapAction*>& actions);
	bool ExecutingPlan();
	WorldState* GetHighestPriorityGoal();

	void updateHousesInMemory();
	template<typename T>
	void SortByDistance(std::vector<T>* entities)
	{
		if (entities->empty()) return;

		std::sort(entities->begin(), entities->end(), [&](const T& a, const T& b)
			{
				const Elite::Vector2 agentPos{ m_pInterface->Agent_GetInfo().Position };
		const float distToA{ a.Location.DistanceSquared(agentPos) };
		const float distToB{ b.Location.DistanceSquared(agentPos) };

		return distToA > distToB;
			}
		);
	}

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