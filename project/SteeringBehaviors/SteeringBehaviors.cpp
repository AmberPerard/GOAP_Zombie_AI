//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"

using namespace Elite;

//SEEK
//****
SteeringPlugin_Output Seek::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	SteeringPlugin_Output steering = {};

	steering.LinearVelocity = m_Target.Position - pAgent.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent.MaxLinearSpeed;

	return steering;
}

SteeringPlugin_Output Flee::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	SteeringPlugin_Output steering = {};
	const Vector2 toTarget = pAgent.Position - m_Target.Position;
	const float distanceSquared = toTarget.MagnitudeSquared();


	steering.LinearVelocity = pAgent.Position - m_Target.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent.MaxLinearSpeed;

	return steering;
}

SteeringPlugin_Output Arrive::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	SteeringPlugin_Output steering = {};
	const Vector2 toTarget = m_Target.Position - pAgent.Position;

	steering.LinearVelocity = toTarget;
	steering.LinearVelocity.Normalize();

	const float distanceSquared = toTarget.MagnitudeSquared();
	if (distanceSquared < m_SlowRadius * m_SlowRadius)
	{
		steering.LinearVelocity *= pAgent.MaxLinearSpeed * distanceSquared / (m_SlowRadius * m_SlowRadius);
	}
	else
	{
		steering.LinearVelocity *= pAgent.MaxLinearSpeed;
	}

	if (distanceSquared < m_TargetRadius * m_TargetRadius)
	{
		steering.LinearVelocity = Vector2{ 0,0 };
	}
	return steering;

}

SteeringPlugin_Output Face::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	// Disable this to avoid issues
	//pAgent.SetAutoOrient(false);

	SteeringPlugin_Output steering{};

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
		steering.AngularVelocity = 0.f;
	}
	else
	{
		// Steer correct side
		steering.AngularVelocity = Clamp(direction, -1.f, 1.f) * pAgent.MaxLinearSpeed;
	}

	return steering;
}

SteeringPlugin_Output Wander::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	const Vector2 centerPointWanderCirlce{
		 pAgent.Position+ (pAgent.LinearVelocity.GetNormalized() * m_OffsetDistance)
	};

	m_WanderAngle += Elite::ToRadians((rand() % m_MaxAngleChange) - (m_MaxAngleChange / 2));
	Vector2 pointOnCircleRandom{ cosf(m_WanderAngle) * m_Radius,sinf(m_WanderAngle) * m_Radius };
	pointOnCircleRandom += centerPointWanderCirlce;

	m_Target = pointOnCircleRandom;
	return Seek::CalculateSteering(deltaT, pAgent);
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

SteeringPlugin_Output Pursuit::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	const Vector2 distanceBetween = m_Target.Position - pAgent.Position;
	const int updatesAhead = distanceBetween.Magnitude() / pAgent.MaxLinearSpeed;
	const Vector2 futurePosition = m_Target.Position + m_Target.LinearVelocity * updatesAhead;
	m_Target = futurePosition;

	return Seek::CalculateSteering(deltaT, pAgent);
}

SteeringPlugin_Output Evade::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	const Vector2 distanceBetween = m_Target.Position - pAgent.Position;
	const int updatesAhead = float(distanceBetween.Magnitude()) / (pAgent.MaxLinearSpeed * abs(m_Target.LinearVelocity.x));
	const Vector2 futurePosition = m_Target.Position + m_Target.LinearVelocity * updatesAhead;
	m_Target = futurePosition;

	return Flee::CalculateSteering(deltaT, pAgent);
}
