#include "AnimData.h"

// Retrieves animation montages based on character race, gender and  type
TArray<UAnimMontage *> UAnimData::GetMontages(const FString &Race, const FString &Gender, const FString &Type) const
{
    TArray<UAnimMontage *> Result;

    if (const FRaceAnimations *RaceAnim = RaceAnimations.Find(Race))
    {
        const FGenderAnimations &GenderAnim = (Gender.Equals(TEXT("Male"), ESearchCase::IgnoreCase))
                                                  ? RaceAnim->MaleAnimations
                                                  : RaceAnim->FemaleAnimations;

        if (const FAnimMontages *Anim = GenderAnim.Animations.Find(Type))
        {
            for (const TObjectPtr<UAnimMontage> &Montage : Anim->Montages)
            {
                Result.Add(Montage.Get());
            }
        }
    }

    return Result;
}