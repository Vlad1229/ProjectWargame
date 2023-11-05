// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PWBaseUnit.generated.h"

class USkeletalMeshComponent;
class APWGameBoard;

DECLARE_DELEGATE_OneParam(FOnUnitBeginCursorOver, APWBaseUnit*);
DECLARE_DELEGATE_OneParam(FOnUnitEndCursorOver, APWBaseUnit*);
DECLARE_DELEGATE(FOnEndUnitMove);

UCLASS(Abstract)
class PROJECTWARGAME_API APWBaseUnit : public AActor
{
	GENERATED_BODY()
	
public:
	APWBaseUnit();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaSeconds) override;

	virtual TArray<FIntPoint> GetAvailableForMovementPositions(const FIntPoint& BoardSize, const TArray<FIntPoint>& TakenPositions) const;

	virtual TArray<FIntPoint> GetAvailableForActionPositions(const FIntPoint& BoardSize, const TArray<FIntPoint>& TakenPositions) const;

	void SetDefaultPosition(const FIntPoint& InPosition);

	UFUNCTION()
	void OnRep_DefaultPosition();

	void SetDefaultAngle(float InAngle);

	UFUNCTION()
	void OnRep_DefaultAngle();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartMove(FIntPoint FinalPosition);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndMove();

	virtual bool PerformActionAtTarget(APWBaseUnit* Target);

	virtual bool PerformActionAtLocation(const FVector& Location);

	void RotateTo(float Angle);

	FIntPoint GetPosition() const;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void BeginCursorOver(AActor* TouchedActor);

	UFUNCTION()
	void EndCursorOver(AActor* TouchedActor);

public:
	FOnUnitBeginCursorOver OnUnitBeginCursorOver;
	FOnUnitEndCursorOver OnUnitEndCursorOver;
	FOnEndUnitMove OnEndMove;

protected:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMeshComponent;

	APWGameBoard* GameBoard;

	UPROPERTY(EditDefaultsOnly)
	float MovementSpeed;

	UPROPERTY(EditDefaultsOnly)
	float RotationSpeed;

	UPROPERTY(EditDefaultsOnly)
	int32 Health;

	UPROPERTY(EditDefaultsOnly)
	int32 Damage;

	UPROPERTY(ReplicatedUsing = OnRep_DefaultPosition)
	FIntPoint DefaultPosition = FIntPoint(-1, -1);

	FIntPoint Position = FIntPoint(-1, -1);

	UPROPERTY(ReplicatedUsing = OnRep_DefaultAngle)
	float DefaultAngle = -1.f;

private:
	FVector DesiredLocation;

	float DesiredAngle;

	float AngleDiff;

	bool bIsRotating;

	bool bIsMoving;
};
