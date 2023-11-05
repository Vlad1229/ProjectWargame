// Fill out your copyright notice in the Description page of Project Settings.


#include "GameLogic/PWWargameGameMode.h"
#include "GameLogic/PWWargameController.h"
#include "GameBoard/PWGameBoard.h"
#include "Units/PWBaseUnit.h"
#include "Kismet/GameplayStatics.h"

void APWWargameGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	GenerateBoard();
}

void APWWargameGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	APWWargameController* WargamePlayer = Cast<APWWargameController>(NewPlayer);
	WargamePlayer->SetGameBoard(GameBoard);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.bNoFail = true;
	APWBaseUnit* Unit = GetWorld()->SpawnActor<APWBaseUnit>(UnitClass, SpawnParameters);
	Unit->OnEndMove.BindUObject(this, &APWWargameGameMode::SwitchPlayer);

	Unit->SetOwner(WargamePlayer);

	WargamePlayer->AddControlledUnitOnServer(Unit);
	if (Players.Num() == 0)
	{
		WargamePlayer->Client_SetAsFirstPlayer();

		Unit->SetDefaultPosition(FIntPoint(0, 0));
		Unit->SetDefaultAngle(0.f);
	}
	else
	{
		WargamePlayer->Client_SetAsSecondPlayer();

		Unit->SetDefaultPosition(FIntPoint(GameBoard->GetBoardSize() - FIntPoint(1, 1)));
		Unit->SetDefaultAngle(180.f);

		StartGame();
	}

	Players.Add(WargamePlayer);
}

void APWWargameGameMode::GenerateBoard()
{
	TArray<AActor*> FoundBoards;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APWGameBoard::StaticClass(), FoundBoards);
	if (FoundBoards.Num() > 0)
	{
		GameBoard = Cast<APWGameBoard>(FoundBoards[0]);
		GameBoard->SetBoardSize(BoardSize);
	}
}

void APWWargameGameMode::StartGame()
{
	CurrentPlayerIndex = 0;
	Players[CurrentPlayerIndex]->SetControlIsEnabled(true);
}

void APWWargameGameMode::SwitchPlayer()
{
	if (CurrentPlayerIndex == 0)
	{
		CurrentPlayerIndex = 1;
	}
	else
	{
		CurrentPlayerIndex = 0;
	}
	Players[CurrentPlayerIndex]->SetControlIsEnabled(true);
}
