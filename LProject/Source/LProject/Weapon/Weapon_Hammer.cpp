

#include "Weapon/Weapon_Hammer.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "Monster/R1Monster.h"

/* Particle */
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h" 

AWeapon_Hammer::AWeapon_Hammer()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon_Hammer"));
	RootComponent = WeaponMesh;

	// StaticMesh 로드
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WeaponObj(TEXT("/Script/Engine.StaticMesh'/Game/Art/Characters_Rough/LowPrototype/Modular_003_Bargin05/Mesh/SM_Hammer_Bargin05.SM_Hammer_Bargin05'"));
	if (WeaponObj.Succeeded())
		WeaponMesh->SetStaticMesh(WeaponObj.Object);
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

	/* Explosion Particle */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_Bolt_01.Par_LoPo_Bolt_01'"));
	if (ParticleSystem.Succeeded())
		Explosion_TargetParticle = ParticleSystem.Object; 

	/* Default_Particle */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> D_ParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_Bolt_02.Par_LoPo_Bolt_02'"));
	if (D_ParticleSystem.Succeeded())
		Default_Particle = D_ParticleSystem.Object;
}

void AWeapon_Hammer::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon_Hammer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AWeapon_Hammer::SpawnParticle(EPLAYER_PATTERN _epattern)
{
    if (R1Player)
    {
        AR1Monster* TargetMonster = {};
        TargetMonster = CurrentPlayerState->TargetMonster;

        if (TargetMonster)
        {
            if (_epattern == EPLAYER_PATTERN::HIDDEN_ATTACK)
            {
                FVector SpawnLocation = TargetMonster->GetActorLocation();
                SpawnLocation.Z -= 130.f;
                FRotator SpawnRotation = FRotator::ZeroRotator;

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