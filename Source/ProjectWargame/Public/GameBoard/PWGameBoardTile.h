// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PWGameBoardTile.generated.h"

class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileBeginCursorOver, APWGameBoardTile*, TouchedTile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileEndCursorOver, APWGameBoardTile*, TouchedTile);

UCLASS()
class PROJECTWARGAME_API APWGameBoardTile : public AActor
{
	GENERATED_BODY()
	
public:	
	APWGameBoardTile();

	void SetPosition(FIntPoint InPosition);

	FIntPoint GetPosition() const;
	
	void SetIsAvailable(bool bInIsAvailable);

	void SetIsHovered(bool bInIsHovered);

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBeginCursorOver(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OneEndCursorOver(UPrimitiveComponent* TouchedComponent);

public:
	FOnTileBeginCursorOver OnTileBeginCursorOver;
	FOnTileEndCursorOver OnTileEndCursorOver;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	UMaterialInterface* Material;

	UPROPERTY(BlueprintReadOnly, Category = "Mesh")
	UMaterialInstanceDynamic* DynamicMaterial;

private:
	FIntPoint Position;

	bool bIsAvailable;

	bool bIsHovered;
};
