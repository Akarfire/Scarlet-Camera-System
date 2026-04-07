// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SCS_CameraController.h"
#include "SCS_FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETCAMERASYSTEM_API USCS_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	// Attempts to find a scarlet camera controller actor in the world
	// If non exists, returns nullptr
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|Utility")
	static ASCS_CameraController* GetScarletCameraController(const UObject* WorldContextObject, int PlayerIndex = 0);

	// Attempts to find a scarlet camera controller actor with a matching class
	// If non exists, returns nullptr
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|Utility")
	static ASCS_CameraController* GetScarletCameraControllerOfClass(const UObject* WorldContextObject, 
		TSubclassOf<ASCS_CameraController> CameraControllerClass, bool AllowDecendants = true, int PlayerIndex = 0);

	// Attempts to find a scarlet camera controller actor in the world
	// If non exists, creates a new one
	UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|Utility")
	static ASCS_CameraController* GetOrCreateScarletCameraController(const UObject* WorldContextObject, 
		TSubclassOf<ASCS_CameraController> CameraControllerClass, const FTransform& DefaultSpawnTransform, bool AllowDecendants = true, int PlayerIndex = 0);

};
