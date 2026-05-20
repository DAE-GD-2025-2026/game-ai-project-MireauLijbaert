#include "ChaseState.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"

namespace GameAI::FSM
{
	ChaseState::ChaseState(ASteeringAgent* Guard, ASteeringAgent* Thief)
		: Guard(Guard)
		, Thief(Thief)
	{
	}

	void ChaseState::OnEnter(UBlackboardComponent* Blackboard)
	{
		Guard->SetSteeringBehavior(&SeekBehavior);
	}

	void ChaseState::OnExit(UBlackboardComponent* Blackboard)
	{
		// Store last known position for SearchState to use
		FVector2D ThiefPos = Thief->GetPosition();
		Blackboard->SetValueAsVector(FName("LastKnownLocation"), FVector(ThiefPos.X, ThiefPos.Y, 0));
        
		Guard->SetSteeringBehavior(nullptr);
	}

	void ChaseState::Update(float DeltaTime, UBlackboardComponent* Blackboard)
	{
		// Update seek target to thief's current position
		FVector2D ThiefPos = Thief->GetPosition();
		FTargetData Target{ThiefPos};
		SeekBehavior.SetTarget(Target);

		// Store on blackboard every tick
		Blackboard->SetValueAsVector(FName("TargetLocation"), FVector(ThiefPos.X, ThiefPos.Y, 0));
	}
}