// Fill out your copyright notice in the Description page of Project Settings.


#include "SCS_CameraController.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "SCS_CameraProfile.h"
#include "SCS_SimpleCameraProfile.h"


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

	// Default profile
	DefaultCameraProfile = "Default";
	SimpleCameraProfiles.Add("Default", FSCS_SimpleCameraProfileDescription());
}

// Called when the game starts or when spawned
void ASCS_CameraController::BeginPlay()
{
	Super::BeginPlay();

	// Activating view target
	if (AutoActivateViewTarget)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);
		PlayerController->SetViewTarget(this);
	}
	
	InitializeProfiles();

	// Acitavting Default Profile
	SetCameraProfile(DefaultCameraProfile, false);
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
		ProfileObject->SetupProfile(this, ProfileName);
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
		ProfileObject->SetupProfile(this, ProfileName);
		CameraProfiles.Add(ProfileName, ProfileObject);
	}
}

// UPDATE

// Called every frame
void ASCS_CameraController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateProfiles(DeltaTime);

	UpdateProfileSwitchQueue(DeltaTime);
	UpdateProfileTransitionAnimation(DeltaTime);

	if (!IsProfileTransitionAnimationPlaying())
	{
		UpdateCurrentCameraStateInterpolation();
		UpdateCurrentCameraState(DeltaTime);
	}
}

// Updates currently active profiles
void ASCS_CameraController::UpdateProfiles(float DeltaTime)
{
	GetCameraProfile(CurrentCameraProfileName)->Update(DeltaTime);

	if (IsProfileTransitionAnimationPlaying() && PreviousCameraProfile != "SCS_FROZEN_STATE")
		GetCameraProfile(PreviousCameraProfile)->Update(DeltaTime);
}

// Starts profile transition animations for enqueued switch requests
void ASCS_CameraController::UpdateProfileSwitchQueue(float DeltaTime)
{
	if (!IsProfileTransitionAnimationPlaying())
	{
		if (ProfileSwitchingQueue.IsEmpty()) return;

		PreviousCameraProfile = CurrentCameraProfileName;
		ProfileSwitchingQueue.Dequeue(CurrentCameraProfileName);

		if (GetCameraProfile(CurrentCameraProfileName)->GetBlendInSettings().AlwaysFreezePreviousState)
		{
			PreviousCameraProfile = "SCS_FROZEN_STATE";
			FreezeCurrentCameraState(FrozenState);
		}

		GetCameraProfile(CurrentCameraProfileName)->Activate();

		BlendingAnimationTime = GetCameraProfile(CurrentCameraProfileName)->GetBlendInSettings().BlendAnimationDuration + DeltaTime;
		// + DeltaTime is to compensate this ticks animation update
	}
}

// Updates profile blending animation if one is currently playing or starts a new one if one is queued
void ASCS_CameraController::UpdateProfileTransitionAnimation(float DeltaTime)
{
	if (!IsProfileTransitionAnimationPlaying())
		return;

	// Updating current animation
	const FSCS_BlendingSettings& BlendInSettings = GetCameraProfile(CurrentCameraProfileName)->GetBlendInSettings();
	float Progress = 1.f - BlendingAnimationTime / BlendInSettings.BlendAnimationDuration;

	AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(this, PlayerIndex);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);

	FSCS_CameraState InitialState;
	if (PreviousCameraProfile == "SCS_FROZEN_STATE") // Special value to use frozen state
		InitialState = FrozenState;
	else
		InitialState = GetCameraProfile(PreviousCameraProfile)->GetCameraState();

	FSCS_CameraState TargetState = GetCameraProfile(CurrentCameraProfileName)->GetCameraState();

	// State interpolation
	TimelineInterpolateCameraState(CurrentCameraState,
		InitialState, TargetState, Progress, BlendInSettings);

	FVector Location = TimelineInterpolateCameraLocation(
		InitialState,TargetState,Progress, BlendInSettings, PlayerActor);

	FRotator BoomArmRotation = TimelineInterpolateCameraArmRotation(
		InitialState, TargetState, Progress, BlendInSettings, PlayerController);

	FRotator CameraRotation = TimelineInterpolateCameraRotation(
		InitialState, TargetState, Progress, BlendInSettings, PlayerController);

	// Application
	ApplyCameraState(CurrentCameraState, false);

	SetActorLocation(Location);
	SetActorRotation(BoomArmRotation);

	Camera->SetWorldRotation(CameraRotation);

	// Time tick
	BlendingAnimationTime -= DeltaTime;

	// Ending animation
	if (BlendingAnimationTime <= 0.f)
	{
		BlendingAnimationTime = 0.f;
		if (PreviousCameraProfile != "SCS_FROZEN_STATE")
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
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);

	SetActorLocation(ProgressiveInterpolateCameraLocation(GetActorLocation(), CurrentCameraState, CurrentCameraStateInterpolation, PlayerActor, DeltaTime));
	SetActorRotation(ProgressiveInterpolateCameraArmRotation(GetActorRotation(), CurrentCameraState, CurrentCameraStateInterpolation, PlayerController, DeltaTime));

	if (CurrentCameraStateInterpolation.SeparateCameraRotation_InterpolationType != ESCS_InterpolationType::None || CurrentCameraState.EnableSeparateCameraRotation)
		Camera->SetWorldRotation(ProgressiveInterpolateCameraRotation(Camera->GetComponentRotation(), CurrentCameraState,
			CurrentCameraStateInterpolation, PlayerController, DeltaTime));

	// Separating this into a special case to avoid stuttering in rotation
	else
		Camera->SetRelativeRotation(FRotator::ZeroRotator);

}


