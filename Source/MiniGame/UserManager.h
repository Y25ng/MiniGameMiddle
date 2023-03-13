// Fill out your copyright notice in the Description page of Project Settings.


#pragma once


#include "CoreMinimal.h"
#include "Protocol.h"


class AMiniGameCharacter;

enum ECollisionNum
{
	ONEPLAYER = 1,
	TWOPLAYER = 2,
};

namespace UserManagerFloatInfo
{
	const float MinSpeedToRecv = 10.0f;
}

class MINIGAME_API UserManager
{
private:
	UserManager();
	~UserManager();

	// �������� 3���� �÷��̾�� �ο��ϴ� owner ���� key��, �÷��̾� ��ü�� value������ TMap ���·� �����ϴ� �����̳�
	TMap< int32, AMiniGameCharacter* > PlayerMap;

	// ���� �������� �÷��̾� key��
	int32 m_MainCharacterKey;

	UWorld* m_world;

public:

	static UserManager& GetInstance()
	{
		static UserManager instance;
		return instance;
	}

	void PushPlayer( int32 key, AMiniGameCharacter* character );
	const TMap< int32, AMiniGameCharacter* >& GetPlayerMap() { return PlayerMap; }


	// 3���� �÷��̾ �������� �� �� �÷��̾��� ó�� ���� ��ġ�� �� �÷��̾ ���� �� ������ �Ҵ�
	void SetPlayerDefaultInfo( int32 key, float x, float y, float directionX, float directionY, int32 color  );

	void SetMainCharacterIndex( int32 key ) { m_MainCharacterKey = key; }
	int32 GetMainCharacterIndex() { return m_MainCharacterKey; }
	int32 GetCharacterColor( int32 key ); 
	void SetCharacterMoveInfo( Packet::Move& p );
	void SetWorld( UWorld* world ) { m_world = world; }
	UWorld* GetWorld() { return m_world; }
};

