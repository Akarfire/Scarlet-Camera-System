// Fill out your copyright notice in the Description page of Project Settings.


#include "SCS_FunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Attempts to find a scarlet camera controller actor in the world
// If non exists, returns nullptr
ASCS_CameraController* USCS_FunctionLibrary::GetScarletCameraController(const UObject* WorldContextObject, int PlayerIndex)
{
    TArray<AActor*> CameraControllerActors;
    UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ASCS_CameraController::StaticClass(), CameraControllerActors);

    for (auto& ControllerActor : CameraControllerActors)
    {
        ASCS_CameraController* CameraController = Cast<ASCS_CameraController>(ControllerActor);
        if (CameraController)
            if (CameraController->PlayerIndex == PlayerIndex)
                return CameraController;
    }

    return nullptr;
}

// Attempts to find a scarlet camera controller actor with a matching class
// If non exists, returns nullptr
ASCS_CameraController* USCS_FunctionLibrary::GetScarletCameraControllerOfClass(const UObject* WorldContextObject, 
    TSubclassOf<ASCS_CameraController> CameraControllerClass, bool AllowDescendants, int PlayerIndex)
{
    TArray<AActor*> CameraControllerActors;
    UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ASCS_CameraController::StaticClass(), CameraControllerActors);

    for (auto& ControllerActor : CameraControllerActors)
    {
        ASCS_CameraController* CameraController = Cast<ASCS_CameraController>(ControllerActor);
        if (CameraController)
            if(CameraController->GetClass() == CameraControllerClass || (AllowDescendants && CameraController->GetClass()->IsChildOf(CameraControllerClass)))
                if (CameraController->PlayerIndex == PlayerIndex)
                    return CameraController;
    }

    return nullptr;
}

// Attempts to find a scarlet camera controller actor in the world
// If non exists, creates a new one
ASCS_CameraController* USCS_FunctionLibrary::GetOrCreateScarletCameraController(const UObject* WorldContextObject, 
    TSubclassOf<ASCS_CameraController> CameraControllerClass, const FTransform& DefaultSpawnTransform, bool AllowDescendants, int PlayerIndex)
{
    ASCS_CameraController* ExistingController = GetScarletCameraControllerOfClass(WorldContextObject, CameraControllerClass, AllowDescendants, PlayerIndex);
    if (ExistingController)
        return ExistingController;

    ASCS_CameraController* Controller = WorldContextObject->GetWorld()->SpawnActor<ASCS_CameraController>(CameraControllerClass, DefaultSpawnTransform);
    Controller->PlayerIndex = PlayerIndex;

    return Controller;
}
