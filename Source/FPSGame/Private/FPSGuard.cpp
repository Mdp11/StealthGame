// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGuard.h"

#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AFPSGuard::AFPSGuard()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AFPSGuard::OnPawnSeen);
	PawnSensingComponent->OnHearNoise.AddDynamic(this, &AFPSGuard::OnNoiseHeard);
}

// Called when the game starts or when spawned
void AFPSGuard::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultOrientation = GetActorRotation();
}

// Called every frame
void AFPSGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFPSGuard::OnPawnSeen(APawn* Pawn)
{
	if (Pawn)
	{
		AFPSGameMode* GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
		if(GameMode)
		{
			GameMode->CompleteMission(Pawn, false);
		}
	}
}

void AFPSGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	FRotator LookAtNoiseRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);
	LookAtNoiseRotation.Pitch = 0.f;
	LookAtNoiseRotation.Roll = 0.f;
	SetActorRotation(LookAtNoiseRotation);
	
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSGuard::ResetOrientation, 3.0f);
}

void AFPSGuard::ResetOrientation()
{
	SetActorRotation(DefaultOrientation);
}
