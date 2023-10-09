// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/PWBaseUnit.h"
#include "PWWarriorUnit.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTWARGAME_API APWWarriorUnit : public APWBaseUnit
{
	GENERATED_BODY()

public:
	virtual TArray<FIntPoint> GetAvailableForMovementPositions(const FIntPoint& BoardSize, const TArray<FIntPoint>& TakenPositions) const override;

	virtual TArray<FIntPoint> GetAvailableForActionPositions(const FIntPoint& BoardSize, const TArray<FIntPoint>& TakenPositions) const override;
	
};
