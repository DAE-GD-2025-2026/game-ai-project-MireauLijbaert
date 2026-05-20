#include "PatrolState.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"

namespace GameAI::FSM
{
	PatrolState::PatrolState(ASteeringAgent* Agent, std::vector<FVector2D> Waypoints)
		: Agent(Agent)
		, Waypoints(Waypoints)
	{
	}

	void PatrolState::OnEnter(UBlackboardComponent* Blackboard)
	{
		PathFollowBehavior.SetPath(Waypoints);
		Agent->SetSteeringBehavior(&PathFollowBehavior);
	}

	void PatrolState::OnExit(UBlackboardComponent* Blackboard)
	{
		Agent->SetSteeringBehavior(nullptr);
	}

	void PatrolState::Update(float DeltaTime, UBlackboardComponent* Blackboard)
	{
		// PathFollow handles everything internally, nothing to do here
	}
}