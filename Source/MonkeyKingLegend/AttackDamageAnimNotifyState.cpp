// Copyright 2020-2021 Jason Ioffe and Di Ioffe


#include "AttackDamageAnimNotifyState.h"
#include "MonkeyKingCharacter.h"

void UAttackDamageAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    if (MeshComp && MeshComp->GetOwner()) {
        AMonkeyKingCharacter* Character = Cast<AMonkeyKingCharacter>(MeshComp->GetOwner());

        if (Character)
        {
            Character->OnAttackDamageStart();
        }
    }
}
void UAttackDamageAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    //Stub
}
void UAttackDamageAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetOwner()) {
        AMonkeyKingCharacter* Character = Cast<AMonkeyKingCharacter>(MeshComp->GetOwner());

        if (Character)
        {
            Character->OnAttackDamageEnd();
        }
    }
}