
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Define/R1Define.h"
#include "Interface/R1Highlight_Interface.h"
#include "Interactive_Buildings.generated.h"

UCLASS()
class LPROJECT_API AInteractive_Buildings : public AActor, public IR1Highlight_Interface
{
	GENERATED_BODY()
	
public:	
	AInteractive_Buildings();

	void Return_InGame();
	float GetMeshHeight(); // 물체의 높이를 알 수 있음

protected:
	virtual void BeginPlay() override;

	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) PURE_VIRTUAL(AInteractive_Buildings::OnHit, ); // 순수 가상 함수

	/* 위젯을 열고 닫기 위한 필수 요소*/
	bool Exit_Widget(class UR1UserWidget* _widget);
	void Show_Widget(class UR1UserWidget* _widget);


public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Highlight() override;
	virtual void UnHighlight() override;

	bool Show_Widget();

	// Building 작동이 끝났다면 Camera 포커스를 Player로 맞추게 한다.
	bool Get_StartOperate() { return bStart;  }
	void Set_ShowWidget(bool _show);

	bool Get_ShowWidget() { return bShowWidget;  }

private :
	bool Find_Player();
	
	
protected :
	TObjectPtr<class AR1Player> R1Player;
	TObjectPtr<class AMyPlayerState> CurrentPlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Static Mesh")
	TObjectPtr<UStaticMeshComponent> uStaticMesh;

	UPROPERTY(EditAnywhere)
	EBUILDING_TYPE Building_Type = EBUILDING_TYPE::END_BUILDING;
	
	TObjectPtr<class UCapsuleComponent> CapsuleComp;
	
protected :
	UPROPERTY()
	bool bHighlighted		= { false };
	UPROPERTY()
	bool bStart				= { false };
	UPROPERTY()
	bool bShowWidget	= { false };
	UPROPERTY()
	float ReturnTimer		= { 0.f }; // 끝나고 다시 인식할 시간
	UPROPERTY()
	bool	bPrevStart = { false };
};
