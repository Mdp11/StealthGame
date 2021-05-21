// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSObjectiveActor.h"


#include "FPSCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSObjectiveActor::AFPSObjectiveActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(MeshComp);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SphereComp->SetupAttachment(RootComponent);

	bReplicates = true;
}

// Called when the game starts or when spawned
void AFPSObjectiveActor::BeginPlay()
{
	Super::BeginPlay();
}

void AFPSObjectiveActor::PlayEffects() const
{
	UGameplayStatics::SpawnEmitterAtLocation(this, PickupFX, GetActorLocation());
}

void AFPSObjectiveActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AFPSCharacter* Player = Cast<AFPSCharacter>(OtherActor);
	if(Player)
	{
		PlayEffects();
		if(HasAuthority())
		{
			Player->bIsCarryingObjective = true;
			Destroy();
		}
		
	}
}
