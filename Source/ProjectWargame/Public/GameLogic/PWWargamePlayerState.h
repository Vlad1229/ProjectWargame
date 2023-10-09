// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PWWargamePlayerState.generated.h"

class APWBaseUnit;

/**
 * 
 */
UCLASS()
class PROJECTWARGAME_API APWWargamePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	void AddUnit(APWBaseUnit* InUnit);

private:
	UPROPERTY()
	TArray<APWBaseUnit*> PlayerUnits;

};
