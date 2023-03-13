// Fill out your copyright notice in the Description page of Project Settings.


#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"


class UUserWidget;
class UMainUI;
class ULogInUI;
class UWorld;


// pair< TSubclassOf< UUserWidget >�� Value�� �ϴ� TMap �����̳��� Key�� ���
// Widget BP�� ��� Fstring�� Value�� �ϴ� TMap �����̳��� Key�� ���
enum EUIPathKey
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

	// enum���� key, WidgetBlueprint ��θ� Value�� ����
	TMap<int32, FString > m_UIClassPath_Map; 

	// enum���� key, WidgetBlueprint Ŭ������ WidgetBlueprint ������ ��ü�� pair ���·� value�� ����
	TMap<int32, std::pair< TSubclassOf< UUserWidget >, UUserWidget* > > m_UI_Map;

public:
	// key ���� ���� ���ϴ� Widget ��ȯ
	UUserWidget* GetWidget( int32 uiIndex );

	// Ÿ�� T�� key���� ���� ���ϴ� Widget ���� �� ���
	template< typename T >
	void CreateUI ( UWorld* world, int32 uiIndex )
	{
		if ( world == nullptr || m_UI_Map.Find( uiIndex ) == false )
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
