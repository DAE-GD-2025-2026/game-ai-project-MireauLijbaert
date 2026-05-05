#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "BehaviorTree/BlackboardComponent.h"
#include "State.h"
#include "Transition.h"

namespace GameAI::FSM
{
	class FSM
	{
	public:
		void AddState(std::unique_ptr<State>&& NewState);
		void AddTransition(State* From, State* To, std::function<bool()> Condition);

		void SetBlackboard(UBlackboardComponent* BB);
		void Start();
		void Stop();
		void Update(float DeltaTime);

	private:
		std::vector<std::unique_ptr<State>> States;
		std::vector<Transition> Transitions;
		State* CurrentState = nullptr;
		UBlackboardComponent* Blackboard = nullptr;
	};
}