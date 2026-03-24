// Fill out your copyright notice in the Description page of Project Settings.


#include "USCS_CameraController.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "USCS_CameraProfile.h"
#include "USCS_SimpleCameraProfile.h"


// Sets default values
ASCS_CameraController::ASCS_CameraController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoomArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("BoomArm"));
	BoomArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(BoomArm);
}

// Called when the game starts or when spawned
void ASCS_CameraController::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeProfiles();
}

// Creates USCS_CameraProfile objects based on camera profile configuration
void ASCS_CameraController::InitializeProfiles()
{
	// Initializing custom profiles
	for (auto& CustomProfile : CustomCameraProfiles)
	{
		const FName& ProfileName = CustomProfile.Key;
		const TSubclassOf<USCS_CameraProfile>& ProfileClass = CustomProfile.Value;

		if (CameraProfiles.Contains(ProfileName))
			UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Duplicate custom profile names!"));

		USCS_CameraProfile* ProfileObject = NewObject<USCS_CameraProfile>(this, ProfileClass);
		CameraProfiles.Add(ProfileName, ProfileObject);
	}

	// Initializing simple profiles
	for (auto& SimpleProfile : SimpleCameraProfiles)
	{
		const FName& ProfileName = SimpleProfile.Key;
		const FSCS_SimpleCameraProfileDescription& ProfileDescription = SimpleProfile.Value;

		if (CameraProfiles.Contains(ProfileName))
			UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Duplicate profile names!"));

		USCS_SimpleCameraProfile* ProfileObject = NewObject<USCS_SimpleCameraProfile>(this, USCS_SimpleCameraProfile::StaticClass());
		ProfileObject->SyncDescription(ProfileDescription);
		CameraProfiles.Add(ProfileName, ProfileObject);
	}
}

// Called every frame
void ASCS_CameraController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

