// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PWWargameGameMode.generated.h"

class APWBaseUnit;
class APWGameBoard;

/**
 * 
 */
UCLASS()
class PROJECTWARGAME_API APWWargameGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	void GenerateBoard();

protected:
	UPROPERTY(EditDefaultsOnly)
	FIntPoint BoardSize;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APWBaseUnit> UnitClass;

	UPROPERTY()
	APWGameBoard* GameBoard;
};
