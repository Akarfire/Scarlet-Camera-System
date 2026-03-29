// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "USCS_CameraState.generated.h"

// Determines the way camera transform is handled
UENUM(BlueprintType)
enum class ESCS_TransformType : uint8
{
	PlayerAttachment UMETA(DisplayName = "Player Attachment"),
	ActorAttachment UMETA(DisplayName = "Actor Attachment"),
	World UMETA(DisplayName = "World")
};

// Defines the way camera's location is handled
USTRUCT(BlueprintType)
struct FSCS_CameraLocation
{
	GENERATED_BODY()

	// Type of location handling
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESCS_TransformType LocationType;

	// The actor used for `ActorAttachment` transform type
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* AttachmentActor;

	// Used either as world location for `World` transform type 
	// or as offset for 'ActorAttachment' and `PlayerAttachment` transform type
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location;

	FSCS_CameraLocation() {}
};

// Defines the way camera's rotation is handled
USTRUCT(BlueprintType)
struct FSCS_CameraRotation
{
	GENERATED_BODY()

	// Type of rotation handling
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESCS_TransformType RotationType;

	// The actor used for `ActorAttachment` transform type
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* AttachmentActor;

	// Used either as world rotation for `World` transform type 
	// or as offset for 'ActorAttachment' and `PlayerAttachment` transform type
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	FSCS_CameraRotation() {}
};


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

	// The way location is handled (location of the origin point of the boom arm)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSCS_CameraLocation CameraLocation;

	// The way boom arm rotation is handled
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSCS_CameraRotation CameraArmRotation;

	// Whether to use camera rotation independent of the boom arm
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool EnableSeparateCameraRotation = false;

	// The way camera rotation is handled
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSCS_CameraRotation SeparateCameraRotation;


	FSCS_CameraState() {}


	// Methods
	
	// Calculates location of the origin point of the boom arm
	// PlayerActor is used for `PlayerAttachment` type of transform
	FVector ResolveLocation(AActor* PlayerActor = nullptr) const;

	// Calculates rotation of the camera
	// PlayerActor is used for `PlayerAttachment` type of transform
	FRotator ResolveCameraRotation(AActor* PlayerActor = nullptr) const { return ResolveRotation(SeparateCameraRotation, PlayerActor); }

	// Calculates rotation of the boom arm
	// PlayerActor is used for `PlayerAttachment` type of transform
	FRotator ResolveBoomArmRotation(AActor* PlayerActor = nullptr) const { return ResolveRotation(CameraArmRotation, PlayerActor); }

protected:

	// Inner method for resolving FSCS_CameraRotation
	FRotator ResolveRotation(const FSCS_CameraRotation& Rotation, AActor* PlayerActor = nullptr) const;
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
	ESCS_InterpolationType FieldOfView_InterpolationType = ESCS_InterpolationType::Ease;

	// Speed of interpolation for camera's field of view
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FieldOfView_InterpolationSpeed = 10.f;


	// Interpolation type for Boom Arm Length state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESCS_InterpolationType BoomArmLength_InterpolationType = ESCS_InterpolationType::Ease;

	// Speed of interpolation for Boom Arm Length state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BoomArmLength_InterpolationSpeed = 10.f;


	// Interpolation type for Camera Offset state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESCS_InterpolationType CameraOffset_InterpolationType = ESCS_InterpolationType::Ease;

	// Speed of interpolation for Camera Offset state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CameraOffset_InterpolationSpeed = 10.f;


	// Interpolation type for Camera Location (location of the origin point of the boom arm)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESCS_InterpolationType Location_InterpolationType = ESCS_InterpolationType::None;

	// Speed of interpolation for Camera Location (location of the origin point of the boom arm)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Location_InterpolationSpeed = 10.f;


	// Interpolation type for Camera Rotation (rotation of the boom arm)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESCS_InterpolationType Rotation_InterpolationType = ESCS_InterpolationType::Ease;

	// Speed of interpolation for Camera Rotation (rotation of the boom arm)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Rotation_InterpolationSpeed = 50.f;


	// Interpolation type for Camera Rotation (rotation of the boom arm)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESCS_InterpolationType SeparateCameraRotation_InterpolationType = ESCS_InterpolationType::None;

	// Speed of interpolation for Camera Rotation (rotation of the boom arm)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SeparateCameraRotation_InterpolationSpeed = 10.f;


	FSCS_CameraStateInterpolation() {}
};


// Defines how blend profiles are blended together during transitions
USTRUCT(BlueprintType)
struct FSCS_BlendingSettings
{
	GENERATED_BODY()

	// Blending animation duration
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BlendAnimationDuration = 0.5f;

	// Blending curve for camera's field of view
	// Ease-In-Out interpolation is used if not curve is specified
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCurveFloat* FieldOfView_Curve;

	// Blending curve for camera's boom arm length
	// Ease-In-Out interpolation is used if not curve is specified
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCurveFloat* BoomArmLength_Curve;

	// Blending curve for camera's offset
	// Ease-In-Out interpolation is used if not curve is specified
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCurveFloat* CameraOffset_Curve;

	// Blending curve for Camera Location (location of the origin point of the boom arm)
	// Ease-In-Out interpolation is used if not curve is specified
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCurveFloat* Location_Curve;

	// Blending curve for Camera Rotation (rotation of the boom arm)
	// Ease-In-Out interpolation is used if not curve is specified
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCurveFloat* Rotation_Curve;

	// Blending curve for Separate Camera Rotation
// Ease-In-Out interpolation is used if not curve is specified
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCurveFloat* SeparateCameraRotation_Curve;


	FSCS_BlendingSettings() {}
};