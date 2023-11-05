// Fill out your copyright notice in the Description page of Project Settings.


#include "GameLogic/PWWargameController.h"
#include "GameLogic/PWWargamePlayerState.h"
#include "GameBoard/PWGameBoard.h"
#include "GameBoard/PWCameraManager.h"
#include "Units/PWBaseUnit.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

void APWWargameController::AddControlledUnitOnServer(APWBaseUnit* NewUnit)
{
	if (NewUnit && GetNetMode() == NM_DedicatedServer)
	{
		ControlledUnits.Add(NewUnit);
		WargamePlayerState->AddUnit(NewUnit);

		NewUnit->SetOwner(this);
	}
}

void APWWargameController::AddControlledUnitOnClient(APWBaseUnit* NewUnit)
{
	if (NewUnit)
	{
		ControlledUnits.Add(NewUnit);
		NewUnit->OnUnitBeginCursorOver.BindUObject(this, &APWWargameController::OnUnitBeginCursorOver);
		NewUnit->OnUnitEndCursorOver.BindUObject(this, &APWWargameController::OnUnitEndCursorOver);
	}
}

void APWWargameController::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APWGameBoard::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		GameBoard = Cast<APWGameBoard>(FoundActors[0]);
	}

	if (GetNetMode() == NM_Client)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APWCameraManager::StaticClass(), FoundActors);
		if (FoundActors.Num() > 0)
		{
			CameraManager = Cast<APWCameraManager>(FoundActors[0]);
			SetViewTarget(bIsFirstPlayer ? CameraManager->GetFistCamera() : CameraManager->GetSecondCamera());
		}

		SetShowMouseCursor(true);
		bEnableMouseOverEvents = true;
	}

	WargamePlayerState = GetPlayerState<APWWargamePlayerState>();
}

void APWWargameController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Move", IE_Pressed, this, &APWWargameController::Move);
	InputComponent->BindAction("Act", IE_Pressed, this, &APWWargameController::Action);
}

void APWWargameController::SetGameBoard(APWGameBoard* InGameBoard)
{
	GameBoard = InGameBoard;
}

void APWWargameController::SetControlIsEnabled(bool bIsEnabled)
{
	bHasControl = bIsEnabled;
	if (bIsEnabled)
	{
		Client_EnableControl();
	}
	else
	{
		Client_DisableControl();
	}
}

void APWWargameController::Client_SetAsFirstPlayer_Implementation()
{
	bIsFirstPlayer = true;
	if (CameraManager)
	{
		SetViewTarget(CameraManager->GetFistCamera());
	}
}

void APWWargameController::Client_SetAsSecondPlayer_Implementation()
{
	bIsFirstPlayer = false;
	if (CameraManager)
	{
		SetViewTarget(CameraManager->GetSecondCamera());
	}
}

void APWWargameController::Move()
{
	if (bHasControl && SelectedUnit)
	{
		FIntPoint HoveredPosition = GameBoard->GetHoveredPosition();
		if (HoveredPosition.X >= 0 && HoveredPosition.Y >= 0)
		{
			Server_PerformMove(SelectedUnit, HoveredPosition);
			bHasControl = false;
		}
	}
}

void APWWargameController::Server_PerformMove_Implementation(APWBaseUnit* Unit, FIntPoint TargetPosition)
{
	if (bHasControl && Unit && GameBoard->PositionIsValid(TargetPosition))
	{
		Unit->Multicast_StartMove(TargetPosition);
		bHasControl = false;

		SetControlIsEnabled(false);
	}
}

void APWWargameController::Action()
{
	if (bHasControl)
	{
		if (HoveredUnit)
		{
			SelectUnit(HoveredUnit);
		}
		if (!HoveredUnit)
		{
			Server_PerformAction_Implementation();
		}
	}
}

void APWWargameController::Server_PerformAction_Implementation()
{
	if (bHasControl)
	{
		
	}
}

void APWWargameController::Client_EnableControl_Implementation()
{
	bHasControl = true;
}

void APWWargameController::Client_DisableControl_Implementation()
{
	bHasControl = false;
	SelectedUnit = nullptr;
	GameBoard->ClearAvailablePositions();
}

void APWWargameController::OnUnitBeginCursorOver(APWBaseUnit* TouchedUnit)
{
	HoveredUnit = TouchedUnit;
}

void APWWargameController::OnUnitEndCursorOver(APWBaseUnit* TouchedUnit)
{
	if (HoveredUnit == TouchedUnit)
	{
		HoveredUnit = nullptr;
	}
}

void APWWargameController::SelectUnit(APWBaseUnit* InUnit)
{
	if (InUnit && InUnit != SelectedUnit)
	{
		SelectedUnit = InUnit;

		MovementAvailablePoints.Empty();
		ActionAvailablePoints.Empty();
		GameBoard->ClearAvailablePositions();

		TArray<FIntPoint> TakenPositions = GameBoard->GetTakenPositions();
		MovementAvailablePoints = SelectedUnit->GetAvailableForMovementPositions(GameBoard->GetBoardSize(), TakenPositions);
		GameBoard->SetPositionsAsAvailable(MovementAvailablePoints);
		ActionAvailablePoints = SelectedUnit->GetAvailableForMovementPositions(GameBoard->GetBoardSize(), TakenPositions);
		GameBoard->SetPositionsAsAvailable(ActionAvailablePoints);
	}
}

