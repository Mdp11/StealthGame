// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGuard.h"

#include "DrawDebugHelpers.h"
#include "FPSCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "FPSGameMode.h"
#include "NavigationSystem.h"
#include "UnrealNetwork.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
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

	SeenAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SeenAudioComp"));
	SeenAudioComponent->SetAutoActivate(false);

	HeardAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("HeardAudioComp"));
	HeardAudioComponent->SetAutoActivate(false);

	GuardEyes = CreateDefaultSubobject<USceneComponent>(TEXT("Eyes"));
	GuardEyes->SetupAttachment(RootComponent);
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

	if (PawnSeen)
	{
		FRotator LookAtPawnRotation = UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(), PawnSeen->GetActorLocation());
		LookAtPawnRotation.Pitch = 0.f;
		LookAtPawnRotation.Roll = 0.f;
		SetActorRotation(LookAtPawnRotation);
		
		AFPSCharacter* PlayerSeen = Cast<AFPSCharacter>(PawnSeen);
		if(!PlayerSeen)
		{
			return;
		}

		FHitResult OutHit;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		bool bTrace = GetWorld()->LineTraceSingleByChannel(OutHit, GuardEyes->GetComponentLocation(), PlayerSeen->PlayerEyes->GetComponentLocation(),
		                                                   ECC_Pawn,
		                                                   CollisionQueryParams);

		if (OutHit.GetActor() != PawnSeen)
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_CaughtTimer);
			SetGuardState(EGuardState::Idle);
			OnNoiseHeard(PawnSeen, PawnSeen->GetActorLocation(), 1.f);
			PawnSeen = nullptr;
		}
	}
}

void AFPSGuard::OnPawnSeen(APawn* Pawn)
{
	if (!PawnSeen)
	{
		if (Pawn)
		{
			PawnSeen = Pawn;
			SetGuardState(EGuardState::Alerted);

			if (bPatrol)
			{
				StopMovement();
			}

			if (!SeenAudioComponent->IsPlaying() && !HeardAudioComponent->IsPlaying())
			{
				SeenAudioComponent->Play();
			}

			FTimerDelegate TimerDelegate_OnCaught;
			TimerDelegate_OnCaught.BindUFunction(this, FName("OnCaught"), Pawn);

			GetWorldTimerManager().SetTimer(TimerHandle_CaughtTimer, TimerDelegate_OnCaught, 1.5f, false);
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

		if (!SeenAudioComponent->IsPlaying() && !HeardAudioComponent->IsPlaying() && bPlayHuhSound)
		{
			HeardAudioComponent->Play();
		}

		bPlayHuhSound = false;

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

		bPlayHuhSound = true;
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

void AFPSGuard::OnCaught(APawn* PawnCaught)
{
	AFPSGameMode* GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->CompleteMission(PawnCaught, false);
	}
}
