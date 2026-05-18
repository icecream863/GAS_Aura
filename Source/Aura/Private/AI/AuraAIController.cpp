// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AuraAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AAuraAIController::AAuraAIController()
{
	// ~-在 AIController 的构造函数里创建 BlackboardComponent 和 BehaviorTreeComponent，并设置它们的属性。
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(Blackboard);
	//AIController 默认会自动拥有一个 BlackboardComponent，所以我们不需要手动设置它的属性。
	
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	check(BehaviorTreeComponent);
}
