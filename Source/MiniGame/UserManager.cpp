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

// 나를 제외한 플레이어의 캐릭터들의 움직임에 대한 정보 세팅 함수
void UserManager::SetCharacterMoveInfo( Packet::Move& p )
{
	// 캐릭터의 시작 위치 정보와 목표 위치 정보 할당
	FVector tempLocation = FVector( p.x, p.y, UserManager::GetInstance().GetPlayerMap()[ p.owner ]->GetActorLocation().Z );
	UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetStartLocation( UserManager::GetInstance().GetPlayerMap()[ p.owner ]->GetActorLocation() );
	UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetTargetLocation( tempLocation );

	// 캐릭터가 현재 향하고 있는 방향 정보 할당
	FVector tempDirection = FVector( p.directionX, p.directionY, UserManager::GetInstance().GetPlayerMap()[ p.owner ]->GetActorForwardVector().Z );
	UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetTargetDirection( tempDirection );

	float tempSpeed = p.speed;
	UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetSpeed( tempSpeed );

	// 서버로부터 캐릭터의 움직임 정보를 받을지 말지의 대한 여부를 나타내는 플래그 변수에 true 값 할당
	UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetbRecvLocation( true );
}

