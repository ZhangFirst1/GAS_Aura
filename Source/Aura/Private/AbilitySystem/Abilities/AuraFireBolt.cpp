// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Actor/AuraProjectile.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT: </>\n\n"

			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			
			"<Default>Launches 1 a bolt of fire, exploding on impact and dealing: </>"
			// Damage
			"<Damage>%d </>"
			"<Default>fire damage with a chance to burn</> \n\n"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			ScaledDamage);
	}else
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT: </>\n\n"

			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Number of firebolt
			"<Default>Launches %d bolts of fire, exploding on impact and dealing: </>"
			// Damage
			"<Damage>%d </>"
			"<Default>fire damage with a chance to burn</> \n\n"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, ProjectileNums),
			ScaledDamage);
		
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
			// Title
			"<Title>NEXT LEVEL: </>\n\n"

			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Number of firebolt
			"<Default>Launches %d bolts of fire, exploding on impact and dealing: </>"
			// Damage
			"<Damage>%d </>"
			"<Default>fire damage with a chance to burn</> \n\n"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, ProjectileNums),
			ScaledDamage);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,
	bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	// 判断是否为服务器
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	// 获取武器socket位置
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	// 获取到目标的旋转矩阵
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch) Rotation.Pitch = PitchOverride;

	const FVector Forward = Rotation.Vector();

	TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, ProjectileNums);

	for (const FRotator& Rot : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

		// 延迟生成投射物，必须搭配 FinishSpawning()使用
		// 投射物类，位置，拥有该技能的角色，造成伤害的对象，无论是否有碰撞强制生成
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
		// 创建DamageEffect
		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
	
		Projectile->FinishSpawning(SpawnTransform);
	}
}