// Applies camera state
void ASCS_CameraController::ApplyCameraState(const FSCS_CameraState& State, bool ApplyTransform)
{
	Camera->FieldOfView = State.FieldOfView;

	BoomArm->TargetArmLength = State.BoomArmLength;
	BoomArm->SocketOffset = State.CameraOffset;
	BoomArm->bDoCollisionTest = State.DoCollisionTest;

	if (ApplyTransform)
	{
		AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(this, PlayerIndex);
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);

		SetActorLocation(State.ResolveLocation(PlayerActor));
		SetActorRotation(State.ResolveBoomArmRotation(PlayerController));

		if (State.EnableSeparateCameraRotation)
			Camera->SetWorldRotation(State.ResolveCameraRotation(PlayerController));
		else
			Camera->SetRelativeRotation(FRotator::ZeroRotator);
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

	// Enable separate camera rotation
	CurrentState.EnableSeparateCameraRotation = TargetState.EnableSeparateCameraRotation;

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

	if (Interpolation.Rotation_InterpolationType == ESCS_InterpolationType::None)
		NewRotation = State.ResolveBoomArmRotation(PlayerActor);

	else if (Interpolation.Rotation_InterpolationType == ESCS_InterpolationType::Ease)
		NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation,
			State.ResolveBoomArmRotation(PlayerActor), DeltaTime,
			Interpolation.Rotation_InterpolationSpeed);

	else if (Interpolation.Rotation_InterpolationType == ESCS_InterpolationType::Linear)
		NewRotation = UKismetMathLibrary::RInterpTo_Constant(CurrentRotation,
			State.ResolveBoomArmRotation(PlayerActor), DeltaTime,
			Interpolation.Rotation_InterpolationSpeed);

	return NewRotation;
}

// Camera arm interpolation over time (not for timelines)
FRotator ASCS_CameraController::ProgressiveInterpolateCameraRotation(const FRotator& CurrentRotation, const FSCS_CameraState& State,
	const FSCS_CameraStateInterpolation& Interpolation, AActor* PlayerActor, float DeltaTime)
{
	FRotator NewRotation;

	FRotator TargetRotation;
	if (State.EnableSeparateCameraRotation)
		TargetRotation = State.ResolveCameraRotation(PlayerActor);
	else
		TargetRotation = State.ResolveBoomArmRotation(PlayerActor);

	if (Interpolation.SeparateCameraRotation_InterpolationType == ESCS_InterpolationType::None)
		NewRotation = TargetRotation;

	if (Interpolation.SeparateCameraRotation_InterpolationType == ESCS_InterpolationType::Ease)
		NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation,
			TargetRotation, DeltaTime,
			Interpolation.SeparateCameraRotation_InterpolationSpeed);

	else if (Interpolation.SeparateCameraRotation_InterpolationType == ESCS_InterpolationType::Linear)
		NewRotation = UKismetMathLibrary::RInterpTo_Constant(CurrentRotation,
			TargetRotation, DeltaTime,
			Interpolation.SeparateCameraRotation_InterpolationSpeed);

	return NewRotation;
}



// TIMELINE INTERPOLATION

// Camera state interpolation over a timeline
void ASCS_CameraController::TimelineInterpolateCameraState(FSCS_CameraState& OutState, const FSCS_CameraState& InitialState, 
	const FSCS_CameraState& TargetState, float Progress, const FSCS_BlendingSettings& BlendingSettings)
{
	// Default value in case no curve is specified
	float EaseInOut = 0.5f * (cos(PI * Progress + PI) + 1);

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
	float EaseInOut = 0.5f * (cos(PI * Progress + PI) + 1);

	float ResultingProgress = EaseInOut;
	if (BlendingSettings.Location_Curve)
		ResultingProgress = BlendingSettings.Location_Curve->GetFloatValue(Progress);
	
	return UKismetMathLibrary::VLerp(InitialState.ResolveLocation(PlayerActor), TargetState.ResolveLocation(PlayerActor), ResultingProgress);
}

