// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "Aura/Public/AuraGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetMathLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, 
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		
}

void UAuraProjectileSpell::SpwanProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;
	
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	
	if (CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();// 获取攻击出发位置， 在auraCharacterBase里实现过
		
		FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SocketLocation, ProjectileTargetLocation);
		SpawnRotation.Pitch = 0.0f;
		
		
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(SpawnRotation.Quaternion());
		
		AAuraProjectile* AuraProjectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass,
			SpawnTransform, 
			GetOwningActorFromActorInfo(), 
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		UAbilitySystemComponent* SourceASC =  UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());
		
		/**这两行是在获取全局的 `FAuraGameplayTags` 单例，
		 *并把 `Damage` 标签对应的 SetByCaller 数值写入到 `EffectSpecHandle`，数值为 50\.0。这样伤害效果就能在应用时读取该标签的数值。*/
		const float ScaledDamage =  Damage.GetValueAtLevel(GetAbilityLevel());// 获取当前技能等级对应的伤害数值
		FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();	
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, GameplayTags.Damage, ScaledDamage);
		
		AuraProjectile->DamageEffectSpecHandle = EffectSpecHandle;
		/**
		 *这三行在获取施法者的 AbilitySystemComponent，基于 `DamageEffectClass` 和当前技能等级创建一个 `FGameplayEffectSpecHandle`
		 *，并把该效果规格保存到投射物上，供命中时应用伤害效果。
		 */
		
		/** owing 拥有者(playerState) Avatar 化身(character) Instigator 发起人
		//SpawnActorDeferred 先生成一个Actor实例，但不立即激活它。这样你就可以在它被激活之前对它进行一些设置，
		例如设置属性或者调用函数。最后需要调用 FinishSpawning 来完成生成过程。
		* 延迟化生成
		*/
		AuraProjectile->FinishSpawning(SpawnTransform);// 完成生成过程，激活Actor
	
	}
}
