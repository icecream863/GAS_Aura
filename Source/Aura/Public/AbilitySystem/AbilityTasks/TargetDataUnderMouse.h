// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"
// 鼠标下目标数据任务：定义动态委托，用于回传命中的目标位置数据
//“可蓝图+可多播+1 个参数”的动态委托声明
// 在广播时会把一个 FVector 传给所有绑定函数
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

/**
 * 
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
 	GENERATED_BODY()
	
public:
	/*HidePin = "OwningAbility"：把参数 OwningAbility 在蓝图节点上隐藏，不让你手动接线。
	DefaultToSelf = "OwningAbility"：即使隐藏了，也自动把当前上下文对象（Self）作为这个参数传进去。
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability|Task", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", display = "TargetDataUnderMouse"))//课程里display name改了名字
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);
	
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;
protected:
	//Activate() 是 UAbilityTask 的生命周期回调，在任务进入激活阶段时调用。
	virtual void Activate() override;//技能的 ActivateAbility 负责“开技能”，Task 的 Activate 负责“开这个子任务”。
	
private:
	
	
	void SendMouseCursorData();
	
	void OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivatedTag);
};
