// Fill out your copyright notice in the Description page of Project Settings.
 

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ColorBottom.generated.h"


class UBoxComponent;
class UMaterial;
class UStaticMeshComponent;


/////////////////////////////////////////////////////////
// 전체 바닥 공간에서 6x6 구역 중 한 공간을 차지할 오브젝트
/////////////////////////////////////////////////////////
UCLASS()
class MINIGAME_API AColorBottom : public AActor
{
	GENERATED_BODY()
	
private:
	// 현재 타일 색깔
	UPROPERTY( VisibleAnywhere )
	int32 m_CurrentColor;

	// mesh
	UPROPERTY( VisibleAnywhere )
	UStaticMeshComponent* m_StaticMesh;

	// box 충돌체
	UPROPERTY( VisibleAnywhere )
	UBoxComponent* m_BoxCollision;

	// matrial 컨테이너
	UPROPERTY( VisibleAnywhere )
	TMap< int32, UMaterial* > m_Material_Map;

public:	
	// Sets default values for this actor's properties
	AColorBottom();

	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	// 타일 색깔을 변경해줄 함수
	void ChangeColor( int32 colorNum );

	int32 GetBottomNumber() { return m_BottomNumber; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 현재 타일 색깔
	UPROPERTY( EditAnywhere )
	int32 m_BottomNumber;
};
