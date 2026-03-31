// Fill out your copyright notice in the Description page of Project Settings.

#include "SCS_CameraProfile.h"

#include "Camera/CameraComponent.h"
#include "SCS_CameraController.h"

// Returns a pointer to the associated camera
UCameraComponent* USCS_CameraProfile::GetCamera()
{
	if (CameraController)
		return CameraController->GetCamera();

	return nullptr;
}