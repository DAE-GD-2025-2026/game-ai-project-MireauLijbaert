#pragma once
#include <functional>

namespace GameAI::FSM
{
	class State;

	class Transition
	{
	public:
		State* From;
		State* To;
		std::function<bool()> Condition;
	};
}