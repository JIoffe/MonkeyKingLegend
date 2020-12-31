// Copyright 2020-2021 Jason Ioffe and Di Ioffe


#include "MovementBlockedAnimNotifyState.h"
#include "MonkeyKingCharacter.h"

void UMovementBlockedAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    if (MeshComp && MeshComp->GetOwner()) {
        AMonkeyKingCharacter* Character = Cast<AMonkeyKingCharacter>(MeshComp->GetOwner());

        if (Character)
        {
            Character->OnMovementBlocked();
        }
    }
}
void UMovementBlockedAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    //Stub
}
void UMovementBlockedAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    //GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Yellow, __FUNCTION__);
    if (MeshComp && MeshComp->GetOwner()) {
        AMonkeyKingCharacter* Character = Cast<AMonkeyKingCharacter>(MeshComp->GetOwner());

        if (Character)
        {
            Character->OnMovementUnblocked();
        }
    }
}
