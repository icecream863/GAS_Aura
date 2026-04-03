// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	virtual void BeginPlay() override;
	
	/** EnemyInterface */
	virtual void HighLightActor() override;
	virtual void UnHighLightActor() override;
	/** End EnemyInterface */
	
	/** CombatInterface */
	virtual int32 GetPlayerLevel() override;
	/** End CombatInterface */
	
protected:
	virtual void InitAbilityActorInfo() override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Class Default")
    int32 Level = 1;// 
	

	
};
