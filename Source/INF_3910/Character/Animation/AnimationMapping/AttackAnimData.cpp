#include "AttackAnimData.h"

TArray<UAnimMontage *> UAttackAnimData::GetAttackMontages(const FString &Race, const FString &Gender, const FString &WeaponType) const
{
    TArray<UAnimMontage *> Result;

    if (const FRaceAnimations *RaceAnim = RaceAnimations.Find(Race))
    {
        const FGenderWeaponAnimations &GenderAnim = (Gender.Equals(TEXT("Male"), ESearchCase::IgnoreCase))
                                                        ? RaceAnim->MaleAnimations
                                                        : RaceAnim->FemaleAnimations;

        if (const FWeaponAnimMontages *WeaponAnim = GenderAnim.WeaponAnimations.Find(WeaponType))
        {
            for (const TObjectPtr<UAnimMontage> &Montage : WeaponAnim->AttackMontages)
            {
                Result.Add(Montage.Get());
            }
        }
    }

    return Result;
}