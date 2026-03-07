// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_Flocking.h"


// Sets default values
ALevel_Flocking::ALevel_Flocking()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_Flocking::BeginPlay()
{
	Super::BeginPlay();

	TrimWorld->SetTrimWorldSize(3000.f);
	TrimWorld->bShouldTrimWorld = true;

	// Evade agent
	pAgentToEvade = GetWorld()->SpawnActor<ASteeringAgent>(
		SteeringAgentClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator
	);
	
	// Give the evade agent a different color to distinguish it
	if (pAgentToEvade)
	{
		if (USkeletalMeshComponent* MeshComp = pAgentToEvade->GetMesh())
		{
			for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
			{
				if (UMaterialInstanceDynamic* MID =
					MeshComp->CreateAndSetMaterialInstanceDynamic(i))
				{
					if (i == 0)	MID->SetVectorParameterValue(TEXT("Color"), FLinearColor::Black);
					if (i == 1)	MID->SetVectorParameterValue(TEXT("Color"), FLinearColor::Blue);
				}
			}
		}
	}
	
	
	pWanderBehavior = std::make_unique<Wander>();
	pAgentToEvade->SetSteeringBehavior(pWanderBehavior.get());
	pAgentToEvade->SetDebugRenderingEnabled(false);
	
	
	pFlock = TUniquePtr<Flock>(
		new Flock(
			GetWorld(),
			SteeringAgentClass,
			FlockSize,
			TrimWorld->GetTrimWorldSize(),
			pAgentToEvade,
			true)
			);
}

// Called every frame
void ALevel_Flocking::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	pFlock->ImGuiRender(WindowPos, WindowSize);
	
	// Tick Agent to evade
	pAgentToEvade->Tick(DeltaTime);
	// Tick Flock
	pFlock->Tick(DeltaTime);
	pFlock->RenderDebug();
	if (bUseMouseTarget)
		pFlock->SetTarget_Seek(MouseTarget);
}

