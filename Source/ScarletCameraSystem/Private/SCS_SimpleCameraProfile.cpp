// Fill out your copyright notice in the Description page of Project Settings.


#include "SCS_SimpleCameraProfile.h"
#include "SCS_CameraController.h"

// Returns desired camera state
const FSCS_CameraState& USCS_SimpleCameraProfile::GetCameraState_Implementation()
{
	return CameraController->SimpleCameraProfiles[ProfileName].CameraState;
}

// Returns desired camera state interpolation
const FSCS_CameraStateInterpolation& USCS_SimpleCameraProfile::GetCameraStateInterpolation_Implementation()
{
	return CameraController->SimpleCameraProfiles[ProfileName].InterpolationSettings;
}

// Returns blending settings for transitioning from previous profile to this one
const FSCS_BlendingSettings& USCS_SimpleCameraProfile::GetBlendInSettings_Implementation()
{
	return CameraController->SimpleCameraProfiles[ProfileName].BlendInSettings;
}
