// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SCS_Types.h"
#include "SCS_SimpleCameraProfile.h"
#include "SCS_CameraController.generated.h"

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

	// Current time value of the blending animation, goes from Duration to 0.0 (or below 0.0) while playing
	// If the value is <= 0.0, then the animation has stopped / is not playing
	float BlendingAnimationTime = 0.0;

	// Frozen state cache (used when profile trasition is iterrupted with a new profile stransition)
	FSCS_CameraState FrozenState;

public:

	// IMPORTANT: Simple and Custom camera profiles MUST NOT SHARE NAMES
	// (and in case they do, custom profiles will be preffered over simple profiles)

	// Simple camera profile descriptors
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|CameraProfiles")
	TMap<FName, FSCS_SimpleCameraProfileDescription> SimpleCameraProfiles;

	// Custom camera profile classes
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|CameraProfiles")
	TMap<FName, TSubclassOf<USCS_CameraProfile>> CustomCameraProfiles;


	// Camera profile used by default
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|CameraProfiles")
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

	// Starts profile transition animations for enqueued switch requests
	void UpdateProfileSwitchQueue(float DeltaTime);

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
	FVector ProgressiveInterpolateCameraLocation(const FVector& CurrentLocation, const FVector& TargetLocation,
		const FSCS_CameraStateInterpolation& Interpolation, float DeltaTime);

	// Camera arm rotation interpolation over time (not for timelines)
	FRotator ProgressiveInterpolateCameraArmRotation(const FRotator& CurrentRotation, const FRotator& TargetRotation,
		const FSCS_CameraStateInterpolation& Interpolation, float DeltaTime);

	// Camera arm interpolation over time (not for timelines)
	FRotator ProgressiveInterpolateCameraRotation(const FRotator& CurrentRotation, const FRotator& TargetRotation,
		const FSCS_CameraStateInterpolation& Interpolation, float DeltaTime);


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

	// Freezes current state, removing dynamically resolved elements from it
	void FreezeCurrentCameraState(FSCS_CameraState& OutFrozenState, 
        bool WorldSpaceLocation, bool WorldSpaceArmRotation, bool WorldSpaceCameraRotation);


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
	void SetCameraProfile(const FName& InProfileName, bool TransitionAnimation = true, bool Queue = false);

	// Whether the specified profile is registered or not
	UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem")
	bool IsCameraProfileValid(const FName& InProfileName) { return CameraProfiles.Contains(InProfileName); }

	// Whether profile transition animation is currently playing or not
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem")
	bool IsProfileTransitionAnimationPlaying() { return BlendingAnimationTime > 0.0f; }


	// Returns camera profile object by it's registry name
	// Returns `nullptr` if no such profile was found
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem")
	class USCS_CameraProfile* GetCameraProfile(const FName& InProfileName);


	// Registers a new simple camera profile
	// Returns `true` if the profile was succesfully added, `false` if profile with this name is already registered
	UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem")
	bool AddSimpleCameraProfile(const FName& InProfileName, const struct FSCS_SimpleCameraProfileDescription& InSimpleProfile);

	// Registers a new custom camera profile
	// Returns `true` if the profile was succesfully added, `false` if profile with this name is already registered
	UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem")
	bool AddCustomCameraProfile(const FName& InProfileName, TSubclassOf<class USCS_CameraProfile> InCustomProfileClass);

	// Registers an alredy existing custom camera profile
	// Returns `true` if the profile was succesfully added, `false` if profile with this name is already registered
	UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem")
	bool AddCustomCameraProfileExisting(const FName& InProfileName, class USCS_CameraProfile* InCustomProfile);


    // UTILITY
    // Complete camera state interpolation between A and B
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|Utility")
    void InterpolateCameraState(FSCS_CameraState& OutState, const FSCS_CameraState& InitialState, const FSCS_CameraState& TargetState,
        float Progress, const FSCS_BlendingSettings& BlendingSettings);


