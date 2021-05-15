// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackHole.generated.h"

class USphereComponent;

UCLASS()
class FPSGAME_API ABlackHole : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABlackHole();

protected:

	UPROPERTY(EditAnywhere, Category="Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category="Components")
	USphereComponent* OuterSphere;
	
	UPROPERTY(EditAnywhere, Category="Components")
	USphereComponent* MiddleSphere;

	UPROPERTY(EditAnywhere, Category="Components")
	USphereComponent* InnerSphere;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	float GravityStrength{-2000.f};

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnInnerSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                 UPrimitiveComponent* OtherComp,
	                                 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
