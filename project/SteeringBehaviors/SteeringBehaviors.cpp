#include "stdafx.h"
#include "SteeringBehaviors.h"
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
#include "../Data/EBlackboard.h"

using namespace Elite;

//SEEK
//****
SteeringPlugin_Output* Seek::CalculateSteering( AgentInfo pAgent)
{
	SteeringPlugin_Output* pSteering{new SteeringPlugin_Output()};

	pSteering->LinearVelocity = (m_pInterface->NavMesh_GetClosestPathPoint(m_Target.Position) - pAgent.Position).GetNormalized();
	pSteering->LinearVelocity *= pAgent.MaxLinearSpeed;

	return pSteering;
}

SteeringPlugin_Output* Flee::CalculateSteering( AgentInfo pAgent)
{
	SteeringPlugin_Output* pSteering{ new SteeringPlugin_Output() };
	const Vector2 toTarget = pAgent.Position - m_Target.Position;

	pSteering->LinearVelocity = (m_pInterface->NavMesh_GetClosestPathPoint(pAgent.Position) - m_Target.Position).GetNormalized();
	pSteering->LinearVelocity *= pAgent.MaxLinearSpeed;

	return pSteering;
}

SteeringPlugin_Output* Arrive::CalculateSteering( AgentInfo pAgent)
{
	SteeringPlugin_Output* pSteering{ new SteeringPlugin_Output() };
	const Vector2 toTarget = m_Target.Position - pAgent.Position;

	pSteering->LinearVelocity = toTarget;
	pSteering->LinearVelocity.Normalize();

	const float distanceSquared = toTarget.MagnitudeSquared();
	if (distanceSquared < m_SlowRadius * m_SlowRadius)
	{
		pSteering->LinearVelocity *= pAgent.MaxLinearSpeed * distanceSquared / (m_SlowRadius * m_SlowRadius);
	}
	else
	{
		pSteering->LinearVelocity *= pAgent.MaxLinearSpeed;
	}

	if (distanceSquared < m_TargetRadius * m_TargetRadius)
	{
		pSteering->LinearVelocity = Vector2{ 0,0 };
	}
	return pSteering;

}

SteeringPlugin_Output* Face::CalculateSteering( AgentInfo pAgent)
{
	SteeringPlugin_Output* pSteering{ new SteeringPlugin_Output() };

	// Get the rotation of the target point based on unit circle.
	const float targetRotation = atan2f(
		m_Target.Position.y - pAgent.Position.y,
		m_Target.Position.x - pAgent.Position.x
	);

	// Vector target to agent
	const Vector2 targetAgent{ m_Target.Position - pAgent.Position };

	// Forward vector agent
	const Vector2 pAgentForward{
		cosf(pAgent.Orientation),
		sinf(pAgent.Orientation)
	};

	// Determine what side to rotate through cross product
	const float direction = Cross(pAgentForward, targetAgent);

	// Stop rotating if within error margin
	if (std::abs(pAgent.Orientation - targetRotation) < 0.2f)
	{
		pSteering->AngularVelocity = 0.f;
	}
	else
	{
		// Steer correct side
		pSteering->AngularVelocity = Clamp(direction, -1.f, 1.f) * pAgent.MaxLinearSpeed;
	}

	return pSteering;
}

SteeringPlugin_Output* Wander::CalculateSteering( AgentInfo pAgent)
{
	const Vector2 centerPointWanderCirlce{
		 pAgent.Position+ (pAgent.LinearVelocity.GetNormalized() * m_OffsetDistance)
	};

	m_WanderAngle += Elite::ToRadians((rand() % m_MaxAngleChange) - (float(m_MaxAngleChange) / 2.f));
	Vector2 pointOnCircleRandom{ cosf(m_WanderAngle) * m_Radius,sinf(m_WanderAngle) * m_Radius };
	pointOnCircleRandom += centerPointWanderCirlce;

	m_Target = pointOnCircleRandom;
	return Seek::CalculateSteering( pAgent);
}

void Wander::SetWanderOffset(float offset)
{
	m_OffsetDistance = offset;
}

void Wander::SetWanderRadius(float radius)
{
	m_Radius = radius;
}

void Wander::SetMaxAngleChange(float rad)
{
	m_WanderAngle = rad;
}

