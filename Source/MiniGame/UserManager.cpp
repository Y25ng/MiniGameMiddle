// Fill out your copyright notice in the Description page of Project Settings.


#include "UserManager.h"
#include "MiniGameCharacter.h"
// #include "ServerManager.h"


UserManager::UserManager()
{
}

UserManager::~UserManager()
{
}

void UserManager::PushPlayer( int32 key, AMiniGameCharacter* character )
{
	PlayerMap.Add( key, character );
	int32 num = PlayerMap.Num();
}

void UserManager::SetPlayerDefaultInfo( int32 key, float x, float y, float directionX, float directionY, int32 color )
{
	if ( PlayerMap.Find( key ) == nullptr )
		return;

	AMiniGameCharacter* tempCharacter = PlayerMap[ key ];

	tempCharacter->SetColor( color );
	tempCharacter->SetDefaultLocation( x, y );
	tempCharacter->SetOwnerIndex(key);

	FVector tempDirection = FVector( directionX, directionY, tempCharacter->GetActorForwardVector().Z );
	tempCharacter->SetActorRotation( tempDirection.Rotation() );
}

int32 UserManager::GetCharacterColor( int32 key )
{
	if ( PlayerMap[ key ] == nullptr )
		return -1;

	return  PlayerMap[ key ]->GetColor();
}

// ���� ������ �÷��̾��� ĳ���͵��� �����ӿ� ���� ���� ���� �Լ�
void UserManager::SetCharacterMoveInfo( Packet::Move& p )
{
	// ĳ������ ���� ��ġ ������ ��ǥ ��ġ ���� �Ҵ�
	FVector tempLocation = FVector( p.x, p.y, UserManager::GetInstance().GetPlayerMap()[ p.owner ]->GetActorLocation().Z );
	// UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetStartLocation( UserManager::GetInstance().GetPlayerMap()[ p.owner ]->GetActorLocation() );
	UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetTargetLocation( tempLocation );

	// ĳ���Ͱ� ���� ���ϰ� �ִ� ���� ���� �Ҵ�
	FVector tempDirection = FVector( p.directionX, p.directionY, UserManager::GetInstance().GetPlayerMap()[ p.owner ]->GetActorForwardVector().Z );
	UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetTargetDirection( tempDirection );

	float tempSpeed = p.speed;
	UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetSpeed( tempSpeed );

	// �����κ��� ĳ������ ������ ������ ������ ������ ���� ���θ� ��Ÿ���� �÷��� ������ true �� �Ҵ�
	UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetbRecvLocation( true );
}

