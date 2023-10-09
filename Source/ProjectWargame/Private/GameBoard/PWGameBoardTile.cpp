// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBoard/PWGameBoardTile.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APWGameBoardTile::APWGameBoardTile()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;
}

void APWGameBoardTile::BeginPlay()
{
	Super::BeginPlay();
	
	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
	StaticMeshComponent->SetMaterial(0, DynamicMaterial);
	
	StaticMeshComponent->OnBeginCursorOver.AddDynamic(this, &APWGameBoardTile::OnBeginCursorOver);
	StaticMeshComponent->OnEndCursorOver.AddDynamic(this, &APWGameBoardTile::OneEndCursorOver);
}

void APWGameBoardTile::OnBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	bIsHovered = true;

	if (bIsAvailable)
	{
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue("bIsHovered", 1.f);
		}

		if (OnTileBeginCursorOver.IsBound())
		{
			OnTileBeginCursorOver.Broadcast(this);
		}
	}
}

void APWGameBoardTile::OneEndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	bIsHovered = false;

	if (bIsAvailable)
	{
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue("bIsHovered", 0.f);
		}

		if (OnTileEndCursorOver.IsBound())
		{
			OnTileEndCursorOver.Broadcast(this);
		}
	}
}

void APWGameBoardTile::SetPosition(FIntPoint InPosition)
{
	Position = InPosition;
}

FIntPoint APWGameBoardTile::GetPosition() const
{
	return Position;
}

void APWGameBoardTile::SetIsAvailable(bool bInIsAvailable)
{
	bIsAvailable = bInIsAvailable;
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue("bIsAvailable", bInIsAvailable ? 1.f : 0.f);
	}

	if (bIsHovered)
	{
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue("bIsHovered", bIsAvailable ? 1.f : 0.f);
		}

		if (bIsAvailable)
		{
			if (OnTileBeginCursorOver.IsBound())
			{
				OnTileBeginCursorOver.Broadcast(this);
			}
		}
		else 
		{
			if (OnTileEndCursorOver.IsBound())
			{
				OnTileEndCursorOver.Broadcast(this);
			}
		}
	}
}

void APWGameBoardTile::SetIsHovered(bool bInIsHovered)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue("bIsHovered", bInIsHovered ? 1.f : 0.f);
	}
}
