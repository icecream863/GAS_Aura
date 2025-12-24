// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AuraCharacterBase.generated.h"

UCLASS(Abstract)// 不会作为实例
class AURA_API AAuraCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	
	AAuraCharacterBase();

protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
};
