#pragma once
#include "BehaviorTree/BlackboardComponent.h"

namespace GameAI::FSM
{
	class State
	{
	public:
		virtual ~State() = default;
		virtual void OnEnter(UBlackboardComponent* Blackboard) {}
		virtual void OnExit(UBlackboardComponent* Blackboard) {}
		virtual void Update(float DeltaTime, UBlackboardComponent* Blackboard) {}
	};
}