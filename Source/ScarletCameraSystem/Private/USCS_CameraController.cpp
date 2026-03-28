// Fill out your copyright notice in the Description page of Project Settings.


#include "USCS_CameraController.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "USCS_CameraProfile.h"
#include "USCS_SimpleCameraProfile.h"


// INITIALIZATION

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



// UPDATE

// Called every frame
void ASCS_CameraController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateProfileTransitionAnimation(DeltaTime);

	if (!IsProfileTransitionAnimationPlaying())
	{
		UpdateCurrentCameraStateInterpolation();
		UpdateCurrentCameraState(DeltaTime);
	}
}

// Updates profile blending animation if one is currently playing or starts a new one if one is queued
void ASCS_CameraController::UpdateProfileTransitionAnimation(float DeltaTime)
{
	// Starting new animations
	if (!IsProfileTransitionAnimationPlaying())
	{
		if (ProfileSwitchingQueue.IsEmpty()) return;

		PreviousCameraProfile = CurrentCameraProfileName;
		ProfileSwitchingQueue.Dequeue(CurrentCameraProfileName);

		GetCameraProfile(CurrentCameraProfileName)->Activate();

		BlendingAnimationTime = GetCameraProfile(CurrentCameraProfileName)->GetBlendInSettings().BlendAnimationDuration;

		return;
	}

	// Updating current animation
	{
		BlendingAnimationTime -= DeltaTime;

		const FSCS_BlendingSettings& BlendInSettings = GetCameraProfile(CurrentCameraProfileName)->GetBlendInSettings();
		float Progress = 1.f - BlendingAnimationTime / BlendInSettings.BlendAnimationDuration;

		AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(this, PlayerIndex);

		// State interpolation
		TimelineInterpolateCameraState(CurrentCameraState,
			GetCameraProfile(PreviousCameraProfile)->GetCameraState(),
			GetCameraProfile(CurrentCameraProfileName)->GetCameraState(),
			Progress, BlendInSettings);

		FVector Location = TimelineInterpolateCameraLocation(
			GetCameraProfile(PreviousCameraProfile)->GetCameraState(),
			GetCameraProfile(CurrentCameraProfileName)->GetCameraState(),
			Progress, BlendInSettings, PlayerActor);

		FRotator BoomArmRotation = TimelineInterpolateCameraArmRotation(
			GetCameraProfile(PreviousCameraProfile)->GetCameraState(),
			GetCameraProfile(CurrentCameraProfileName)->GetCameraState(),
			Progress, BlendInSettings, PlayerActor);

		FRotator CameraRotation = TimelineInterpolateCameraRotation(
			GetCameraProfile(PreviousCameraProfile)->GetCameraState(),
			GetCameraProfile(CurrentCameraProfileName)->GetCameraState(),
			Progress, BlendInSettings, PlayerActor);

		// Application
		ApplyCameraState(CurrentCameraState, false);

		SetActorLocation(Location);
		SetActorRotation(BoomArmRotation);

		if (CurrentCameraState.EnableSeparateCameraRotation)
			Camera->SetWorldRotation(CameraRotation);
	}

	// Ending animation
	if (BlendingAnimationTime <= 0.f)
	{
		BlendingAnimationTime = 0.f;
		GetCameraProfile(PreviousCameraProfile)->Deactivate();
	}
}

// Fetches camera state interpolation from the profile
void ASCS_CameraController::UpdateCurrentCameraStateInterpolation()
{
	CurrentCameraStateInterpolation = GetCameraProfile(CurrentCameraProfileName)->GetCameraStateInterpolation();
}

// Updates current camera state based on interpolation configuration
void ASCS_CameraController::UpdateCurrentCameraState(float DeltaTime)
{
	// Interpolating camera state
	FSCS_CameraState TargetState = GetCameraProfile(CurrentCameraProfileName)->GetCameraState();
	ProgressiveInterpolateCameraState(CurrentCameraState, TargetState, DeltaTime, CurrentCameraStateInterpolation);

	// Parameter application
	ApplyCameraState(CurrentCameraState, false);

	// Applying transform
	AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(this, PlayerIndex);

	SetActorLocation(ProgressiveInterpolateCameraLocation(GetActorLocation(), CurrentCameraState, CurrentCameraStateInterpolation, PlayerActor, DeltaTime));
	SetActorRotation(ProgressiveInterpolateCameraArmRotation(GetActorRotation(), CurrentCameraState, CurrentCameraStateInterpolation, PlayerActor, DeltaTime));

	if (CurrentCameraState.EnableSeparateCameraRotation)
		Camera->SetWorldRotation(ProgressiveInterpolateCameraRotation(GetActorRotation(), CurrentCameraState, CurrentCameraStateInterpolation, PlayerActor, DeltaTime));

}


