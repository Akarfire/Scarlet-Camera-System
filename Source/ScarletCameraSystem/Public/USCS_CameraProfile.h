// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "USCS_CameraState.h"
#include "USCS_CameraProfile.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SCARLETCAMERASYSTEM_API USCS_CameraProfile : public UObject
{
	GENERATED_BODY()
	
protected:
	class ASCS_CameraController* CameraController;

public:

	// Returns a pointer to the associated camera controller
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|CameraProfile")
	class ASCS_CameraController* GetCameraController() { return CameraController; }

	// Returns a pointer to the associated camera
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|CameraProfile")
	class UCameraComponent* GetCamera();


	// VIRTUAL METHODS

	// Returns desired camera state
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "ScarletCameraSystem|CameraProfile")
	FSCS_CameraState GetCameraState();
	virtual FSCS_CameraState GetCameraState_Implementation() { return FSCS_CameraState(); }

	// Returns desired camera state interpolation
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "ScarletCameraSystem|CameraProfile")
	FSCS_CameraStateInterpolation GetCameraStateInterpolation();
	virtual FSCS_CameraStateInterpolation GetCameraStateInterpolation_Implementation() { return FSCS_CameraStateInterpolation(); }

	// Returns blending settings for transitioning from previous profile to this one
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "ScarletCameraSystem|CameraProfile")
	FSCS_BlendingSettings GetBlendInSettings();
	virtual FSCS_BlendingSettings GetBlendInSettings_Implementation() { return FSCS_BlendingSettings(); }


	// Called when this profile is activated
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ScarletCameraSystem|CameraProfile")
	void Activate();
	virtual void Activate_Implementation() {}

	// Called when this profile is deactivated
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ScarletCameraSystem|CameraProfile")
	void Deactivate();
	virtual void Deactivate_Implementation() {}

	// Called every time the camera controller ticks (while this profile is active)
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ScarletCameraSystem|CameraProfile")
	void Update(float DeltaTime);
	virtual void Update_Implementation(float DeltaTime) {}
};
