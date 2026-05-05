// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraEnhancedInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig* AuraInputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

// 说明：上面的 BindAbilityActions\(\) 是一个模板辅助函数，用于把 `UAuraInputConfig` 里配置的 Ability 输入动作批量绑定到回调上。
// \- PressedFunc 绑定 `ETriggerEvent::Started`\(\)：按下开始触发，并把对应的 `InputTag` 作为额外参数传入回调。
// \- ReleasedFunc 绑定 `ETriggerEvent::Completed`\(\)：松开时触发，同样传入 `InputTag`。
// \- HeldFunc 绑定 `ETriggerEvent::Triggered`\(\)：按住/持续触发（通常每帧或按触发规则触发），并传入 `InputTag`。
// 这样可以在同一个回调函数里通过 `InputTag` 区分是哪一个技能/输入动作触发的。
template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraEnhancedInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);
	
	for (const FAuraInputAction& InputAction : InputConfig->AbilityInputActions)
	{
		if (InputAction.InputAction && InputAction.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(InputAction.InputAction, ETriggerEvent::Started, Object, PressedFunc, InputAction.InputTag);//因为不是默认参数函数，需要在函数后面加参数InputAction.InputTag
			}
			
			if (ReleasedFunc)
			{
				BindAction(InputAction.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputAction.InputTag);
			}
			
			if (HeldFunc)
			{
				BindAction(InputAction.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, InputAction.InputTag);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Input Action is null for tag: %s"), *InputAction.InputTag.ToString());
		}
		
	}
	
}
