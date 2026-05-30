// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()//在蓝图创建 UI 时调用， 把当前属性值广播给界面
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfoDataAsset);
	
	/**
	* FAuraAttributeInfo Info = AttributeInfoDataAsset->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength);
	* Info.AttributeValue = AS->GetStrength();
	* AttributeInfoDelegate.Broadcast(Info);
	* 这是存委托的写法
	*/
	
	for (const auto& Pair : AS->TagsToAttribute)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	
	
	
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()//在get里调用绑定
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfoDataAsset);
	
	for (const auto& Pair : AS->TagsToAttribute)
	{
		 
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			});
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfoDataAsset->FindAttributeInfoForTag(AttributeTag);//创建map方便遍历
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);//主要就是 设置value
	AttributeInfoDelegate.Broadcast(Info);
}
