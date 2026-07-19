// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = FMath::RoundToInt(Damage.GetValueAtLevel(Level));
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const int32 Projectiles = FMath::Min(Level, NumProjectiles);

	if (Level == 1)
	{
		return FString::Printf(
			TEXT("<Title>FIREBOLT</>\n\n")
			TEXT("<Small>Level: </><Level>%d</>\n")
			TEXT("<Small>ManaCost: </><ManaCost>%.1f</>\n")
			TEXT("<Small>Cooldown: </><Cooldown>%.1f</>\n\n")
			TEXT("<Default>Launches a bolt of fire, exploding on impact and dealing </>")
			TEXT("<Damage>%d</>")
			TEXT("<Default> fire damage with a chance to burn.</>"),
			Level,
			ManaCost,
			Cooldown,
			ScaledDamage);
	}

	return FString::Printf(
		TEXT("<Title>FIREBOLT</>\n\n")
		TEXT("<Small>Level: </><Level>%d</>\n")
		TEXT("<Small>ManaCost: </><ManaCost>%.1f</>\n")
		TEXT("<Small>Cooldown: </><Cooldown>%.1f</>\n\n")
		TEXT("<Default>Launches %d bolts of fire, exploding on impact and dealing </>")
		TEXT("<Damage>%d</>")
		TEXT("<Default> fire damage with a chance to burn.</>"),
		Level,
		ManaCost,
		Cooldown,
		Projectiles,
		ScaledDamage);
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = FMath::RoundToInt(Damage.GetValueAtLevel(Level));
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const int32 Projectiles = FMath::Min(Level, NumProjectiles);

	return FString::Printf(
		TEXT("<Title>NEXT LEVEL:</>\n\n")
		TEXT("<Small>Level: </><Level>%d</>\n")
		TEXT("<Small>ManaCost: </><ManaCost>%.1f</>\n")
		TEXT("<Small>Cooldown: </><Cooldown>%.1f</>\n\n")
		TEXT("<Default>Launches %d bolts of fire, exploding on impact and dealing </>")
		TEXT("<Damage>%d</>")
		TEXT("<Default> fire damage with a chance to burn.</>"),
		Level,
		ManaCost,
		Cooldown,
		Projectiles,
		ScaledDamage);
}
