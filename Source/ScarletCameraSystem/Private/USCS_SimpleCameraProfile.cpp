// Fill out your copyright notice in the Description page of Project Settings.


#include "USCS_SimpleCameraProfile.h"
#include "USCS_CameraController.h"

// Returns desired camera state
FSCS_CameraState USCS_SimpleCameraProfile::GetCameraState_Implementation()
{
	return CameraController->SimpleCameraProfiles[ProfileName].CameraState;
}

// Returns desired camera state interpolation
FSCS_CameraStateInterpolation USCS_SimpleCameraProfile::GetCameraStateInterpolation_Implementation()
{
	return CameraController->SimpleCameraProfiles[ProfileName].InterpolationSettings;
}

// Returns blending settings for transitioning from previous profile to this one
FSCS_BlendingSettings USCS_SimpleCameraProfile::GetBlendInSettings_Implementation()
{
	return CameraController->SimpleCameraProfiles[ProfileName].BlendInSettings;
}