// Camera arm rotation interpolation over a timeline
FRotator ASCS_CameraController::TimelineInterpolateCameraArmRotation(const FSCS_CameraState& InitialState, 
	const FSCS_CameraState& TargetState, float Progress, const FSCS_BlendingSettings& BlendingSettings, AActor* PlayerActor)
{
	// Default value in case no curve is specified
	float EaseInOut = 0.5f * (cos(PI * Progress + PI) + 1);

	float ResultingProgress = EaseInOut;
	if (BlendingSettings.Rotation_Curve)
		ResultingProgress = BlendingSettings.Rotation_Curve->GetFloatValue(Progress);

	return UKismetMathLibrary::RLerp(InitialState.ResolveBoomArmRotation(PlayerActor), TargetState.ResolveBoomArmRotation(PlayerActor), ResultingProgress, true);
}

// Camera arm interpolation over a timeline
FRotator ASCS_CameraController::TimelineInterpolateCameraRotation(const FSCS_CameraState& InitialState, 
	const FSCS_CameraState& TargetState, float Progress, const FSCS_BlendingSettings& BlendingSettings, AActor* PlayerActor)
{
	// Default value in case no curve is specified
	float EaseInOut = 0.5f * (cos(PI * Progress + PI) + 1);

	float ResultingProgress = EaseInOut;
	if (BlendingSettings.SeparateCameraRotation_Curve)
		ResultingProgress = BlendingSettings.SeparateCameraRotation_Curve->GetFloatValue(Progress);

	FRotator InitialRotation;
	if (InitialState.EnableSeparateCameraRotation)
		InitialRotation = InitialState.ResolveCameraRotation(PlayerActor);
	else
		InitialRotation = InitialState.ResolveBoomArmRotation(PlayerActor);

	FRotator TargetRotation;
	if (TargetState.EnableSeparateCameraRotation)
		InitialRotation = TargetState.ResolveCameraRotation(PlayerActor);
	else
		InitialRotation = TargetState.ResolveBoomArmRotation(PlayerActor);

	return UKismetMathLibrary::RLerp(InitialRotation, InitialRotation, ResultingProgress, true);
}


// Stops any currently playing animation
bool ASCS_CameraController::InterruptProfileTransitionAnimation()
{
	bool WasPlaying = IsProfileTransitionAnimationPlaying();
	if (WasPlaying && PreviousCameraProfile != "SCS_FROZEN_STATE")
		GetCameraProfile(PreviousCameraProfile)->Deactivate();

	BlendingAnimationTime = 0.0f;

	return WasPlaying;
}

// Freezes current state, removing dynamically resolved elements from it
void ASCS_CameraController::FreezeCurrentCameraState(FSCS_CameraState& OutFrozenState)
{
	OutFrozenState = CurrentCameraState;

	AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(this, PlayerIndex);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);

	FVector Location = CurrentCameraState.ResolveLocation(PlayerActor);
	FRotator ArmRotation = CurrentCameraState.ResolveBoomArmRotation(PlayerController);
	FRotator CameraRotation = CurrentCameraState.ResolveCameraRotation(PlayerController);

	OutFrozenState.CameraLocation.LocationType = ESCS_TransformType::World;
	OutFrozenState.CameraLocation.Location = Location;

	OutFrozenState.CameraArmRotation.RotationType = ESCS_TransformType::World;
	OutFrozenState.CameraArmRotation.Rotation = ArmRotation;

	OutFrozenState.SeparateCameraRotation.RotationType = ESCS_TransformType::World;
	OutFrozenState.SeparateCameraRotation.Rotation = CameraRotation;
}