// Applies camera state
void ASCS_CameraController::ApplyCameraState(const FSCS_CameraState& State, bool ApplyTransform)
{
	Camera->FieldOfView = State.FieldOfView;

	BoomArm->TargetArmLength = State.BoomArmLength;
	BoomArm->TargetOffset = State.CameraOffset;
	BoomArm->bDoCollisionTest = State.DoCollisionTest;

	if (ApplyTransform)
	{
		AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(this, PlayerIndex);

		SetActorLocation(State.ResolveLocation(PlayerActor));
		SetActorRotation(State.ResolveBoomArmRotation(PlayerActor));

		if (State.EnableSeparateCameraRotation)
			Camera->SetWorldRotation(State.ResolveCameraRotation(PlayerActor));
	}
}



// PROGRESSIVE INTERPOLATION

// Camera state interpolation over time (not for timelines)
void ASCS_CameraController::ProgressiveInterpolateCameraState(FSCS_CameraState& CurrentState, const FSCS_CameraState& TargetState, float DeltaTime, const FSCS_CameraStateInterpolation& Interpolation)
{
	// Field of view
	if (Interpolation.FieldOfView_InterpolationType == ESCS_InterpolationType::None)
		CurrentState.FieldOfView = TargetState.FieldOfView;

	else if (Interpolation.FieldOfView_InterpolationType == ESCS_InterpolationType::Ease)
		CurrentState.FieldOfView = UKismetMathLibrary::FInterpTo(CurrentState.FieldOfView,
																 TargetState.FieldOfView, DeltaTime, 
																 Interpolation.FieldOfView_InterpolationSpeed);

	else if (Interpolation.FieldOfView_InterpolationType == ESCS_InterpolationType::Linear)
		CurrentState.FieldOfView = UKismetMathLibrary::FInterpTo_Constant(CurrentState.FieldOfView,
																		  TargetState.FieldOfView, DeltaTime,
																		  Interpolation.FieldOfView_InterpolationSpeed);

	// Boom Arm Length
	if (Interpolation.BoomArmLength_InterpolationType == ESCS_InterpolationType::None)
		CurrentState.BoomArmLength = TargetState.BoomArmLength;

	else if (Interpolation.BoomArmLength_InterpolationType == ESCS_InterpolationType::Ease)
		CurrentState.BoomArmLength = UKismetMathLibrary::FInterpTo(CurrentState.BoomArmLength,
																   TargetState.BoomArmLength, DeltaTime,
																   Interpolation.BoomArmLength_InterpolationSpeed);

	else if (Interpolation.BoomArmLength_InterpolationType == ESCS_InterpolationType::Linear)
		CurrentState.BoomArmLength = UKismetMathLibrary::FInterpTo_Constant(CurrentState.BoomArmLength,
																			TargetState.BoomArmLength, DeltaTime,
																			Interpolation.BoomArmLength_InterpolationSpeed);

	// Camera Offset
	if (Interpolation.CameraOffset_InterpolationType == ESCS_InterpolationType::None)
		CurrentState.CameraOffset = TargetState.CameraOffset;

	else if (Interpolation.CameraOffset_InterpolationType == ESCS_InterpolationType::Ease)
		CurrentState.CameraOffset = UKismetMathLibrary::VInterpTo(CurrentState.CameraOffset,
			TargetState.CameraOffset, DeltaTime,
			Interpolation.CameraOffset_InterpolationSpeed);

	else if (Interpolation.CameraOffset_InterpolationType == ESCS_InterpolationType::Linear)
		CurrentState.CameraOffset = UKismetMathLibrary::VInterpTo_Constant(CurrentState.CameraOffset,
			TargetState.CameraOffset, DeltaTime,
			Interpolation.CameraOffset_InterpolationSpeed);


	// Do collision test
	CurrentState.DoCollisionTest = TargetState.DoCollisionTest;

	// Transform (No interpolation here, interpolation happens on the application stage)
	CurrentState.CameraLocation = TargetState.CameraLocation;
	CurrentState.SeparateCameraRotation = TargetState.SeparateCameraRotation;
	CurrentState.CameraArmRotation = TargetState.CameraArmRotation;
}

