#pragma once

#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

USTRUCT(BlueprintType)
// 把一次伤害所需的所有数据集中起来，避免每增加一个战斗参数就修改多个函数签名。
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams() = default;

	// 用于创建 EffectContext；即使没有目标，也必须有一个有效的世界上下文。
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	// 伤害由谁发出、最终应用到谁，分别由源 ASC 和目标 ASC 表示。
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;

	// 这些值最终会写入 GameplayEffectSpec 的 SetByCaller。
	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType;

	UPROPERTY(BlueprintReadWrite)
	float DebuffChance = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffDamage = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffFrequency = 0.f;
};

/**
 * 自定义 GameplayEffectContext。
 *
 * 目的：在 GAS 的 EffectContext 中携带我们自己的额外信息（例如：是否格挡/是否暴击），
 * 并且能够通过网络复制（NetSerialize）同步到客户端，用于 UI/飘字/GameplayCue 等。
 *
 * 备注：FGameplayEffectContext 本身已经包含 Instigator、EffectCauser、HitResult 等信息。
 * 我们在此基础上扩展，并确保：
 * - Duplicate() 深拷贝必要的数据（尤其是 HitResult）
 * - NetSerialize() 同步父类字段 + 我们新增字段
 */
USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
	/** 是否发生格挡（用于客户端显示/逻辑判断） */
	bool IsBlockedHit() const { return bIsBlockedHit; }
	/** 是否发生暴击（用于客户端显示/逻辑判断） */
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsSuccessfulDebuff() const { return bIsSuccessfulDebuff; }
	float GetDebuffDamage() const { return DebuffDamage; }
	float GetDebuffDuration() const { return DebuffDuration; }
	float GetDebuffFrequency() const { return DebuffFrequency; }
	TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; }
	
	/** 设置是否格挡（一般在服务端伤害结算时写入） */
	void SetBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	/** 设置是否暴击（一般在服务端伤害结算时写入） */
	void SetCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsSuccessfulDebuff(bool bInIsSuccessfulDebuff) { bIsSuccessfulDebuff = bInIsSuccessfulDebuff; }
	void SetDebuffDamage(float InDebuffDamage) { DebuffDamage = InDebuffDamage; }
	void SetDebuffDuration(float InDebuffDuration) { DebuffDuration = InDebuffDuration; }
	void SetDebuffFrequency(float InDebuffFrequency) { DebuffFrequency = InDebuffFrequency; }
	void SetDamageType(const FGameplayTag& InDamageType) { DamageType = MakeShared<FGameplayTag>(InDamageType); }
	
	/**
	 * 创建该 Context 的拷贝。
	 *
	 * GAS 在一些场景会复制 EffectSpec/EffectContext（例如预测、应用前的修改等）。
	 * 这里需要保证：
	 * - 基类数据被复制
	 * - HitResult 做深拷贝（因为它在基类里以共享指针形式保存）
	 */
	virtual FAuraGameplayEffectContext* Duplicate() const
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		if (DamageType.IsValid())
		{
			// DamageType 也是共享指针，复制 Context 时避免新旧 Context 共用同一份数据。
			NewContext->DamageType = MakeShared<FGameplayTag>(*DamageType);
		}
		return NewContext;
	}
	
	/**
	 * 自定义网络序列化（NetSerialize）。
	 * 重要：
	 * - 必须同时序列化父类字段（Instigator/EffectCauser/HitResult...）以及我们新增的字段
	 * - 读/写两端的字段顺序必须完全一致
	 */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	
	/**
	 * 返回该 Context 的实际 UScriptStruct。
	 * 这会影响引擎在（反）序列化时将其识别为“哪个结构体类型”。
	 *
	 * 对于子类 Context，应返回自己的 StaticStruct()。
	 */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
	
protected:

	/**
	 * 是否格挡。
	 * UPROPERTY 的意义：让引擎能正确追踪/序列化该字段（虽然这里主要通过 NetSerialize 走网络）。
	 */
	UPROPERTY()
	bool bIsBlockedHit = false;

	/** 是否暴击（同上） */
	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	bool bIsSuccessfulDebuff = false;

	UPROPERTY()
	float DebuffDamage = 0.f;

	UPROPERTY()
	float DebuffDuration = 0.f;

	UPROPERTY()
	float DebuffFrequency = 0.f;

	// FGameplayTag 自己支持 NetSerialize；共享指针允许在反序列化时按需创建实例。
	TSharedPtr<FGameplayTag> DamageType;
};

/**
 * 为该 USTRUCT 指定额外的“结构体操作特性”。
 * - WithNetSerializer = true：告诉引擎使用我们自定义的 NetSerialize。
 * - WithCopy = true：允许安全拷贝（对于含有 TSharedPtr<FHitResult> 的基类数据很关键）。
 */
template<>
struct TStructOpsTypeTraits< FAuraGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FAuraGameplayEffectContext >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};
