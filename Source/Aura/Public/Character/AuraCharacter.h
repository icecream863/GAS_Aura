// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
	
public:
	AAuraCharacter();
	
	// 服务器在 Pawn 被控制器接管时调用（常用于初始化 ASC/能力相关信息）
	virtual void PossessedBy(AController* NewController) override;
	
	// 客户端 PlayerState 复制到位后调用（用于补齐客户端初始化）
	virtual void OnRep_PlayerState() override;
	
	/* CombatInterface */
	// 提供给战斗接口的角色等级（通常从 PlayerState 读取）
	virtual int32 GetPlayerLevel() override;
	/* End CombatInterface */
	
	
protected:
	// 角色开局初始化入口
	virtual void BeginPlay() override;
	
private:
	// 统一初始化 AbilityActorInfo，绑定 Owner/Avatar 到 ASC
	virtual void InitAbilityActorInfo() override;
	
	
}; 
