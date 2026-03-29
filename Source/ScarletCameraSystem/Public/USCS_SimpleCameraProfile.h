// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "USCS_CameraProfile.h"
#include "USCS_SimpleCameraProfile.generated.h"

USTRUCT(BlueprintType)
struct FSCS_SimpleCameraProfileDescription
{
	GENERATED_BODY()

	// Static camera state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSCS_CameraState CameraState;

	// Static camera state interpolation configuration
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSCS_CameraStateInterpolation InterpolationSettings;

	// Blending in settings for this profile
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSCS_BlendingSettings BlendInSettings;
};

/**
 * 
 */
UCLASS()
class SCARLETCAMERASYSTEM_API USCS_SimpleCameraProfile : public USCS_CameraProfile
{
	GENERATED_BODY()
	
public:
	USCS_SimpleCameraProfile() {}

public:

	// OVERRIDES

	// Returns desired camera state
	virtual FSCS_CameraState GetCameraState_Implementation() override;

	// Returns desired camera state interpolation
	virtual FSCS_CameraStateInterpolation GetCameraStateInterpolation_Implementation() override;

	// Returns blending settings for transitioning from previous profile to this one
	virtual FSCS_BlendingSettings GetBlendInSettings_Implementation() override;
};
