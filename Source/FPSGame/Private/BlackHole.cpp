// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackHole.h"

#include "Components/SphereComponent.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
ABlackHole::ABlackHole()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OuterSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OuterSphere"));
	OuterSphere->SetSphereRadius(2000.f);
	OuterSphere->SetupAttachment(RootComponent);
	OuterSphere->SetGenerateOverlapEvents(true);
	OuterSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OuterSphere->SetCollisionResponseToAllChannels(ECR_Overlap);

	MiddleSphere = CreateDefaultSubobject<USphereComponent>(TEXT("MiddleSphere"));
	MiddleSphere->SetSphereRadius(1000.f);
	MiddleSphere->SetupAttachment(OuterSphere);
	MiddleSphere->SetGenerateOverlapEvents(true);
	MiddleSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MiddleSphere->SetCollisionResponseToAllChannels(ECR_Overlap);

	InnerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphere"));
	InnerSphere->SetSphereRadius(500.f);
	InnerSphere->SetupAttachment(MiddleSphere);
	InnerSphere->SetGenerateOverlapEvents(true);
	InnerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InnerSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
	InnerSphere->OnComponentBeginOverlap.AddDynamic(this, &ABlackHole::OnInnerSphereOverlap);
	
	
}

// Called when the game starts or when spawned
void ABlackHole::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TArray<UPrimitiveComponent*> OuterOverlappingComponents;
	OuterSphere->GetOverlappingComponents(OuterOverlappingComponents);

	for(const auto& OverlappingComponent : OuterOverlappingComponents)
	{
		if(OverlappingComponent && OverlappingComponent->IsSimulatingPhysics())
		{
			OverlappingComponent->AddRadialForce(GetActorLocation(), OuterSphere->GetScaledSphereRadius(), GravityStrength, ERadialImpulseFalloff::RIF_Linear, true);
		}
	}

	TArray<UPrimitiveComponent*> MiddleOverlappingComponents;
	MiddleSphere->GetOverlappingComponents(MiddleOverlappingComponents);
	
	for(const auto& OverlappingComponent : MiddleOverlappingComponents)
	{
		if(OverlappingComponent && OverlappingComponent->IsSimulatingPhysics())
		{
			const auto Distance = (OverlappingComponent->GetComponentLocation() - InnerSphere->GetComponentLocation()).Size();
			const auto MiddleSphereScaledRadius = MiddleSphere->GetScaledSphereRadius();
			const auto InitialScale = OverlappingComponent->GetComponentScale();
			
			const auto ScaleX = FMath::FInterpConstantTo(InitialScale.X, Distance/MiddleSphereScaledRadius, DeltaTime, 10.f);
			const auto ScaleY = FMath::FInterpConstantTo(InitialScale.Y, Distance/MiddleSphereScaledRadius, DeltaTime, 10.f);
			const auto ScaleZ = FMath::FInterpConstantTo(InitialScale.Z, Distance/MiddleSphereScaledRadius, DeltaTime, 10.f);
			
			OverlappingComponent->SetWorldScale3D(FVector{ScaleX, ScaleY, ScaleZ});
		}
	}

}

void ABlackHole::OnInnerSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherActor)
	{
		OtherActor->Destroy();
	}
}
