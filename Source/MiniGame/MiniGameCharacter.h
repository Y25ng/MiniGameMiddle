// Copyright Epic Games, Inc. All Rights Reserved.


#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Protocol.h"
#include "MiniGameCharacter.generated.h"


class UAnimMontage;


UCLASS( config=Game )
class AMiniGameCharacter : public ACharacter
{
private:
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = ( AllowPrivateAccess = "true" ) )
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = ( AllowPrivateAccess = "true" ) )
	class UCameraComponent* FollowCamera;

	// 서버에게 패킷을 보내는 주기를 체크할 변수
	float m_PeriodSendToServer; 
	// 충돌이 발생했을 때 사용자의 조작 입력이 적용되지 않는 쿨타임을 체크할 변수
	float m_CollisionCoolTime; 
	// 스킬을 사용한 유저와 충돌했을 때 적용된 스턴 지속 시간을 체크할 변수
	float m_PlayerStunTime; 
	float m_XLocation; 
	float m_YLocation;
	// 서버로부터 업데이트 받는 다른 유저의 속도
	float m_Speed; 
	// 스킬을 사용하기 위한 게이지
	float m_MP; 

	// 게임 내에서의 캐릭터 고유 인덱스
	int32 m_OwnerIndex; 

	// FVector m_StartLocation;
	// 서버로부터 받은 타 유저의 위치 정보
	FVector m_TargetLocation;  
	// 서버로부터 받은 타 유저의 회전 정보
	FVector m_TargetDirection; 
	// FVector m_Velocity;
	 
	// bool m_bIsRun; 
	// 충돌 쿨타임 동안 캐릭터 조작 제한에 이용할 플래그 변수
	bool m_bPlayerCanControll;
	// 플레이어가 스턴 상태인지 여부를 저장할 변수
	bool m_bPlayerStun; 
	// 플레이어가 스킬 사용에 의한 무적상태인지 여부를 저장하는 변수
	bool m_bStrong; 

public:
	AMiniGameCharacter();
	~AMiniGameCharacter();

	// 서버로부터 타 유저의 위치 정보를 받아 보간 움직임을 수행할 함수
	void SmoothMoveToLocation( const FVector& TargetLocation, float DeltaTime ); 
	// 플레이어의 게임 초기 위치를 세팅해줄 함수
	void SetDefaultLocation( float x, float y );
	// 플레이어와 충돌했을 때의 움직임을 수행하는 함수
	void ApplyPlayerForces( int owners[] );
	void ApplyWallForces( FVector& previousVelocity, FVector& hitNormal );

	///////////* 멤버 변수에 대한 Get, Set 함수 *//////////
	float GetMP() { return m_MP; }
	int32 GetColor() { return m_Color; }
	bool GetbStrong() { return m_bStrong; }
	bool GetPlayerStun() { return m_bPlayerStun; }
	void SetColor( int32 colorNum ) { m_Color = colorNum; }
	void SetOwnerIndex( int var ) { m_OwnerIndex = var; }
	void SetbRecvLocation( bool var ) { m_bRecvLocation = var; }
	// void SetStartLocation( FVector var ) { m_StartLocation = var; }
	void SetTargetLocation( FVector var ) { m_TargetLocation = var; }
	void SetTargetDirection( FVector var ) { m_TargetDirection = var; }
	void SetSpeed( float var ) { m_Speed = var; }
	// void SetIsRun( bool var ) { m_bIsRun = var; }
	void SetPlayerCanControll() { m_bPlayerCanControll = true; }
	void SetbStrong( bool var ) { m_bStrong = var; }	
	void SetMP( float var ) { m_MP = var; }
	void SetPlayerStun( bool var ) { m_bPlayerStun = var; }
	void SetNickName( FString var ) { m_NickName = FName( *var ); }
	
	// 다른 오브젝트와 충돌 시 자동 호출되는 함수 -> 벽과의 충돌 시 물리 연산에 사용
	virtual void NotifyHit( class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, 
							bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit ) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category=Camera )
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category=Camera )
	float BaseLookUpRate;

protected:
	// 캐릭터가 가지는 고유 색깔 속성
	UPROPERTY( EditAnywhere )
	int32 m_Color;

	UPROPERTY( EditAnywhere )
	FName m_NickName;

	UPROPERTY( EditAnywhere )
	FName m_CharacterType;

	// 캐릭터끼리의 충돌 시 적용되는 힘의 크기
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float m_ForceCollisionPlayer;

	// 벽과 충돌 시 적용되는 힘의 크기
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float m_ForceCollisionWall;

	// 서버로부터 다른 플레이어 위치 정보를 받았는지에 대한 여부를 저장하는 변수
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	bool m_bRecvLocation;

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaTime ) override;

	void SkillStun();

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward( float Value );

	/** Called for side to side input */
	void MoveRight( float Value );

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate( float Rate );

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate( float Rate );

	/** Handler for when a touch input begins. */
	void TouchStarted( ETouchIndex::Type FingerIndex, FVector Location );

	/** Handler for when a touch input stops. */
	void TouchStopped( ETouchIndex::Type FingerIndex, FVector Location );

	/*
	UPROPERTY( EditDefaultsOnly, Category = Pawn )
		UAnimMontage* m_RunAnim;
	*/

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent ) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
