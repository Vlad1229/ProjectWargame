// Fill out your copyright notice in the Description page of Project Settings.


#include "GameLogic/PWWargameController.h"
#include "GameLogic/PWWargamePlayerState.h"
#include "GameBoard/PWGameBoard.h"
#include "Units/PWBaseUnit.h"
#include "Kismet/GameplayStatics.h"

void APWWargameController::AddControlledUnit(APWBaseUnit* NewUnit)
{
	if (NewUnit)
	{
		ControlledUnits.Add(NewUnit);
		NewUnit->SetIndex(ControlledUnits.Num() - 1);
		NewUnit->OnMoveEnd.BindUObject(this, &APWWargameController::OnMoveEnd);

		WargamePlayerState->AddUnit(NewUnit);

		Client_AddControlledUnit(NewUnit);
	}
}

void APWWargameController::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundBoards;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APWGameBoard::StaticClass(), FoundBoards);
	if (FoundBoards.Num() > 0)
	{
		GameBoard = Cast<APWGameBoard>(FoundBoards[0]);
	}
}

void APWWargameController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	SetShowMouseCursor(true);
	bEnableMouseOverEvents = true;

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

void APWWargameController::Client_AddControlledUnit_Implementation(APWBaseUnit* NewUnit)
{
	ControlledUnits.Add(NewUnit);
	NewUnit->OnUnitBeginCursorOver.BindUObject(this, &APWWargameController::OnUnitBeginCursorOver);
	NewUnit->OnUnitEndCursorOver.BindUObject(this, &APWWargameController::OnUnitEndCursorOver);
}

void APWWargameController::Move()
{
	if (bHasControl && SelectedUnit)
	{
		FIntPoint HoveredPosition = GameBoard->GetHoveredPosition();
		if (HoveredPosition.X >= 0 && HoveredPosition.Y >= 0)
		{
			GameBoard->ReleasePosition(SelectedUnit->GetPosition());

			SelectedUnit->MoveTo(GameBoard->GetLocationOf(HoveredPosition));
			SelectedUnit->SetPosition(HoveredPosition);

			GameBoard->TakePosition(HoveredPosition);
			GameBoard->ClearAvailablePositions();
			bHasControl = false;
			SelectedUnit = nullptr;
		}
	}
}

void APWWargameController::Server_PerformMove_Implementation(APWBaseUnit* Unit, FIntPoint TargetPosition)
{
	if (bHasControl && Unit && GameBoard->PositionIsValid(TargetPosition))
	{
		GameBoard->ReleasePosition(SelectedUnit->GetPosition());

		Unit->MoveTo(GameBoard->GetLocationOf(TargetPosition));
		Unit->SetPosition(TargetPosition);

		GameBoard->TakePosition(TargetPosition);
		GameBoard->ClearAvailablePositions();
		bHasControl = false;
	}
}

void APWWargameController::Action()
{
	if (bHasControl && HoveredUnitIndex != -1)
	{
		Server_PerformAction_Implementation();
	}
}

void APWWargameController::Server_SelectUnit_Implementation(int32 UnitIndex)
{
	if (bHasControl)
	{
		SelectUnit(UnitIndex);
	}
}

void APWWargameController::Server_PerformAction_Implementation()
{
	if (bHasControl)
	{
		
	}
}

void APWWargameController::OnUnitBeginCursorOver(int32 UnitIndex)
{
	HoveredUnitIndex = UnitIndex;
}

void APWWargameController::OnUnitEndCursorOver(int32 UnitIndex)
{
	if (HoveredUnitIndex == UnitIndex)
	{
		HoveredUnitIndex = -1;
	}
}

void APWWargameController::SelectUnit(int32 UnitIndex)
{
	if (UnitIndex >= 0 && UnitIndex < ControlledUnits.Num())
	{
		SelectedUnit = ControlledUnits[UnitIndex];

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

void APWWargameController::OnMoveEnd()
{
	bHasControl = true;
}