SteeringPlugin_Output* Pursuit::CalculateSteering( AgentInfo pAgent)
{
	const Vector2 distanceBetween = m_Target.Position - pAgent.Position;
	const float updatesAhead = distanceBetween.Magnitude() / pAgent.MaxLinearSpeed;
	const Vector2 futurePosition = m_Target.Position + m_Target.LinearVelocity * updatesAhead;
	m_Target = futurePosition;

	return Seek::CalculateSteering( pAgent);
}

SteeringPlugin_Output* Evade::CalculateSteering( AgentInfo pAgent)
{
	const Vector2 distanceBetween = m_Target.Position - pAgent.Position;
	const float updatesAhead = float(distanceBetween.Magnitude()) / (pAgent.MaxLinearSpeed * abs(m_Target.LinearVelocity.x));
	const Vector2 futurePosition = m_Target.Position + m_Target.LinearVelocity * updatesAhead;
	m_Target = futurePosition;

	return Flee::CalculateSteering( pAgent);
}

SteeringPlugin_Output* Hide::CalculateSteering(AgentInfo pAgent)
{
	///* Find the closest hiding spot. */
	//float distToClostest = INFINITE;
	//Vector2 bestHidingSpot{};

	//for (Obstacle* obstacle : *pAgent->GetObstacles())
	//{
	//	float distAway = obstacle->GetRadius() + 5;

	//	Vector2 dir = obstacle->GetCenter() - m_Target.Position;
	//	dir.Normalize();

	//	Vector2 hidingPos = obstacle->GetCenter() + dir * distAway;

	//	float distance = Distance(hidingPos, pAgent->GetPosition());

	//	if (distance < distToClostest)
	//	{
	//		distToClostest = distance;
	//		bestHidingSpot = hidingPos;
	//	}
	//}
	//m_Target = bestHidingSpot;
	return  Seek::CalculateSteering( pAgent);
}

SteeringPlugin_Output* Interpose::CalculateSteering( AgentInfo pAgent)
{
	//auto midCalcu = m_SecondTarget.Position + m_Target.Position;
	//auto middlePostion = Vector2{ midCalcu.x / 2,midCalcu.y / 2 };

	//m_Target = middlePostion;
	return Seek::CalculateSteering( pAgent);
}

SteeringPlugin_Output* AvoidObstacle::CalculateSteering(AgentInfo pAgent)
{
	//float maxahead = 2.f;
	//HouseInfo* closestobstacle{};
	//float disttoclostest = float(INFINITE);
	//Vector2 ahead = pAgent.Position + pAgent.LinearVelocity * maxahead;
	//Vector2 ahead2 = pAgent.Position + pAgent.LinearVelocity * maxahead * 0.5;

	//for (HouseInfo* house : m_pBlackboard->GetData("Houses", m_pMemoryHouse))
	//{
	//	float distance = distance(house->Center, pAgent.Position);
	//	bool intersecting = distance(house->Center, ahead) <= house->Size.x || distance(house->Center, ahead2) <= house->Size.x + 5;
	//	if (distance < disttoclostest && intersecting)
	//	{
	//		disttoclostest = distance;
	//		closestobstacle = house;
	//	}
	//}
	//Vector2 avoidance = Vector2{ 0,0 };
	////check if these calculations are inside a object
	//if (closestobstacle != nullptr)
	//{
	//	avoidance.x = ahead.x - closestobstacle->Center.x;
	//	avoidance.y = ahead.y - closestobstacle->Center.y;
	//}
	//else
	//{
	//	avoidance = Vector2{ 0,0 };
	//}
	//
	auto seekSteering = Seek::CalculateSteering( pAgent);
	//seekSteering->LinearVelocity += avoidance;
	return seekSteering;
}

SteeringPlugin_Output* OffsetPursuit::CalculateSteering(AgentInfo pAgent)
{
	Vector2 displacement = m_Offset - pAgent.Position;
	float distance = displacement.Magnitude();

	/* Get the character's speed */
	float speed = pAgent.LinearVelocity.Magnitude();

	/* Calculate the prediction time */
	const Vector2 distanceBetween = m_Target.Position - pAgent.Position;
	const float updatesAhead = distanceBetween.Magnitude() / pAgent.MaxLinearSpeed;
	const Vector2 futurePosition = m_Target.Position + m_Offset + m_Target.LinearVelocity * updatesAhead;

	m_Target = futurePosition;

	return Pursuit::CalculateSteering( pAgent);
}