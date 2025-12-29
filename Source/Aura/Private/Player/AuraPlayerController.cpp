// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h" 
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Interaction/EnemyInterface.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	check(AuraContext);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	check(Subsystem);
	Subsystem->AddMappingContext(AuraContext, 0);
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
	
	
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{	
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return ;
	
	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	
	/**
	 * 
	 */
	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			ThisActor->HighLightActor();
		}
		else
		{
			//do nothing 
		}
	}
	else// LastActor is Valid
	{
		if (ThisActor == nullptr)
		{
			LastActor->UnHighLightActor();
		}
		else
		{
			if (ThisActor != LastActor)
			{
				LastActor->UnHighLightActor();
				ThisActor->HighLightActor();
			}
			else
			{
				//do nothing 
			}
		}
	}
		
	
}

void AAuraPlayerController::Move(const struct FInputActionValue& InputActinValue)
{	
	const FVector2D InputAxiVector2D = InputActinValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRatation = FRotator(0.f, Rotation.Yaw, 0.f);
	
	
	const FVector ForwardDirection = FRotationMatrix(YawRatation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRatation).GetUnitAxis(EAxis::Y);
	
	if (APawn* ControlPawn = GetPawn())
	{
		ControlPawn->AddMovementInput(ForwardDirection, InputAxiVector2D.Y);
		ControlPawn->AddMovementInput(RightDirection, InputAxiVector2D.X);
	}
}


