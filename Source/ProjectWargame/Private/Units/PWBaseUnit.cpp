// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/PWBaseUnit.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

APWBaseUnit::APWBaseUnit()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
}

void APWBaseUnit::BeginPlay()
{
	Super::BeginPlay();
}

void APWBaseUnit::OnBeginCursorOver(AActor* TouchedActor)
{
	OnUnitBeginCursorOver.ExecuteIfBound(Index);
}

void APWBaseUnit::OnEndCursorOver(AActor* TouchedActor)
{
	OnUnitEndCursorOver.ExecuteIfBound(Index);
}

void APWBaseUnit::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsRotating)
	{
		float AngleDiff = DesiredAngle - GetActorRotation().Yaw;
		if (AngleDiff > 180.f)
		{
			AngleDiff -= 360.f;
		}

		if (RotationSpeed < FMath::Abs(AngleDiff))
		{
			int32 Multiplier = AngleDiff >= 0.f ? 1 : -1;
			AddActorWorldRotation(FRotator(0.f, RotationSpeed * Multiplier, 0.f));
		}
		else
		{
			AddActorWorldRotation(FRotator(0.f, AngleDiff, 0.f));
			bIsRotating = false;
		}
	}
	else if (bIsMoving)
	{
		FVector LocationDiff = DesiredLocation - GetActorLocation();
		float Distance = LocationDiff.Length();

		if (MovementSpeed < Distance)
		{
			float Alpha = MovementSpeed / Distance;
			AddActorWorldOffset(LocationDiff * Alpha);
		}
		else
		{
			SetActorLocation(DesiredLocation);
			bIsMoving = false;

			RotateTo(0);
			bIsRotating = true;
		}
	}
	else
	{
		SetActorTickEnabled(false);

		OnMoveEnd.ExecuteIfBound();
	}
}

void APWBaseUnit::SetIndex(int32 InIndex)
{
	Index = InIndex;
}

TArray<FIntPoint> APWBaseUnit::GetAvailableForMovementPositions(const FIntPoint& BoardSize, const TArray<FIntPoint>& TakenPositions) const
{
	return TArray<FIntPoint>();
}

TArray<FIntPoint> APWBaseUnit::GetAvailableForActionPositions(const FIntPoint& BoardSize, const TArray<FIntPoint>& TakenPositions) const
{
	return TArray<FIntPoint>();
}

void APWBaseUnit::MoveTo(const FVector& Location)
{
	RotateTo(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location).Yaw);

	DesiredLocation = Location;
	bIsMoving = true;

	SetActorTickEnabled(true);
}

bool APWBaseUnit::PerformActionAtTarget(APWBaseUnit* Target)
{
	return true;
}

bool APWBaseUnit::PerformActionAtLocation(const FVector& Location)
{
	return true;
}

void APWBaseUnit::RotateTo(float Angle)
{
	DesiredAngle = Angle;
	bIsRotating = true;

	SetActorTickEnabled(true);
}

void APWBaseUnit::SetPosition(const FIntPoint& InPosition)
{
	Position = InPosition;
}

FIntPoint APWBaseUnit::GetPosition() const
{
	return Position;
}
