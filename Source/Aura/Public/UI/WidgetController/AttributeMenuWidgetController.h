// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayAttribute;
struct FGameplayTag;
class UAttributeInfo;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeInfoSignature, const FAuraAttributeInfo&, Info);

/**
 * 
 */

//~ Blueprintable 允许在蓝图里创建这个类的子类， BlueprintType 允许在蓝图里使用这个类作为变量类型
UCLASS(Blueprintable, BlueprintType)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	
	// 初始化 UI：把当前属性值广播给界面（进入关卡/创建 UI 时用）
	virtual void BroadcastInitialValues() override;
	
	// 绑定 ASC/属性变化回调（把属性变化转为 UI 事件）
	virtual void BindCallbacksToDependencies() override;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")//~ BlueprintAssignable 允许在蓝图里绑定这个事件
	FOnAttributeInfoSignature AttributeInfoDelegate;//~ 当属性信息发生变化时广播，界面可以绑定这个事件来更新显示
	 
	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnPlayerStatChangedSignature AttributePointsChangedDelegate;
	
	UFUNCTION(BlueprintCallable)
	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfoDataAsset;//~ 存储属性信息的 DataAsset，包含属性标签、名称、描述等信息
	
private:
	
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
};
