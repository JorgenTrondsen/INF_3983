#include "AttackAnimData.h"

TArray<UAnimMontage *> UAttackAnimData::GetAttackMontages(ECharacterRace Race, ECharacterGender Gender, EWeaponType WeaponType) const
{
    TArray<UAnimMontage *> Result;

    if (const FRaceAnimations *RaceAnim = RaceAnimations.Find(Race))
    {
        const FGenderWeaponAnimations &GenderAnim = (Gender == ECharacterGender::Male)
                                                        ? RaceAnim->MaleAnimations
                                                        : RaceAnim->FemaleAnimations;

        if (const FWeaponAnimMontages *WeaponAnim = GenderAnim.WeaponAnimations.Find(WeaponType))
        {
            // Convert TObjectPtr array to raw pointer array
            for (const TObjectPtr<UAnimMontage> &Montage : WeaponAnim->AttackMontages)
            {
                Result.Add(Montage.Get());
            }
        }
    }

    return Result;
}

UAnimMontage *UAttackAnimData::GetAttackMontage(ECharacterRace Race, ECharacterGender Gender, EWeaponType WeaponType, int32 MontageIndex) const
{
    TArray<UAnimMontage *> Montages = GetAttackMontages(Race, Gender, WeaponType);

    if (Montages.IsValidIndex(MontageIndex))
    {
        return Montages[MontageIndex];
    }

    return nullptr;
}