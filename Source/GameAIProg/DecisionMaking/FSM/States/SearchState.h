#pragma once
#include "DecisionMaking/FSM/State.h"
#include "Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

class ASteeringAgent;

namespace GameAI::FSM
{
	class SearchState : public State
	{
	public:
		SearchState(ASteeringAgent* Guard);

		virtual void OnEnter(UBlackboardComponent* Blackboard) override;
		virtual void OnExit(UBlackboardComponent* Blackboard) override;
		virtual void Update(float DeltaTime, UBlackboardComponent* Blackboard) override;

	private:
		ASteeringAgent* Guard;
		Arrive ArriveBehavior;
		Wander WanderBehavior;
		bool bReachedLastKnownLocation = false;
	};
}