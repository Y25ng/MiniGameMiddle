// Copyright Epic Games, Inc. All Rights Reserved.


#include "MiniGameCharacter.h"
#include "MiniGameGameMode.h" // 프로젝트 디폴트 GameMode 헤더
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h" // 카메라 헤더
#include "Components/CapsuleComponent.h" // 캐릭터 충돌체에 사용할 캡슐 헤더
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Wall.h"
#include "UserManager.h"
#include "ServerManager.h"
#include "Protocol.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
#include "Protocol.h"


AMiniGameCharacter::AMiniGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator( 0.0f, 540.0f, 0.0f ); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject< USpringArmComponent >( TEXT( "CameraBoom" ) );
	CameraBoom->SetupAttachment( RootComponent );
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject< UCameraComponent >( TEXT( "FollowCamera" ) );
	FollowCamera->SetupAttachment( CameraBoom, USpringArmComponent::SocketName ); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	m_XLocation = GetActorLocation().X;
	m_YLocation = GetActorLocation().Y;
	m_PeriodSendToServer = 0.0f;
	m_LerpTime = 0.0f;
	m_GameTimeSec = 0;
	m_Speed = 0.0f;
	m_bIsRun = false;
	m_bPlayerCanControll = true;
	m_CollisionCollTime = 0.0f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

AMiniGameCharacter::~AMiniGameCharacter()
{
	if ( m_OwnerIndex == UserManager::GetInstance().GetMainCharacterIndex() )
	{
		ServerManager::GetInstance().ShutDown();
	}
}

void AMiniGameCharacter::SetDefaultLocation( float x, float y )
{
	m_XLocation = x;
	m_YLocation = y;

	float tempZ = GetActorLocation().Z;

	FVector targetLocation = FVector( x, y, tempZ );

	SetActorLocation( targetLocation );
}

void AMiniGameCharacter::ApplyPlayerForces( int owners[] )
{
	int32 mainPlayerIndex = -1;
	TArray< int32 > otherPlayerIndexArr;

	for ( int i = 0; i < InitWorld::INGAMEPLAYER_NUM; ++i )
	{
		if ( owners[ i ] == -1 )
			continue;
		else
		{
			if ( owners[ i ] == UserManager::GetInstance().GetMainCharacterIndex() )
			{
				mainPlayerIndex = owners[ i ];
			}
			else
			{
				otherPlayerIndexArr.Add( owners[ i ] );
			}
		}
	}

	if ( otherPlayerIndexArr.Num() == 1 )
	{
		if ( mainPlayerIndex == -1 )
			return;

		if ( UserManager::GetInstance().GetPlayerMap()[ mainPlayerIndex ] != this )
			return;

		AMiniGameCharacter* otherPlayerCharacter = UserManager::GetInstance().GetPlayerMap()[ otherPlayerIndexArr[0] ];

		if ( otherPlayerCharacter == nullptr )
			return;

		// 충돌한 위치에서 튕겨져 나가는 방향을 계산
		FVector AwayFromOther = ( GetActorLocation() - otherPlayerCharacter->GetActorLocation() ).GetSafeNormal();

		// 튕겨져 나가는 힘의 크기를 설정
		float BounceForceMagnitude = 3000.f;

		// 힘을 계산하여 AddImpulse 함수를 사용하여 튕겨져 나가도록 처리
		FVector BounceForce = AwayFromOther * BounceForceMagnitude;
		// GetCharacterMovement()->AddImpulse( BounceForce, true );
		//  BounceForce = otherCharacter->GetVelocity();// 5000.f;

		LaunchCharacter( BounceForce, true, true );

		m_bPlayerCanControll = false;
	}
	else if ( otherPlayerIndexArr.Num() == 2 )
	{
		if ( mainPlayerIndex == -1 )
			return;

		if ( UserManager::GetInstance().GetPlayerMap()[ mainPlayerIndex ] != this )
			return;

		AMiniGameCharacter* otherPlayerCharacter1 = UserManager::GetInstance().GetPlayerMap()[ otherPlayerIndexArr[ 0 ] ];

		if ( otherPlayerCharacter1 == nullptr )
			return;

		AMiniGameCharacter* otherPlayerCharacter2 = UserManager::GetInstance().GetPlayerMap()[ otherPlayerIndexArr[ 1 ] ];

		if ( otherPlayerCharacter2 == nullptr )
			return;

		FVector centerLocation;
		centerLocation.X = ( GetActorLocation().X + otherPlayerCharacter1->GetActorLocation().X + otherPlayerCharacter2->GetActorLocation().X ) / 3.0f;
		centerLocation.Y = ( GetActorLocation().Y + otherPlayerCharacter1->GetActorLocation().Y + otherPlayerCharacter2->GetActorLocation().Y ) / 3.0f;
		centerLocation.Z = GetActorLocation().Z;

		// 충돌한 위치에서 튕겨져 나가는 방향을 계산
		FVector AwayFromOther = ( GetActorLocation() - centerLocation ).GetSafeNormal();

		// 튕겨져 나가는 힘의 크기를 설정
		float BounceForceMagnitude = 3000.f;

		// 힘을 계산하여 AddImpulse 함수를 사용하여 튕겨져 나가도록 처리
		FVector BounceForce = AwayFromOther * BounceForceMagnitude;
		// GetCharacterMovement()->AddImpulse( BounceForce, true );
		//  BounceForce = otherCharacter->GetVelocity();// 5000.f;

		LaunchCharacter( BounceForce, true, true );

		m_bPlayerCanControll = false;
	}
}


