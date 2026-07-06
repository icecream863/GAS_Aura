// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "GameplayEffectTypes.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"


UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();	
			UAttributeSet* AS = PS->GetAttributeSet();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();

			FWidgetControllerParams WidgetControllerParams = FWidgetControllerParams(PS, PC, ASC, AS);
			
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();	
			UAttributeSet* AS = PS->GetAttributeSet();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();

			FWidgetControllerParams WidgetControllerParams = FWidgetControllerParams(PS, PC, ASC, AS);
			
			return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return;//客户端没有 GameMode,在客户端执行就会返回 空指针
	
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
	
	if (ClassDefaultInfo.PrimaryAttribute)
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(WorldContextObject);
		
		FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttribute, Level, EffectContext);
		if (PrimaryAttributesSpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());
		}
		
		FGameplayEffectSpecHandle SecdaryAttibutesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttribute, Level, EffectContext);
		if (SecdaryAttibutesSpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SecdaryAttibutesSpecHandle.Data.Get());
		}
		
		FGameplayEffectSpecHandle VitalAttibutesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttribute, Level, EffectContext);
		if (VitalAttibutesSpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*VitalAttibutesSpecHandle.Data.Get());
		}
		
	}
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject,UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return;//客户端没有 GameMode,在客户端执行就会返回 空指针
	
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;
	
	
	for (TSubclassOf<UGameplayAbility> AbilityClass: CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}	
	
	const FCharacterClassDefaultInfo& ClassDefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass: ClassDefaultInfo.StartupAbilities)
	{
		if (AActor* AvatarActor = ASC->GetAvatarActor(); AvatarActor && AvatarActor->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(AvatarActor));
			ASC->GiveAbility(AbilitySpec);
		}
		
	}
}

int32 UAuraAbilitySystemLibrary::GetRewardForClassAndLevel(const UObject* WorldContextObject,
	const ECharacterClass CharacterClass, const int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;
	
	const FCharacterClassDefaultInfo& Info = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
	
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);
	
	return FMath::RoundToInt(XPReward);
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return nullptr;//客户端没有 GameMode,在客户端执行就会返回 空指针
	
	return AuraGameMode->CharacterClassInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{	
	/**
	 * 这里使用 static_cast 而不是 Cast<>()：
	 * - EffectContextHandle.Get() 返回的是 FGameplayEffectContext*（这是 USTRUCT，不是 UObject）
	 * - UE 的 Cast<>() 只能用于 UObject 体系（依赖 UClass/IsA），对 UStruct 无效
	 *
	 * 因为我们通过 UAuraAbilitySystemGlobals::AllocGameplayEffectContext()
	 * 让项目默认分配的是 FAuraGameplayEffectContext，所以这里把父类指针还原为子类指针来读取自定义字段。
	 *
	 * 注意：static_cast 不做运行时类型检查。
	 * 如果某些 EffectContext 不是 FAuraGameplayEffectContext（例如未正确配置 AbilitySystemGlobalsClassName），
	 * 这里的转换可能导致未定义行为。更稳的做法是先检查：
	 *   EffectContextHandle.Get()->GetScriptStruct() == FAuraGameplayEffectContext::StaticStruct()
	 */
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsBlockedHit();
	}
	
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{	
	/**
	 * 同 IsBlockedHit()：EffectContext 是 UStruct（非 UObject），因此不能用 Cast<>。
	 * 使用 static_cast 将 FGameplayEffectContext* 还原为 FAuraGameplayEffectContext* 以读取自定义字段。
	 */
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsCriticalHit();
	}
	
	return false;
}

void UAuraAbilitySystemLibrary::SetBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bIsBlockedHit)
{
	if ( FAuraGameplayEffectContext* AuraEffectContext = static_cast< FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetBlockedHit(bIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetCriticalHit(bInIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors,const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	
	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		//它在指定位置以给定半径做球形碰撞查询，只检索动态对象类型，并把忽略列表应用到查询参数里；结果写入 `Overlaps`。
		World->OverlapMultiByObjectType(Overlaps,SphereOrigin , FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()) )
			{
				OutOverlappingActors.AddUnique(Overlap.GetActor());
			}
		}
	}

}

bool UAuraAbilitySystemLibrary::IsNotFriend(const AActor* FirstActor, const AActor* SecondActor)
{
	const bool bBothArePlayer = FirstActor->ActorHasTag("Player") && SecondActor->ActorHasTag("Player");
	const bool bBothAreEnemy = FirstActor->ActorHasTag("Enemy") && SecondActor->ActorHasTag("Enemy");
	
	const bool bIsNotFriend = !(bBothArePlayer || bBothAreEnemy);
	return bIsNotFriend;
}


