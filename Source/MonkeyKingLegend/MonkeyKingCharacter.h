// Copyright 2020-2021 Jason Ioffe and Di Ioffe

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"

//#include "./PlayerStates/StatefulCharacter.h"
//#include "./FX/FOneShotEffect.h"
//#include "./Structs/AttackInfo.h"

//#include "AttackComboComponent.h"
#include "Components/AudioComponent.h"
//#include "Sound/SoundCue.h"
//#include "Particles/ParticleSystem.h"

#include "./Structs/CameraSettings.h"
#include "./PlayerStates/PlayerStateEnum.h"
#include "./PlayerStates/AttackStateEnum.h"
#include "./PlayerStates/MovementStateEnum.h"
#include "AttackComboComponent.h"

#include "MonkeyKingCharacter.generated.h"

USTRUCT(BlueprintType)
struct FSpecialAttackSettings
{
	GENERATED_BODY()

	/// <summary>
	/// How long to hold primary / secondary button before a special attack is engaged
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SpecialAttackHoldTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* SpecialReadySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* SpecialReadyMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FAttackInfo PrimarySpecialAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FAttackInfo SecondarySpecialAttack;

	FSpecialAttackSettings()
	{
		SpecialAttackHoldTime = 1.0f;
		PrimarySpecialAttack = {};
		SecondarySpecialAttack = {};
	}
};

UCLASS()
class MONKEYKINGLEGEND_API AMonkeyKingCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonkeyKingCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Input Callbacks
	void OnAxis_MoveForwardBackward(float value);
	void OnAxis_MoveLeftRight(float value);
	void OnAxis_Aim_Horizontal(float value);
	void OnAxis_Aim_Vertical(float value);
	void OnAxis_Zoom(float value);

	void OnJump_Pressed();
	void OnJump_Released();

	void OnPrimaryAttack_Pressed();
	void OnPrimaryAttack_Released();

	void OnSecondaryAttack_Pressed();
	void OnSecondaryAttack_Released();

	void OnDash_Pressed();
	void OnDash_Released();
	
	void ExecuteChargedSpecial();

	//The camera rotation has to be rebuilt every frame from Euler Angles.
	//Otherwise the additive nature would result in odd behavior
	FVector CameraAngles;
	EPlayerState CurrentPlayerState;
	EAttackState CurrentAttackState;
	EMovementState CurrentMovementState;

	FAttackInfo CurrentAttackInfo;

	bool ShouldChargeSpecial;
	float ChargingSpecialTimer;
	AActor* StaffSpecialReadyFX;

	//Used to ensure each attack only hits each enemy once
	TSet<uint32> ActorIDsStruck;

	//For dodging / sprint
	float DashHoldTimer;
	bool IsHoldingDash;
	FVector DodgeDirection;
	float RegularSpeed;

	//bool IsSprinting;
	//bool CanMove;
	//bool IsSpecialAttackReady;

	//ELastAttack LastAttack;
	//ELastAttack AttackToCharge;
	//float CurrentAttackHoldtime;

	//AActor* StaffSpecialReadyFX;

	void OnStaffStrikeWorldStatic(const FHitResult& HitResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
	void Jump() override;

	void OnAttackDamageStart();
	void OnAttackDamageEnd();

	void OnMovementBlocked();
	void OnMovementUnblocked();

	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UAudioComponent* SpeechAudio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBoxComponent* StaffHitbox;

	//Actor Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UAttackComboComponent* PrimaryAttackCombo;

	// BP Properties / settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		FCameraSettings CameraSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		float BaseStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Effects")
		USoundCue* JumpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Effects")
		USoundCue* DoubleJumpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Effects")
		UParticleSystem* DoubleJumpParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Attacks")
		FSpecialAttackSettings SpecialAttackSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks")
		FAttackInfo SecondaryAttack; //Poke them.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks")
		FAttackInfo AirPrimaryAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks")
		FAttackInfo AirSecondaryAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprinting")
		float HoldTimeForSprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprinting")
		float SprintMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodging")
		float DodgeSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodging")
		UAnimMontage* DodgeForwardMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodging")
		UAnimMontage* DodgeBackwardMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodging")
		USoundCue* DodgeSound;

	// BP Callable public functions
	UFUNCTION(BlueprintCallable, Category = "Player")
		EPlayerState GetCurrentPlayerState() { return CurrentPlayerState; }

	UFUNCTION(BlueprintCallable, Category = "Player")
		EAttackState GetCurrentAttackState() { return CurrentAttackState; }

	UFUNCTION(BlueprintCallable, Category = "Player")
		float GetPoseYawOffset();

	UFUNCTION(BlueprintCallable, Category = "Player")
		float GetPosePitchOffset();

	UFUNCTION(BlueprintCallable, Category = "Player")
		EMovementState GetCurrentMovementState() { return CurrentMovementState; }

	UFUNCTION(BlueprintCallable, Category = "Player")
		bool CanMove() { return CurrentMovementState != EMovementState::BoundByAction && CurrentMovementState != EMovementState::BoundByEffect && CurrentPlayerState != EPlayerState::Dodging; }

	UFUNCTION(BlueprintCallable, Category = "Player")
		bool CanAttack() { return CurrentMovementState != EMovementState::BoundByAction && CurrentMovementState != EMovementState::BoundByEffect && CurrentPlayerState != EPlayerState::Dodging;  }


	//Variables




	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monkey King")
	//	float SprintSpeed;

	////Blueprint read only
	//UFUNCTION(BlueprintCallable, Category = "Monkey King")
	//	bool GetIsSprinting() { return IsSprinting; }

	//UFUNCTION(BlueprintCallable, Category = "Monkey King")
	//	bool GetIsSpecialAttackReady() { return IsSpecialAttackReady; }

	//Callbacks for collisions
	UFUNCTION()
	void OnStaffHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
