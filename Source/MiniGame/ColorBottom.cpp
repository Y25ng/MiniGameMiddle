// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorBottom.h"
#include "Components/BoxComponent.h" // box 충돌체 헤더
#include "Components/StaticMeshComponent.h" // mesh component 헤더
#include "GameFramework/Character.h" // UE4 제공 Character 헤더
#include "GameFramework/CharacterMovementComponent.h"
// #include "Kismet/KismetMathLibrary.h" // 선형보간을 위한 헤더
#include "MiniGameCharacter.h" // 프로젝트 디폴트 Character 헤더
#include "MiniGameGameMode.h" // 프로젝트 디폴트 GameMode 헤더
#include "Runtime/Engine/Classes/Engine/World.h" // 현재 World 반환을 위한 헤더
#include "ActorManager.h"


// Sets default values
AColorBottom::AColorBottom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// box 충돌체
	m_BoxCollision = CreateDefaultSubobject< UBoxComponent >(TEXT("BoxObj"));

	if ( m_BoxCollision )
	{
		m_BoxCollision->SetupAttachment( RootComponent );
		RootComponent = m_BoxCollision;
	}

	// mesh
	m_StaticMesh = CreateDefaultSubobject< UStaticMeshComponent >( TEXT( "MeshObj" ) );
	
	if( m_StaticMesh )
		m_StaticMesh->SetupAttachment( RootComponent );

	// 타일 색깔 변화를 위해 사용할 머터리얼 
	static ConstructorHelpers::FObjectFinder< UMaterial > MaterialAsset_White( TEXT( "/Game/StarterContent/Materials/White" ) );
	static ConstructorHelpers::FObjectFinder< UMaterial > MaterialAsset_Red( TEXT( "/Game/StarterContent/Materials/Red" ) );
	static ConstructorHelpers::FObjectFinder< UMaterial > MaterialAsset_Blue( TEXT( "/Game/StarterContent/Materials/Blue" ) );
	static ConstructorHelpers::FObjectFinder< UMaterial > MaterialAsset_Yellow(TEXT("/Game/StarterContent/Materials/Yellow"));

	// key, value( int32, material )쌍으로 컨테이너에 원소 추가
	if ( MaterialAsset_White.Succeeded() )
		m_Material_Map.Add( EColorNum::WHITE ) = MaterialAsset_White.Object;
	else
		m_Material_Map.Add( EColorNum::WHITE ) = nullptr;

	if ( MaterialAsset_Red.Succeeded() )
		m_Material_Map.Add( EColorNum::RED ) = MaterialAsset_Red.Object;
	else
		m_Material_Map.Add( EColorNum::RED ) = nullptr;

	if ( MaterialAsset_Blue.Succeeded() )
		m_Material_Map.Add( EColorNum::BLUE ) = MaterialAsset_Blue.Object;
	else
		m_Material_Map.Add( EColorNum::BLUE ) = nullptr;

	if (MaterialAsset_Yellow.Succeeded())
		m_Material_Map.Add(EColorNum::YELLOW) = MaterialAsset_Yellow.Object;
	else
		m_Material_Map.Add(EColorNum::YELLOW) = nullptr;


	if ( ActorManager::GetInstance().GetColorBottom() == nullptr )
	{
		ActorManager::GetInstance().SetColorBottom( this );
	}
}


// Called when the game starts or when spawned
void AColorBottom::BeginPlay()
{
	Super::BeginPlay();

	if ( m_StaticMesh )
	{
		// 타일의 초기 색깔을 흰색으로 설정
		m_StaticMesh->SetMaterial(0, m_Material_Map[EColorNum::WHITE]);
		// 타일의 현재 색깔 상태를 나타내는 변수에 하얀색을 의미하는 상수 할당
		m_CurrentColor = EColorNum::WHITE;
	}

	ActorManager::GetInstance().PushBottom( m_BottomNumber, this );
}

// Called every frame
void AColorBottom::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// m_BottomDeltaTime += DeltaTime;
}

// 현재 타일의 머터리얼을 바꿔주는 함수
void AColorBottom::ChangeColor( int32 colorNum )
{
	if ( m_Material_Map.Find( colorNum ) == nullptr )
		return;

	if ( m_StaticMesh == nullptr )
		return;

	m_StaticMesh->SetMaterial( 0, m_Material_Map[ colorNum ] );
}
