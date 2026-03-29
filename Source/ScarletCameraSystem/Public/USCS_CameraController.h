// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "USCS_CameraState.h"
#include "USCS_SimpleCameraProfile.h"
#include "USCS_CameraController.generated.h"

UCLASS()
class SCARLETCAMERASYSTEM_API ASCS_CameraController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASCS_CameraController();

	// Components

	// Camera boom positioning the camera behind the character 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* BoomArm;

	// Camera component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;


protected:

	// Currently active camera state
	FSCS_CameraState CurrentCameraState;

	// Currently active camera state interpolation
	FSCS_CameraStateInterpolation CurrentCameraStateInterpolation;

	// Currently active camera profile name
	FName CurrentCameraProfileName;

	// All initialized profiles are stored here
	UPROPERTY()
	TMap<FName, class USCS_CameraProfile*> CameraProfiles;


protected:
	// Profile switching

	// Queue of profiles we need to switch to
	TQueue<FName> ProfileSwitchingQueue;

	// Name of the camera profile that was active before the latest switch animation has started playing
	FName PreviousCameraProfile;

	// Current time value of the blending animation, goes from Duration to 0.0 (or below 0.0) while playing
	// If the value is <= 0.0, then the animation has stopped / is not playing
	float BlendingAnimationTime = 0.0;

public:

	// IMPORTANT: Simple and Custom camera profiles MUST NOT SHARE NAMES
	// (and in case they do, custom profiles will be preffered over simple profiles)

	// Simple camera profile descriptors
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CameraProfiles")
	TMap<FName, FSCS_SimpleCameraProfileDescription> SimpleCameraProfiles;

	// Custom camera profile classes
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CameraProfiles")
	TMap<FName, TSubclassOf<USCS_CameraProfile>> CustomCameraProfiles;


	// Camera profile used by default
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CameraProfiles")
	FName DefaultCameraProfile;


	// Whether this camera controller should become the active view target on begin play
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool AutoActivateViewTarget = true;

	// Index of the player, owning this camera
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int PlayerIndex = 0;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Creates USCS_CameraProfile objects based on camera profile configuration
	void InitializeProfiles();


	// UPDATE

	// Updates currently active profiles
	void UpdateProfiles(float DeltaTime);

	// Updates profile blending animation if one is currently playing or starts a new one if one is queued
	void UpdateProfileTransitionAnimation(float DeltaTime);

	// Fetches camera state interpolation from the profile
	void UpdateCurrentCameraStateInterpolation();

	// Updates current camera state based on interpolation configuration
	void UpdateCurrentCameraState(float DeltaTime);


	// Applies camera state
	void ApplyCameraState(const FSCS_CameraState& State, bool ApplyTransform = true);


	// PROGRESSIVE INTERPOLATION

	// Camera state interpolation over time (not for timelines)
	void ProgressiveInterpolateCameraState(FSCS_CameraState& CurrentState, const FSCS_CameraState& TargetState,
		float DeltaTime, const FSCS_CameraStateInterpolation& Interpolation);

	// Camera location interpolation over time (not for timelines)
	FVector ProgressiveInterpolateCameraLocation(const FVector& CurrentLocation, const FSCS_CameraState& State, 
		const FSCS_CameraStateInterpolation& Interpolation, AActor* PlayerActor, float DeltaTime);

	// Camera arm rotation interpolation over time (not for timelines)
	FRotator ProgressiveInterpolateCameraArmRotation(const FRotator& CurrentRotation, const FSCS_CameraState& State, 
		const FSCS_CameraStateInterpolation& Interpolation, AActor* PlayerActor, float DeltaTime);

	// Camera arm interpolation over time (not for timelines)
	FRotator ProgressiveInterpolateCameraRotation(const FRotator& CurrentRotation, const FSCS_CameraState& State,
		const FSCS_CameraStateInterpolation& Interpolation, AActor* PlayerActor, float DeltaTime);


	// TIMELINE INTERPOLATION

	// Camera state interpolation over a timeline
	void TimelineInterpolateCameraState(FSCS_CameraState& OutState, const FSCS_CameraState& InitialState, const FSCS_CameraState& TargetState,
		float Progress, const FSCS_BlendingSettings& BlendingSettings);

	// Camera location interpolation over a timeline
	FVector TimelineInterpolateCameraLocation(const FSCS_CameraState& InitialState, const FSCS_CameraState& TargetState,
		float Progress, const FSCS_BlendingSettings& BlendingSettings, AActor* PlayerActor);

	// Camera arm rotation interpolation over a timeline
	FRotator TimelineInterpolateCameraArmRotation(const FSCS_CameraState& InitialState, const FSCS_CameraState& TargetState,
		float Progress, const FSCS_BlendingSettings& BlendingSettings, AActor* PlayerActor);

	// Camera arm interpolation over a timeline
	FRotator TimelineInterpolateCameraRotation(const FSCS_CameraState& InitialState, const FSCS_CameraState& TargetState,
		float Progress, const FSCS_BlendingSettings& BlendingSettings, AActor* PlayerActor);


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	// INTERFACE

	// Returns camera component
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem")
	class UCameraComponent* GetCamera() { return Camera; }

	// Returns current camera state
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem")
	const FSCS_CameraState& GetCameraState() { return CurrentCameraState; }


	// Returns current camera profile name
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem")
	const FName& GetCurrentCameraProfileName() { return CurrentCameraProfileName; }

	// Changes current camera profile with a transition animation
	UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem")
	void SetCameraProfile(const FName& InProfileName, bool TransitionAnimation = true);

	// Whether profile transition animation is currently playing or not
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem")
	bool IsProfileTransitionAnimationPlaying() { return BlendingAnimationTime > 0.0f; }


	// Returns camera profile object by it's registry name
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem")
	class USCS_CameraProfile* GetCameraProfile(const FName& InProfileName);

/*
	// Registers a new simple camera profile
	// Returns `true` if the profile was succesfully added, `false` if profile with this name is already registered
	UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem")
	bool AddSimpleCameraProfile(const FName& InProfileName, const struct FSCS_SimpleCameraProfileDescription& InSimpleProfile);

	// Registers a new custom camera profile
	// Returns `true` if the profile was succesfully added, `false` if profile with this name is already registered
	UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem")
	bool AddCustomCameraProfile(const FName& InProfileName, class USCS_CameraProfile* InCustomProfile);
*/
};
