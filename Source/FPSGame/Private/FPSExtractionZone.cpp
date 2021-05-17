// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSExtractionZone.h"

#include "FPSCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"

// Sets default values
AFPSExtractionZone::AFPSExtractionZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	OverlapComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComponent"));
	SetRootComponent(OverlapComponent);

	OverlapComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapComponent->SetBoxExtent(FVector{200.f});
	OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &AFPSExtractionZone::HandleOverlap);

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetupAttachment(RootComponent);
	DecalComponent->DecalSize = FVector{200.f};
}

void AFPSExtractionZone::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult)
{
	AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		if (PlayerCharacter->bIsCarryingObjective)
		{
			UE_LOG(LogTemp, Warning, TEXT("YEEE"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NOOO"));
		}
	}
}
