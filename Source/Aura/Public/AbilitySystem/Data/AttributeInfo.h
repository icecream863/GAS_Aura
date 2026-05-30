// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttributeInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraAttributeInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription = FText();
	
	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.f;
};


/**
 * 属性信息数据资产
 * 方便在蓝图里配置属性标签、名称、描述等信息，并通过标签查找对应的属性信息
 * 例如，UI界面可以通过绑定这个数据资产来显示属性的名称、描述和数值，或者在属性变化时根据标签查找对应的属性信息来更新显示
 * 通过使用数据资产，可以集中管理属性相关的信息，方便调整和扩展属性系统，而不需要修改代码逻辑
 */

// 属性菜单 controller 会更新和广播 这个数据资产
UCLASS()
class AURA_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	FAuraAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLagNotFound = false);
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraAttributeInfo> AttributeInformation;
	// 这个类是一个数据资产，包含一个属性信息数组。每个属性信息都包含一个标签、名称、描述和数值。通过标签可以查找对应的属性信息。
};
