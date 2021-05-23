// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"

#include "EngineUtils.h"

void AFPSGameState::MulticastCompleteMission_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{
	for(auto Pawn = TActorIterator<APawn>{GetWorld()}; Pawn; ++Pawn)
	{
		if(Pawn && Pawn->IsLocallyControlled())
		{
			Pawn->DisableInput(nullptr);
		}
	}
}
