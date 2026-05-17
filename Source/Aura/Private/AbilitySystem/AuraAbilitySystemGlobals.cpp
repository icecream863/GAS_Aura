// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemGlobals.h"

#include "AuraAbilityTypes.h"

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	/**
	 * 这里返回我们自定义的 EffectContext。
	 *
	 * 只要项目在 DefaultGame.ini 中把 AbilitySystemGlobalsClassName 指向 UAuraAbilitySystemGlobals，
	 * 后续 GAS 创建的 EffectContext 默认都会是 FAuraGameplayEffectContext。
	 */
	return new FAuraGameplayEffectContext();
}
