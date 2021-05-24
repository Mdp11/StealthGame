// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGuard.h"

#include "Perception/PawnSensingComponent.h"
#include "FPSGameMode.h"
#include "NavigationSystem.h"
#include "UnrealNetwork.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	GuardState = EGuardState::Idle;

	CurrentPatrolPointIndex = 0;
	bPawnSeen = false;
}

// Called when the game starts or when spawned
void AFPSGuard::BeginPlay()
{
	Super::BeginPlay();
	DefaultOrientation = GetActorRotation();

	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
}

void AFPSGuard::MoveToNextPatrolPoint()
{
	UpdateCurrentPatrolPoint();
	if (CurrentPatrolPoint)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
	}
}

void AFPSGuard::UpdateCurrentPatrolPoint()
{
	if (CurrentPatrolPoint != nullptr)
	{
		if (++CurrentPatrolPointIndex >= PatrolPoints.Num())
		{
			CurrentPatrolPointIndex = 0;
		}
	}
	CurrentPatrolPoint = PatrolPoints[CurrentPatrolPointIndex];
}

void AFPSGuard::StopMovement() const
{
	AController* PawnController = GetController();
	if (PawnController)
	{
		PawnController->StopMovement();
	}
}

// Called every frame
void AFPSGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPatrol)
	{
		const float DistanceFromCurrentPatrolPoint = (GetActorLocation() - CurrentPatrolPoint->GetActorLocation()).
			Size();

		if (DistanceFromCurrentPatrolPoint <= 100.f)
		{
			MoveToNextPatrolPoint();
		}
	}
}

void AFPSGuard::OnPawnSeen(APawn* Pawn)
{
	if (!bPawnSeen)
	{
		if (Pawn)
		{
			bPawnSeen = true;
			SetGuardState(EGuardState::Alerted);

			AFPSGameMode* GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
			if (GameMode)
			{
				GameMode->CompleteMission(Pawn, false);
			}

			if (bPatrol)
			{
				StopMovement();
			}
		}
	}
}

void AFPSGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState != EGuardState::Alerted)
	{
		SetGuardState(EGuardState::Suspicious);
		FRotator LookAtNoiseRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);
		LookAtNoiseRotation.Pitch = 0.f;
		LookAtNoiseRotation.Roll = 0.f;
		SetActorRotation(LookAtNoiseRotation);

		GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSGuard::ResetOrientation, 3.0f);

		if (bPatrol)
		{
			StopMovement();
		}
	}
}

void AFPSGuard::ResetOrientation()
{
	if (GuardState != EGuardState::Alerted)
	{
		SetActorRotation(DefaultOrientation);
		SetGuardState(EGuardState::Idle);

		if (bPatrol)
		{
			MoveToNextPatrolPoint();
		}
	}
}

void AFPSGuard::SetGuardState(const EGuardState NewState)
{
	if (GuardState != NewState)
	{
		GuardState = NewState;
		OnRep_GuardState();
	}
}

void AFPSGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}


void AFPSGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSGuard, GuardState);
}
