// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;
class UBehaviorTree;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual AActor* GetCombatTarget_Implementation() override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	
	/** EnemyInterface */
	virtual void HighLightActor() override;
	virtual void UnHighLightActor() override;
	/** End EnemyInterface */
	
	/** CombatInterface */
	virtual int32 GetPlayerLevel() override;
	/** End CombatInterface */
	
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangeSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangeSignature OnMaxHealthChanged;
	
	virtual void BeginPlay() override;
	
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	virtual void Die() override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReact = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;
	
protected:
	virtual void InitAbilityActorInfo() override;
	virtual void InitialDefaultAttributes() const override;
	
	
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Class Default")
    int32 Level = 1;// 
	
	//不需要每个角色都 CharacterClassInfo,只需要一个全局的 CharacterClassInfo 就够了，角色类里只需要一个 CharacterClass 枚举来标识自己的职业类型就行了
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Class Default")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;//默认元素使
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy | Widget")
	TObjectPtr<UWidgetComponent> HealthBar;
	//这个是 控件组件，需要自己在ue里设置 widgetClass

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
	//这是 实例，蓝图里需要 设置一个 类
	
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;
};
