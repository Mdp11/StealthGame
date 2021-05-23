// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "FPSGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialExpressionBlendMaterialAttributes.h"
#include "UObject/ConstructorHelpers.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();


	GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	if (InstigatorPawn)
	{

		APlayerController* PlayerController = Cast<APlayerController>(InstigatorPawn->GetController());

		if (PlayerController)
		{
			TArray<AActor*> ViewTargets;
			UGameplayStatics::GetAllActorsOfClass(this, PanoramicViewPointClass, ViewTargets);

			if (ViewTargets.Num() > 0)
			{
				PlayerController->SetViewTargetWithBlend(ViewTargets[0], 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
			}
		}
	}

	AFPSGameState* GS = GetGameState<AFPSGameState>();
	if (GS)
	{
		GS->MulticastCompleteMission(InstigatorPawn, bMissionSuccess);
	}

	OnMissionCompleted(InstigatorPawn, bMissionSuccess);
}
