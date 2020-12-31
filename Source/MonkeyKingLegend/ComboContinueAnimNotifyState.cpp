// Copyright 2020-2021 Jason Ioffe and Di Ioffe


#include "ComboContinueAnimNotifyState.h"
#include "GameFramework/Actor.h"
#include "AttackComboComponent.h"
#include "MonkeyKingCharacter.h"

void UComboContinueAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    if (MeshComp && MeshComp->GetOwner()) {
        AActor* Actor = MeshComp->GetOwner();
        TArray<UActorComponent*> AttackComboComponents = Actor->GetComponentsByClass(UAttackComboComponent::StaticClass());

        if (AttackComboComponents.Num() == 0)
            return;

        for (auto& component : AttackComboComponents)
        {
            UAttackComboComponent* AttackComboComponent = Cast<UAttackComboComponent>(component);
            AttackComboComponent->OnNextStepReady();
        }
    }
}
void UComboContinueAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    //Stub
}
void UComboContinueAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetOwner()) {
        AActor* Actor = MeshComp->GetOwner();
        TArray<UActorComponent*> AttackComboComponents = Actor->GetComponentsByClass(UAttackComboComponent::StaticClass());

        if (AttackComboComponents.Num() == 0)
            return;

        for (auto& component : AttackComboComponents)
        {
            UAttackComboComponent* AttackComboComponent = Cast<UAttackComboComponent>(component);
            AttackComboComponent->OnComboReset();
        }
    }
}