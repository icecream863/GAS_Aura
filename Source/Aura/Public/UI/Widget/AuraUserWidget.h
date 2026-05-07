// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */

UCLASS() // MVC View
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);
	//这是一个公开的成员函数声明，用于接收一个 `UObject\*` 控制器并设置到该控件实例中，
	//便于在蓝图或 C\+\+ 中关联和使用控制器逻辑
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
	
	//~ c++调用， 蓝图实现, 这是一个事件
	// 有作用域 只在overlay蓝图实现， 在mana和heal蓝图中不会实现，不共用
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
	
};
