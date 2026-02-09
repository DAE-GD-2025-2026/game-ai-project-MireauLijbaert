#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

//SEEK
SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = Target.Position - Agent.GetPosition();

	// Add debug rendering

	return Steering;
}

//FLEE
SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{Seek::CalculateSteering(DeltaT, Agent)};
	Steering.LinearVelocity *= -1.f; // Flee is just the opposite of Seek, so we can reuse its logic and just invert the velocity

	// Add debug rendering

	return Steering;
}

//ARRIVE
SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	// Get original max speed, max speed can never be less than 0 so this should only happen once after construction
	if (m_OriginalMaxSpeed < 0.f)
	{
		m_OriginalMaxSpeed = Agent.GetMaxLinearSpeed();
	}
	
	SteeringOutput Steering{Seek::CalculateSteering(DeltaT, Agent)};
	float DistancePlayerToTarget = (Target.Position - Agent.GetPosition()).Length();
	
	 if (DistancePlayerToTarget < m_SlowRadius)
	 {
	 	// Calculate how far along the target is between the SlowRadius and the target radius
	 	float MaxSpeedScalePercentage = (DistancePlayerToTarget - m_TargetRadius) / (m_SlowRadius - m_TargetRadius);
	 	Agent.SetMaxLinearSpeed(MaxSpeedScalePercentage*m_OriginalMaxSpeed);
	 }
	 	
	 else if (DistancePlayerToTarget < m_TargetRadius)
	 {
	 	Agent.SetMaxLinearSpeed(0);
	 }
	
	else Agent.SetMaxLinearSpeed(m_OriginalMaxSpeed);
	
	// Debug
	
	return Steering;
}

