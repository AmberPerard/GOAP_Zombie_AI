#pragma once
#include "BaseGoapAction.h"
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
		Wander* m_pWander = {};
		SteeringPlugin_Output* m_pSteering;
	};
	///////////////////////////////////////
	//MOVETO
	//****
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
		ItemInfo m_TargetItem;
		std::vector<ItemInfo>* m_pFood{};
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
		ItemInfo m_TargetItem;
		std::vector<ItemInfo>* m_pMedkits{};
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
		ItemInfo m_TargetItem;
		std::vector<ItemInfo>* m_pPistol{};
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
		ItemInfo m_TargetItem;
		std::vector<ItemInfo>* m_pShotgun{};
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
		ItemInfo m_TargetItem;
		std::vector<EntityInfo>* m_pGarbage{};
		SteeringPlugin_Output* m_pSteering;
		Seek* m_pSeek = {};
	};
}
