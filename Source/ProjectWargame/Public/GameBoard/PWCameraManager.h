// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PWCameraManager.generated.h"

class ACameraActor;

UCLASS()
class PROJECTWARGAME_API APWCameraManager : public AActor
{
	GENERATED_BODY()
	
public:	
	APWCameraManager();

	ACameraActor* GetFistCamera() const;

	ACameraActor* GetSecondCamera() const;

protected:
	UPROPERTY(EditAnywhere)
	TArray<ACameraActor*> CameraActors;
};
