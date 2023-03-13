// Fill out your copyright notice in the Description page of Project Settings.


#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"


class UUserWidget;
class UMainUI;
class ULogInUI;
class UWorld;


// pair< TSubclassOf< UUserWidget >를 Value로 하는 TMap 컨테이너의 Key로 사용
// Widget BP의 경로 Fstring을 Value로 하는 TMap 컨테이너의 Key로 사용
enum EUIPathKey : int32
{
	LOGIN = 0,
	MAIN = 1,
	SIGNUP = 2,
	ENDPATH = 3,
};


class MINIGAME_API UIManager
{
public:
	static UIManager& GetInstance()
	{
		static UIManager instance;
		return instance;
	}

private:
	UIManager();
	~UIManager();

	// enum값을 key, WidgetBlueprint 경로를 Value로 저장
	TMap<int32, FString > m_UIClassPath_Map; 

	// enum값을 key, WidgetBlueprint 클래스와 WidgetBlueprint 포인터 객체를 pair 형태로 value로 저장
	TMap<int32, std::pair< TSubclassOf< UUserWidget >, UUserWidget* > > m_UI_Map;

public:
	// key 값을 통해 원하는 Widget 반환
	UUserWidget* GetWidget( int32 uiIndex );

	// 타입 T와 key값을 통해 원하는 Widget 생성 후 출력
	template< typename T >
	void CreateUI ( UWorld* world, int32 uiIndex )
	{
		if ( world == nullptr || m_UI_Map.Find( uiIndex ) == nullptr )
			return;

		m_UI_Map[ uiIndex ].first = ConstructorHelpersInternal::FindOrLoadClass( m_UIClassPath_Map[ uiIndex ], T::StaticClass() );

		if ( m_UI_Map[ uiIndex ].first == nullptr )
			return;

		m_UI_Map[ uiIndex ].second = CreateWidget< UUserWidget >( world, m_UI_Map[ uiIndex ].first );

		if ( Cast< T >( m_UI_Map[ uiIndex ].second ) == nullptr )
			return;
	}

	void AddUI( int32 uiIndex );
	void RemoveUI( int32 uiIndex );
};
