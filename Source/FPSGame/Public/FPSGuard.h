// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "FPSGuard.generated.h"

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EGuardState : uint8
{
	Idle,
	Suspicious,
	Alerted
};

UCLASS()
class FPSGAME_API AFPSGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSGuard();

protected:
	UPROPERTY(VisibleAnywhere, Category="Components")
	UPawnSensingComponent* PawnSensingComponent;

	FRotator DefaultOrientation;

	FTimerHandle TimerHandle_ResetOrientation;

	EGuardState GuardState;

	UPROPERTY(EditInstanceOnly, Category="AI")
	bool bPatrol;

	UPROPERTY(EditInstanceOnly, Category="AI", meta=(EditCondition="bPatrol"))
	TArray<AActor*> PatrolPoints;

	UPROPERTY()
	AActor* CurrentPatrolPoint;

	int CurrentPatrolPointIndex;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

	void ResetOrientation();

	void SetGuardState(EGuardState NewState);

	UFUNCTION(BlueprintImplementableEvent)
	void OnStateChanged(EGuardState NewState);

	void MoveToNextPatrolPoint();

	void UpdateCurrentPatrolPoint();

	void StopMovement() const;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
