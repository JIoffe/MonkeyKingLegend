// Copyright 2020-2021 Jason Ioffe and Di Ioffe


#include "AttackComboComponent.h"

// Sets default values for this component's properties
UAttackComboComponent::UAttackComboComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ComboCooldownTimer = CooldownBetweenCombos;
	CurrentAttackIndex = 0;
	CanAdvanceCombo = true;
	PendingAttackSteps = 0;
}


// Called when the game starts
void UAttackComboComponent::BeginPlay()
{
	Super::BeginPlay();

	//Ready right away!
	ComboCooldownTimer = CooldownBetweenCombos;
}


// Called every frame
void UAttackComboComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ComboCooldownTimer < CooldownBetweenCombos)
	{
		ComboCooldownTimer += DeltaTime;
	}
}

void UAttackComboComponent::OnNextStepReady()
{
	CanAdvanceCombo = true;
}

void UAttackComboComponent::OnComboReset()
{
	if (--PendingAttackSteps > 0)
		return;

	PendingAttackSteps = 0;
	CurrentAttackIndex = 0;
	ComboCooldownTimer = 0.0f;
	CanAdvanceCombo = true;
}

bool UAttackComboComponent::TryAttack(FAttackInfo& AttackStepOutput)
{
	if (!CanAdvanceCombo)
	{
		return false;
	}

	if (ComboCooldownTimer < CooldownBetweenCombos)
	{
		return false;
	}

	if (CurrentAttackIndex >= GetNumberOfStepsInCombo())
	{
		return false;
	}

	AttackStepOutput = ComboAttacks[CurrentAttackIndex++];
	CanAdvanceCombo = false;
	++PendingAttackSteps;

	return true;
}

int UAttackComboComponent::GetCurrentAttackIndex()
{
	return CurrentAttackIndex;
}

bool UAttackComboComponent::IsInCombo()
{
	return CurrentAttackIndex != 0;
}

FAttackInfo UAttackComboComponent::GetCurrentAttackStep()
{
	return ComboAttacks[CurrentAttackIndex - 1];
}

int UAttackComboComponent::GetNumberOfStepsInCombo()
{
	return ComboAttacks.Num();
}

FAttackInfo UAttackComboComponent::GetLastAttackStep()
{
	return ComboAttacks[ComboAttacks.Num() - 1];
}