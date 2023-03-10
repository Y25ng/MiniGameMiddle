// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MINIGAME_API TimeManager
{

private:
	TimeManager();
	~TimeManager();

	int32 m_GameTimeSec;

public:

	static TimeManager& GetInstance()
	{
		static TimeManager instance;
		return instance;
	}

	void SetGameTimeSec( int32 val ) { m_GameTimeSec = val; }
	int32 GetGameTimeSec() { return m_GameTimeSec; }

};
