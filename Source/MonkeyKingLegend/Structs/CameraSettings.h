// Copyright 2020-2021 Jason Ioffe and Di Ioffe

#pragma once

#include "UObject/Class.h"
#include "CameraSettings.generated.h"

USTRUCT(BlueprintType)
struct FCameraSettings
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SensitivityX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SensitivityY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ZoomMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ZoomMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ZoomSensitivity;

	FCameraSettings()
	{
		SensitivityX = 20.0f;
		SensitivityY = 20.0f;
		ZoomMin = 200.0f;
		ZoomMax = 700.0f;
		ZoomSensitivity = 80.0f;
	}
};