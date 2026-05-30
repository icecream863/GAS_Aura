// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

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
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		/*
		 * 1.如果广播先触发，后绑定，那么在绑定之前发生的事件将不会被监听到，因为当时没有任何函数绑定到委托上来接收这些事件。
		 * 2.如果绑定先触发，后广播，那么在广播事件时，已经有函数绑定到委托上了，所以这些函数能够正确地接收和处理广播的事件。
		 *  时机触发不确定，要做 保险操作
		 */
		if (AuraASC->bStartupAbilitiesGiven)
		{
			OnInitializeStartupAbilities(AuraASC);
			//~ 如果广播已触发，直接回调
		}
		else
		{
			AuraASC->AbilityGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
			//~ 如果广播未触发，绑定回调，等广播触发时回调被调用
		}
		
		
		AuraASC->EffectAssetTagsDelegate.AddLambda(
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
		
	
	
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC)
{
	if (!AuraASC->bStartupAbilitiesGiven) return;
	
	FForEachAbility BroadcastDelegate; //绑定，再广播
	BroadcastDelegate.BindLambda(
	[this, AuraASC](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AuraASC->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = AuraASC->GetInputTagFromSpec(AbilitySpec);//没有在数据表里设置，需要自己在c++获取
		AbilityInfoDelegate.Broadcast(Info);
	} );
	
	AuraASC->ForEachAbility(BroadcastDelegate);
	// ~ 遍历所有能力，广播给界面
	// AbilityGivenDelegate(AuraASC) -->  每个技能 BroadcastDelegate(AbilitySpec) --> AbilityInfoDelegate(Info)
}


