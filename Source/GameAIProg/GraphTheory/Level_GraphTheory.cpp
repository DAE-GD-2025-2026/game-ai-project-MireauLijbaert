// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_GraphTheory.h"

#include "Algorithms/EulerianPath.h"
#include "Shared/GameAISpectator.h"

using namespace GameAI;

// Sets default values
ALevel_GraphTheory::ALevel_GraphTheory()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_GraphTheory::BeginPlay()
{
	Super::BeginPlay();
	
	// Add the graph editor to our player
	if (PlayerController = Cast<APlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->PlayerController); 
		GraphEditorClass && PlayerController)
	{
		PlayerGraphEditor = NewObject<UGraphEditorComponent>(PlayerController->GetPawn(), GraphEditorClass);
		PlayerGraphEditor->RegisterComponent();
		PlayerGraphEditor->SetEditedGraph(&Graph);
		PlayerGraphEditor->SetNodeFactory(&NodeFactory);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to get PlayerController from LocalPlayer or GraphEditorClass is null"))
		return;
	}

	
	// Make the view orthogonal for less perspective issues
	if (AGameAISpectator* Player = Cast<AGameAISpectator>(PlayerController->GetPawnOrSpectator()); Player)
	{
		Player->SetCameraProjection(ECameraProjectionMode::Orthographic);
	}
	
	// Set the world for the renderer
	Renderer = GameAI::GraphRenderer(GetWorld());
	
	// TODO Make the graph and a couple connected nodes here...
	// Make nodes
	Graph.AddNode(NodeFactory.CreateNode(FVector2D{100.f, 100.f}));
	Graph.AddNode(NodeFactory.CreateNode(FVector2D{300.f, 100.f}));
	Graph.AddNode(NodeFactory.CreateNode(FVector2D{200.f, 250.f}));

	// Connect nodes
	Graph.AddConnection(0, 1);
	Graph.AddConnection(1, 2);
	Graph.AddConnection(2, 0);
	
	
	
	// Spawn the Agent
	Agent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{0,0,90}, FRotator::ZeroRotator);
	Agent->SetSteeringBehavior(&PathFollow);
	
	// Create path for the first time
	EulerianPath eulerianPath(&Graph);
	Eulerianity eulerianity = eulerianPath.IsEulerian();
	auto trail = eulerianPath.FindPath(eulerianity);
	UpdateAgentPath(trail);
	
	UE_LOG(LogTemp, Warning, TEXT("Graph node count: %i"), Graph.GetNodeCount());
	UE_LOG(LogTemp, Warning, TEXT("Active node count: %i"), Graph.GetActiveNodes().size());
}

void ALevel_GraphTheory::BeginDestroy()
{
	Super::BeginDestroy();
}

void ALevel_GraphTheory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
#pragma region UI
	{
		//Setup
		bool windowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::SetWindowFocus();
		ImGui::PushItemWidth(70);
		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
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

		ImGui::Text("Graph Theory");
		ImGui::Spacing();
		ImGui::Spacing();

		//End
		ImGui::End();
	}
#pragma endregion UI
	
	Renderer.RenderGraph(Graph);
	
	// TODO Check if the graph has updated
	if (PlayerGraphEditor && PlayerGraphEditor->HasGraphUpdated())
	{
		// TODO if so, run the EulerianPath algorithm
		EulerianPath EulerianPath(&Graph);
		Eulerianity eulerianity = EulerianPath.IsEulerian();
		auto trail = EulerianPath.FindPath(eulerianity);
		// TODO if a path is found, have the agent follow it
		// If no path is found we want the agent to not move so we pass the trail if it is empty too
		UpdateAgentPath(trail);
	}
	
	
}

void ALevel_GraphTheory::UpdateAgentPath(std::vector<Node*> const& Trail)
{
	std::vector<FVector2D> path{};
	
	// TODO convert Node vector to positions vector
	for (auto const& node : Trail)
	{
		path.emplace_back(node->GetPosition());
	}

	PathFollow.SetPath(path);
	if (path.size() > 0)
	{
		Agent->SetPosition(path[0]);
	}
}




