
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Environment_Cloud.generated.h"

UCLASS()
class LPROJECT_API AEnvironment_Cloud : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnvironment_Cloud();

protected:
	virtual void BeginPlay() override;

private:
	bool Find_Player();
	void Destroy_Cloud();
	bool Destroy_Judgment();

public : // For. Getter Setter 
	void	 Set_CloudNumber(int32 _cnt) { CloudCnt = _cnt;  }
	void	 AroundCloud_Destroy(FVector _targetPos);


protected:
	TObjectPtr<class AR1Player> R1Player;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> uStaticMesh;

public:	
	virtual void Tick(float DeltaTime) override;
	
private :
	UPROPERTY()
	bool bDist				= { false };
	UPROPERTY()
	int32 CloudCnt			= { 0 };
};
