// Fill out your copyright notice in the Description page of Project Settings.


#include "GASAbilitySystemLibrary.h"
#include "GASCharacterClassInfo.h"
#include "Game/GASGameMode.h"
#include "Kismet/GameplayStatics.h"

UGASCharacterClassInfo *UGASAbilitySystemLibrary::GetCharacterClassDefaultInfo(const UObject *WorldContextObject)
{
    if (const AGASGameMode *GASGameMode = Cast<AGASGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
    {
        return GASGameMode->GetCharacterClassDefaultInfo();
    }

    return nullptr;
}