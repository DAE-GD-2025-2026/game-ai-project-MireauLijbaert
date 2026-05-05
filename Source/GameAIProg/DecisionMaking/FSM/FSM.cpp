#include "FSM.h"

namespace GameAI::FSM
{
	void FSM::AddState(std::unique_ptr<State>&& NewState)
	{
		if (!CurrentState)
			CurrentState = NewState.get(); // first state added becomes initial state
		States.push_back(std::move(NewState));
	}

	void FSM::AddTransition(State* From, State* To, std::function<bool()> Condition)
	{
		Transitions.push_back({From, To, Condition});
	}

	void FSM::SetBlackboard(UBlackboardComponent* BB)
	{
		Blackboard = BB;
	}

	void FSM::Start()
	{
		if (CurrentState)
			CurrentState->OnEnter(Blackboard);
	}

	void FSM::Stop()
	{
		if (CurrentState)
			CurrentState->OnExit(Blackboard);
	}

	void FSM::Update(float DeltaTime)
	{
		if (!CurrentState) return;

		// check all transitions from current state
		for (auto& T : Transitions)
		{
			if (T.From == CurrentState && T.Condition())
			{
				CurrentState->OnExit(Blackboard);
				CurrentState = T.To;
				CurrentState->OnEnter(Blackboard);
				break; // only one transition per tick
			}
		}

		CurrentState->Update(DeltaTime, Blackboard);
	}
}