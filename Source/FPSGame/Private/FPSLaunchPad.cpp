// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSLaunchPad.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSLaunchPad::AFPSLaunchPad()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetBoxExtent(FVector{75.f, 75.f, 50.f});
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFPSLaunchPad::OnLaunchPadOverlap);
	SetRootComponent(BoxComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	LaunchStrength = 1500.f;
	LaunchPitchAngle = 35.f;
}

// Called when the game starts or when spawned
void AFPSLaunchPad::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("PROVA"));

}

void AFPSLaunchPad::OnLaunchPadOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OVERLAPPPP"));
	FRotator LaunchDirection{GetActorRotation()};
	LaunchDirection.Pitch += LaunchPitchAngle;

	const FVector LaunchVelocity{LaunchDirection.Vector() * LaunchStrength};

	ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);
	if (OtherCharacter)
	{
		OtherCharacter->LaunchCharacter(LaunchVelocity, true, true);
		UGameplayStatics::SpawnEmitterAtLocation(this, LaunchEffect, GetActorLocation());
	}
	else if (OtherComp && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulse(LaunchVelocity, NAME_None, true);
		UGameplayStatics::SpawnEmitterAtLocation(this, LaunchEffect, GetActorLocation());
	}
}
