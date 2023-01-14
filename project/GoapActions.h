#pragma once
#include "BaseGoapAction.h"
#include "HelperStructExpension.h"
#include "SteeringBehaviors/SteeringBehaviors.h"

namespace GOAP
{
	///////////////////////////////////////
	//EXPLORE
	//****
	class Action_Explore final : public BaseGoapAction
	{
	public:
		Action_Explore();
		~Action_Explore() override;
		bool checkProceduralPreconditions(Elite::Blackboard* pBlackboard) override;
		bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		Seek* m_pSeek = {};
		SteeringPlugin_Output* m_pSteering;
		Elite::Vector2 m_OldWanderPost{};
	};
	///////////////////////////////////////
	//LootHouse
	//****
	class Action_LootHouse final : public BaseGoapAction
	{
	public:
		Action_LootHouse();
		~Action_LootHouse() override;
		bool checkProceduralPreconditions(Elite::Blackboard* pBlackboard) override;
		bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		SteeringPlugin_Output* m_pSteering;
		Seek* m_pSeek = {};
		HouseInfoExtended* m_TargetHouse;
	};
	///////////////////////////////////////
	//GRAB FOOD
	//****
	class Action_GrabFood final : public BaseGoapAction
	{
	public:
		Action_GrabFood();
		~Action_GrabFood() override;
		bool checkProceduralPreconditions(Elite::Blackboard* pBlackboard) override;
		bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		EntityInfoExtended m_TargetItem;
		std::vector<EntityInfoExtended>* m_pFood{};
		SteeringPlugin_Output* m_pSteering;
		Seek* m_pSeek = {};
	};
	///////////////////////////////////////
	//GRAB MEDKIT
	//****
	class Action_GrabMedkit final : public BaseGoapAction
	{
	public:
		Action_GrabMedkit();
		~Action_GrabMedkit() override;
		bool checkProceduralPreconditions(Elite::Blackboard* pBlackboard) override;
		bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		EntityInfoExtended m_TargetItem;
		std::vector<EntityInfoExtended>* m_pMedkits{};
		SteeringPlugin_Output* m_pSteering;
		Seek* m_pSeek = {};
	};
	///////////////////////////////////////
	//GRAB PISTOL
	//****
	class Action_GrabPistol final : public BaseGoapAction
	{
	public:
		Action_GrabPistol();
		~Action_GrabPistol() override;
		bool checkProceduralPreconditions(Elite::Blackboard* pBlackboard) override;
		bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		EntityInfoExtended m_TargetItem;
		std::vector<EntityInfoExtended>* m_pPistol{};
		SteeringPlugin_Output* m_pSteering;
		Seek* m_pSeek = {};
	};
	///////////////////////////////////////
	//GRAB SHOTGUN
	//****
	class Action_GrabShotGun final : public BaseGoapAction
	{
	public:
		Action_GrabShotGun();
		~Action_GrabShotGun() override;
		bool checkProceduralPreconditions(Elite::Blackboard* pBlackboard) override;
		bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		EntityInfoExtended m_TargetItem;
		std::vector<EntityInfoExtended>* m_pShotgun{};
		SteeringPlugin_Output* m_pSteering;
		Seek* m_pSeek = {};
	};
	///////////////////////////////////////
	//DESTROY GARBAGE
	//****
	class Action_DestroyGarbage final : public BaseGoapAction
	{
	public:
		Action_DestroyGarbage();
		~Action_DestroyGarbage() override;
		bool checkProceduralPreconditions(Elite::Blackboard* pBlackboard) override;
		bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		std::vector<EntityInfoExtended>* m_pGarbage{};
		SteeringPlugin_Output* m_pSteering;
		Seek* m_pSeek = {};
	};
	///////////////////////////////////////
	//CONSUME MEDKIT
	//****
	class Action_ConsumeMedKit final : public BaseGoapAction
	{
	public:
		Action_ConsumeMedKit();
		~Action_ConsumeMedKit() override = default;
		bool checkProceduralPreconditions(Elite::Blackboard* pBlackboard) override;
		bool Execute(Elite::Blackboard* pBlackboard) override;
	};
	///////////////////////////////////////
	//CONSUME FOOD
	//****
	class Action_ConsumeFood final : public BaseGoapAction
	{
	public:
		Action_ConsumeFood();
		~Action_ConsumeFood() override = default;
		bool checkProceduralPreconditions(Elite::Blackboard* pBlackboard) override;
		bool Execute(Elite::Blackboard* pBlackboard) override;
	};
}
