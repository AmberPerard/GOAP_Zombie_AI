#include "stdafx.h"
#include "../stdafx.h"
#include "SteeringBehaviors.h"
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
#include "../Data/EBlackboard.h"

using namespace Elite;

//SEEK
//****
SteeringPlugin_Output* Seek::CalculateSteering(AgentInfo pAgent)
{
	SteeringPlugin_Output* pSteering{ new SteeringPlugin_Output() };

	pSteering->LinearVelocity = (m_pInterface->NavMesh_GetClosestPathPoint(m_Target.Position) - pAgent.Position).GetNormalized();
	pSteering->LinearVelocity *= pAgent.MaxLinearSpeed;

	m_pInterface->Draw_Circle(m_Target.Position, 3.f, { 1,1,0 });

	return pSteering;
}

SteeringPlugin_Output* Flee::CalculateSteering(AgentInfo pAgent)
{
	SteeringPlugin_Output* pSteering{ new SteeringPlugin_Output() };
	const Vector2 toTarget = pAgent.Position - m_Target.Position;

	pSteering->LinearVelocity = (m_pInterface->NavMesh_GetClosestPathPoint(toTarget)).GetNormalized();
	pSteering->LinearVelocity *= pAgent.MaxLinearSpeed;

	return pSteering;
}

SteeringPlugin_Output* Arrive::CalculateSteering(AgentInfo pAgent)
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

SteeringPlugin_Output* Face::CalculateSteering(AgentInfo pAgent)
{
	SteeringPlugin_Output* pSteering{ new SteeringPlugin_Output() };
	pSteering->AutoOrient = false;

	// Vector target to agent
	const Vector2 directionVector{ m_Target.Position - pAgent.Position };
	const float desiredRotation{ VectorToOrientation(directionVector) };

	// Forward vector agent
	const float orientAgent{ pAgent.Orientation
	};

	float orientationDifference{ desiredRotation - orientAgent };

	// Stop rotating if within error margin
	if (std::abs(orientationDifference) < 0.2f)
	{
		pSteering->AngularVelocity = 0.f;
	}
	else
	{
		// Steer correct side
		pSteering->AngularVelocity = orientationDifference * pAgent.MaxLinearSpeed;
	}

	return pSteering;
}

SteeringPlugin_Output* Wander::CalculateSteering(AgentInfo pAgent)
{
	const Vector2 centerPointWanderCirlce{
		 pAgent.Position + (pAgent.LinearVelocity.GetNormalized() * m_OffsetDistance)
	};

	m_WanderAngle += Elite::ToRadians((rand() % m_MaxAngleChange) - (float(m_MaxAngleChange) / 2.f));
	Vector2 pointOnCircleRandom{ cosf(m_WanderAngle) * m_Radius,sinf(m_WanderAngle) * m_Radius };
	pointOnCircleRandom += centerPointWanderCirlce;

	m_Target = pointOnCircleRandom;

	return Seek::CalculateSteering(pAgent);
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

SteeringPlugin_Output* Pursuit::CalculateSteering(AgentInfo pAgent)
{
	const Vector2 distanceBetween = m_Target.Position - pAgent.Position;
	const float updatesAhead = distanceBetween.Magnitude() / pAgent.MaxLinearSpeed;
	const Vector2 futurePosition = m_Target.Position + m_Target.LinearVelocity * updatesAhead;
	m_Target = futurePosition;

	return Seek::CalculateSteering(pAgent);
}

SteeringPlugin_Output* Evade::CalculateSteering(AgentInfo pAgent)
{
	const Vector2 distanceBetween = m_Target.Position - pAgent.Position;
	const float updatesAhead = float(distanceBetween.Magnitude()) / (pAgent.MaxLinearSpeed * abs(m_Target.LinearVelocity.x));
	const Vector2 futurePosition = m_Target.Position + m_Target.LinearVelocity * updatesAhead;
	m_Target = futurePosition;

	return Flee::CalculateSteering(pAgent);
}

SteeringPlugin_Output* Hide::CalculateSteering(AgentInfo pAgent)
{
	/* Find the closest hiding spot. */
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
	return  Seek::CalculateSteering(pAgent);
}

SteeringPlugin_Output* Interpose::CalculateSteering(AgentInfo pAgent)
{
	//auto midCalcu = m_SecondTarget.Position + m_Target.Position;
	//auto middlePostion = Vector2{ midCalcu.x / 2,midCalcu.y / 2 };

	//m_Target = middlePostion;
	return Seek::CalculateSteering(pAgent);
}

SteeringPlugin_Output* AvoidObstacle::CalculateSteering(AgentInfo pAgent)
{
	float maxahead = 2.f;
	HouseInfo* closestobstacle{};
	float disttoclostest = float(INFINITE);
	Vector2 ahead = pAgent.Position + pAgent.LinearVelocity * maxahead;
	Vector2 ahead2 = pAgent.Position + pAgent.LinearVelocity * maxahead * 0.5;
	if (m_pBlackboard->GetData("Houses", m_pMemoryHouse))
	{
		if (!m_pMemoryHouse->empty())
		{

			for (int index{ 0 }; index < m_pMemoryHouse->size(); index++)
			{
				//float distance = Distance(ahead, m_pMemoryHouse->at(index).Center);
				//if (distance < disttoclostest)
				//{
				//	disttoclostest = distance;
				//	closestobstacle = &m_pMemoryHouse->at(index);
				//}
				float distanceBetweenObstacleAndAi = Distance(m_pMemoryHouse->at(index).Center, pAgent.Position);
				bool intersecting = Distance(m_pMemoryHouse->at(index).Center, ahead) <= m_pMemoryHouse->at(index).Size.x || Distance(m_pMemoryHouse->at(index).Center, ahead2) <= m_pMemoryHouse->at(index).Size.x + 5;
				if (distanceBetweenObstacleAndAi < disttoclostest && intersecting)
				{
					disttoclostest = distanceBetweenObstacleAndAi;
					closestobstacle = &m_pMemoryHouse->at(index);
				}
			}
		}
	}
	Vector2 avoidance = Vector2{ 0,0 };
	//check if these calculations are inside a object
	if (closestobstacle != nullptr)
	{
		avoidance.x = ahead.x - closestobstacle->Center.x;
		avoidance.y = ahead.y - closestobstacle->Center.y;
	}
	else
	{
		avoidance = Vector2{ 0,0 };
	}

	auto seekSteering = Seek::CalculateSteering(pAgent);
	seekSteering->LinearVelocity += avoidance;
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

	return Pursuit::CalculateSteering(pAgent);
}