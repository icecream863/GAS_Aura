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
	
public:
	/** 初始化 ASC 的 ActorInfo，并注册 GameplayEffect 应用回调（用于后续在客户端广播效果标签等）。 */
	void AbilityActorInfoSet();
	
	/** 
	 * 当有 GameplayEffect 应用到自身时，把该效果的 Asset Tags 通过此委托广播出去。
	 * 常用于 UI\(/\)提示\(/\)音效等监听“发生了带哪些标签的效果”。
	 */
	FEffectAssetTags EffectAssetTags;
	
	/**
	 * 输入标签被按住时触发：遍历可激活能力，找到匹配该输入标签的能力并通知 GAS 输入按下，
	 * 必要时尝试激活该能力
	 */
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	
	/**
	 * 输入标签被松开时触发：遍历可激活能力，找到匹配该输入标签的能力并通知 GAS 输入松开。
	 */
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	/**
	 * 授予角色起始能力：把 StartupAbilities 里的能力创建为 Spec 并 GiveAbility，
	 * 同时把能力的输入标签写入 DynamicAbilityTags，便于通过输入标签驱动释放。
	 */
	void AddCharacterAbility(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	
	/** 只在 Held 逻辑里调用 TryActivateAbility 确实是核心逻辑所在
	* 1. 为什么要放在 Held 而不是 Pressed？
	在 GAS 处理输入时，将激活逻辑放在 Held 主要是为了支持自动施法/连发或者蓄力技能。

	按下即触发： 当你按下按键的第一帧，Held 事件就会触发。由于此时技能尚未激活，!AbilitySpec.IsActive() 为真，技能会立即启动。

	按住连续触发： 对于那种“按住就一直放”的技能（比如法师的平 A 或持续施法），
	如果技能结束了，但你还没松手，Held 会在下一帧检测到 !IsActive()，从而再次自动调用 TryActivateAbility。
	*/
	
};
