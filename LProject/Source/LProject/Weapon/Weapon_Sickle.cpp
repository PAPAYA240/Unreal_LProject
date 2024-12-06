

#include "Weapon/Weapon_Sickle.h"
#include "Weapon_Sickle.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "Monster/R1Monster.h"

/* Particle */
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h" 

AWeapon_Sickle::AWeapon_Sickle()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon_Sickle"));
	RootComponent = WeaponMesh;

	// StaticMesh 로드
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WeaponObj(TEXT("/Script/Engine.StaticMesh'/Game/Art/Characters_Rough/LowPrototype/Modular_003_Bargin05/Mesh/SM_Hammer_Bargin05.SM_Hammer_Bargin05'"));
	if (WeaponObj.Succeeded())
		WeaponMesh->SetStaticMesh(WeaponObj.Object);
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

	/* Default_Particle */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> D_ParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_Rock_Burst_01.Par_LoPo_Rock_Burst_01'"));
	if (D_ParticleSystem.Succeeded())
		Default_Particle = D_ParticleSystem.Object;

	/* Explosion Particle */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_RockSpikes_02.Par_LoPo_RockSpikes_02'"));
	if (ParticleSystem.Succeeded())
		Explosion_TargetParticle = ParticleSystem.Object;
}

void AWeapon_Sickle::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon_Sickle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon_Sickle::SpawnParticle(EPLAYER_PATTERN _epattern)
{
    if (R1Player)
    {
        AR1Monster* TargetMonster = {};
        TargetMonster = CurrentPlayerState->TargetMonster;

        if (TargetMonster)
        {
            if (_epattern == EPLAYER_PATTERN::HIDDEN_ATTACK)
            {
                FVector SpawnLocation = this->GetActorLocation();
                SpawnLocation.Z -= 88.f;

                FVector PlayerLot = R1Player->GetActorLocation();
                FVector TargetLot = R1Player->Get_CurrentPlayerState()->TargetMonster->GetActorLocation();
                TargetLot.Y += 20.f;
                FVector NewDist = TargetLot - PlayerLot;
                NewDist.GetSafeNormal();

                FRotator SpawnRotation = NewDist.Rotation();
                if (Explosion_TargetParticle)
                    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Explosion_TargetParticle, SpawnLocation, SpawnRotation);
            }
            else if (_epattern == EPLAYER_PATTERN::ATTACK)
            {
                FVector SpawnLocation = this->GetActorLocation();
                SpawnLocation.Z -= 88.f;
                FRotator SpawnRotation = FRotator::ZeroRotator;

                if (Default_Particle)
                    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Default_Particle, SpawnLocation, SpawnRotation);
            }

            CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::DEFAULT)] = false;
        }
    }
}