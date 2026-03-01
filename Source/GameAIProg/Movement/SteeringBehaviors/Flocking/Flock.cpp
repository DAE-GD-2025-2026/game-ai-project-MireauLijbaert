#include "Flock.h"
#include "FlockingSteeringBehaviors.h"
#include "Shared/ImGuiHelpers.h"


Flock::Flock(
	UWorld* pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize,
	float WorldSize,
	ASteeringAgent* const pAgentToEvade,
	bool bTrimWorld)
	: pWorld{pWorld}
	, FlockSize{ FlockSize }
	, pAgentToEvade{pAgentToEvade}
{
	// Reserve space for the agents in flock and the neighbors, use a big memory to have a memory pool without resizing
	Agents.SetNum(FlockSize);
	Agents.Reserve(FlockSize);
	Neighbors.SetNum(FlockSize);
	Neighbors.Reserve(FlockSize);
	
	// Initialize behaviours
	//std::unique_ptr<Separation> pSeparationBehavior{};
	pCohesionBehavior = std::make_unique<Cohesion>(this);
	//std::unique_ptr<VelocityMatch> pVelMatchBehavior{};
	pSeekBehavior = std::make_unique<Seek>();
	pWanderBehavior = std::make_unique<Wander>();
	pEvadeBehavior = std::make_unique<Evade>();
	pBlendedSteering = std::make_unique<BlendedSteering>(
	std::vector<BlendedSteering::WeightedBehavior>
	{
		{pCohesionBehavior.get(), 0.f},
		{pSeekBehavior.get(), 0.f},
		{pWanderBehavior.get(), 0.f},
		{pEvadeBehavior.get(), 0.f}
	});
	pPrioritySteering = std::make_unique<PrioritySteering>(
	std::vector<ISteeringBehavior*>{pEvadeBehavior.get(), pBlendedSteering.get()}
	);
	
 // TODO: initialize the flock and the memory pool
	for (auto& agent : Agents)
	{
		agent = pWorld->SpawnActor<ASteeringAgent>(AgentClass);
		agent->SetSteeringBehavior(pPrioritySteering.get());
		agent->SetActorLocation(FVector{0.f, 0.f, 0.f});
	}
}

Flock::~Flock()
{
 // TODO: Cleanup any additional data
}

void Flock::Tick(float DeltaTime)
{
 // TODO: update the flock
 // TODO: for every agent:
  // TODO: register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent)
  // TODO: update the agent (-> the steeringbehaviors use the neighbors in the memory pool)
  // TODO: trim the agent to the world
}

void Flock::RenderDebug()
{
 // TODO: Render all the agents in the flock
}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		bool bWindowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &bWindowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();

  // TODO: implement ImGUI checkboxes for debug rendering here

		ImGui::Text("Behavior Weights");
		ImGui::Spacing();

  // TODO: implement ImGUI sliders for steering behavior weights here
		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
 // TODO: Debugrender the neighbors for the first agent in the flock
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(ASteeringAgent* const pAgent)
{
 // TODO: Implement
	for (auto agent : Agents)
	{
		if (agent != pAgent) continue;
			
		auto Distance = std::abs((agent->GetPosition() - pAgent->GetPosition()).Length());
		if (Distance <= NeighborhoodRadius)
		{
			Neighbors.Add(agent); 
		}
	}
}
#endif

FVector2D Flock::GetAverageNeighborPos() const
{
	FVector2D avgPosition = FVector2D::ZeroVector;

 // TODO: Implement
	
	return avgPosition;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D avgVelocity = FVector2D::ZeroVector;

 // TODO: Implement

	return avgVelocity;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
 // TODO: Implement
}

