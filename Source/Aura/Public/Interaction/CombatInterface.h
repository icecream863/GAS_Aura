// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetPlayerLevel();  
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)//这个函数只能在蓝图中实现，不能在C++中实现，如果C++中调用这个函数，必须在蓝图中实现，否则会报错。
	void UpdateFacingTarget(const FVector& FacingTarget);
	//利用函数重载，具体类重载这个函数，而掉用只需转化成接口指针调用这个函数就好，接口指针会根据实际对象类型调用对应的重载函数。
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)//这个函数既可以在蓝图中实现，也可以在C++中实现，如果蓝图没有实现，就会调用C++中的实现。
	UAnimMontage* GetHitReactMontage();
	
	virtual void Die() = 0;
	
	
};
