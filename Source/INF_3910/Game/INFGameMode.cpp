// Fill out your copyright notice in the Description page of Project Settings.

#include "INFGameMode.h"

UCharacterClassInfo *AINFGameMode::GetCharacterClassDefaultInfo() const
{
    return ClassDefaults;
}

UProjectileInfo *AINFGameMode::GetProjectileInfo() const
{
    return ProjectileInfo;
}