// Changes current camera profile with a transition animation
void ASCS_CameraController::SetCameraProfile(const FName& InProfileName, bool TransitionAnimation, bool Queue)
{
	if (!CameraProfiles.Contains(InProfileName))
	{
		UE_LOG(LogTemp, Error, TEXT("SCARLET CAMERA SYSTEM : Invalid camera profile name passed into `SwitchCameraProfile(...)`"));
		return;
	}

	if (Queue && TransitionAnimation)
		ProfileSwitchingQueue.Enqueue(InProfileName);

	else if (TransitionAnimation)
	{
		// Interrupting Animation
		bool WasAnimationPlaying = InterruptProfileTransitionAnimation();

		// Empty the queue
		ProfileSwitchingQueue.Empty();

		// Setting up previous camera profile (or a special value)
		if (WasAnimationPlaying || GetCameraProfile(InProfileName)->GetBlendInSettings().AlwaysFreezePreviousState)
		{
			PreviousCameraProfile = "SCS_FROZEN_STATE";

			// Freezing the state
			FreezeCurrentCameraState(FrozenState);
		}
		else
		{
			PreviousCameraProfile = CurrentCameraProfileName;
		}

		// Start the transition animation
		CurrentCameraProfileName = InProfileName;
		GetCameraProfile(CurrentCameraProfileName)->Activate();

		BlendingAnimationTime = GetCameraProfile(CurrentCameraProfileName)->GetBlendInSettings().BlendAnimationDuration;
	}

	else
	{
		USCS_CameraProfile* CurrentProfile = GetCameraProfile(CurrentCameraProfileName);
		if (CurrentProfile)
			CurrentProfile->Deactivate();

		// Interrupting Animation
		InterruptProfileTransitionAnimation();

		// Empty the queue
		ProfileSwitchingQueue.Empty();

		CurrentCameraProfileName = InProfileName;
		GetCameraProfile(CurrentCameraProfileName)->Activate();

		ApplyCameraState(CurrentCameraState, true);
	}
}

// Returns camera profile object by it's registry name
// Returns `nullptr` if no such profile was found
USCS_CameraProfile* ASCS_CameraController::GetCameraProfile(const FName& InProfileName)
{
	USCS_CameraProfile** Profile = CameraProfiles.Find(InProfileName);
	if (Profile)
	{
		return *Profile;
	}
	return nullptr;
}


// Registers a new simple camera profile
// Returns `true` if the profile was succesfully added, `false` if profile with this name is already registered
bool ASCS_CameraController::AddSimpleCameraProfile(const FName& InProfileName, const FSCS_SimpleCameraProfileDescription& InSimpleProfile)
{
	if (CameraProfiles.Contains(InProfileName))
		return false;

	// Registering descriptor
	SimpleCameraProfiles.Add(InProfileName, InSimpleProfile);
	
	// Creating profile object
	USCS_SimpleCameraProfile* ProfileObject = NewObject<USCS_SimpleCameraProfile>(this, USCS_SimpleCameraProfile::StaticClass());
	ProfileObject->SetupProfile(this, InProfileName);
	CameraProfiles.Add(InProfileName, ProfileObject);

	return true;
}

// Registers a new custom camera profile
// Returns `true` if the profile was succesfully added, `false` if profile with this name is already registered
bool ASCS_CameraController::AddCustomCameraProfile(const FName& InProfileName, TSubclassOf<class USCS_CameraProfile> InCustomProfileClass)
{
	if (CameraProfiles.Contains(InProfileName))
		return false;

	USCS_CameraProfile* ProfileObject = NewObject<USCS_CameraProfile>(this, InCustomProfileClass);
	ProfileObject->SetupProfile(this, InProfileName);
	CameraProfiles.Add(InProfileName, ProfileObject);

	return true;
}

// Registers an alredy existing custom camera profile
// Returns `true` if the profile was succesfully added, `false` if profile with this name is already registered
bool ASCS_CameraController::AddCustomCameraProfileExisting(const FName& InProfileName, USCS_CameraProfile* InCustomProfile)
{
	if (CameraProfiles.Contains(InProfileName))
		return false;

	InCustomProfile->SetupProfile(this, InProfileName);
	CameraProfiles.Add(InProfileName, InCustomProfile);

	return true;
}


// UTILITY

// Complete camera state interpolation between A and B
void ASCS_CameraController::InterpolateCameraState(FSCS_CameraState& OutState, const FSCS_CameraState& InitialState, const FSCS_CameraState& TargetState, float Progress, const FSCS_BlendingSettings& BlendingSettings)
{
	TimelineInterpolateCameraState(OutState, InitialState, TargetState, Progress, BlendingSettings);

	OutState.CameraLocation.LocationType = ESCS_TransformType::World;
	OutState.CameraArmRotation.RotationType = ESCS_TransformType::World;
	OutState.SeparateCameraRotation.RotationType = ESCS_TransformType::World;

	AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(this, PlayerIndex);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);

	OutState.CameraLocation.Location = TimelineInterpolateCameraLocation(InitialState, TargetState, Progress, BlendingSettings, PlayerActor);
	OutState.CameraArmRotation.Rotation = TimelineInterpolateCameraArmRotation(InitialState, TargetState, Progress, BlendingSettings, PlayerController);
	OutState.SeparateCameraRotation.Rotation = TimelineInterpolateCameraRotation(InitialState, TargetState, Progress, BlendingSettings, PlayerController);
}

