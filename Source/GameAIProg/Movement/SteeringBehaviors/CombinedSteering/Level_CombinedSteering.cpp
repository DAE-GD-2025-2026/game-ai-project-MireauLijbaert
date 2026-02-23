#include "Level_CombinedSteering.h"

#include "imgui.h"
#include "format"
#include "MovieSceneTrack.h"
#include "Movement/SteeringBehaviors/Steering/Level_SteeringBehaviors.h"


// Sets default values
ALevel_CombinedSteering::ALevel_CombinedSteering()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_CombinedSteering::BeginPlay()
{
	Super::BeginPlay();
	
	// Make drunk agent
	// Spawn unreal actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	m_pDrunkAgent = GetWorld()->SpawnActor<ASteeringAgent>(
		SteeringAgentClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);
	
	// Configure DrunkAgent Behaviour
	if (!m_pDrunkAgent) return;
	m_pSeekBehaviour =  new Seek{};
	m_pDrunkWanderBehavior = new Wander{};
	m_pDrunkSteering = new BlendedSteering{
				{
					{m_pSeekBehaviour , 0.5f}, 
					{m_pDrunkWanderBehavior , 0.5f}
			
				}};
	m_pDrunkAgent->SetSteeringBehavior(m_pDrunkSteering);
	
	// Make evading agent
	// Spawn unreal actor
	m_pEvadingAgent = GetWorld()->SpawnActor<ASteeringAgent>(
		SteeringAgentClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	// Configure EvadingAgent Behaviour
	if (!m_pEvadingAgent) return;
	m_pEvadeBehaviour = new Evade{};
	m_pEvadingWanderBehaviour = new Wander{};
	m_pEvadingSteering = new PrioritySteering{
			{
				m_pEvadeBehaviour, m_pEvadingWanderBehaviour
			}};
	
	
	m_pEvadingAgent->SetSteeringBehavior(m_pEvadingSteering);
}

void ALevel_CombinedSteering::BeginDestroy()
{
	// Cleanup Drunk behaviors
	delete m_pDrunkSteering;
	delete m_pSeekBehaviour;
	delete m_pDrunkWanderBehavior;
	
	// Cleanup Evading behaviors
	delete m_pEvadingSteering;
	delete m_pEvadeBehaviour;
	delete m_pEvadingWanderBehaviour;
	
	Super::BeginDestroy();
	
	
}

// Called every frame
void ALevel_CombinedSteering::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
#pragma region UI
	//UI
	{
		//Setup
		bool windowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Game AI", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	
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
		ImGui::Spacing();
	
		ImGui::Text("Flocking");
		ImGui::Spacing();
		ImGui::Spacing();
	
		if (ImGui::Checkbox("Debug Rendering", &CanDebugRender))
		{
   // TODO: Handle the debug rendering of your agents here :)
		}
		ImGui::Checkbox("Trim World", &TrimWorld->bShouldTrimWorld);
		if (TrimWorld->bShouldTrimWorld)
		{
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Trim Size",
				TrimWorld->GetTrimWorldSize(), 1000.f, 3000.f,
				[this](float InVal) { TrimWorld->SetTrimWorldSize(InVal); });
		}
		
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
	
		ImGui::Text("Behavior Weights");
		ImGui::Spacing();


		// ImGuiHelpers::ImGuiSliderFloatWithSetter("Seek",
		// 	pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
		// 	[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight = InVal; }, "%.2f");
		//
		// ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
		// pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
		// [this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight = InVal; }, "%.2f");
	
		//End
		ImGui::End();
	}
#pragma endregion
	
	// Combined Steering Update
 // TODO: implement handling mouse click input for seek
	m_pSeekBehaviour->SetTarget(MouseTarget);
	DrawDebugPoint(GetWorld(), FVector(MouseTarget.Position, 0), 10, FColor::Yellow );
	
	
 // TODO: implement Make sure to also evade the wanderer
	// Make the drunk agent the evading agent's target
	FTargetData Target;
	Target.Position = m_pDrunkAgent->GetPosition();
	Target.Orientation = m_pDrunkAgent->GetRotation();
	Target.LinearVelocity = m_pDrunkAgent->GetLinearVelocity();
	Target.AngularVelocity = m_pDrunkAgent->GetAngularVelocity();
	
	m_pEvadeBehaviour->SetTarget(Target);
}
