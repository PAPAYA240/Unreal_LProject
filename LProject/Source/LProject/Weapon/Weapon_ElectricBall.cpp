

#include "Weapon/Weapon_ElectricBall.h"
#include "Player/R1PlayerController.h"
#include "Player/R1Player.h"
#include "Monster/R1Monster.h"
#include "Player/R1Character.h"

/* Particle */
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h" 

AWeapon_ElectricBall::AWeapon_ElectricBall()
{
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon_ElectricBall"));
	RootComponent = WeaponMesh;

	// StaticMesh 로드
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WeaponObj(TEXT("/Script/Engine.StaticMesh'/Game/Art/StarterContent/Props/MaterialSphere.MaterialSphere'"));

	if (WeaponObj.Succeeded())
		WeaponMesh->SetStaticMesh(WeaponObj.Object);
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));


	/* Explosion Particle */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/Art/FXVarietyPack/Particles/P_ky_thunderBall.P_ky_thunderBall'"));
	if (ParticleSystem.Succeeded())
		Explosion_TargetParticle = ParticleSystem.Object;
}

void AWeapon_ElectricBall::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

}

void AWeapon_ElectricBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (WeaponMesh->IsVisible() == false)
	{
		Timer += DeltaTime;

		if (Timer > 0.1f)
		{
			R1Player->bGara = false;
			Timer = 0.f;
		}
	}*/
}

bool AWeapon_ElectricBall::Fire_Ball(AR1Character* _target)
{
	if (!_target)
		return false;

	// 파티클 시스템이 유효한지 확인
	//if (false == bParticleStart)
	//{
	//	bParticleStart = true;
	//	if (Explosion_TargetParticle)
	//	{
	//		FVector SpawnLocation = this->GetActorLocation();
	//		FRotator SpawnRotation = FRotator::ZeroRotator;

	//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Explosion_TargetParticle, SpawnLocation, SpawnRotation);
	//	}
	//}

	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = _target->GetActorLocation();

	FVector NewLocation = FMath::Lerp(CurrentLocation, TargetLocation, FireSpeed * GetWorld()->GetDeltaSeconds());
	SetActorLocation(NewLocation);

	if (FVector::Dist(CurrentLocation, TargetLocation) < 50.0f)
	{		
		bParticleStart = false;
		Set_ShowMesh(false);
		return true; 
	}

	return false; 
}
