// Copyright 2020-2021 Jason Ioffe and Di Ioffe


#include "MonkeyKingCharacter.h"
#include "MonkeyKingMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

static const FVector PrimaryAttackHitboxExtents				= { 80.0f, 45.0f, 45.0f };
static const FVector PrimaryAttackHitboxLocation			= { 80.0f, 0.0f, 0.0f };

static const FVector SecondaryAttackHitboxExtents			= { 410.0f, 40.0f, 40.0f };
static const FVector SecondaryAttackHitboxLocation			= { 410.0f,  0.0f,  0.0f };

static const FVector SecondarySpecialAttackHitboxExtents	= { 60.0f, 30.0f, 25.0f };
static const FVector SecondarySpecialAttackHitboxLocation	= { 0.0f, 0.0f, 0.0f };
static const float SecondarySpecialAttackHitboxRotation		= -18.0f;

static const FName StaffMid_SocketLocation				= "FX_Staff_Mid";

static const int ActorStruckSlack = 32;

// Sets default values
// Uses UMonkeyKingMovementComponent
AMonkeyKingCharacter::AMonkeyKingCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMonkeyKingMovementComponent>(ACharacter::CharacterMovementComponentName))
{

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setup State Machine
	CurrentPlayerState		= EPlayerState::CoreLocomotion;
	CurrentAttackState		= EAttackState::None;
	CurrentMovementState	= EMovementState::Normal;

	//Init components - these are accessed via blueprint
	SpringArm				= CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	Camera					= CreateDefaultSubobject<UCameraComponent>("Camera");
	SpeechAudio				= CreateDefaultSubobject<UAudioComponent>("SpeechAudio");
	StaffHitbox				= CreateDefaultSubobject<UBoxComponent>("StaffHitbox");
	PrimaryAttackCombo		= CreateDefaultSubobject<UAttackComboComponent>("PrimaryAttackCombo");

	JumpSound = nullptr;
	DoubleJumpSound = nullptr;
	DoubleJumpParticle = nullptr;

	//Setup Scene Component hierarchy
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	SpeechAudio->SetupAttachment(RootComponent);
	SpeechAudio->bAutoActivate = false;
	SpeechAudio->SetRelativeLocation({ 30.0f, 0.0f, 40.0f });
	StaffHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Use absolute rotation so the camera is independent of the character by default
	SpringArm->SetUsingAbsoluteRotation(true);
	PrimaryAttackCombo->CooldownBetweenCombos = 0.25f;

	//Most of the Wukong assets from Paragon need to be offset in this way by default
	GetCapsuleComponent()->SetCapsuleHalfHeight(100.0f);
	GetCapsuleComponent()->SetCapsuleRadius(50.0f);
	GetMesh()->SetRelativeLocation({ 5.0f, 0.0f, -100.0f });
	GetMesh()->SetRelativeRotation(FRotator::MakeFromEuler({ 0.0f, 0.0f, -90.0f }));

	//Allow the character to rotate independently
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//Allow double jump by default
	JumpMaxCount = 2;
	JumpMaxHoldTime = 0.25f;
	DodgeSpeed = 1000.0f;
	HoldTimeForSprint = 0.1f;
	SprintMovementSpeed = 1000.0f;
	//RegularSpeed = GetMovementComponent()->GetMaxSpeed();
	//SprintSpeed = 1000.0f;
	//CanMove = true;

	//Setup Euler angles for camera spin
	CameraAngles.X = 0.0f;
	CameraAngles.Y = 0.0f;
	CameraAngles.Z = 0.0f;

	ShouldChargeSpecial = false;
	ChargingSpecialTimer = 0.0f;
	StaffSpecialReadyFX = nullptr;

	BaseStrength = 30;
}

// Called when the game starts or when spawned
void AMonkeyKingCharacter::BeginPlay()
{
	Super::BeginPlay();

	ActorIDsStruck.Empty(ActorStruckSlack);
	//Attach hitbox to staff
	StaffHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, StaffMid_SocketLocation);
	StaffHitbox->OnComponentBeginOverlap.AddDynamic(this, &AMonkeyKingCharacter::OnStaffHitboxOverlap);

	//Look for FX attached to the player
	TArray<AActor*> Attachments;
	GetAttachedActors(Attachments);

	if (Attachments.Num() > 0)
	{
		for (const auto& Actor : Attachments)
		{
			if (Actor->Tags.Find("STAFF_SPECIAL_FX"))
			{
				StaffSpecialReadyFX = Actor;
				Actor->SetActorHiddenInGame(true);
				Actor->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, StaffMid_SocketLocation);
			}
		}
	}

	StaffHitbox->SetBoxExtent(PrimaryAttackHitboxExtents);
	StaffHitbox->SetRelativeLocation(PrimaryAttackHitboxLocation);

	//Set up normal speed to return to for sprinting
	RegularSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

