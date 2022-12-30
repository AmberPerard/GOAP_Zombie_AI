#include "stdafx.h"
#include "CombinedSteeringBehaviors.h"

BlendedSteering::BlendedSteering(std::vector<WeightedBehavior> weightedBehaviors)
	:m_WeightedBehaviors(weightedBehaviors)
{
};

//****************
//BLENDED STEERING
SteeringOutput BlendedSteering::CalculateSteering(float deltaT, AgentInfo* pAgent)
{
	SteeringOutput blendedSteering = {};
	auto totalWeight = 0.f;

	for (auto weightedBehavior : m_WeightedBehaviors)
	{
		auto steering = weightedBehavior.pBehavior->CalculateSteering(deltaT, pAgent);
		blendedSteering.LinearVelocity += weightedBehavior.weight * steering.LinearVelocity;
		blendedSteering.AngularVelocity += weightedBehavior.weight * steering.AngularVelocity;

		totalWeight += weightedBehavior.weight;
	}

	if (totalWeight > 0.f)
	{
		auto scale = 1.f / totalWeight;
		blendedSteering *= scale;
	}

	return blendedSteering;
}

//*****************
//PRIORITY STEERING
SteeringOutput PrioritySteering::CalculateSteering(float deltaT, AgentInfo* pAgent)
{
	SteeringOutput steering = {};

	for (auto pBehavior : m_PriorityBehaviors)
	{
		steering = pBehavior->CalculateSteering(deltaT, pAgent);

		if (steering.IsValid)
			break;
	}

	//If non of the behavior return a valid output, last behavior is returned
	return steering;
}