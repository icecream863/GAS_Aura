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
	
	//~ c++调用， 蓝图实现, 这是一个回调函数
	// 限制，只有在 SetWidgetController 被调用后，才会触发 WidgetControllerSet 事件，确保蓝图逻辑在控制器被正确设置后执行。
	// 主要是 理清谁在 调用，并在 调用这哪里实现
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
	
};
