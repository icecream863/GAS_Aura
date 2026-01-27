// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
/**
 * 
 */
UCLASS()//MVC controller
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
	
protected:
	/**
	 * 用基类声明是为了**“面向接口编程，
	 * 而非面向实现编程”**。 
	 * 这能让你的 WidgetController 变得非常健壮，
	 * 不会因为底层具体类的变动而轻易崩溃。
	 * 解耦
	 */
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet> UAttributeSet;
};
