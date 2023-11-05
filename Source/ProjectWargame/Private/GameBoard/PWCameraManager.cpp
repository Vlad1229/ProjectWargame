// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBoard/PWCameraManager.h"

APWCameraManager::APWCameraManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

ACameraActor* APWCameraManager::GetFistCamera() const
{
	return CameraActors[0];
}

ACameraActor* APWCameraManager::GetSecondCamera() const
{
	return CameraActors[1];
}


