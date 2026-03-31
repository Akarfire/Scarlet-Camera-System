// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SCS_CameraState.h"
#include "SCS_CameraProfile.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SCARLETCAMERASYSTEM_API USCS_CameraProfile : public UObject
{
	GENERATED_BODY()
	
protected:

	// Associated USCS_CameraController
	class ASCS_CameraController* CameraController;

	// Name of the profile
	FName ProfileName;


	// Cached values
	// Returned by default, should be modified in `Update(...)` method
	FSCS_CameraState CameraState;
	FSCS_CameraStateInterpolation CameraStateInterpolation;
	FSCS_BlendingSettings BlendInSettings;

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
	const FSCS_CameraState& GetCameraState();
	virtual const FSCS_CameraState& GetCameraState_Implementation() { return CameraState; }

	// Returns desired camera state interpolation
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "ScarletCameraSystem|CameraProfile")
	const FSCS_CameraStateInterpolation& GetCameraStateInterpolation();
	virtual const FSCS_CameraStateInterpolation& GetCameraStateInterpolation_Implementation() { return CameraStateInterpolation; }

	// Returns blending settings for transitioning from previous profile to this one
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "ScarletCameraSystem|CameraProfile")
	const FSCS_BlendingSettings& GetBlendInSettings();
	virtual const FSCS_BlendingSettings& GetBlendInSettings_Implementation() { return BlendInSettings; }


	// Called at the end of the setup process
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ScarletCameraSystem|CameraProfile")
	void Initialize();
	virtual void Initialize_Implementation() {}

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


	// SYSTEM

	// CALLED BY THE CAMERA CONTROLLER. Sets up profile's name
	void SetupProfile(class ASCS_CameraController* InCameraController, const FName& InProfileName)
	{ 
		CameraController = InCameraController;
		ProfileName = InProfileName; 

		Initialize();
	}
};
