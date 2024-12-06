// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Define/R1Define.h"
#include "R1GameModeBase.generated.h"

USTRUCT(BlueprintType) 
struct FPlayerManager
{
	GENERATED_BODY()

	UPROPERTY() 
	class APlayerController* PlayerController;

	UPROPERTY()
	class AR1Character* Player;

	UPROPERTY()
	class AMyPlayerState* PlayerState;
};

UCLASS()
class LPROJECT_API AR1GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public :
	AR1GameModeBase();

public :
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void SwitchPlayer();

	// Object 찾기
	//AActor* Find_NameWorldObject(FName* _name);

private :
	class AR1Player*		Create_Player(EJOB_TYPE _job, FVector _spawnLoc, FRotator _spawnRot);

public:
	class AR1Player*			CurrentPlayer;
	class AR1Player*			PlayerArray[3];
    class AMyPlayerState*	PlayerStates[3];

	bool							bSetting_Number = { false };

};
