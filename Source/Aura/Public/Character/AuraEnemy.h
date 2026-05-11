// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraEnemy.generated.h"

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
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Class Default")
	ECharacterClass CharacterClass = ECharacterClass::Elementalist;//默认元素使
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy | Widget")
	TObjectPtr<UWidgetComponent> HealthBar;
	//这个是 控件组件，需要自己在ue里设置 widgetClass

	
	
};
