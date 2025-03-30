// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileAbility.h"
#include "INF_3910/GameplayAbilitySystem/AbilitySystem/AbilityTypes.h"
#include "INF_3910/Projectiles/ProjectileInfo.h"
#include "INF_3910/Projectiles/ProjectileBase.h"
#include "INF_3910/Interfaces/GASAbilitySystemInterface.h"
#include "INF_3910/GameplayAbilitySystem/GASAbilitySystemLibrary.h"
#include "INF_3910/GameplayAbilitySystem/AbilitySystem/AbilityTypes.h"

UProjectileAbility::UProjectileAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UProjectileAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	AvatarActorFromInfo = GetAvatarActorFromActorInfo();
	
	if (!ProjectileToSpawnTag.IsValid() || !IsValid(AvatarActorFromInfo)) return;

	if (UProjectileInfo* ProjectileInfo = UGASAbilitySystemLibrary::GetProjectileInfo(AvatarActorFromInfo))
	{
		CurrentProjectileParams = *ProjectileInfo->ProjectileInfoMap.Find(ProjectileToSpawnTag);
	}
	
}

void UProjectileAbility::SpawnProjectile()
{

	if (!IsValid(CurrentProjectileParams.ProjectileClass)) return;


	if (const USceneComponent* SpawnPointComp = IGASAbilitySystemInterface::Execute_GetDynamicSpawnPoint(AvatarActorFromInfo))
	{
		const FVector SpawnPoint = SpawnPointComp->GetComponentLocation();
		const FVector TargetLocation = AvatarActorFromInfo->GetActorForwardVector() * 10000;
		const FRotator TargetRotation = (TargetLocation - SpawnPoint).Rotation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnPoint);
		SpawnTransform.SetRotation(TargetRotation.Quaternion());

		if (AProjectileBase* SpawnedProjectile = GetWorld()->SpawnActorDeferred<AProjectileBase>(CurrentProjectileParams.ProjectileClass, SpawnTransform, AvatarActorFromInfo))
		{
			SpawnedProjectile->SetProjectileParams(CurrentProjectileParams);

			FDamageEffectInfo DamageEffectInfo;
			CaptureDamageEffectInfo(nullptr, DamageEffectInfo);

			SpawnedProjectile->DamageEffectInfo = DamageEffectInfo;

			SpawnedProjectile->FinishSpawning(SpawnTransform);
		}
	}
}