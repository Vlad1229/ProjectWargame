// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/PWBaseUnit.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameLogic/PWWargameController.h"
#include "GameBoard/PWGameBoard.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

APWBaseUnit::APWBaseUnit()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicatingMovement(false);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	OnBeginCursorOver.AddDynamic(this, &APWBaseUnit::BeginCursorOver);
	OnEndCursorOver.AddDynamic(this, &APWBaseUnit::EndCursorOver);
}

void APWBaseUnit::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundBoards;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APWGameBoard::StaticClass(), FoundBoards);
	if (FoundBoards.Num() > 0)
	{
		GameBoard = Cast<APWGameBoard>(FoundBoards[0]);

		if (GameBoard->PositionIsValid(Position))
		{
			GameBoard->TakePosition(Position);
			SetActorLocation(GameBoard->GetLocationOf(Position));
		}
	}

	if (GetNetMode() == NM_Client)
	{
		if (Owner == GetWorld()->GetFirstPlayerController())
		{
			Cast<APWWargameController>(Owner)->AddControlledUnitOnClient(this);
		}
	}

	SetActorTickEnabled(false);
}

void APWBaseUnit::BeginCursorOver(AActor* TouchedActor)
{
	OnUnitBeginCursorOver.ExecuteIfBound(this);
}

void APWBaseUnit::EndCursorOver(AActor* TouchedActor)
{
	OnUnitEndCursorOver.ExecuteIfBound(this);
}

void APWBaseUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APWBaseUnit, DefaultPosition);
	DOREPLIFETIME(APWBaseUnit, DefaultAngle);
}

void APWBaseUnit::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsRotating)
	{
		float RotationStep = RotationSpeed * DeltaSeconds;
		if (RotationStep < FMath::Abs(AngleDiff))
		{
			int32 Multiplier = AngleDiff >= 0.f ? 1 : -1;
			AngleDiff -= RotationStep * Multiplier;
			SetActorRotation(GetActorRotation() + FRotator(0.f, RotationStep * Multiplier, 0.f));
		}
		else
		{
			SetActorRotation(FRotator(0.f, DesiredAngle, 0.f));
			bIsRotating = false;
		}
	}
	else if (bIsMoving)
	{
		FVector LocationDiff = DesiredLocation - GetActorLocation();
		float Distance = LocationDiff.Length();

		float MovementStep = MovementSpeed * DeltaSeconds;
		if (MovementStep < Distance)
		{
			float Alpha = MovementStep / Distance;
			SetActorLocation(GetActorLocation() + LocationDiff * Alpha);
		}
		else
		{
			SetActorLocation(DesiredLocation);
			bIsMoving = false;

			RotateTo(DefaultAngle);
			bIsRotating = true;
		}
	}
	else
	{
		Multicast_EndMove();
	}
}

TArray<FIntPoint> APWBaseUnit::GetAvailableForMovementPositions(const FIntPoint& BoardSize, const TArray<FIntPoint>& TakenPositions) const
{
	return TArray<FIntPoint>();
}

TArray<FIntPoint> APWBaseUnit::GetAvailableForActionPositions(const FIntPoint& BoardSize, const TArray<FIntPoint>& TakenPositions) const
{
	return TArray<FIntPoint>();
}

void APWBaseUnit::SetDefaultPosition(const FIntPoint& InPosition)
{
	DefaultPosition = InPosition;
	Position = InPosition;

	GameBoard->TakePosition(Position);
	SetActorLocation(GameBoard->GetLocationOf(Position));
}

void APWBaseUnit::OnRep_DefaultPosition()
{
	Position = DefaultPosition;
	if (GameBoard)
	{
		GameBoard->TakePosition(Position);
		SetActorLocation(GameBoard->GetLocationOf(Position));
	}
}

void APWBaseUnit::SetDefaultAngle(float InAngle)
{
	DefaultAngle = InAngle;
	SetActorRotation(FRotator(0.f, DefaultAngle, 0.f));
}

void APWBaseUnit::OnRep_DefaultAngle()
{
	SetActorRotation(FRotator(0.f, DefaultAngle, 0.f));
}

void APWBaseUnit::Multicast_StartMove_Implementation(FIntPoint FinalPosition)
{
	GameBoard->ReleasePosition(Position);

	Position = FinalPosition;
	DesiredLocation = GameBoard->GetLocationOf(Position);
	bIsMoving = true;

	FVector Location = GetActorLocation();
	RotateTo(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DesiredLocation).Yaw);

	SetActorTickEnabled(true);

	GameBoard->TakePosition(Position);
}

void APWBaseUnit::Multicast_EndMove_Implementation()
{
	SetActorTickEnabled(false);

	SetActorLocation(DesiredLocation);
	SetActorRotation(FRotator(0.f, DefaultAngle, 0.f));

	if (GetNetMode() == NM_DedicatedServer)
	{
		OnEndMove.Execute();
	}
}

bool APWBaseUnit::PerformActionAtTarget(APWBaseUnit* Target)
{
	return true;
}

bool APWBaseUnit::PerformActionAtLocation(const FVector& Location)
{
	return true;
}

#pragma optimize("", off)
void APWBaseUnit::RotateTo(float Angle)
{
	DesiredAngle = Angle;
	bIsRotating = true;

	float a = GetActorRotation().Yaw;
	AngleDiff = DesiredAngle - a;
	if (AngleDiff > 180.f)
	{
		AngleDiff -= 360.f;
	}
	else if (AngleDiff < -180.f)
	{
		AngleDiff += 360.f;
	}

	SetActorTickEnabled(true);
}
#pragma optimize("", on)

FIntPoint APWBaseUnit::GetPosition() const
{
	return Position;
}
