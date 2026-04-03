// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"


// ~-在AbilityActorInfoSet函数中，我们将EffectApplied函数绑定到OnGameplayEffectAppliedDelegateToSelf委托上，这样每当一个GameplayEffect被应用到这个AbilitySystemComponent时，EffectApplied函数就会被调用。
// ~-在EffectApplied函数中，我们从EffectSpec中获取所有的AssetTags，并通过EffectAssetTags委托广播这些标签，以便其他系统可以响应这些标签的变化。
void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
	
}

void UAuraAbilitySystemComponent::AddCharacterAbility(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (TSubclassOf<UGameplayAbility> StartupAbility : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(StartupAbility, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
		//GiveAbility();
	}
}


void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle) const
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}


