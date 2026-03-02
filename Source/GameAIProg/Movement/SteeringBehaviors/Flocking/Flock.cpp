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
	Neighbors.SetNum(FlockSize);
	
	// Initialize behaviours
	pSeparationBehavior = std::make_unique<Separation>(this);
	pCohesionBehavior = std::make_unique<Cohesion>(this);
	pAlignmentBehavior = std::make_unique<Alignment>(this);{};
	pSeekBehavior = std::make_unique<Seek>();
	pWanderBehavior = std::make_unique<Wander>();
	pEvadeBehavior = std::make_unique<Evade>();
	pBlendedSteering = std::make_unique<BlendedSteering>(
	std::vector<BlendedSteering::WeightedBehavior>
	{
		{pSeparationBehavior.get(), 0.f},
		{pCohesionBehavior.get(), 0.f},
		{pAlignmentBehavior.get(), 0.f},
		{pSeekBehavior.get(), 0.f},
		{pWanderBehavior.get(), 1.f},
		{pEvadeBehavior.get(), 0.f}
	});
	pPrioritySteering = std::make_unique<PrioritySteering>(
	std::vector<ISteeringBehavior*>{pEvadeBehavior.get(), pBlendedSteering.get()}
	);
	
 // TODO: initialize the flock and the memory pool
	// Unreal doesn't want to spawn them because they all spawn at 0,0 collide and default is don't spawn if they collide
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (auto& agent : Agents)
	{
		agent = pWorld->SpawnActor<ASteeringAgent>(AgentClass, SpawnParams);
		agent->SetSteeringBehavior(pBlendedSteering.get());
		agent->SetActorLocation(FVector( FMath::FRandRange(-1000.f, 1000.f),
		FMath::FRandRange(-1000.f, 1000.f), 0.f));
		agent->SetDebugRenderingEnabled(false);
	}
	Agents[0]->SetDebugRenderingEnabled(true);
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
	for (auto& agent : Agents )
	{
		RegisterNeighbors(agent);
		agent->Tick(DeltaTime);
	}
}

void Flock::RenderDebug()
{
 // TODO: Render all the agents in the flock
	FVector2D Velocity = Agents[0]->GetLinearVelocity();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,                 // key (-1 = new message every time)
			0.f,                // display time (0 = one frame)
			FColor::Green,
			FString::Printf(
				TEXT("Velocity: X=%.2f Y=%.2f"),
				Velocity.X,
				Velocity.Y
			)
		);
	}
	
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

		// Debug
		bool isChecked = Agents[0]->GetDebugRenderingEnabled();
		if (ImGui::Checkbox("Debug Rendering first agent", &isChecked))
		{
			Agents[0]->SetDebugRenderingEnabled(isChecked);
		}
		ImGui::Spacing();
		
		// Weight sliders
		ImGui::Text("Behavior Weights");
		ImGui::Spacing();
		ImGui::SliderFloat("Separation", &pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0, 1);
		ImGui::SliderFloat("Cohesion", &pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0, 1);
		ImGui::SliderFloat("Alignment", &pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight, 0, 1);
		ImGui::SliderFloat("Seek", &pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight, 0, 1);
		ImGui::SliderFloat("Wander", &pBlendedSteering->GetWeightedBehaviorsRef()[4].Weight, 0, 1);
		ImGui::SliderFloat("Evade", &pBlendedSteering->GetWeightedBehaviorsRef()[5].Weight, 0, 1);
		ImGui::Spacing();
		
		
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
	// Keeps the nr of neighbors
	NrOfNeighbors = 0;
	for (auto agent : Agents)
	{
		if (agent == pAgent) continue;
			
		auto DistanceSquared = (agent->GetPosition() - pAgent->GetPosition()).SquaredLength();
		auto RadiusSquared = NeighborhoodRadius * NeighborhoodRadius;
		if (DistanceSquared <= RadiusSquared)
		{
			// Only need to change neighbors in the array perposition of nrofneighbors, 
			// old neighbors still in here (if there were more previously just don't get registers as it's outside of nr of neighbors
			Neighbors[NrOfNeighbors] = agent;
			++NrOfNeighbors;
		}
	}
}
#endif

FVector2D Flock::GetAverageNeighborPos() const
{
	FVector2D avgPosition = FVector2D::ZeroVector;
	
	for (int i = 0; i < NrOfNeighbors; ++i)
	{
		avgPosition += Neighbors[i]->GetPosition();
	}
	
	// Check so we don't divide by 0
	if (NrOfNeighbors > 0)avgPosition /= NrOfNeighbors;
	return avgPosition;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D avgVelocity = FVector2D::ZeroVector;
	
	for (int i = 0; i < NrOfNeighbors; ++i)
	{
		avgVelocity += Neighbors[i]->GetLinearVelocity();
	}
	
	// Check so we don't divide by 0
	if (NrOfNeighbors > 0)avgVelocity /= NrOfNeighbors;

	return avgVelocity;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
  pSeekBehavior->SetTarget(Target);
}