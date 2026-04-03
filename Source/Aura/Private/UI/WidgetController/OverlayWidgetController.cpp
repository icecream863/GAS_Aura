// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"


#include "IPropertyTable.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{	
	// 不需要 继承父类函数
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()//依赖是 AuraAttribute底层
{
	// Super::BindCallbacksToDependencies(); 不需要继承父类函数，只需重载
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	//监测 AttributeValue 变化， 触发回调
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
		{		
			//~ 监听到属性变化， 就广播给界面
			OnHealthChanged.Broadcast(Data.NewValue);
		}
		);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{		
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
		);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{		
			OnManaChanged.Broadcast(Data.NewValue);
		}
		);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{		
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
		);
	
// ~ Tags
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
	[this](FGameplayTagContainer& TagContainer)
		{
			//~ 监听到标签变化， 就去datatable里找对应标签的UI数据，广播给界面
			//~ 筛选标签，找出符合条件的标签（比如说包含 "Message" 标签的标签），然后根据这些标签去 DataTable 里找对应的 UI 数据，最后把这些数据广播给界面。
			FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
		
			for (const FGameplayTag& Tag : TagContainer)
			{
				if (Tag.MatchesTag(MessageTag))//"A.1".MatchesTag("A") will return True, "A".MatchesTag("A.1") will return False
				{
					FUIWidgetRow* Row =  GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
				
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		});
}


