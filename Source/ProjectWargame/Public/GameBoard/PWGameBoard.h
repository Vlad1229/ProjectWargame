// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PWGameBoard.generated.h"

class APWGameBoardTile;
class APWBaseUnit;

UCLASS()
class PROJECTWARGAME_API APWGameBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	APWGameBoard();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetBoardSize(const FIntPoint& InBoardSize);

	void SetOrientation(bool bInIsOrientedForward);

	FIntPoint GetBoardSize() const;

	bool PositionIsValid(const FIntPoint& Position) const;

	FIntPoint GetHoveredPosition() const;

	void TakePosition(const FIntPoint& InPosition);

	void ReleasePosition(const FIntPoint& InPosition);

	TArray<FIntPoint> GetTakenPositions() const;

	void SetPositionsAsAvailable(const TArray<FIntPoint>& InPositions);

	void ClearAvailablePositions();

	FVector GetLocationOf(const FIntPoint& Position) const;

protected:
	UFUNCTION(CallInEditor)
	void SpawnTiles();

private:
	UFUNCTION()
	void OnRep_BoardSize();

	UFUNCTION()
	void OneTileBeginCursorOver(APWGameBoardTile* TouchedTile);

	UFUNCTION()
	void OneTileEndCursorOver(APWGameBoardTile* TouchedTile);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", ReplicatedUsing = OnRep_BoardSize)
	FIntPoint BoardSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FVector2D TileSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float VerticalOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TSubclassOf<APWGameBoardTile> TileClass;

	UPROPERTY()
	TArray<APWGameBoardTile*> Tiles;

	UPROPERTY()
	TArray<APWGameBoardTile*> AvailableTiles;
	
	UPROPERTY()
	APWGameBoardTile* HoveredTile;

	TArray<FIntPoint> TakenPositions;

private:
	bool bIsOrientedForward;
};
