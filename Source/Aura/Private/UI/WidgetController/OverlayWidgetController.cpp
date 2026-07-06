// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"


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
	//绑定 经验条 
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	AuraPlayerState->OnLevelChangedDelegate.AddLambda(
	[this](int32 NewLevel)
	{
		OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
	} );
	
	
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
	
// ~ Tags 初始化技能图标
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
		
		// 当有 Message 标签的 GameplayEffect 应用时，广播对应 UI 数据给界面
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

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	const ULevelUpInfo* LevelUpInfo = AuraPlayerState->LevelUpInfo;
	
	checkf(LevelUpInfo, TEXT("LevelUpInfo is null! Please assign a LevelUpInfo Data Asset to the PlayerState."));
	
	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num() - 1; // 因为下标0是占位，所以最大等级是数组长度减1
	
	if (Level <= MaxLevel && Level > 0)
	{
		const int32 XPForCurrentLevel = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 XPForPreviousLevel = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;
		
		const float XPPercent = (XPForCurrentLevel - XPForPreviousLevel) > 0 ? 
			static_cast<float>(NewXP - XPForPreviousLevel) / (XPForCurrentLevel - XPForPreviousLevel) : 0.0f;
		
		OnXPPercentChangedDelegate.Broadcast(XPPercent);
	}
	else
	{
		OnXPPercentChangedDelegate.Broadcast(0.0f); // 超出范围，显示满格或空格
	}
	
}


