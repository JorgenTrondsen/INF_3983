#include "ProjectileAbility.h"
#include "INF_3910/AbilitySystem/AbilityTypes.h"
#include "INF_3910/Projectiles/ProjectileInfo.h"
#include "INF_3910/Projectiles/ProjectileBase.h"
#include "INF_3910/Interfaces/INFAbilitySystemInterface.h"
#include "INF_3910/Libraries/INFAbilitySystemLibrary.h"
#include "INF_3910/AbilitySystem/AbilityTypes.h"

// Constructor that sets the instancing policy to per-actor
UProjectileAbility::UProjectileAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

// Initialize ability when granted to an actor and cache projectile parameters
void UProjectileAbility::OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	AvatarActorFromInfo = GetAvatarActorFromActorInfo();
	InstigatorPawnFromInfo = Cast<APawn>(AvatarActorFromInfo);

	if (!ProjectileToSpawnTag.IsValid() || !IsValid(AvatarActorFromInfo))
		return;

	if (UProjectileInfo *ProjectileInfo = UINFAbilitySystemLibrary::GetProjectileInfo(AvatarActorFromInfo))
	{
		CurrentProjectileParams = *ProjectileInfo->ProjectileInfoMap.Find(ProjectileToSpawnTag);
	}
}

// Spawn a projectile at the dynamic spawn point with proper aim direction
void UProjectileAbility::SpawnProjectile()
{
	if (!IsValid(AvatarActorFromInfo) || !IsValid(CurrentProjectileParams.ProjectileClass))
		return;

	if (const USceneComponent *SpawnPointComp = IINFAbilitySystemInterface::Execute_GetDynamicSpawnPoint(AvatarActorFromInfo))
	{
		const FVector SpawnPoint = SpawnPointComp->GetComponentLocation();

		FVector EyeLocation;
		FRotator AimRotation;
		AvatarActorFromInfo->GetActorEyesViewPoint(EyeLocation, AimRotation);

		const FVector AimDirection = AimRotation.Vector();
		const FVector FarTargetPoint = EyeLocation + AimDirection * 10000.f;
		const FRotator ProjectileRotation = (FarTargetPoint - SpawnPoint).Rotation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnPoint);
		SpawnTransform.SetRotation(ProjectileRotation.Quaternion());

		if (AProjectileBase *SpawnedProjectile = GetWorld()->SpawnActorDeferred<AProjectileBase>(CurrentProjectileParams.ProjectileClass, SpawnTransform, AvatarActorFromInfo, InstigatorPawnFromInfo))
		{
			SpawnedProjectile->SetProjectileParams(CurrentProjectileParams);

			FDamageEffectInfo DamageEffectInfo;
			CaptureDamageEffectInfo(nullptr, DamageEffectInfo);

			SpawnedProjectile->DamageEffectInfo = DamageEffectInfo;

			SpawnedProjectile->FinishSpawning(SpawnTransform);
		}
	}
}