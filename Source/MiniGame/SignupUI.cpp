// Fill out your copyright notice in the Description page of Project Settings.


#include "SignupUI.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Protocol.h"
#include "ServerManager.h"
#include "UserManager.h"


void USignupUI::NativeConstruct()
{
	if ( Text_IDCondition != nullptr )
	{
		Text_IDCondition->SetRenderOpacity( 0.0f );
	}

	if ( Text_PWCondition != nullptr )
	{
		Text_PWCondition->SetRenderOpacity( 0.0f );
	}

	if ( Text_SignupCondition != nullptr )
	{
		Text_SignupCondition->SetRenderOpacity( 0.0f );
	}

	if ( Btn_SignUp != nullptr )
	{
		Btn_SignUp->OnClicked.AddDynamic( this, &USignupUI::Btn_SignUp_Func );
	}

	if ( Btn_Exit != nullptr )
	{
		Btn_Exit->OnClicked.AddDynamic( this, &USignupUI::Btn_Exit_Func );
	}
}

void USignupUI::Btn_SignUp_Func()
{
	Text_SignupCondition->SetRenderOpacity( 0.0f );

	FString tempUserID = ( EditableBox_ID->GetText() ).ToString();
	FString tempUserPW = ( EditableBox_PW->GetText() ).ToString();

	bool bCanSignUp = true;

	if ( tempUserID.Len() > 6 )
	{
		Text_IDCondition->SetRenderOpacity( 1.0f );
		bCanSignUp = false;
	}
	else
	{
		Text_IDCondition->SetRenderOpacity( 0.0f );
	}

	if ( tempUserPW.Len() > 10 )
	{
		Text_PWCondition->SetRenderOpacity( 1.0f );
		bCanSignUp = false;
	}
	else
	{
		Text_PWCondition->SetRenderOpacity( 0.0f );
	}

	if ( bCanSignUp == false )
	{
		return;
	}

	char* userID = TCHAR_TO_ANSI( *tempUserID );
	char* userPW = TCHAR_TO_ANSI( *tempUserPW );

	Packet::SignUpRequest objSignUpRequest( UserManager::GetInstance().GetMainCharacterIndex() );

	for ( int i = 0; i < tempUserID.Len(); i++ )
	{
		objSignUpRequest.name[ i ] = userID[ i ];
	}

	for ( int i = 0; i < tempUserPW.Len(); i++ )
	{
		objSignUpRequest.password[ i ] = userPW[ i ];
	}

	objSignUpRequest.name[ tempUserID.Len() ] = 0;
	objSignUpRequest.password[ tempUserPW.Len() ] = 0;

	ServerManager::GetInstance().SendPacket( ClientToServer::SIGNUP_REQUEST, &objSignUpRequest );
}

void USignupUI::Btn_Exit_Func()
{
	RemoveFromViewport();
}

void USignupUI::SetSignupConditionOpacity( float var )
{ 
	Text_SignupCondition->SetRenderOpacity( var ); 
}

