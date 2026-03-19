// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireBall.h"

FString UAuraFireBlast::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
	// Title
	"<Title>FIRE BLAST: </>\n\n"

	// Level
	"<Small>Level: </><Level>%d</>\n"
	// ManaCost
	"<Small>ManaCost: </><ManaCost>%.1f</>\n"
	// Cooldown
	"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			
	"<Default>Launches %d fire balls in all directions, each coming back and exploding upon return ,causing </>"
	// Damage
	"<Damage>%d </>"
	"<Default>radial fire damage with a chance to burn</> \n\n"),

	// Values
	Level,
	ManaCost,
	Cooldown,
	NumFireBalls,
	ScaledDamage);
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level)
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
			
	"<Default>Launches %d fire balls in all directions, each coming back and exploding upon return ,causing </>"
	// Damage
	"<Damage>%d </>"
	"<Default>radial fire damage with a chance to burn</> \n\n"),

	// Values
	Level,
	ManaCost,
	Cooldown,
	NumFireBalls,
	ScaledDamage);
}

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	TArray<AAuraFireBall*> FireBalls;
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	// 发射方向
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, NumFireBalls);

	// 遍历所有方向
	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());

		// 延迟生成火球
		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(FireBallClass,		// 要生成的火球蓝图类
			SpawnTransform,												// 变换矩阵
			GetOwningActorFromActorInfo(),								// 拥有者
			CurrentActorInfo->PlayerController->GetPawn(),				// 实例化对象
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);			

		// 设置伤害参数，飞到最远处返回目标
		FireBall->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->ReturnToActor = GetAvatarActorFromActorInfo();
		FireBall->ExplosionDamageParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->SetOwner(GetAvatarActorFromActorInfo());
		
		FireBalls.Add(FireBall);
		FireBall->FinishSpawning(SpawnTransform);  
	}
	
	return FireBalls;
} 
