// Fill out your copyright notice in the Description page of Project Settings.?


#include "AuraAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine)
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	
	return *AuraAssetManager; 
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	
	// ~-在游戏开始时，初始化 GameplayTags
	FAuraGameplayTags::InitializeNativeGameplayTags();
	/** 初始化 Unreal Gameplay Ability System(GAS) 的全局数据:
	* - 注册/加载 AbilitySystem 相关的全局配置与单例数据(由 UAbilitySystemGlobals 管理)
	* - 预加载/缓存全局资源(如 GameplayCue、属性/曲线表等，取决于项目配置)
	* - 确保后续 AbilitySystemComponent、GameplayEffect 等功能可正常使用
	*/
	UAbilitySystemGlobals::Get().InitGlobalData();
}
