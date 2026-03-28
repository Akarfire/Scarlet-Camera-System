// Fill out your copyright notice in the Description page of Project Settings.


#include "USCS_CameraState.h"


// Calculates location of the origin point of the boom arm
FVector FSCS_CameraState::ResolveLocation(AActor* PlayerActor) const
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
FRotator FSCS_CameraState::ResolveRotation(const FSCS_CameraRotation& Rotation, AActor* PlayerActor) const
{
	FRotator Result;

	switch (Rotation.RotationType)
	{

	case ESCS_TransformType::PlayerAttachment:
		if (PlayerActor)
			Result = PlayerActor->GetActorRotation() + Rotation.Rotation;
		else
			Result = Rotation.Rotation;
		break;

	case ESCS_TransformType::ActorAttachment:
		if (Rotation.AttachmentActor)
			Result = Rotation.AttachmentActor->GetActorRotation() + Rotation.Rotation;
		else
			Result = Rotation.Rotation;
		break;

	case ESCS_TransformType::World:
		Result = Rotation.Rotation;
		break;

	default:
		break;
	}

	return Result;
}
