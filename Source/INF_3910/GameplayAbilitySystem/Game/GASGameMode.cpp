// Fill out your copyright notice in the Description page of Project Settings.


#include "GASGameMode.h"

UGASCharacterClassInfo* AGASGameMode::GetCharacterClassDefaultInfo() const
{
    return ClassDefaults;
}

UProjectileInfo* AGASGameMode::GetProjectileInfo() const
{
    return ProjectileInfo;
}
