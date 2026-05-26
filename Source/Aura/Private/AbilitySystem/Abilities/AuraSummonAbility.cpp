// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraSummonAbility.h"


TArray<FVector> UAuraSummonAbility::GetSpawnLocations() 
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = SpawnSpread / NumMinion;
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2, FVector::UpVector);
	TArray<FVector> SpawnLocations;
	
	for (int i = 0; i < NumMinion; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);//每个方向的随机位置
		
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f) ,ECC_Visibility);
		if (Hit.bBlockingHit)
		{
			ChosenSpawnLocation = Hit.ImpactPoint;// 射线检测，如果击中了地面，就把生成位置设置为击中点，生成在地面上；如果没有击中，就保持原来的位置，生成在空中。
		}
		
		SpawnLocations.Add(ChosenSpawnLocation);
	}
	
	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[Selection];
}
