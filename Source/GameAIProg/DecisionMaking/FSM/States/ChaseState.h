#pragma once
#include "DecisionMaking/FSM/State.h"
#include "Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

class ASteeringAgent;

namespace GameAI::FSM
{
	class ChaseState : public State
	{
	public:
		ChaseState(ASteeringAgent* Guard, ASteeringAgent* Thief);

		virtual void OnEnter(UBlackboardComponent* Blackboard) override;
		virtual void OnExit(UBlackboardComponent* Blackboard) override;
		virtual void Update(float DeltaTime, UBlackboardComponent* Blackboard) override;

	private:
		ASteeringAgent* Guard;
		ASteeringAgent* Thief;
		Seek SeekBehavior;
	};
}