// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxMana.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMMC_MaxMana : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMMC_MaxMana();
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	virtual FOnExternalGameplayModifierDependencyChange* GetExternalModifierDependencyMulticast(const FGameplayEffectSpec& Spec, UWorld* World) const override;
	
private:
	
	// ~FGameplayEffectAttributeCaptureDefinition 是 GAS里的一个“属性捕获定义”结构体，用来告诉 MMC/Execution：
	//~要抓哪个属性（AttributeToCapture）- 从谁身上抓（AttributeSource：Source 或 Target）- 是快照还是实时（bSnapshot）
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
};