// Camera location interpolation over time (not for timelines)
FVector ASCS_CameraController::ProgressiveInterpolateCameraLocation(const FVector& CurrentLocation, const FSCS_CameraState& State, 
	const FSCS_CameraStateInterpolation& Interpolation, AActor* PlayerActor, float DeltaTime)
{
	FVector NewLocation;

	if (Interpolation.Location_InterpolationType == ESCS_InterpolationType::None)
		NewLocation = State.ResolveLocation(PlayerActor);

	else if (Interpolation.Location_InterpolationType == ESCS_InterpolationType::Ease)
		NewLocation = UKismetMathLibrary::VInterpTo(CurrentLocation,
			State.ResolveLocation(PlayerActor), DeltaTime,
			Interpolation.Location_InterpolationSpeed);

	else if (Interpolation.Location_InterpolationType == ESCS_InterpolationType::Linear)
		NewLocation = UKismetMathLibrary::VInterpTo_Constant(CurrentLocation,
			State.ResolveLocation(PlayerActor), DeltaTime,
			Interpolation.Location_InterpolationSpeed);

	return NewLocation;
}

// Camera rotation interpolation over time (not for timelines)
FRotator ASCS_CameraController::ProgressiveInterpolateCameraArmRotation(const FRotator& CurrentRotation, const FSCS_CameraState& State,
	const FSCS_CameraStateInterpolation& Interpolation, AActor* PlayerActor, float DeltaTime)
{
	FRotator NewRotation;

	if (Interpolation.Location_InterpolationType == ESCS_InterpolationType::None)
		NewRotation = State.ResolveBoomArmRotation(PlayerActor);

	else if (Interpolation.Location_InterpolationType == ESCS_InterpolationType::Ease)
		NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation,
			State.ResolveBoomArmRotation(PlayerActor), DeltaTime,
			Interpolation.Location_InterpolationSpeed);

	else if (Interpolation.Location_InterpolationType == ESCS_InterpolationType::Linear)
		NewRotation = UKismetMathLibrary::RInterpTo_Constant(CurrentRotation,
			State.ResolveBoomArmRotation(PlayerActor), DeltaTime,
			Interpolation.Location_InterpolationSpeed);

	return NewRotation;
}

// Camera arm interpolation over time (not for timelines)
FRotator ASCS_CameraController::ProgressiveInterpolateCameraRotation(const FRotator& CurrentRotation, const FSCS_CameraState& State,
	const FSCS_CameraStateInterpolation& Interpolation, AActor* PlayerActor, float DeltaTime)
{
	FRotator NewRotation;

	if (Interpolation.Location_InterpolationType == ESCS_InterpolationType::None)
		NewRotation = State.ResolveCameraRotation(PlayerActor);

	else if (Interpolation.Location_InterpolationType == ESCS_InterpolationType::Ease)
		NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation,
			State.ResolveCameraRotation(PlayerActor), DeltaTime,
			Interpolation.Location_InterpolationSpeed);

	else if (Interpolation.Location_InterpolationType == ESCS_InterpolationType::Linear)
		NewRotation = UKismetMathLibrary::RInterpTo_Constant(CurrentRotation,
			State.ResolveCameraRotation(PlayerActor), DeltaTime,
			Interpolation.Location_InterpolationSpeed);

	return NewRotation;
}



// TIMELINE INTERPOLATION

