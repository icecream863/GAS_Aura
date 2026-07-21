// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DebuffNiagaraComponent.generated.h"

class UAbilitySystemComponent;

/** Niagara component whose activation follows an owner's debuff GameplayTag. */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AURA_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	UDebuffNiagaraComponent();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Debuff")
	FGameplayTag DebuffTag;

protected:
	void RegisterWithASC(UAbilitySystemComponent* ASC);
	void DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);
};