// Called every frame
void AMonkeyKingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentPlayerState)
	{
	case EPlayerState::Dodging:
		//AddMovementInput(DodgeDirection, DodgeSpeed);
		AddActorWorldOffset(DodgeDirection * DodgeSpeed * DeltaTime, false);
		break;
	default:

		if (ShouldChargeSpecial)
		{
			ChargingSpecialTimer += DeltaTime;
			if (ChargingSpecialTimer >= SpecialAttackSettings.SpecialAttackHoldTime)
			{
				//SPECIAL ATTACK READY
				CurrentPlayerState = EPlayerState::SpecialAttackReady;
				if (SpecialAttackSettings.SpecialReadySound)
				{
					SpeechAudio->SetSound(SpecialAttackSettings.SpecialReadySound);
					SpeechAudio->Play();
				}
				if (SpecialAttackSettings.SpecialReadyMontage)
				{
					PlayAnimMontage(SpecialAttackSettings.SpecialReadyMontage);
				}
				ShouldChargeSpecial = false;
			}
		}

		if (StaffSpecialReadyFX)
		{
			StaffSpecialReadyFX->SetActorHiddenInGame(CurrentPlayerState != EPlayerState::SpecialAttackReady);
		}

		if (IsHoldingDash)
		{
			DashHoldTimer += DeltaTime;
			if (DashHoldTimer >= HoldTimeForSprint)
			{
				CurrentPlayerState = EPlayerState::Sprinting;
				GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;
			}
		}
		break;
	}
}

// Called to bind functionality to input
void AMonkeyKingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("Move_ForwardBack", this, &AMonkeyKingCharacter::OnAxis_MoveForwardBackward);
	InputComponent->BindAxis("Move_LeftRight", this, &AMonkeyKingCharacter::OnAxis_MoveLeftRight);
	InputComponent->BindAxis("Aim_Horizontal", this, &AMonkeyKingCharacter::OnAxis_Aim_Horizontal);
	InputComponent->BindAxis("Aim_Vertical", this, &AMonkeyKingCharacter::OnAxis_Aim_Vertical);
	InputComponent->BindAxis("Zoom", this, &AMonkeyKingCharacter::OnAxis_Zoom);

	InputComponent->BindAction("Jump", IE_Pressed, this, &AMonkeyKingCharacter::OnJump_Pressed);
	InputComponent->BindAction("Jump", IE_Released, this, &AMonkeyKingCharacter::OnJump_Released);

	InputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &AMonkeyKingCharacter::OnPrimaryAttack_Pressed);
	InputComponent->BindAction("PrimaryAttack", IE_Released, this, &AMonkeyKingCharacter::OnPrimaryAttack_Released);
	InputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &AMonkeyKingCharacter::OnSecondaryAttack_Pressed);
	InputComponent->BindAction("SecondaryAttack", IE_Released, this, &AMonkeyKingCharacter::OnSecondaryAttack_Released);

	InputComponent->BindAction("Dash", IE_Pressed, this, &AMonkeyKingCharacter::OnDash_Pressed);
	InputComponent->BindAction("Dash", IE_Released, this, &AMonkeyKingCharacter::OnDash_Released);
}

//Input Callbacks
void AMonkeyKingCharacter::OnAxis_MoveForwardBackward(float value)
{
	//switch (CurrentPlayerState)
	//{
	//default:
	//{
		if (!CanMove())
		{
			return;
		}

		FVector ForwardVector = Camera->GetForwardVector();
		ForwardVector.Z = 0.0f;
		ForwardVector.Normalize();

		AddMovementInput(ForwardVector, value);
		return;
	//}
	//}
}

