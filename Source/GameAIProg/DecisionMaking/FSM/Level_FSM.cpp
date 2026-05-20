// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_FSM.h"
#include "FSMComponent.h"
#include "DecisionMaking/FSM/States/PatrolState.h"
#include "DecisionMaking/FSM/States/ChaseState.h"
#include "DecisionMaking/FSM/States/SearchState.h"
#include "DecisionMaking/GameAIController.h"


// Sets default values
ALevel_FSM::ALevel_FSM()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_FSM::BeginPlay()
{
	Super::BeginPlay();
	
	Thief = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{200,0,90}, FRotator::ZeroRotator);
	ThiefSeek = new Seek{};
	Thief->SetSteeringBehavior(ThiefSeek);
	
	Guard = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{0,0,90}, FRotator::ZeroRotator);
	Guard->SetDebugRenderingEnabled(false);
	Guard->SetMaxLinearSpeed(Guard->GetMaxLinearSpeed()*0.5f);
	
	// Setup patrol waypoints
	std::vector<FVector2D> PatrolWaypoints = {
		FVector2D{-500, -500},
		FVector2D{ 500, -500},
		FVector2D{ 500,  500},
		FVector2D{-500,  500}
	};
	
	
	// Setup FSM
	if (AGameAIController* AIController = Cast<AGameAIController>(Guard->GetController()))
	{
		if (UFSMComponent* FSM = Cast<UFSMComponent>(AIController->GetBrainComponent()))
		{
			// Create states
			auto* Patrol = new GameAI::FSM::PatrolState(Guard, PatrolWaypoints);
			auto* Chase  = new GameAI::FSM::ChaseState(Guard, Thief);
			auto* Search = new GameAI::FSM::SearchState(Guard);

			// Add states (first added = initial state)
			FSM->AddState(std::unique_ptr<GameAI::FSM::State>(Patrol));
			FSM->AddState(std::unique_ptr<GameAI::FSM::State>(Chase));
			FSM->AddState(std::unique_ptr<GameAI::FSM::State>(Search));
			
			// Define transition lambdas by capturing 'this'
			auto IsTargetVisible = [this]() -> bool
			{
				float Distance = (Thief->GetPosition() - Guard->GetPosition()).Size();
				if (Distance > 500.f) return false;

				FHitResult HitResult;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(Guard);
				Params.AddIgnoredActor(Thief);
				bool bHit = Guard->GetWorld()->LineTraceSingleByChannel(
				   HitResult,
				   Guard->GetActorLocation(),
				   Thief->GetActorLocation(),
				   ECC_Visibility,
				   Params);
				return !bHit;
			};

			auto IsTargetNotVisible = [IsTargetVisible]() -> bool
			{
				return !IsTargetVisible();
			};

			auto IsSearchingTooLong = [this, AIController]() -> bool
			{
				if (UBlackboardComponent* BB = AIController->GetBlackboardComponent())
				{
					float SearchStartTime = BB->GetValueAsFloat(FName("SearchStartTime"));
					float CurrentTime = Guard->GetWorld()->GetTimeSeconds();
					return (CurrentTime - SearchStartTime) > 10.f;
				}
				return false;
			};

			// Add transitions
			FSM->AddTransition(Patrol, Chase, IsTargetVisible);
			FSM->AddTransition(Chase, Search, IsTargetNotVisible);
			FSM->AddTransition(Search, Chase, IsTargetVisible);
			FSM->AddTransition(Search, Patrol, IsSearchingTooLong);
			
			AIController->RunFiniteStateMachine();
		}
	}
	
}

// Called every frame
void ALevel_FSM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Set Seek target to mouse
	ThiefSeek->SetTarget(MouseTarget);
}

