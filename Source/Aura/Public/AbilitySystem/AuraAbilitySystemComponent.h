// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, FGameplayTagContainer& /*AssetTagContainer*/);

/**
 * 
 */


UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
protected:
	
	UFUNCTION(Client, Reliable) // ~-Client: 这个函数将在客户端执行，服务器调用这个函数时，函数体内的代码会在客户端运行。
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle) const;
	
	
public:
	void AbilityActorInfoSet();
	
	FEffectAssetTags EffectAssetTags;//获取标签的委托
	
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	void AddCharacterAbility(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
};
