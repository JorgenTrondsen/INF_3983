// Fill out your copyright notice in the Description page of Project Settings.


#include "GASGameMode.h"
#include "../GASCharacterClassInfo.h"

UGASCharacterClassInfo* AGASGameMode::GetCharacterClassDefaultInfo() const
{
    return ClassDefaults;
}