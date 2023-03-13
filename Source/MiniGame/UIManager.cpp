// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManager.h"
#include "UObject/ConstructorHelpers.h"
#include "MainUI.h"
#include "LogInUI.h"
#include "MiniGameCharacter.h"


UIManager::UIManager()
{
	// Widget BP 경로 TMap 형태로 저장
	m_UIClassPath_Map.Add( EUIPathKey::LOGIN, FString("/Game/StarterContent/Blueprints/BP_LogInUI") );
	m_UIClassPath_Map.Add( EUIPathKey::MAIN, FString("/Game/StarterContent/Blueprints/BP_MainUI") );
	m_UIClassPath_Map.Add( EUIPathKey::SIGNUP, FString( "/Game/StarterContent/Blueprints/BP_SignUp" ) );

	for ( int i = EUIPathKey::LOGIN; i < EUIPathKey::ENDPATH; ++i )
	{
		TSubclassOf< UUserWidget > UIClass;
		UUserWidget* UIWidget;

		m_UI_Map.Add( i, { UIClass, UIWidget } );
	}
}

UIManager::~UIManager()
{
}

UUserWidget* UIManager::GetWidget( int32 uiIndex )
{
	if ( m_UI_Map.Find( uiIndex )->second == nullptr )
		return nullptr;

	return m_UI_Map[ uiIndex ].second;
}

void UIManager::AddUI( int32 uiIndex )
{
	if ( m_UI_Map.Find( uiIndex )->second == nullptr )
		return;

	m_UI_Map[ uiIndex ].second->AddToViewport();
}

void UIManager::RemoveUI( int32 uiIndex )
{
	if ( m_UI_Map.Find( uiIndex )->second == nullptr )
		return;

	m_UI_Map[ uiIndex ].second->RemoveFromViewport();
}


