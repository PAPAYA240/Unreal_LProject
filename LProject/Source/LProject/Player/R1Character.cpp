

#include "Player/R1Character.h"
#include "Item/R1Coin.h"
#include "Player/R1Player.h"
#include "Monster/R1Monster.h"

AR1Character::AR1Character()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AR1Character::BeginPlay()
{
	Super::BeginPlay();

	AbilityValues.Init(10.0f, static_cast<int32>(ABILITY_TYPE::END));
	TargetMaxHp = MaxHp;
}

void AR1Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AR1Character::HitAndDash(AR1Character* _thisObj, FVector _targetPos, bool _endTiming, float _timing)
{
	if(false == bCompletedDash)
	{
		FVector CurrentPos = _thisObj->GetActorLocation();
		FVector TargetPos = _targetPos;
		TargetPos.X += 100.f;

		FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, GetWorld()->GetDeltaSeconds(), 2.f); // 2.f는 보간 속도
		_thisObj->SetActorLocation(NewPos);

		if (FVector::Dist(NewPos, TargetPos) <= 100.f)
		{
			if(false == _endTiming)
			{
				// 몬스터의 경우
				DashAttack_Timer += GetWorld()->GetDeltaSeconds();
				if (DashAttack_Timer >= _timing)
				{
					bCompletedDash = true;
					DashAttack_Timer = 0.f;
				}
			}

			else if (true == _endTiming)
			{
				// 플레이어의 경우 : 애니메이션이 끝났을 때 
				if(true == Cast<AR1Player>(this)->Get_EndTimingAttack())
				{
					bCompletedDash = true;
				}
			}
		}
	}

	else if (true == bCompletedDash)
	{
		FVector CurrentPos = _thisObj->GetActorLocation();
		FVector TargetPos = _thisObj->InitPos;

		FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, GetWorld()->GetDeltaSeconds(), 4.f); // 2.f는 보간 속도

		if (FVector::Dist(NewPos, TargetPos) < 1.f)
		{
			_thisObj->SetActorLocation(TargetPos);
			bCompletedDash = false;
			return true;
		}
		else
		{
			_thisObj->SetActorLocation(NewPos);
		}
	}
	return false;
}

bool AR1Character::Drop_Coin(FVector _target, bool _bFollowCoin, FVector _followTarget)
{
	float RandomCoin = FMath::RandRange(4, 10);
	for(int i = 0; i < RandomCoin; i++)
	{
		_target.Z =+ 176.f;
		AR1Coin* SpawnedObject = GetWorld()->SpawnActor<AR1Coin>(AR1Coin::StaticClass(), _target, FRotator::ZeroRotator);
		if (SpawnedObject)
		{
			SpawnedObject->Set_DropCoin(_target, _bFollowCoin, _followTarget);
		}
	}
	return true;
}

void AR1Character::HandleGameplayEvent(FGameplayTag EventTag)
{
}

void AR1Character::Highlight()
{
	bHighlighted = true;
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(250);
}

void AR1Character::UnHighlight()
{
	bHighlighted = false;
	GetMesh()->SetRenderCustomDepth(false);
}

void AR1Character::OnDamged(int32 _damage, TObjectPtr<AR1Character> _Attacker)
{
	Hp = FMath::Clamp(Hp - _damage, 0, MaxHp);

	if (0 == Hp)
	{
		if(nullptr != _Attacker)
			OnDead(_Attacker);
	}
}

void AR1Character::OnHill(int32 _damage, TObjectPtr<AR1Character> _Attacker)
{
	if (!(MaxHp <= Hp))
		Hp = FMath::Clamp(Hp + _damage, 0, MaxHp);
}

void AR1Character::OnDead(TObjectPtr<AR1Character> _Attacker)
{
	if (EMonsterState::DEAD == MonsterState)
		return;

	MonsterState = EMonsterState::DEAD;
}


void AR1Character::Set_AbilityValue(ABILITY_TYPE _ability, float _value)
{
	AbilityValues[static_cast<int32>(_ability)] = _value;
}

float AR1Character::Get_AbilityValue(ABILITY_TYPE _ability) const
{
	return AbilityValues[static_cast<int32>(_ability)];
}

bool AR1Character::CalculateAbility(ABILITY_TYPE _ability)
{
	float RandomValue = FMath::FRand();

	if (static_cast<int32>(_ability) < 0 || static_cast<int32>(_ability) >= AbilityValues.Num())
		return false;
	else
	{
		return RandomValue <(AbilityValues[static_cast<int32>(_ability)] / 100.0f) + (AbilityValues[static_cast<int32>(ABILITY_TYPE::LUCKY)] / 100.0f);
	}
}


// AR1Character.cpp
bool AR1Character::DecreaseHP(float& _currentHP, float _targetHP, float DeltaTime, float _speed)
{
	if (_currentHP != _targetHP)
	{
		_currentHP = FMath::FInterpTo(_currentHP, _targetHP, DeltaTime, _speed);

		return false;
	}
	else
	{
		_currentHP = Hp;
		return true;
	}
}

