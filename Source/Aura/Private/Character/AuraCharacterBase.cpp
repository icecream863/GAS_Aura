// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Weapon") );
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	/** 
	* ECC = `ECollisionChannel`（碰撞通道，*Collision Channel*）
	用来表示“你在和哪一类对象/用途进行碰撞查询或响应”。
	例如：ECC_Camera 表示相机通道（相机的碰撞/遮挡检测常用这个通道）。
	ECR = `ECollisionResponse`（碰撞响应，*Collision Response*）
	用来表示“对这个通道要怎么响应”。常见有：
	*/
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{ 
	Super::BeginPlay();
	
}

FVector AAuraCharacterBase::GetCombatSocketLocation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(GetAbilitySystemComponent());
	check(GameplayEffectClass);
	
	FGameplayEffectContextHandle EffectContextHandle =  GetAbilitySystemComponent()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, 1, EffectContextHandle);
	
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

void AAuraCharacterBase::InitialDefaultAttributes() const
{
	// 先初始化主属性，后续次属性可能会依赖主属性的数值（例如 MaxHealth 可能依赖 Vigor），所以先初始化主属性比较合理。当然具体顺序也要看你的设计需求。
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);//intant
	ApplyEffectToSelf(DefaultSecdaryAttributes, 1.f);//infinite
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);//初始化为最大值，instant就行
	//顺序很重要
}

void AAuraCharacterBase::AddCharacterAbilities() const
{
	if (!HasAuthority()) return;
	// 只有服务器才会添加能力，客户端不需要添加能力，
	// 因为能力的执行和效果的应用都是由服务器控制的，客户端只需要接收服务器的状态更新即可。
	
	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
	if (AuraASC)
	{
		AuraASC->AddCharacterAbility(StartupAbilities);
	}
	
}


