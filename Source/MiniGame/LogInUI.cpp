// Fill out your copyright notice in the Description page of Project Settings.


#include "LogInUI.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Protocol.h"
#include "ServerManager.h"
#include "UIManager.h"
#include "UserManager.h"
#include "SignupUI.h"


void ULogInUI::NativeConstruct()
{
	if ( Text_IDCondition != nullptr )
	{
		Text_IDCondition->SetRenderOpacity( 0.0f );
	}

	if ( Text_PWCondition != nullptr )
	{
		Text_PWCondition->SetRenderOpacity( 0.0f );
	}

	if ( Text_LogInCondition != nullptr )
	{
		Text_LogInCondition->SetRenderOpacity( 0.0f );
	}

	if ( Btn_LogIn != nullptr )
	{
		Btn_LogIn->OnClicked.AddDynamic( this, &ULogInUI::Btn_LogIn_Func );
	}

	if ( Btn_SignUp != nullptr )
	{
		Btn_SignUp->OnClicked.AddDynamic( this, &ULogInUI::Btn_SignUp_Func );
	}
}

void ULogInUI::Btn_LogIn_Func()
{
	Text_LogInCondition->SetRenderOpacity( 0.0f );

	FString tempUserID = ( EditableBox_ID->GetText() ).ToString();
	FString tempUserPW = ( EditableBox_PW->GetText() ).ToString();

	bool bCanLogIn = true;

	if ( tempUserID.Len() > ECondition::IDLENGTH )
	{
		Text_IDCondition->SetRenderOpacity( 1.0f );
		bCanLogIn = false;
	}
	else
	{
		Text_IDCondition->SetRenderOpacity( 0.0f );
	}

	if ( tempUserPW.Len() > ECondition::PWLENGTH )
	{
		Text_PWCondition->SetRenderOpacity( 1.0f );
		bCanLogIn = false;
	}
	else
	{
		Text_PWCondition->SetRenderOpacity( 0.0f );
	}

	if ( bCanLogIn == false )
	{
		return;
	}

	char* userID = TCHAR_TO_ANSI( *tempUserID );
	char* userPW = TCHAR_TO_ANSI( *tempUserPW );

	Packet::LoginRequest objLoginRequest( UserManager::GetInstance().GetMainCharacterIndex() );

	for ( int i = 0; i < tempUserID.Len(); i++ )
	{
		objLoginRequest.name[ i ] = userID[ i ];
	}

	for ( int i = 0; i < tempUserPW.Len(); i++ )
	{
		objLoginRequest.password[ i ] = userPW[ i ];
	}

	objLoginRequest.name[ tempUserID.Len() ] = 0;
	objLoginRequest.password[ tempUserPW.Len() ] = 0;

	ServerManager::GetInstance().SendPacket( ClientToServer::LOGIN_REQUEST, &objLoginRequest );
}

void ULogInUI::Btn_SignUp_Func()
{
	UIManager::GetInstance().CreateUI< USignupUI >( GetWorld(), EUIPathKey::SIGNUP );
	UIManager::GetInstance().AddUI( EUIPathKey::SIGNUP );
}

void ULogInUI::SetLogInConditionOpacity( float var )
{
	Text_LogInCondition->SetRenderOpacity( var );
}
