// Fill out your copyright notice in the Description page of Project Settings.


#include "USCS_CameraState.h"

#include "Kismet/GameplayStatics.h"

// Calculates location of the origin point of the boom arm
FVector FSCS_CameraState::ResolveLocation(AActor* PlayerActor)
{
    FVector Result;

	switch (CameraLocation.LocationType)
	{

	case ESCS_TransformType::PlayerAttachment:
		if (PlayerActor)
			Result = PlayerActor->GetActorLocation() + PlayerActor->GetActorRotation().RotateVector(CameraLocation.Location);
		else
			Result = CameraLocation.Location;
		break;

	case ESCS_TransformType::ActorAttachment:
		if (CameraLocation.AttachmentActor)
			Result = CameraLocation.AttachmentActor->GetActorLocation() + CameraLocation.AttachmentActor->GetActorRotation().RotateVector(CameraLocation.Location);
		else
			Result = CameraLocation.Location;
		break;

	case ESCS_TransformType::World:
		Result = CameraLocation.Location;
		break;

	default:
		break;
	}

    return Result;
}

// Calculates rotation of the boom arm
FRotator FSCS_CameraState::ResolveRotation(AActor* PlayerActor)
{
	FRotator Result;

	switch (CameraRotation.RotationType)
	{

	case ESCS_TransformType::PlayerAttachment:
		if (PlayerActor)
			Result = PlayerActor->GetActorRotation() + CameraRotation.Rotation;
		else
			Result = CameraRotation.Rotation;
		break;

	case ESCS_TransformType::ActorAttachment:
		if (CameraRotation.AttachmentActor)
			Result = CameraRotation.AttachmentActor->GetActorRotation() + CameraRotation.Rotation;
		else
			Result = CameraRotation.Rotation;
		break;

	case ESCS_TransformType::World:
		Result = CameraRotation.Rotation;
		break;

	default:
		break;
	}

	return Result;
}
