// Fill out your copyright notice in the Description page of Project Settings.


#include "GameLogic/PWWargamePlayerState.h"
#include "Units/PWBaseUnit.h"


void APWWargamePlayerState::AddUnit(APWBaseUnit* InUnit)
{
	PlayerUnits.Add(InUnit);
}
