// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h" 
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"


UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WcParams)
{
	if (OverlayWidgetController == nullptr)
	{
		//~ 这里的创建方式是 NewObject，因为 WidgetController 是一个 UObject，而不是 Actor 或 Component；如果是 Actor 或 Component，应该用 SpawnActor 或 CreateDefaultSubobject。
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WcParams);
		
		OverlayWidgetController->BindCallbacksToDependencies();
		return OverlayWidgetController;
	}
	else
	{
		return OverlayWidgetController;
	}
	
}

USpellMenuWidgetController* AAuraHUD::GetSpellMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (SpellMenuWidgetController == nullptr)
	{
		SpellMenuWidgetController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
		SpellMenuWidgetController->SetWidgetControllerParams(WCParams);
		
		SpellMenuWidgetController->BindCallbacksToDependencies();
		return SpellMenuWidgetController;
	}
	else
	{
		return SpellMenuWidgetController;
	}
}


UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
		
		AttributeMenuWidgetController->BindCallbacksToDependencies();//~ 绑定回调，把 ASC/属性变化转为 UI 事件
		return AttributeMenuWidgetController;
	}
	else
	{
		return AttributeMenuWidgetController;
	}
}

void AAuraHUD::InitOverlay(APlayerState* PS, APlayerController* PC, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass 在蓝图里没有填"));
	checkf(OverlayWidgetControllerClass, TEXT("OverlayWidgetController 在蓝图里没有填"));
	
	OverlayWidget =  CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	
	const FWidgetControllerParams WidgetControllerParams(PS, PC, ASC, AS);
	UOverlayWidgetController* WidgetController =  GetOverlayWidgetController(WidgetControllerParams);
	
	OverlayWidget->SetWidgetController(WidgetController);
	
	WidgetController->BroadcastInitialValues();
	
	OverlayWidget->AddToViewport();
}


