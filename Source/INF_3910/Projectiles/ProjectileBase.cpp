#include "ProjectileBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SphereComponent.h"
#include "INF_3910/AbilitySystem/AbilityTypes.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "INF_3910/Libraries/INFAbilitySystemLibrary.h"

AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	SetRootComponent(ProjectileMesh);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProjectileMesh->SetCollisionObjectType(ECC_WorldDynamic);
	ProjectileMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileMesh->SetIsReplicated(true);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
	OverlapSphere = CreateDefaultSubobject<USphereComponent>("OverlapSphere");
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	OverlapSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	OverlapSphere->SetupAttachment(GetRootComponent());
}

void AProjectileBase::SetProjectileParams(const FProjectileParams &Params)
{
	if (IsValid(ProjectileMesh))
	{
		ProjectileMesh->SetStaticMesh(Params.ProjectileMesh);
	}

	if (IsValid(ProjectileMovementComponent))
	{
		ProjectileMovementComponent->InitialSpeed = Params.InitialSpeed;
		ProjectileMovementComponent->ProjectileGravityScale = Params.GravityScale;
		ProjectileMovementComponent->bShouldBounce = Params.bShouldBounce;
		ProjectileMovementComponent->Bounciness = Params.Bounciness;
	}
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);
	}
}

void AProjectileBase::OnSphereBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
										   UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor == GetOwner() || OtherActor->GetOwner() == GetOwner())
		return;

	if (UAbilitySystemComponent *TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
	{
		DamageEffectInfo.TargetASC = TargetASC;
		UINFAbilitySystemLibrary::ApplyDamageEffect(DamageEffectInfo);
	}
	Destroy();
}