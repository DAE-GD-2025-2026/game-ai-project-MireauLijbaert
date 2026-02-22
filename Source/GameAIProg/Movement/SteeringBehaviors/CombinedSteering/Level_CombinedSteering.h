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

	// Copied over from Level_SteeringBehaviors with modifications
	enum class CombinedBehaviorTypes
	{
		Blended,
		Priority,

		// @ End
		Count
	};

	struct ImGui_Agent final
	{
		ASteeringAgent* Agent{nullptr};
		std::unique_ptr<ISteeringBehavior> Behavior{nullptr};
		int SelectedBehavior{static_cast<int>(CombinedBehaviorTypes::Blended)};
		int SelectedTarget = -1;
	};
	
	std::vector<ImGui_Agent> SteeringAgents{};
	std::vector<std::string> TargetLabels{};
	
	int AgentIndexToRemove = -1;
	
	bool AddAgent(CombinedBehaviorTypes BehaviorType = CombinedBehaviorTypes::Blended, bool AutoOrient = true);
	void RemoveAgent(unsigned int Index);
	void SetAgentBehavior(ImGui_Agent& Agent);

	void RefreshTargetLabels();
	void UpdateTarget(ImGui_Agent& Agent);
	void RefreshAgentTargets(unsigned int IndexRemoved);
	
};
