// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly)
	USoundBase* VictorySound;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* LoseSound;
	
public:

	UFUNCTION(NetMulticast, Reliable)
	void MulticastCompleteMission(APawn* InstigatorPawn, bool bMissionSuccess);
};
