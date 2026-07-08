// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

//创建时 先广播一次，初始化界面
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
	
	//~ 遍历所有属性标签，广播当前属性值
	for (const auto& Pair : AS->TagsToAttribute)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	
	//~ 广播当前属性点数
	AAuraPlayerState* PS = CastChecked<AAuraPlayerState>(PlayerState);
	AttributePointsChangedDelegate.Broadcast(PS->GetAttributePoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()//在get里调用绑定
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfoDataAsset);
	
	//~ 绑定 Attribute 变化回调
	for (const auto& Pair : AS->TagsToAttribute)
	{
		 
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			});
	}
	
	//~ 绑定 AttributePoints 变化回调
	AAuraPlayerState* PS = CastChecked<AAuraPlayerState>(PlayerState);
	PS->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points)
			{
				AttributePointsChangedDelegate.Broadcast(Points);
			});
	
	
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	ASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
                                                            const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfoDataAsset->FindAttributeInfoForTag(AttributeTag);//创建map方便遍历
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);//主要就是 设置value
	AttributeInfoDelegate.Broadcast(Info);
}
