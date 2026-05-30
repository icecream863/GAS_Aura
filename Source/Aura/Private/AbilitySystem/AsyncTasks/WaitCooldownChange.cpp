 // Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"

#include "AbilitySystemComponent.h"

 UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
                                                                 const FGameplayTag& InCooldownTag)
{
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->ASC = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;
	
	if (!AbilitySystemComponent || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}
	
 	//~ 注册监听，当冷却标签的数量发生变化时，调用CooldownTagChanged函数
	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(WaitCooldownChange, &UWaitCooldownChange::CooldownTagChanged);
 	
 	return WaitCooldownChange;
}

 void UWaitCooldownChange::EndTask()
 {
	if (!ASC) return;
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	
 	//~ 标记这个任务对象为准备销毁，并将其标记为垃圾，以便引擎在适当的时候清理它，释放资源。
 	SetReadyToDestroy();
 	MarkAsGarbage();
 	
 }

 void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
 {
	if (NewCount == 0)//~ 这个标签被移除了 也就是冷却结束了
	{
		CooldownEnd.Broadcast(0.f);
	}
 	else
	{
 		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(CooldownTag.GetSingleTagContainer());
 		TArray<float> TimeRemainingArray = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);

 		if (TimeRemainingArray.Num() > 0)
 		{
 			const float TimeRemaining = FMath::Max(TimeRemainingArray);

 			// ~ 这里的逻辑是 如果有多个 cooldown effect 叠加了 那么以剩余时间最长的那个为准（一般 只有 1 个标签）
 			CooldownStart.Broadcast(TimeRemaining);
 		}
	
	}
 }


