#include "SteeringBehaviors.h"

#include "MeshPaintVisualize.h"
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
	 	Agent.SetMaxLinearSpeed(0.f);
	 }
	
	else Agent.SetMaxLinearSpeed(m_OriginalMaxSpeed);
	
	// Debug
	
	return Steering;
}

SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	Agent.SetMaxLinearSpeed(0.f);
	
	return Steering;
}

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	float DistancePlayerToTarget = (Target.Position - Agent.GetPosition()).Length();
	float TimeToArrive = (DistancePlayerToTarget/ Agent.GetMaxLinearSpeed());
	FVector2D PredictedTarget = Target.Position + TimeToArrive*Target.LinearVelocity;
	Target.Position = PredictedTarget;
	Steering = Seek::CalculateSteering(DeltaT, Agent);
	
	return Steering;
}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{Pursuit::CalculateSteering(DeltaT, Agent)};
	Steering.LinearVelocity *= -1.f;
	return Steering;
	
}

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	FVector2D CircleCenter = Agent.GetPosition() + Agent.GetLinearVelocity() * m_CircleDistance;
	float Angle = FMath::FRandRange(0.f, 360.f);
	
	FVector2D Offset;
	Offset.X = FMath::Cos(Angle) * m_CircleRadius;
	Offset.Y = FMath::Sin(Angle) * m_CircleRadius;
	
	Target.Position = CircleCenter + Offset;
	Steering = Seek::CalculateSteering(DeltaT, Agent);
	
	return Steering;
}

