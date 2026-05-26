// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	AAuraProjectile();
	
	virtual void Destroyed() override;
	
	UPROPERTY(VisibleAnywhere)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true) )
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
	/**
	*作用：这是最关键的设置。当你使用 SpawnActorFromClass 节点生成这个 Actor（比如子弹）时，这个变量会直接出现在生成节点的输入引脚上。
	解决的问题：它避免了“先生成、再赋值”的尴尬。如果在赋值前子弹就撞到了物体，此时变量为空就会报错；
	使用 ExposeOnSpawn 可以确保子弹在诞生那一刻就已经持有了伤害数据。	
	*/
	
protected:
	
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;
	
	
private:
	
	float LifeSpan = 15.f;
	bool bHit = false;
	
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;
	
	
};