void AMonkeyKingCharacter::OnAxis_MoveLeftRight(float value)
{
	//switch (CurrentPlayerState)
	//{
	//default:
	//{
		if (!CanMove())
		{
			return;
		}

		FVector RightVector = Camera->GetRightVector();
		RightVector.Z = 0.0f;
		RightVector.Normalize();

		AddMovementInput(RightVector, value);
		return;
	//}
	//}
}

void AMonkeyKingCharacter::OnAxis_Aim_Horizontal(float value)
{
	//switch (CurrentPlayerState)
	//{
	//default:
		CameraAngles.Z += value * CameraSettings.SensitivityX;
		FRotator Rotator = FRotator::MakeFromEuler(CameraAngles);
		SpringArm->SetRelativeRotation(Rotator);
		return;
	//}
}

void AMonkeyKingCharacter::OnAxis_Aim_Vertical(float value)
{
	//switch (CurrentPlayerState)
	//{
	//default:
		CameraAngles.Y = FMath::ClampAngle(CameraAngles.Y + value * CameraSettings.SensitivityY, -89.0f, 0.0f);
		FRotator Rotator = FRotator::MakeFromEuler(CameraAngles);
		SpringArm->SetRelativeRotation(Rotator);
		return;
	//}
}

void AMonkeyKingCharacter::OnAxis_Zoom(float value)
{
	//switch (CurrentPlayerState)
	//{
	//default:
		float TargetArmLength = SpringArm->TargetArmLength - value * CameraSettings.ZoomSensitivity;
		SpringArm->TargetArmLength = FMath::Clamp(TargetArmLength, CameraSettings.ZoomMin, CameraSettings.ZoomMax);
		return;
	//}
}

void AMonkeyKingCharacter::Jump()
{
	Super::Jump();
}

void AMonkeyKingCharacter::OnJump_Pressed()
{
	//switch (CurrentPlayerState)
	//{
	//default:
		if (!CanMove())
		{
			return;
		}

		if (!CanJump())
		{
			return;
		}

		Jump();
		switch (JumpCurrentCount)
		{
		case 1:
		case 2:
			if (DoubleJumpSound)
			{
				SpeechAudio->SetSound(DoubleJumpSound);
				SpeechAudio->Play();

				if (DoubleJumpParticle)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DoubleJumpParticle, GetActorLocation());
				}

				break;
			}

			//Fall through without double jump sound
		default:
			if (JumpSound)
			{
				SpeechAudio->SetSound(JumpSound);
				SpeechAudio->Play();
			}
			break;
		}
	//}
}

void AMonkeyKingCharacter::OnJump_Released()
{
	StopJumping();
}

void AMonkeyKingCharacter::OnPrimaryAttack_Pressed()
{
	if (!CanAttack()) 
	{
		return;
	}

	if (CurrentPlayerState == EPlayerState::Sprinting)
	{
		//Attack with the last step in the primary combo
		CurrentAttackInfo = PrimaryAttackCombo->GetLastAttackStep();
		CurrentAttackState = EAttackState::Primary;
		PlayAnimMontage(CurrentAttackInfo.AnimMontage);
		CurrentPlayerState = EPlayerState::PrimaryAttackCombo;
	}

	if (GetMovementComponent()->IsFalling())
	{
		if (AirPrimaryAttack.AnimMontage)
		{
			CurrentAttackState = EAttackState::PrimaryAir;
			CurrentAttackInfo = AirPrimaryAttack;
			PlayAnimMontage(AirPrimaryAttack.AnimMontage);

			if (JumpCurrentCount == 2)
			{
				//Bit of a cheat for the animation
				JumpCurrentCount = 3;
			}

			return;
		}
	}

	FAttackInfo PrimaryAttackStep;
	if (PrimaryAttackCombo->TryAttack(PrimaryAttackStep))
	{
		CurrentAttackState = EAttackState::Primary;
		PlayAnimMontage(PrimaryAttackStep.AnimMontage);
		CurrentAttackInfo = PrimaryAttackStep;

		ShouldChargeSpecial = true;
		ChargingSpecialTimer = 0.0f;
	}
}

void AMonkeyKingCharacter::OnPrimaryAttack_Released()
{
	switch (CurrentPlayerState)
	{
	case EPlayerState::SpecialAttackReady:
		CurrentAttackInfo = SpecialAttackSettings.PrimarySpecialAttack;
		PlayAnimMontage(CurrentAttackInfo.AnimMontage);
		CurrentPlayerState = EPlayerState::CoreLocomotion;
		CurrentAttackState = EAttackState::PrimaryBuffed;

		//Fall through
	default:
		ChargingSpecialTimer = 0.0f;
		ShouldChargeSpecial = false;
		return;
	}
}


