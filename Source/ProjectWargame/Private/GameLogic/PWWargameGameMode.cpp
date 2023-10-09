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

	WargamePlayer->AddControlledUnit(Unit);
	if (GameBoard)
	{
		Unit->SetActorLocation(GameBoard->GetLocationOf(FIntPoint(0, 0)));
		GameBoard->TakePosition(FIntPoint(0, 0));
	}
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
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game Board is missing"));
	}
}
