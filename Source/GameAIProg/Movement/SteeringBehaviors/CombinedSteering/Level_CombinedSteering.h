// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombinedSteeringBehaviors.h"
#include "GameAIProg/Shared/Level_Base.h"
#include "GameAIProg/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include <memory>
#include <vector>
#include <string>
#include "Level_CombinedSteering.generated.h"

UCLASS()
class GAMEAIPROG_API ALevel_CombinedSteering : public ALevel_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevel_CombinedSteering();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

private:
	//Datamembers
	bool UseMouseTarget = false;
	bool CanDebugRender = false;
	
	// Drunk agent behaviour
	ISteeringBehavior* m_pSeekBehaviour{nullptr};
	ISteeringBehavior* m_pDrunkWanderBehavior{nullptr};
	ISteeringBehavior* m_pDrunkSteering{nullptr};
	ASteeringAgent* m_pDrunkAgent{nullptr};
	
	// Evading agent behavior
	ISteeringBehavior* m_pEvadeBehaviour{nullptr};
	ISteeringBehavior* m_pEvadingWanderBehaviour{nullptr};
	ISteeringBehavior* m_pEvadingSteering{nullptr};
	
	ASteeringAgent* m_pEvadingAgent{nullptr};
	
	
};
