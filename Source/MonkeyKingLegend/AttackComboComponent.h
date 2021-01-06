// Copyright 2020-2021 Jason Ioffe and Di Ioffe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "./Structs/AttackInfo.h"
#include "Sound/SoundCue.h"
#include "AttackComboComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MONKEYKINGLEGEND_API UAttackComboComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackComboComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	float ComboCooldownTimer;

	int CurrentAttackIndex;
	int PendingAttackSteps;
	bool CanAdvanceCombo;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Combo")
		float CooldownBetweenCombos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Combo")
		TArray<FAttackInfo> ComboAttacks;

	UFUNCTION(BlueprintCallable, Category = "Attack Combo")
		virtual void OnNextStepReady();

	UFUNCTION(BlueprintCallable, Category = "Attack Combo")
		virtual void OnComboReset();

	UFUNCTION(BlueprintCallable, Category = "Attack Combo")
		virtual int GetCurrentAttackIndex();

	UFUNCTION(BlueprintCallable, Category = "Attack Combo")
		virtual int GetNumberOfStepsInCombo();

	UFUNCTION(BlueprintCallable, Category = "Attack Combo")
		virtual FAttackInfo GetCurrentAttackStep();

	UFUNCTION(BlueprintCallable, Category = "Attack Combo")
		virtual FAttackInfo GetLastAttackStep();

	UFUNCTION(BlueprintCallable, Category = "Attack Combo")
		virtual bool IsInCombo();

	virtual bool TryAttack(FAttackInfo& AttackStepOutput);
};
