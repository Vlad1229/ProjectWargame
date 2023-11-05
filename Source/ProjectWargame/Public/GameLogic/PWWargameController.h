// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PWWargameController.generated.h"

class APWWargamePlayerState;
class APWBaseUnit;
class APWGameBoard;
class APWCameraManager;

/**
 * 
 */
UCLASS()
class PROJECTWARGAME_API APWWargameController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void AddControlledUnitOnServer(APWBaseUnit* NewUnit);

	void AddControlledUnitOnClient(APWBaseUnit* NewUnit);

	void SetGameBoard(APWGameBoard* InGameBoard);

	void SetControlIsEnabled(bool bIsEnabled);

	void SetAsFirstPlayer();

	UFUNCTION(Client, Reliable)
	void Client_SetAsFirstPlayer();

	void SetAsSecondPlayer();

	UFUNCTION(Client, Reliable)
	void Client_SetAsSecondPlayer();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

private:
	UFUNCTION()
	void Move();

	UFUNCTION(Server, Reliable)
	void Server_PerformMove(APWBaseUnit* Unit, FIntPoint TargetPosition);

	UFUNCTION()
	void Action();

	UFUNCTION(Server, Reliable)
	void Server_PerformAction();

	UFUNCTION(Client, Reliable)
	void Client_EnableControl();

	UFUNCTION(Client, Reliable)
	void Client_DisableControl();

	UFUNCTION()
	void OnUnitBeginCursorOver(APWBaseUnit* TouchedUnit);

	UFUNCTION()
	void OnUnitEndCursorOver(APWBaseUnit* TouchedUnit);

	void SelectUnit(APWBaseUnit* InUnit);

protected:
	APWWargamePlayerState* WargamePlayerState;

	APWGameBoard* GameBoard;

	APWCameraManager* CameraManager;

	TArray<APWBaseUnit*> ControlledUnits;

	APWBaseUnit* SelectedUnit;

private:
	bool bIsFirstPlayer;

	APWBaseUnit* HoveredUnit;

	TArray<FIntPoint> MovementAvailablePoints;

	TArray<FIntPoint> ActionAvailablePoints;

	bool bHasControl = false;
};
