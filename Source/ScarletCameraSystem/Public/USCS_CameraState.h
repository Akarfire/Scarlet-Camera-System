// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// Defines a camera state
USTRUCT(BlueprintType)
struct FSCS_CameraState
{
	GENERATED_BODY()

	// Camera's field of view
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FieldOfView = 90.f;

	// How far away is the camera from it's center of rotation
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BoomArmLength = 100.f;

	// Offset of the camera, relative to the boom arm's end
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector CameraOffset = FVector::ZeroVector;

	// Whether boom arm's collision should be performed or not
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool DoCollisionTest = true;
};


// Determines which type of interpolation will be used
UENUM(BlueprintType)
enum class ESCS_InterpolationType : uint8
{
	None UMETA(DisplayName = "None"),
	Linear UMETA(DisplayName = "Linear"),
	Ease UMETA(DisplayName = "Ease")
};


// Defines how camera state is interpolated
USTRUCT(BlueprintType)
struct FSCS_CameraStateInterpolation
{
	GENERATED_BODY()

	// Interpolation type for camera's field of view
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESCS_InterpolationType FieldOfView_InterpolationType = ESCS_InterpolationType::None;

	// Speed of interpolation for camera's field of view
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FieldOfView_InterpolationSpeed = 10.f;


	// Interpolation type for Boom Arm Length state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESCS_InterpolationType BoomArmLength_InterpolationType = ESCS_InterpolationType::None;

	// Speed of interpolation for Boom Arm Length state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BoomArmLength_InterpolationSpeed = 10.f;


	// Interpolation type for Camera Offset state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESCS_InterpolationType CameraOffset_InterpolationType = ESCS_InterpolationType::None;

	// Speed of interpolation for Camera Offset state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CameraOffset_InterpolationSpeed = 10.f;
};


// Defines static camera boom arm parameters
USTRUCT(BlueprintType)
struct FSCS_BoomArmParameters
{
	GENERATED_BODY()

	// TO DO: Add more boom arm parameters

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool EnableLag = false;
};