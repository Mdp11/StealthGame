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

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassFinder(
		TEXT("/Game/Blueprints/BP_PlayerController"));
	PlayerControllerClass = PlayerControllerClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();


	GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	if (InstigatorPawn)
	{
		if (PanoramicViewPointClass)
		{
			TArray<AActor*> ViewTargets;
			UGameplayStatics::GetAllActorsOfClass(this, PanoramicViewPointClass, ViewTargets);

			if (ViewTargets.Num() > 0)
			{
				for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
				{
					APlayerController* PC = It->Get();
					if (PC)
					{
						PC->SetViewTargetWithBlend(ViewTargets[0], 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
					}
				}
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