// SIMPLE PROFILE INTERFACE
public:


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    float GetSimpleProfileFieldOfView(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->CameraState.FieldOfView;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileFieldOfView()!"));
        return float{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileFieldOfView(const FName& InProfileName, float InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->CameraState.FieldOfView = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileFieldOfView()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    float GetSimpleProfileBoomArmLength(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->CameraState.BoomArmLength;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileBoomArmLength()!"));
        return float{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileBoomArmLength(const FName& InProfileName, float InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->CameraState.BoomArmLength = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileBoomArmLength()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    FVector GetSimpleProfileCameraOffset(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->CameraState.CameraOffset;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileCameraOffset()!"));
        return FVector{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileCameraOffset(const FName& InProfileName, FVector InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->CameraState.CameraOffset = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileCameraOffset()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    bool GetSimpleProfileDoCollisionTest(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->CameraState.DoCollisionTest;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileDoCollisionTest()!"));
        return bool{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileDoCollisionTest(const FName& InProfileName, bool InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->CameraState.DoCollisionTest = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileDoCollisionTest()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    FSCS_CameraLocation GetSimpleProfileCameraLocation(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->CameraState.CameraLocation;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileCameraLocation()!"));
        return FSCS_CameraLocation{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileCameraLocation(const FName& InProfileName, FSCS_CameraLocation InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->CameraState.CameraLocation = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileCameraLocation()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    FSCS_CameraRotation GetSimpleProfileCameraArmRotation(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->CameraState.CameraArmRotation;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileCameraArmRotation()!"));
        return FSCS_CameraRotation{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileCameraArmRotation(const FName& InProfileName, FSCS_CameraRotation InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->CameraState.CameraArmRotation = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileCameraArmRotation()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    bool GetSimpleProfileEnableSeparateCameraRotation(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->CameraState.EnableSeparateCameraRotation;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileEnableSeparateCameraRotation()!"));
        return bool{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileEnableSeparateCameraRotation(const FName& InProfileName, bool InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->CameraState.EnableSeparateCameraRotation = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileEnableSeparateCameraRotation()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    FSCS_CameraRotation GetSimpleProfileSeparateCameraRotation(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->CameraState.SeparateCameraRotation;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileSeparateCameraRotation()!"));
        return FSCS_CameraRotation{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileSeparateCameraRotation(const FName& InProfileName, FSCS_CameraRotation InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->CameraState.SeparateCameraRotation = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileSeparateCameraRotation()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    ESCS_InterpolationType GetSimpleProfileFieldOfView_InterpolationType(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->InterpolationSettings.FieldOfView_InterpolationType;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileFieldOfView_InterpolationType()!"));
        return ESCS_InterpolationType{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileFieldOfView_InterpolationType(const FName& InProfileName, ESCS_InterpolationType InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->InterpolationSettings.FieldOfView_InterpolationType = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileFieldOfView_InterpolationType()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    float GetSimpleProfileFieldOfView_InterpolationSpeed(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->InterpolationSettings.FieldOfView_InterpolationSpeed;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileFieldOfView_InterpolationSpeed()!"));
        return float{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileFieldOfView_InterpolationSpeed(const FName& InProfileName, float InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->InterpolationSettings.FieldOfView_InterpolationSpeed = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileFieldOfView_InterpolationSpeed()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    ESCS_InterpolationType GetSimpleProfileBoomArmLength_InterpolationType(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->InterpolationSettings.BoomArmLength_InterpolationType;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileBoomArmLength_InterpolationType()!"));
        return ESCS_InterpolationType{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileBoomArmLength_InterpolationType(const FName& InProfileName, ESCS_InterpolationType InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->InterpolationSettings.BoomArmLength_InterpolationType = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileBoomArmLength_InterpolationType()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    float GetSimpleProfileBoomArmLength_InterpolationSpeed(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->InterpolationSettings.BoomArmLength_InterpolationSpeed;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileBoomArmLength_InterpolationSpeed()!"));
        return float{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileBoomArmLength_InterpolationSpeed(const FName& InProfileName, float InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->InterpolationSettings.BoomArmLength_InterpolationSpeed = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileBoomArmLength_InterpolationSpeed()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    ESCS_InterpolationType GetSimpleProfileCameraOffset_InterpolationType(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->InterpolationSettings.CameraOffset_InterpolationType;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileCameraOffset_InterpolationType()!"));
        return ESCS_InterpolationType{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileCameraOffset_InterpolationType(const FName& InProfileName, ESCS_InterpolationType InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->InterpolationSettings.CameraOffset_InterpolationType = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileCameraOffset_InterpolationType()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    float GetSimpleProfileCameraOffset_InterpolationSpeed(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->InterpolationSettings.CameraOffset_InterpolationSpeed;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileCameraOffset_InterpolationSpeed()!"));
        return float{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileCameraOffset_InterpolationSpeed(const FName& InProfileName, float InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->InterpolationSettings.CameraOffset_InterpolationSpeed = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileCameraOffset_InterpolationSpeed()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    ESCS_InterpolationType GetSimpleProfileLocation_InterpolationType(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->InterpolationSettings.Location_InterpolationType;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileLocation_InterpolationType()!"));
        return ESCS_InterpolationType{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileLocation_InterpolationType(const FName& InProfileName, ESCS_InterpolationType InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->InterpolationSettings.Location_InterpolationType = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileLocation_InterpolationType()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    float GetSimpleProfileLocation_InterpolationSpeed(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->InterpolationSettings.Location_InterpolationSpeed;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileLocation_InterpolationSpeed()!"));
        return float{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileLocation_InterpolationSpeed(const FName& InProfileName, float InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->InterpolationSettings.Location_InterpolationSpeed = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileLocation_InterpolationSpeed()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    ESCS_InterpolationType GetSimpleProfileRotation_InterpolationType(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->InterpolationSettings.Rotation_InterpolationType;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileRotation_InterpolationType()!"));
        return ESCS_InterpolationType{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileRotation_InterpolationType(const FName& InProfileName, ESCS_InterpolationType InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->InterpolationSettings.Rotation_InterpolationType = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileRotation_InterpolationType()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    float GetSimpleProfileRotation_InterpolationSpeed(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->InterpolationSettings.Rotation_InterpolationSpeed;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileRotation_InterpolationSpeed()!"));
        return float{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileRotation_InterpolationSpeed(const FName& InProfileName, float InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->InterpolationSettings.Rotation_InterpolationSpeed = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileRotation_InterpolationSpeed()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    ESCS_InterpolationType GetSimpleProfileSeparateCameraRotation_InterpolationType(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->InterpolationSettings.SeparateCameraRotation_InterpolationType;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileSeparateCameraRotation_InterpolationType()!"));
        return ESCS_InterpolationType{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileSeparateCameraRotation_InterpolationType(const FName& InProfileName, ESCS_InterpolationType InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->InterpolationSettings.SeparateCameraRotation_InterpolationType = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileSeparateCameraRotation_InterpolationType()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    float GetSimpleProfileSeparateCameraRotation_InterpolationSpeed(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->InterpolationSettings.SeparateCameraRotation_InterpolationSpeed;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileSeparateCameraRotation_InterpolationSpeed()!"));
        return float{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileSeparateCameraRotation_InterpolationSpeed(const FName& InProfileName, float InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->InterpolationSettings.SeparateCameraRotation_InterpolationSpeed = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileSeparateCameraRotation_InterpolationSpeed()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    float GetSimpleProfileTransitionAnimationDuration(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->BlendInSettings.TransitionAnimationDuration;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileTransitionAnimationDuration()!"));
        return float{};
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileTransitionAnimationDuration(const FName& InProfileName, float InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->BlendInSettings.TransitionAnimationDuration = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileTransitionAnimationDuration()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    UCurveFloat* GetSimpleProfileFieldOfView_Curve(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->BlendInSettings.FieldOfView_Curve;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileFieldOfView_Curve()!"));
        return nullptr;
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileFieldOfView_Curve(const FName& InProfileName, UCurveFloat* InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->BlendInSettings.FieldOfView_Curve = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileFieldOfView_Curve()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    UCurveFloat* GetSimpleProfileBoomArmLength_Curve(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->BlendInSettings.BoomArmLength_Curve;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileBoomArmLength_Curve()!"));
        return nullptr;
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileBoomArmLength_Curve(const FName& InProfileName, UCurveFloat* InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->BlendInSettings.BoomArmLength_Curve = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileBoomArmLength_Curve()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    UCurveFloat* GetSimpleProfileCameraOffset_Curve(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->BlendInSettings.CameraOffset_Curve;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileCameraOffset_Curve()!"));
        return nullptr;
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileCameraOffset_Curve(const FName& InProfileName, UCurveFloat* InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->BlendInSettings.CameraOffset_Curve = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileCameraOffset_Curve()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    UCurveFloat* GetSimpleProfileLocation_Curve(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->BlendInSettings.Location_Curve;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileLocation_Curve()!"));
        return nullptr;
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileLocation_Curve(const FName& InProfileName, UCurveFloat* InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->BlendInSettings.Location_Curve = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileLocation_Curve()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    UCurveFloat* GetSimpleProfileRotation_Curve(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->BlendInSettings.Rotation_Curve;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileRotation_Curve()!"));
        return nullptr;
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileRotation_Curve(const FName& InProfileName, UCurveFloat* InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->BlendInSettings.Rotation_Curve = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileRotation_Curve()!"));
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem|SimpleProfiles")
    UCurveFloat* GetSimpleProfileSeparateCameraRotation_Curve(const FName& InProfileName)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            return Description->BlendInSettings.SeparateCameraRotation_Curve;
        }
        UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into GetSimpleProfileSeparateCameraRotation_Curve()!"));
        return nullptr;
    }

    UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem|SimpleProfiles")
    void SetSimpleProfileSeparateCameraRotation_Curve(const FName& InProfileName, UCurveFloat* InValue)
    {
        FSCS_SimpleCameraProfileDescription* Description = SimpleCameraProfiles.Find(InProfileName);
        if (Description)
        {
            Description->BlendInSettings.SeparateCameraRotation_Curve = InValue;
        }
        else
            UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid profile name passed into SetSimpleProfileSeparateCameraRotation_Curve()!"));
    }
};
