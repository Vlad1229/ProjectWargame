// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/PWWarriorUnit.h"

TArray<FIntPoint> APWWarriorUnit::GetAvailableForMovementPositions(const FIntPoint& BoardSize, const TArray<FIntPoint>& TakenPositions) const
{
	TArray<FIntPoint> AvailablePoints;
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			for (int k = 1; k < 3; k++)
			{
				FIntPoint MovementPoint = Position + FIntPoint(i * k, j * k);
				if (MovementPoint.X >= 0 && MovementPoint.X < BoardSize.X &&
					MovementPoint.Y >= 0 && MovementPoint.Y < BoardSize.Y &&
					!TakenPositions.Contains(MovementPoint))
				{
					AvailablePoints.Add(MovementPoint);
				}
				else
				{
					break;
				}
			}
		}
	}
	return AvailablePoints;
}

TArray<FIntPoint> APWWarriorUnit::GetAvailableForActionPositions(const FIntPoint& BoardSize, const TArray<FIntPoint>& TakenPositions) const
{
    return TArray<FIntPoint>();
}
