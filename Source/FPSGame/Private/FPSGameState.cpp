// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"

#include "EngineUtils.h"
#include "FPSPlayerController.h"
#include "Kismet/GameplayStatics.h"

void AFPSGameState::MulticastCompleteMission_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{
	for(auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get());
		if(PC && PC->IsLocalController())
		{
			UGameplayStatics::PlaySoundAtLocation(InstigatorPawn, VictorySound, InstigatorPawn->GetActorLocation());
			PC->OnMissionCompleted(InstigatorPawn, bMissionSuccess);

			APawn* PlayerPawn = PC->GetPawn();
			if(PlayerPawn)
			{
				PlayerPawn->DisableInput(nullptr);
			}
		}
	}
}
