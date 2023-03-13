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

	// �������� ��Ŷ�� ������ �ֱ⸦ üũ�� ����
	float m_PeriodSendToServer; 
	// �浹�� �߻����� �� ������� ���� �Է��� ������� �ʴ� ��Ÿ���� üũ�� ����
	float m_CollisionCoolTime; 
	// ��ų�� ����� ������ �浹���� �� ����� ���� ���� �ð��� üũ�� ����
	float m_PlayerStunTime; 
	float m_XLocation; 
	float m_YLocation;
	// �����κ��� ������Ʈ �޴� �ٸ� ������ �ӵ�
	float m_Speed; 
	// ��ų�� ����ϱ� ���� ������
	float m_MP; 

	// ���� �������� ĳ���� ���� �ε���
	int32 m_OwnerIndex; 

	// FVector m_StartLocation;
	// �����κ��� ���� Ÿ ������ ��ġ ����
	FVector m_TargetLocation;  
	// �����κ��� ���� Ÿ ������ ȸ�� ����
	FVector m_TargetDirection; 
	// FVector m_Velocity;
	 
	// bool m_bIsRun; 
	// �浹 ��Ÿ�� ���� ĳ���� ���� ���ѿ� �̿��� �÷��� ����
	bool m_bPlayerCanControll;
	// �÷��̾ ���� �������� ���θ� ������ ����
	bool m_bPlayerStun; 
	// �÷��̾ ��ų ��뿡 ���� ������������ ���θ� �����ϴ� ����
	bool m_bStrong; 

public:
	AMiniGameCharacter();
	~AMiniGameCharacter();

	// �����κ��� Ÿ ������ ��ġ ������ �޾� ���� �������� ������ �Լ�
	void SmoothMoveToLocation( const FVector& TargetLocation, float DeltaTime ); 
	// �÷��̾��� ���� �ʱ� ��ġ�� �������� �Լ�
	void SetDefaultLocation( float x, float y );
	// �÷��̾�� �浹���� ���� �������� �����ϴ� �Լ�
	void ApplyPlayerForces( int owners[] );
	void ApplyWallForces( FVector& previousVelocity, FVector& hitNormal );

	///////////* ��� ������ ���� Get, Set �Լ� *//////////
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
	
	// �ٸ� ������Ʈ�� �浹 �� �ڵ� ȣ��Ǵ� �Լ� -> ������ �浹 �� ���� ���꿡 ���
	virtual void NotifyHit( class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, 
							bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit ) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category=Camera )
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category=Camera )
	float BaseLookUpRate;

protected:
	// ĳ���Ͱ� ������ ���� ���� �Ӽ�
	UPROPERTY( EditAnywhere )
	int32 m_Color;

	UPROPERTY( EditAnywhere )
	FName m_NickName;

	UPROPERTY( EditAnywhere )
	FName m_CharacterType;

	// ĳ���ͳ����� �浹 �� ����Ǵ� ���� ũ��
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float m_ForceCollisionPlayer;

	// ���� �浹 �� ����Ǵ� ���� ũ��
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float m_ForceCollisionWall;

	// �����κ��� �ٸ� �÷��̾� ��ġ ������ �޾Ҵ����� ���� ���θ� �����ϴ� ����
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
