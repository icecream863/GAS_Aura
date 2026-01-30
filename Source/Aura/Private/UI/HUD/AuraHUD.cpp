// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h" 
#include "UI/WidgetController/OverlayWidgetController.h"


UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		
		OverlayWidgetController->BindCallbacksToDependencies();
		return OverlayWidgetController;
	}
	else
	{
		return OverlayWidgetController;
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