// Camera state interpolation over a timeline
void ASCS_CameraController::TimelineInterpolateCameraState(FSCS_CameraState& OutState, const FSCS_CameraState& InitialState, 
	const FSCS_CameraState& TargetState, float Progress, const FSCS_BlendingSettings& BlendingSettings)
{
	// Default value in case no curve is specified
	float EaseInOut = 0.5f * (cos(Progress + PI) + 1);

	// FOV
	float FOV_Progress = EaseInOut;
	if (BlendingSettings.FieldOfView_Curve)
		FOV_Progress = BlendingSettings.FieldOfView_Curve->GetFloatValue(Progress);

	OutState.FieldOfView = UKismetMathLibrary::Lerp(InitialState.FieldOfView, TargetState.FieldOfView, FOV_Progress);

	// Boom Arm Length
	float BoomArmLength_Progress = EaseInOut;
	if (BlendingSettings.BoomArmLength_Curve)
		BoomArmLength_Progress = BlendingSettings.BoomArmLength_Curve->GetFloatValue(Progress);

	OutState.BoomArmLength = UKismetMathLibrary::Lerp(InitialState.BoomArmLength, TargetState.BoomArmLength, BoomArmLength_Progress);

	// Camera Offset
	float CameraOffset_Progress = EaseInOut;
	if (BlendingSettings.CameraOffset_Curve)
		CameraOffset_Progress = BlendingSettings.CameraOffset_Curve->GetFloatValue(Progress);

	OutState.CameraOffset = UKismetMathLibrary::VLerp(InitialState.CameraOffset, TargetState.CameraOffset, CameraOffset_Progress);
}

// Camera location interpolation over a timeline
FVector ASCS_CameraController::TimelineInterpolateCameraLocation(const FSCS_CameraState& InitialState, const FSCS_CameraState& TargetState, 
	float Progress, const FSCS_BlendingSettings& BlendingSettings, AActor* PlayerActor)
{
	// Default value in case no curve is specified
	float EaseInOut = 0.5f * (cos(Progress + PI) + 1);

	float ResultingProgress = EaseInOut;
	if (BlendingSettings.Location_Curve)
		ResultingProgress = BlendingSettings.Location_Curve->GetFloatValue(Progress);
	
	return UKismetMathLibrary::VLerp(InitialState.ResolveLocation(), TargetState.ResolveLocation(), ResultingProgress);
}

// Camera arm rotation interpolation over a timeline
FRotator ASCS_CameraController::TimelineInterpolateCameraArmRotation(const FSCS_CameraState& InitialState, 
	const FSCS_CameraState& TargetState, float Progress, const FSCS_BlendingSettings& BlendingSettings, AActor* PlayerActor)
{
	// Default value in case no curve is specified
	float EaseInOut = 0.5f * (cos(Progress + PI) + 1);

	float ResultingProgress = EaseInOut;
	if (BlendingSettings.Location_Curve)
		ResultingProgress = BlendingSettings.Location_Curve->GetFloatValue(Progress);

	return UKismetMathLibrary::RLerp(InitialState.ResolveBoomArmRotation(), TargetState.ResolveBoomArmRotation(), ResultingProgress, true);
}

// Camera arm interpolation over a timeline
FRotator ASCS_CameraController::TimelineInterpolateCameraRotation(const FSCS_CameraState& InitialState, 
	const FSCS_CameraState& TargetState, float Progress, const FSCS_BlendingSettings& BlendingSettings, AActor* PlayerActor)
{
	// Default value in case no curve is specified
	float EaseInOut = 0.5f * (cos(Progress + PI) + 1);

	float ResultingProgress = EaseInOut;
	if (BlendingSettings.Location_Curve)
		ResultingProgress = BlendingSettings.Location_Curve->GetFloatValue(Progress);

	return UKismetMathLibrary::RLerp(InitialState.ResolveCameraRotation(), TargetState.ResolveCameraRotation(), ResultingProgress, true);
}



// Changes current camera profile with a transition animation
void ASCS_CameraController::SwitchCameraProfile(const FName& InProfileName)
{
	ProfileSwitchingQueue.Enqueue(InProfileName);
}

// Forces a new camera profile and stops all transition animations
void ASCS_CameraController::ForceSetCameraProfile(const FName& InProfileName)
{
	// TO DO : Implement this
}

// Returns camera profile object by it's registry name
USCS_CameraProfile* ASCS_CameraController::GetCameraProfile(const FName& InProfileName)
{
	USCS_CameraProfile** Profile = CameraProfiles.Find(InProfileName);
	if (Profile)
	{
		return *Profile;
	}
	return nullptr;
}