void AMiniGameCharacter::ApplyWallForces( const unsigned char wallIndex )
{
	/*
	// FString DebugMessage = FString::Printf( TEXT( "%d" ), wallIndex );
	// GEngine->AddOnScreenDebugMessage( -1, 5.0f, FColor::Yellow, DebugMessage );

	if ( static_cast<int>(wallIndex) >= InitWorld::NOTWALLCOLLISION || static_cast< int >(wallIndex) < InitWorld::LEFT_WALL )
	{
		return;
	}

	if ( UserManager::GetInstance().GetPlayerMap()[ UserManager::GetInstance().GetMainCharacterIndex() ] != this )
		return;

	// 충돌한 위치에서 튕겨져 나가는 방향을 계산
	FVector AwayFromOther = -1 * GetActorForwardVector();

	if ( wallIndex == InitWorld::LEFT_WALL)
	{
		AwayFromOther *= FVector( 1, -1, 0 );
	}
	else if ( wallIndex == InitWorld::RIGHT_WALL )
	{
		AwayFromOther *= FVector( 1, -1, 0 );
	}
	else if ( wallIndex == InitWorld::FORWARD_WALL )
	{
		AwayFromOther *= FVector( -1, 1, 0 );
	}
	else if ( wallIndex == InitWorld::BACKWARD_WALL )
	{
		AwayFromOther *= FVector( -1, 1, 0 );
	}

	AwayFromOther.GetSafeNormal();

	/*
	// 충돌 지점의 법선 벡터
	FVector nomalVectorWall;

	// Character의 이전 속도 벡터
	FVector previousVelocity = GetVelocity();

	if ( wallIndex == InitWorld::LEFT_WALL )
	{
		nomalVectorWall = FVector( 1, 0, 0 );
	}
	else if ( wallIndex == ::RIGHT_WALL )
	{
		nomalVectorWall = FVector( -1, 0, 0 );
	}
	else if ( wallIndex == InitWorld::FORWARD_WALL )
	{
		nomalVectorWall = FVector( 0, 1, 0 );
	}
	else if ( wallIndex == InitWorld::BACKWARD_WALL )
	{
		nomalVectorWall = FVector( 0, -1, 0 );
	}

	// 이전 속도 벡터와 충돌 지점의 법선 벡터를 정규화
	previousVelocity.Normalize();

	// 이전 속도 벡터와 법선 벡터의 내적을 계산
	float dotProduct = FVector::DotProduct( nomalVectorWall, previousVelocity );

	// 내적값을 사용하여 Character가 충돌 지점에서 멈춰있는 Actor의 어느 방향으로 튕겨져 나가야 하는지 계산
	FVector reflectedVelocity = previousVelocity - 2.0f * dotProduct * nomalVectorWall;

	*/
	// 새로운 속도 벡터 설정
	// float impulseStrength = 2000.0f;
	// FVector BounceForce = AwayFromOther * impulseStrength;

	// LaunchCharacter( BounceForce, true, true );

	// m_bPlayerCanControll = false;

	/*
	// 충돌 시간
	float HitTime = Hit.Time;

	// 충돌한 Actor의 속도 벡터
	FVector OtherActorVelocity = Other->GetVelocity();

	// Character의 이전 속도 벡터
	FVector PreviousVelocity = GetVelocity();

	// 충돌 지점의 법선 벡터
	FVector tempHitNormal = Hit.ImpactNormal;

	// 이전 속도 벡터와 충돌 지점의 법선 벡터를 정규화
	PreviousVelocity.Normalize();
	tempHitNormal.Normalize();

	// 이전 속도 벡터와 법선 벡터의 내적을 계산
	float DotProduct = FVector::DotProduct( tempHitNormal, PreviousVelocity );

	// 내적값을 사용하여 Character가 충돌 지점에서 멈춰있는 Actor의 어느 방향으로 튕겨져 나가야 하는지 계산
	FVector ReflectedVelocity = PreviousVelocity - 2.0f * DotProduct * tempHitNormal;

	// 새로운 속도 벡터 설정
	float ImpulseStrength = 10000.0f;
	GetCharacterMovement()->AddImpulse( ReflectedVelocity * ImpulseStrength, true );
	*/
}


void AMiniGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if ( NickName.ToString() == TEXT( "Default" ) ) // 플레이어가 조종할 캐릭터
	{
		ServerManager::GetInstance().SetCharacter( this );
	}
	else if ( NickName.ToString() == TEXT( "second" ) ) // 플레이어와 대결할 캐릭터1
	{
		ServerManager::GetInstance().SetCharacter2( this );
	}
	else if ( NickName.ToString() == TEXT( "third" ) ) // 플레이오아 대결할 캐릭터2
	{
		ServerManager::GetInstance().SetCharacter3( this );
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>( GetWorld() );
	NavSys->RegisterNavigationInvoker( this );

}

void AMiniGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	m_PeriodSendToServer += DeltaTime;

	float currentXLocation = GetActorLocation().X;
	float currentYLocation = GetActorLocation().Y;
	
	if ( ServerManager::GetInstance().GetbGameStart() && m_PeriodSendToServer >= 0.05f 
		&& ( static_cast< int >( currentXLocation * 100 ) != static_cast< int >( m_XLocation * 100 )
		|| static_cast< int >( currentYLocation * 100 ) != static_cast< int >( m_YLocation * 100 ) ) )
	{
		m_Velocity = FVector( currentXLocation - m_XLocation, currentYLocation - m_YLocation, 0 );

		m_PeriodSendToServer = 0.0f;
		m_XLocation = currentXLocation;
		m_YLocation = currentYLocation;

		if ( UserManager::GetInstance().GetMainCharacterIndex() == m_OwnerIndex && ServerManager::GetInstance().GetbGameStart() == true )
		{
			Packet::Move objMove( m_OwnerIndex, ClientToServer::MOVE );

			objMove.x = m_XLocation;
			objMove.y = m_YLocation;
			objMove.directionX = GetActorForwardVector().X;
			objMove.directionY = GetActorForwardVector().Y;
			// objMove.directionX = GetVelocity().GetSafeNormal().X;
			// objMove.directionY = GetVelocity().GetSafeNormal().Y;
			objMove.speed = GetVelocity().Size();

			ServerManager::GetInstance().SendPacket( ClientToServer::MOVE, &objMove);
		}
	}
	

	if ( m_bRecvLocation )
	{
		if ( m_Speed <= 10.0f )

		{
			m_bRecvLocation = false;
			FString DebugMessage = FString::Printf( TEXT( "anyword" ));
			GEngine->AddOnScreenDebugMessage( -1, 5.0f, FColor::Yellow, DebugMessage );
		}

		if ( m_bPlayerCanControll == true )
		{
			SmoothMoveToLocation( m_TargetLocation, DeltaTime );
		}

		FRotator targetRotation = m_TargetDirection.Rotation();
		SetActorRotation( FMath::RInterpTo(GetActorRotation(), targetRotation, GetWorld()->GetDeltaSeconds(), 40.f ) );
		//SetActorRotation( targetRotation )
	}

	if ( m_bPlayerCanControll == false )
	{
		m_CollisionCollTime += DeltaTime;

		if ( m_CollisionCollTime >= 0.4f )
		{
			m_bPlayerCanControll = true;
			m_CollisionCollTime = 0.0f;
		}
	}
}	

