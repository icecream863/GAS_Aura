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

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;
	
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor && AvatarActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
	{
		// BlueprintNativeEvent 必须通过 Execute_XXX 调用，不能直接调用接口事件函数。
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, FAuraGameplayTags::Get().CombatSocket_Weapon);
		
		FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SocketLocation, ProjectileTargetLocation);
		
		
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(SpawnRotation.Quaternion());
		
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass,
			SpawnTransform, 
			GetOwningActorFromActorInfo(), 
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);
		
		
		UAbilitySystemComponent* SourceASC =  UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());
		
		/**
		*这行是在给 GameplayEffectSpec 写入一个 `SetByCaller` 数值，键是 `GameplayTags.Damage`（一个 `GameplayTag`），值是 `ScaledDamage`。
		`EffectSpecHandle`：即将应用到目标身上的 `GameplayEffect` 的“规格/实例数据”
		`GameplayTags.Damage`：用作 `SetByCaller` 的标识（类似参数名）
		`ScaledDamage`：实际要传递的伤害数值（这里按技能等级缩放后的伤害）
		后续当这个 `GameplayEffect` 被应用时，效果里（或执行计算 `ExecutionCalculation` / MMC 等）会通过同一个 Tag（`Damage`）把这个数值读出来，用于计算最终伤害
		 */
		
		for (const auto& Pair : DamageTypeMap)
		{
			const FGameplayTag& DamageTypeTag = Pair.Key;
			const FScalableFloat& DamageTypeValue = Pair.Value;
			const float ScaledDamageTypeValue = DamageTypeValue.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, DamageTypeTag, ScaledDamageTypeValue);
		}
		
		Projectile->DamageEffectSpecHandle = EffectSpecHandle;
		/**
		 *这三行在获取施法者的 AbilitySystemComponent，基于 `DamageEffectClass` 和当前技能等级创建一个 `FGameplayEffectSpecHandle`
		 *，并把该效果规格保存到投射物上，供命中时应用伤害效果。
		 */
		
		/** owing 拥有者(playerState) Avatar 化身(character) Instigator 发起人
		//SpawnActorDeferred 先生成一个Actor实例，但不立即激活它。这样你就可以在它被激活之前对它进行一些设置，
		例如设置属性或者调用函数。最后需要调用 FinishSpawning 来完成生成过程。
		* 延迟化生成
		*/
		Projectile->FinishSpawning(SpawnTransform);// 完成生成过程，激活Actor
	
	}
}
