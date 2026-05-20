#include "SearchState.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"

namespace GameAI::FSM
{
	SearchState::SearchState(ASteeringAgent* Guard)
		: Guard(Guard)
	{
	}

	void SearchState::OnEnter(UBlackboardComponent* Blackboard)
	{
		// Reset phase flag
		bReachedLastKnownLocation = false;

		// Store search start time for IsSearchingTooLong transition
		float CurrentTime = Guard->GetWorld()->GetTimeSeconds();
		Blackboard->SetValueAsFloat(FName("SearchStartTime"), CurrentTime);

		// Read last known location and move there
		FVector LastKnown = Blackboard->GetValueAsVector(FName("LastKnownLocation"));
		FTargetData Target{FVector2D(LastKnown.X, LastKnown.Y)};
		ArriveBehavior.SetTarget(Target);

		Guard->SetSteeringBehavior(&ArriveBehavior);
	}

	void SearchState::OnExit(UBlackboardComponent* Blackboard)
	{
		Guard->SetSteeringBehavior(nullptr);
		bReachedLastKnownLocation = false;
	}

	void SearchState::Update(float DeltaTime, UBlackboardComponent* Blackboard)
	{
		if (!bReachedLastKnownLocation)
		{
			// Check if we've arrived at last known location
			FVector LastKnown = Blackboard->GetValueAsVector(FName("LastKnownLocation"));
			FVector2D LastKnown2D{LastKnown.X, LastKnown.Y};
			FVector2D GuardPos = Guard->GetPosition();

			float DistanceToTarget = (LastKnown2D - GuardPos).Size();
			if (DistanceToTarget < Guard->GetCapsuleRadius())
			{
				// Arrived! Switch to wander phase
				bReachedLastKnownLocation = true;
				Guard->SetSteeringBehavior(&WanderBehavior);
			}
		}
	} 
}