void AMiniGameCharacter::SmoothMoveToLocation( const FVector& TargetLocation, float DeltaTime )
{
	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = FMath::VInterpTo( CurrentLocation, TargetLocation, DeltaTime, m_Speed );

	SetActorLocation( NewLocation );
	
	if ( m_bIsRun == false )
	{
		m_bIsRun = true;
		PlayAnimMontage( m_RunAnim );
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMiniGameCharacter::SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent )
{
	// Set up gameplay key bindings
	check( PlayerInputComponent );
	PlayerInputComponent->BindAction( "Jump", IE_Pressed, this, &ACharacter::Jump );
	PlayerInputComponent->BindAction( "Jump", IE_Released, this, &ACharacter::StopJumping );

	PlayerInputComponent->BindAxis( "MoveForward", this, &AMiniGameCharacter::MoveForward );
	PlayerInputComponent->BindAxis( "MoveRight", this, &AMiniGameCharacter::MoveRight );

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis( "Turn", this, &APawn::AddControllerYawInput );
	PlayerInputComponent->BindAxis( "TurnRate", this, &AMiniGameCharacter::TurnAtRate );
	PlayerInputComponent->BindAxis( "LookUp", this, &APawn::AddControllerPitchInput );
	PlayerInputComponent->BindAxis( "LookUpRate", this, &AMiniGameCharacter::LookUpAtRate );

	// handle touch devices
	PlayerInputComponent->BindTouch( IE_Pressed, this, &AMiniGameCharacter::TouchStarted );
	PlayerInputComponent->BindTouch( IE_Released, this, &AMiniGameCharacter::TouchStopped );

	// VR headset functionality
	PlayerInputComponent->BindAction( "ResetVR", IE_Pressed, this, &AMiniGameCharacter::OnResetVR );
}

void AMiniGameCharacter::NotifyActorBeginOverlap( AActor* OtherActor )
{
	AMiniGameCharacter* otherCharacter = nullptr;
	otherCharacter = Cast< AMiniGameCharacter >( OtherActor );
}

void AMiniGameCharacter::NotifyHit( UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit )
{
	Super::NotifyHit( MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit );

	if ( Other->IsA( AWall::StaticClass() ) )
	{
		// 충돌 시간
		float HitTime = Hit.Time;

		// 충돌한 Actor의 속도 벡터
		FVector OtherActorVelocity = Other->GetVelocity();

		// Character의 이전 속도 벡터
		FVector PreviousVelocity = GetVelocity();

		// 충돌 지점의 법선 벡터
		FVector tempHitNormal = Hit.ImpactNormal;

		// 이전 속도 벡터와 충돌 지점의 법선 벡터를 정규화
		PreviousVelocity.Normalize();
		tempHitNormal.Normalize();

		// 이전 속도 벡터와 법선 벡터의 내적을 계산
		float DotProduct = FVector::DotProduct(tempHitNormal, PreviousVelocity);

		// 내적값을 사용하여 Character가 충돌 지점에서 멈춰있는 Actor의 어느 방향으로 튕겨져 나가야 하는지 계산
		FVector ReflectedVelocity = PreviousVelocity - 2.0f * DotProduct * tempHitNormal;

		// 새로운 속도 벡터 설정
		float ImpulseStrength = 2000.0f;

		LaunchCharacter( ReflectedVelocity * ImpulseStrength, true, true );

		m_bPlayerCanControll = false;

		return;
	}

	/*
	AMiniGameCharacter* otherCharacter = nullptr;
	otherCharacter = Cast< AMiniGameCharacter >( Other );

	if (otherCharacter)
	{
		// 충돌한 위치에서 튕겨져 나가는 방향을 계산
		FVector AwayFromOther = ( GetActorLocation() - otherCharacter->GetActorLocation() ).GetSafeNormal();

		// 튕겨져 나가는 힘의 크기를 설정
		float BounceForceMagnitude = 5000.f;

		// 힘을 계산하여 AddImpulse 함수를 사용하여 튕겨져 나가도록 처리
		FVector BounceForce = AwayFromOther * BounceForceMagnitude;
		GetCharacterMovement()->AddImpulse( BounceForce, true );
	}
	*/
}

void AMiniGameCharacter::OnResetVR()
{
	// If MiniGame is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in MiniGame.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMiniGameCharacter::TouchStarted( ETouchIndex::Type FingerIndex, FVector Location )
{
	Jump();
}

void AMiniGameCharacter::TouchStopped( ETouchIndex::Type FingerIndex, FVector Location )
{
	StopJumping();
}

void AMiniGameCharacter::TurnAtRate( float Rate )
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput( Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() );
}

void AMiniGameCharacter::LookUpAtRate( float Rate )
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput( Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() );
}

void AMiniGameCharacter::MoveForward( float Value )
{
	if ( (Controller != nullptr) && (Value != 0.0f) && (m_bPlayerCanControll == true) )
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation( 0, Rotation.Yaw, 0 );

		// get forward vector
		const FVector Direction = FRotationMatrix( YawRotation ).GetUnitAxis( EAxis::X );
		AddMovementInput( Direction, Value );
	}
}

void AMiniGameCharacter::MoveRight( float Value )
{
	if ( (Controller != nullptr) && (Value != 0.0f) && (m_bPlayerCanControll == true) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation( 0, Rotation.Yaw, 0 );
	 
		// get right vector 
		const FVector Direction = FRotationMatrix( YawRotation ).GetUnitAxis( EAxis::Y );
		// add movement in that direction
		AddMovementInput( Direction, Value );
	}
}

