// Fill out your copyright notice in the Description page of Project Settings.


#pragma once


#include "CoreMinimal.h"


enum ETimeType
{
	DELETELOBBYBOTTOM = 3,
	GAMESTART = 6,
};

class MINIGAME_API TimeManager
{

private:
	TimeManager();
	~TimeManager();

	int32 m_GameTimeSec;
	bool m_bGameStart;

public:

	static TimeManager& GetInstance()
	{
		static TimeManager instance;
		return instance;
	}

	void SetGameTimeSec( int32 val ) { m_GameTimeSec = val; }
	int32 GetGameTimeSec() { return m_GameTimeSec; }
	bool GetbGameStart() { return m_bGameStart; }
	void SetbGameStart( bool var ) { m_bGameStart = var; }
};
