#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	Target.Position = pFlock->GetAverageNeighborPos();
	
	return Seek::CalculateSteering(deltaT, pAgent);
}

//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput steering{};
	
	if (pFlock->GetNrOfNeighbors() == 0) return steering;
	
	for (int i = 0; i < pFlock->GetNrOfNeighbors(); ++i)
	{
		FVector Direction = pAgent.GetActorLocation()- pFlock->GetNeighbors()[i]->GetActorLocation();
		FVector2D Direction2D = FVector2D{ Direction.X, Direction.Y };
		steering.LinearVelocity += Direction2D.GetSafeNormal() * (1.f/Direction2D.Length());
	}
	steering.LinearVelocity = steering.LinearVelocity.GetSafeNormal() * pAgent.GetMaxLinearSpeed();
	return steering;
}


//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput Alignment::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput steering{};
	steering.LinearVelocity = pFlock->GetAverageNeighborVelocity().GetSafeNormal() * pAgent.GetMaxLinearSpeed();
	return steering; 
}