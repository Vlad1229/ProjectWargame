// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PWBaseUnit.generated.h"

class USkeletalMeshComponent;

DECLARE_DELEGATE_OneParam(FOnUnitBeginCursorOver, int32);
DECLARE_DELEGATE_OneParam(FOnUnitEndCursorOver, int32);
DECLARE_DELEGATE(FOnMoveEnd);

UCLASS(Abstract)
class PROJECTWARGAME_API APWBaseUnit : public AActor
{
	GENERATED_BODY()
	
public:
	APWBaseUnit();

	virtual void Tick(float DeltaSeconds) override;

	void SetIndex(int32 InIndex);

	virtual TArray<FIntPoint> GetAvailableForMovementPositions(const FIntPoint& BoardSize, const TArray<FIntPoint>& TakenPositions) const;

	virtual TArray<FIntPoint> GetAvailableForActionPositions(const FIntPoint& BoardSize, const TArray<FIntPoint>& TakenPositions) const;

	void MoveTo(const FVector& Location);

	virtual bool PerformActionAtTarget(APWBaseUnit* Target);

	virtual bool PerformActionAtLocation(const FVector& Location);

	void RotateTo(float Angle);

	void SetPosition(const FIntPoint& InPosition);

	FIntPoint GetPosition() const;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBeginCursorOver(AActor* TouchedActor);

	UFUNCTION()
	void OnEndCursorOver(AActor* TouchedActor);

public:
	FOnUnitBeginCursorOver OnUnitBeginCursorOver;
	FOnUnitEndCursorOver OnUnitEndCursorOver;
	FOnMoveEnd OnMoveEnd;

protected:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	int32 Health;

	UPROPERTY(EditDefaultsOnly)
	int32 Damage;

	FIntPoint Position;

private:
	int32 Index;

	FVector DesiredLocation;

	float DesiredAngle;

	UPROPERTY(EditDefaultsOnly)
	float MovementSpeed;

	UPROPERTY(EditDefaultsOnly)
	float RotationSpeed;

	bool bIsRotating;

	bool bIsMoving;
};
