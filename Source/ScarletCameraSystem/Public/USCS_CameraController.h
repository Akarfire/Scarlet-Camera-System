// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "USCS_CameraState.h"
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
	class USpringArmComponent* CameraBoom;

	// Camera component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;


protected:

	// Currently active camera state
	FSCS_CameraState CurrentCameraState;

	// Currently active camera profile name
	FName CurrentCameraProfileName;

public:

	// IMPORTANT: Simple and Custom camera profiles MUST NOT SHARE NAMES
	// (and in case they do, custom profiles will be preffered over simple profiles during access)

	// Simple camera profile descriptors
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TMap<FName, struct FSCS_SimpleCameraProfileDescription> SimpleCameraProfiles;

	// Custom camera profile classes
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TMap<FName, TSubclassOf<class USCS_CameraProfile*>> CustomCameraProfiles;


	// Camera profile used by default
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FName DefaultCameraProfile;

	// Camera boom arm parameters
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FSCS_BoomArmParameters BoomArmParameters;

protected:
	// CACHE
	FName PreviousCameraProfileName;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Returns camera component
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem")
	class UCameraComponent* GetCamera() { return Camera; }

	// Returns current camera state
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem")
	const FSCS_CameraState& GetCameraState() { return CurrentCameraState; }


	// Returns current camera profile name
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem")
	const FName& GetCurrentCameraProfileName() { return CurrentCameraProfileName; }

	// Changes current camera profile
	UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem")
	void SetCurrentCameraProfile(const FName& InProfileName) { CurrentCameraProfileName = InProfileName; }


	// Returns camera profile object by it's registry name
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletCameraSystem")
	class USCS_CameraProfile* GetCameraProfile(const FName& InProfileName);


	// Registers a new simple camera profile
	// Returns `true` if the profile was succesfully added, `false` if profile with this name is already registered
	UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem")
	bool AddSimpleCameraProfile(const FName& InProfileName, const struct FSCS_SimpleCameraProfileDescription& InSimpleProfile);

	// Registers a new custom camera profile
	// Returns `true` if the profile was succesfully added, `false` if profile with this name is already registered
	UFUNCTION(BlueprintCallable, Category = "ScarletCameraSystem")
	bool AddCustomCameraProfile(const FName& InProfileName, class USCS_CameraProfile* InCustomProfile);
};
