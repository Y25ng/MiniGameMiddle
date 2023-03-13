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

	Packet::LoginRequest objLoginRequest( UserManager::GetInstance().GetMainCharacterIndex() );

	int32 idSize = strlen( TCHAR_TO_UTF8( *tempUserID ) ) + 1;
	FCStringAnsi::Strncpy( objLoginRequest.name, TCHAR_TO_UTF8( *tempUserID ), idSize );

	int32 pwSize = strlen( TCHAR_TO_UTF8( *tempUserPW ) ) + 1;
	FCStringAnsi::Strncpy( objLoginRequest.password, TCHAR_TO_UTF8( *tempUserPW ), pwSize );

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
