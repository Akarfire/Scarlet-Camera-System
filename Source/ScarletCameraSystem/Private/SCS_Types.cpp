// Fill out your copyright notice in the Description page of Project Settings.

#include "SCS_Types.h"


// Calculates location of the origin point of the boom arm
FVector FSCS_CameraState::ResolveLocation(AActor* PlayerActor) const
{
	switch (CameraLocation.LocationType)
	{

	case ESCS_TransformType::PlayerAttachment:
		if (PlayerActor)
			return PlayerActor->GetActorLocation() + PlayerActor->GetActorRotation().RotateVector(CameraLocation.Location);
		else
			return FVector::ZeroVector;

	case ESCS_TransformType::ActorAttachment:
		if (CameraLocation.AttachmentActor)
			return CameraLocation.AttachmentActor->GetActorLocation() + CameraLocation.AttachmentActor->GetActorRotation().RotateVector(CameraLocation.Location);
		else
			return FVector::ZeroVector;

	case ESCS_TransformType::World:
		return CameraLocation.Location;

	default:
		return FVector::ZeroVector;
	}
}

// Calculates rotation of the boom arm
FRotator FSCS_CameraState::ResolveRotation(const FSCS_CameraRotation& Rotation, AActor* PlayerActor) const
{
	switch (Rotation.RotationType)
	{

	case ESCS_TransformType::PlayerAttachment:
		if (PlayerActor)
			return PlayerActor->GetActorRotation() + Rotation.Rotation;
		else
			return FRotator::ZeroRotator;

	case ESCS_TransformType::ActorAttachment:
		if (Rotation.AttachmentActor)
			return Rotation.AttachmentActor->GetActorRotation() + Rotation.Rotation;
		else
			return FRotator::ZeroRotator;

	case ESCS_TransformType::World:
		return Rotation.Rotation;

	default:
		return FRotator::ZeroRotator;
	}
}
