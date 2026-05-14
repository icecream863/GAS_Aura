#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

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
	
	/** 设置是否格挡（一般在服务端伤害结算时写入） */
	void SetBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	/** 设置是否暴击（一般在服务端伤害结算时写入） */
	void SetCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	
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
		return NewContext;
	}
	
	/**
	 * 自定义网络序列化（NetSerialize）。
	 *
	 * 重要：
	 * - 必须同时序列化父类字段（Instigator/EffectCauser/HitResult...）以及我们新增的字段
	 * - 读/写两端的字段顺序必须完全一致
	 */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	
	/**
	 * 返回该 Context 的实际 UScriptStruct。
	 *
	 * 这会影响引擎在（反）序列化时将其识别为“哪个结构体类型”。
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
};

/**
 * 为该 USTRUCT 指定额外的“结构体操作特性”。
 *
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
