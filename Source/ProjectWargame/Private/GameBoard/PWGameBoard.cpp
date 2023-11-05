// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBoard/PWGameBoard.h"
#include "GameBoard/PWGameBoardTile.h"
#include "Units/PWBaseUnit.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APWGameBoard::APWGameBoard()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void APWGameBoard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APWGameBoard, BoardSize);
}

void APWGameBoard::SetBoardSize(const FIntPoint& InBoardSize)
{
	BoardSize = InBoardSize;
}

void APWGameBoard::SetOrientation(bool bInIsOrientedForward)
{
	if (bInIsOrientedForward)
	{
		SetActorRotation(FRotator(0.f, 0.f, 0.f));
	}
	else
	{
		SetActorRotation(FRotator(0.f, 180.f, 0.f));
	}
}

FIntPoint APWGameBoard::GetBoardSize() const
{
	return BoardSize;
}

bool APWGameBoard::PositionIsValid(const FIntPoint& Position) const
{
	return Position.X >= 0 && Position.X < BoardSize.X && Position.Y >= 0 && Position.Y < BoardSize.Y;
}

FIntPoint APWGameBoard::GetHoveredPosition() const
{
	if (HoveredTile)
	{
		return HoveredTile->GetPosition();
	}
	return FIntPoint(-1, -1);
}

void APWGameBoard::TakePosition(const FIntPoint& InPosition)
{
	TakenPositions.Add(InPosition);
}

void APWGameBoard::ReleasePosition(const FIntPoint& InPosition)
{
	TakenPositions.Remove(InPosition);
}

TArray<FIntPoint> APWGameBoard::GetTakenPositions() const
{
	return TakenPositions;
}

void APWGameBoard::SetPositionsAsAvailable(const TArray<FIntPoint>& InPositions)
{
	for (const FIntPoint& Position : InPositions)
	{
		APWGameBoardTile* Tile = Tiles[Position.Y * BoardSize.Y + Position.X];
		Tile->SetIsAvailable(true);
		AvailableTiles.Add(Tile);
	}
}

void APWGameBoard::ClearAvailablePositions()
{
	for (APWGameBoardTile* Tile : AvailableTiles)
	{
		Tile->SetIsAvailable(false);
	}
	AvailableTiles.Empty();
}

FVector APWGameBoard::GetLocationOf(const FIntPoint& Position) const
{
	if (BoardSize.X == 0 && BoardSize.Y == 0) return FVector();

	FIntPoint ClampedPosition;
	ClampedPosition.X = FMath::Clamp(Position.X, 0, BoardSize.X - 1);
	ClampedPosition.Y = FMath::Clamp(Position.Y, 0, BoardSize.Y - 1);

	FVector ResultLocation = GetActorLocation()
		+ FVector((float(ClampedPosition.Y) - float(BoardSize.Y) / 2.f + 0.5f) * TileSize.Y,
				  (float(ClampedPosition.X) - float(BoardSize.X) / 2.f + 0.5f) * TileSize.X,
				  0.f);
	 
	return ResultLocation;
}

void APWGameBoard::SpawnTiles()
{
	ENetMode Mode = GetNetMode();

	for (APWGameBoardTile* Tile : Tiles)
	{
		if (IsValid(Tile))
		{
			Tile->Destroy(true, true);
		}
	}
	Tiles.Empty();

	for (int YIndex = 0; YIndex < BoardSize.Y; YIndex++)
	{
		for (int XIndex = 0; XIndex < BoardSize.X; XIndex++)
		{
			FVector SpawnLocation = FVector((float(YIndex) - float(BoardSize.Y) / 2.f + 0.5f) * TileSize.Y,
											(float(XIndex) - float(BoardSize.X) / 2.f + 0.5f) * TileSize.X,
											VerticalOffset);

			FRotator SpawnRotation = FRotator(0.f, FMath::RandRange(0, 3), 0.f) * 90.f;

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			APWGameBoardTile* SpawnedTile = Cast<APWGameBoardTile>(GetWorld()->SpawnActor(TileClass, &SpawnLocation, &SpawnRotation, SpawnParameters));
			SpawnedTile->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			SpawnedTile->SetPosition(FIntPoint(XIndex, YIndex));
			SpawnedTile->SetIsAvailable(false);
			SpawnedTile->OnTileBeginCursorOver.AddDynamic(this, &APWGameBoard::OneTileBeginCursorOver);
			SpawnedTile->OnTileEndCursorOver.AddDynamic(this, &APWGameBoard::OneTileEndCursorOver);
			Tiles.Add(SpawnedTile);
		}
	}
}

void APWGameBoard::OnRep_BoardSize()
{
	SpawnTiles();
}

void APWGameBoard::OneTileBeginCursorOver(APWGameBoardTile* TouchedTile)
{
	HoveredTile = TouchedTile;
}

void APWGameBoard::OneTileEndCursorOver(APWGameBoardTile* TouchedTile)
{
	if (TouchedTile == HoveredTile)
	{
		HoveredTile = nullptr;
	}
}