void AMonkeyKingCharacter::OnSecondaryAttack_Pressed()
{
	if (!CanAttack())
	{
		return;
	}

	if (GetMovementComponent()->IsFalling())
	{
		if (AirSecondaryAttack.AnimMontage)
		{
			CurrentAttackState = EAttackState::SecondaryAir;
			CurrentAttackInfo = AirSecondaryAttack;
			PlayAnimMontage(AirSecondaryAttack.AnimMontage);

			if (JumpCurrentCount == 2)
			{
				//Bit of a cheat for the animation
				JumpCurrentCount = 3;
			}

			return;
		}
	}

	if (SecondaryAttack.AnimMontage)
	{
		CurrentAttackState = EAttackState::Secondary;
		CurrentAttackInfo = SecondaryAttack;
		PlayAnimMontage(SecondaryAttack.AnimMontage);

		ShouldChargeSpecial = true;
		ChargingSpecialTimer = 0.0f;
	}
}

void AMonkeyKingCharacter::OnSecondaryAttack_Released()
{
	switch (CurrentPlayerState)
	{
	case EPlayerState::SpecialAttackReady:
		CurrentAttackInfo = SpecialAttackSettings.SecondarySpecialAttack;
		PlayAnimMontage(CurrentAttackInfo.AnimMontage);
		CurrentPlayerState = EPlayerState::CoreLocomotion;
		CurrentAttackState = EAttackState::SecondaryBuffed;

		//Fall through
	default:
		ChargingSpecialTimer = 0.0f;
		ShouldChargeSpecial = false;
		return;
	}
}

void AMonkeyKingCharacter::OnDash_Pressed()
{
	switch (CurrentPlayerState)
	{
	case EPlayerState::Dodging:
		return;
	default:
		DashHoldTimer = 0.0f;
		IsHoldingDash = true;
		return;
	}
}

void AMonkeyKingCharacter::OnDash_Released()
{
	DashHoldTimer = 0.0f;
	IsHoldingDash = false;
	GetCharacterMovement()->MaxWalkSpeed = RegularSpeed;

	switch (CurrentPlayerState)
	{
	case EPlayerState::Dodging:
		return;
	case EPlayerState::Sprinting:
		CurrentPlayerState = EPlayerState::CoreLocomotion;
		return;
	default:
		if (!CanMove())
			return;

		//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Yellow, "DODGE NOW");
		DodgeDirection = GetLastMovementInputVector();

		if (FMath::IsNearlyZero(DodgeDirection.X) && FMath::IsNearlyZero(DodgeDirection.Y))
		{
			DodgeDirection = -GetActorForwardVector();
		}

		DodgeDirection.Z = 0.0f;
		DodgeDirection.Normalize();

		//Determine if we're dodging forward or backwards
		FVector FacingDirection = GetActorForwardVector();
		FacingDirection.Z = 0.0f;
		FacingDirection.Normalize();

		UAnimMontage* DodgeMontage;
		DodgeMontage = DodgeForwardMontage;
		if (FVector::DotProduct(FacingDirection, DodgeDirection) < 0.0f)
		{
			//Dodge backwards
			DodgeMontage = DodgeBackwardMontage;
			FacingDirection = -DodgeDirection;
		}
		else
		{
			//Roll forwards towards dodge direction
			DodgeMontage = DodgeForwardMontage;
			FacingDirection = DodgeDirection;
		}

		FMatrix ActorRotation = FRotationMatrix::MakeFromXZ(FacingDirection, FVector::UpVector);
		SetActorRotation(ActorRotation.Rotator());

		if (DodgeMontage)
		{
			PlayAnimMontage(DodgeMontage);
		}

		if (DodgeSound)
		{
			SpeechAudio->SetSound(DodgeSound);
			SpeechAudio->Play();
		}

		CurrentPlayerState = EPlayerState::Dodging;
		return;
	}
}

void AMonkeyKingCharacter::ExecuteChargedSpecial()
{

}

void AMonkeyKingCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode == EMovementMode::MOVE_Falling)
	{
		//JumpCount = 0;
		//
	}
}

//Anim Callbacks
void AMonkeyKingCharacter::OnAttackDamageStart()
{
	ActorIDsStruck.Empty(ActorStruckSlack);

	switch (CurrentAttackState) 
	{
		case EAttackState::Primary:
		case EAttackState::PrimaryAir:
		case EAttackState::PrimaryBuffed:
			StaffHitbox->SetBoxExtent(PrimaryAttackHitboxExtents);
			StaffHitbox->SetRelativeLocation(PrimaryAttackHitboxLocation);
			StaffHitbox->SetRelativeRotation(FRotator::MakeFromEuler(FVector::ZeroVector));
			break;
		case EAttackState::Secondary:
		case EAttackState::SecondaryAir:
			StaffHitbox->SetBoxExtent(SecondaryAttackHitboxExtents);
			StaffHitbox->SetRelativeLocation(SecondaryAttackHitboxLocation);
			StaffHitbox->SetRelativeRotation(FRotator::MakeFromEuler(FVector::ZeroVector));
			break;
		case EAttackState::SecondaryBuffed:
			StaffHitbox->SetBoxExtent(SecondarySpecialAttackHitboxExtents);
			StaffHitbox->SetRelativeLocation(SecondarySpecialAttackHitboxLocation);
			StaffHitbox->SetRelativeRotation(FRotator::MakeFromEuler({ SecondarySpecialAttackHitboxRotation, 0.0f, 0.0f }));
			break;
		default:
			StaffHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			return;
	}

	StaffHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (CurrentAttackInfo.AttackSoundCue)
	{
		SpeechAudio->SetSound(CurrentAttackInfo.AttackSoundCue);
		SpeechAudio->Play();
	}
}

void AMonkeyKingCharacter::OnAttackDamageEnd()
{
	CurrentAttackState = EAttackState::None;
	StaffHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMonkeyKingCharacter::OnMovementBlocked()
{
	switch (CurrentPlayerState)
	{
	case EPlayerState::Dodging:
		return;
	default:
		CurrentMovementState = EMovementState::BoundByAction;
		return;
	}
}

void AMonkeyKingCharacter::OnMovementUnblocked()
{
	switch (CurrentPlayerState)
	{
	case EPlayerState::Dodging:
		CurrentPlayerState = EPlayerState::CoreLocomotion;
		//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Yellow, "DODGE COMPLETE");
		//fall through
	default:
		CurrentMovementState = EMovementState::Normal;
		return;
	}
}

void AMonkeyKingCharacter::OnStaffHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CurrentAttackState == EAttackState::None)
		return;

	if (OtherActor == OverlappedComponent->GetOwner())
		return;

	bool WasActorStruckThisFrame;
	ActorIDsStruck.Emplace(OtherActor->GetUniqueID(), &WasActorStruckThisFrame);
	if (WasActorStruckThisFrame)
	{
		return;
	}

	if (OtherActor->CanBeDamaged())
	{
		float DamageAmount = BaseStrength * CurrentAttackInfo.BaseDamageMultiplier;
		FDamageEvent DamageEvent;
		OtherActor->TakeDamage(DamageAmount, DamageEvent, GetController(), this);
	}

	//Damage sparklies
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.bIgnoreTouches = false;
	FVector Start = GetMesh()->GetSocketLocation(StaffMid_SocketLocation);
	FVector End = OtherComp->GetComponentLocation();

	FVector HitLocation;
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams))
	{
		HitLocation = HitResult.Location;
	}
	else 
	{
		HitLocation = OverlappedComponent->GetComponentLocation();
	}

	FOneShotEffect ImpactEffect;
	if (Cast<APawn>(OtherActor))
	{
		ImpactEffect = CurrentAttackInfo.StrikeEnemyEffect;
	}
	else {
		ImpactEffect = CurrentAttackInfo.StrikeWorldEffect;
	}

	if (ImpactEffect.ParticleEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect.ParticleEffect, HitLocation);
	}
}

//For help with animation BP
float AMonkeyKingCharacter::GetPoseYawOffset()
{
	return SpringArm->GetComponentRotation().Yaw;
}

float AMonkeyKingCharacter::GetPosePitchOffset()
{
	return SpringArm->GetComponentRotation().Pitch;
}