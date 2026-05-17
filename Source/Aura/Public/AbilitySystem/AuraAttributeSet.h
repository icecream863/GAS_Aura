// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"

#include "AuraAttributeSet.generated.h"

// AttributeSet 属性访问器宏：一次性生成 Getter/Setter/Init 等函数。
// 主要给 GAS 使用（例如在 GameplayEffect/Execution 里方便读写属性）。
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


//DECLARE_DELEGATE_RetVal(FGameplayAttribute, FAttributeSignature);
//~ FAttributeSignature：一个返回 FGameplayAttribute 的委托类型，用于在蓝图里绑定属性访问器（例如 UI 绑定某个属性的 Getter）。
//下面

DECLARE_DELEGATE_RetVal(FGameplayAttribute, FAttributeSignature);


//一个“无参数、返回 FGameplayAttribute”的静态委托实例
//并且内部保存了一个函数指针 FunctionPointer

/**
 * 统一收集一次 GE（GameplayEffect）结算时的“来源(Source) / 目标(Target)”上下文，
 * 避免在 PostGameplayEffectExecute 里到处重复 Cast/判空。
*/
USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()
	
	FEffectProperties(){}
	
	// EffectContext：能拿到 Instigator、Causer、HitResult 等信息
	FGameplayEffectContextHandle EffectContextHandle;
	
	// Source：施加效果的一方（谁打的/谁释放的）
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;
	
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;
	/** SourceASC 的 AvatarActor（通常是 Pawn/Character）; 有了它就能拿到更多信息（例如 Controller）。
	//`OwnerActor`：ASC 的拥有者（可能是 `PlayerState`、`Character` 等，用于生命周期/网络等）。
	//`AvatarActor`：ASC 当前操控/映射到的那个“肉身”（Pawn/Character）
	*/
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;
	
	UPROPERTY()
	AController* SourceController = nullptr;
	
	// Target：承受效果的一方（谁被打/谁被影响）
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
	
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
	
	UPROPERTY()
	AController* TargetController = nullptr;
};

template<typename T>
using TStaticFuncPtr = TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;//~ 静态函数指针类型（无参数、返回 FGameplayAttribute）

UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	
	UAuraAttributeSet();
	
	// 注册需要网络复制的属性（GetLifetimeReplicatedProps + DOREPLIFFETIME_* 宏）。
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 属性即将被修改（常用于 Clamp，例如 Health 不能超过 MaxHealth）。
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	// GameplayEffect 应用并结算后回调（常用于处理伤害、回血、触发事件等）。
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	/** ReplicatedUsing：客户端收到服务器同步该属性时，触发对应 OnRep_*。
	* UI 通常在 OnRep 里刷新；服务器本地修改不会自动走 OnRep（需手动调用或走 ASC 的通知）。
	*/
	
	/**TMap<FGameplayTag, FAttributeSignature> TagsToAttribute;
	*~ 属性委托映射表：把属性和对应的 Getter 委托绑定，方便在蓝图里通过属性访问器来获取属性值（例如 UI 绑定）。
	*~ 这里使用 FBaseStaticDelegateInstance 来存储静态函数指针（属性访问器通常是静态函数），并且指定了返回值和用户策略。
	* 这里FBaseStaticDelegateInstance也可以有其他表示方法，本质就是函数指针的封装，方便在蓝图里绑定和调用。
	* 例如可替换为 FGameplayAttribute(*)()
	* 或者觉得太长可以 typedef TBaseStaticDelegateInstance,用uisng 可以做成模板
	* 上面用了 using 定义了 TStaticFuncPtr 来简化类型，表示一个无参数、返回 FGameplayAttribute 的静态函数指针。
	*/
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttribute;
	
	
	//~ Vital Attributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attribute")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);
	
	
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attribute")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);
	
	
	
	//~ Primary Attributes（示例，实际项目可根据需要添加更多）
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Strength, Category = "Primary Attribute")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Intelligence, Category = "Primary Attribute")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Resilience, Category = "Primary Attribute")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Vigor, Category = "Primary Attribute")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor);
	
	// ~Seconary Attributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Secondary Attribute")
	FGameplayAttributeData Armor;// 护甲：降低受到的伤害（通常是物理伤害）的能力
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Secondary Attribute")
	FGameplayAttributeData ArmorPenetration;// 护甲穿透：无视目标一部分护甲的能力
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArmorPenetration);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category = "Secondary Attribute")
	FGameplayAttributeData BlockChance;// 格挡几率：触发格挡时可减免/抵消本次伤害（具体比例看你的计算逻辑）
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, BlockChance);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitChance, Category = "Secondary Attribute")
	FGameplayAttributeData CriticalHitChance;// 暴击几率：攻击触发暴击的概率
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitChance);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitDamage, Category = "Secondary Attribute")
	FGameplayAttributeData CriticalHitDamage;// 暴击伤害：暴击时的额外伤害系数/增量
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitDamage);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitResistance, Category = "Secondary Attribute")
	FGameplayAttributeData CriticalHitResistance;// 暴击伤害：暴击时的额外伤害系数/增量
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitResistance);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category = "Secondary Attribute")
	FGameplayAttributeData HealthRegeneration;// 生命回复：单位时间自动回复生命值的速度（常用于每秒回血）
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, HealthRegeneration);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegeneration, Category = "Secondary Attribute")
	FGameplayAttributeData ManaRegeneration;// 法力回复：单位时间自动回复法力值的速度（常用于每秒回蓝）
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ManaRegeneration);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Secdary Attribute")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Secdary Attribute")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);
	
	//伤害抗性属性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category = "Resistance Attribute")
	FGameplayAttributeData FireResistance;// 火焰抗性：减少受到的火焰伤害的能力
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, FireResistance);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningResistance, Category = "Resistance Attribute")
	FGameplayAttributeData LightningResistance;// 火焰抗性：减少受到的火焰伤害的能力
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, LightningResistance);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArcaneResistance, Category = "Resistance Attribute")
	FGameplayAttributeData ArcaneResistance;// 火焰抗性：减少受到的火焰伤害的能力
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArcaneResistance);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalResistance, Category = "Resistance Attribute")
	FGameplayAttributeData PhysicalResistance;// 火焰抗性：减少受到的火焰伤害的能力
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, PhysicalResistance);
	
	/**
	 *	Meta Attribute
		1. 处理伤害的临时缓冲：在受到攻击时，GameplayEffect (GE) 通常不会直接修改 Health（生命值），而是把基础伤害值赋予给 IncomingDamage 这个元属性。 
		2. 支持复杂的结算逻辑：当带有 IncomingDamage 的 GE 结算时，会在 PostGameplayEffectExecute (或者 Execution Calculation) 里拦截到这个值。此时你可以结合目标当前的 Armor（护甲）、BlockChance（格挡几率）等次级属性进行统一计算，得出最终的实际伤害后，再去扣除 Health。 
		3. 无需网络同步（非状态属性）：与 Health 或 Mana 这种持续存在且需要同步的常规属性不同，元属性是用于过程中计算的临时变量。因此你会发现它的 UPROPERTY 中没有 ReplicatedUsing 标签，它不需要且不应该跨网络复制，通常在伤害结算完成后就会被清零。 
		4.生成访问器：ATTRIBUTE_ACCESSORS 宏为 IncomingDamage 自动生成了标准的 Getter 和 Setter 函数，方便在 C++ 中快速读取和修改该数值。 
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attribute")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingDamage);
	
	
	
	// OnRep 参数 OldX：复制更新前的旧值（便于做差值、播放动画等）。 
	// 客户端收到服务器同步该属性时触发。服务器本地修改不会自动走 OnRep（需手动调用或走 ASC 的通知）。
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldHealth) const;
	
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldHealth) const;
	
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldHealth) const;
	
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;
	
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;
	
	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;
	
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;
	
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;
	
	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;
	
	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;
	
	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;
	
	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;
	
	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;
	
	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;
	
	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;
	
	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const;
	
	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const;
	
	UFUNCTION()
	void OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const;
	
	UFUNCTION()
	void OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const;
	
private:
	
	// 从 Data 里解析 Source/Target 相关对象并填充到 Props（内部会做 Cast/判空）。
	void SetEffectProperties(const struct FGameplayEffectModCallbackData& Data, FEffectProperties& Props);
	
	void ShowFloatingText(FEffectProperties& Props, float Damage, bool bIsBlockedHit, bool bIsCriticalHit);
};
