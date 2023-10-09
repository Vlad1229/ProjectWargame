// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PWWargameController.generated.h"

class APWWargamePlayerState;
class APWBaseUnit;
class APWGameBoard;

/**
 * 
 */
UCLASS()
class PROJECTWARGAME_API APWWargameController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void AddControlledUnit(APWBaseUnit* NewUnit);

	void SetGameBoard(APWGameBoard* InGameBoard);

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* aPawn) override;

	virtual void SetupInputComponent() override;

private:
	UFUNCTION(Client, Reliable)
	void Client_AddControlledUnit(APWBaseUnit* NewUnit);

	UFUNCTION()
	void Move();

	UFUNCTION(Server, Reliable)
	void Server_PerformMove(APWBaseUnit* Unit, FIntPoint TargetPosition);

	UFUNCTION()
	void Action();

	UFUNCTION(Server, Reliable)
	void Server_SelectUnit(int32 UnitIndex);

	UFUNCTION(Server, Reliable)
	void Server_PerformAction();

	UFUNCTION()
	void OnUnitBeginCursorOver(int32 UnitIndex);

	UFUNCTION()
	void OnUnitEndCursorOver(int32 UnitIndex);

	void SelectUnit(int32 UnitIndex);

	UFUNCTION()
	void OnMoveEnd();

protected:
	APWWargamePlayerState* WargamePlayerState;

	APWGameBoard* GameBoard;

	TArray<APWBaseUnit*> ControlledUnits;

	APWBaseUnit* SelectedUnit;

private:
	int32 HoveredUnitIndex = -1;

	TArray<FIntPoint> MovementAvailablePoints;

	TArray<FIntPoint> ActionAvailablePoints;

	bool bHasControl = true;
};
