// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"
// #include "Camera/CameraComponent.h" // 카메라 헤더
#include "Components/Image.h" // UImage 사용을 위한 헤더
#include "Components/TextBlock.h"
#include "UserManager.h"
#include "MiniGameCharacter.h"
// #include "Components/SceneCaptureComponent2D.h"
// #include "Engine/TextureRenderTarget2D.h"
// #include "Kismet/GameplayStatics.h"
// #include "UObject/ConstructorHelpers.h"
// #include "CameraForMinimap.h"
// #include "UIManager.h"


UMainUI::UMainUI( const FObjectInitializer& ObjectInitializer ): Super( ObjectInitializer )
{
}

void UMainUI::NativeConstruct()
{
}


void UMainUI::CreateUI()
{
	AddToViewport();
}


void UMainUI::UpdateMP()
{
    if ( UserManager::GetInstance().GetPlayerMap().Find( UserManager::GetInstance().GetMainCharacterIndex() ) == nullptr )
        return;

    float tempMP = UserManager::GetInstance().GetPlayerMap()[ UserManager::GetInstance().GetMainCharacterIndex() ]->GetMP();

    MP->SetText( FText::FromString( FString::Printf( TEXT( "%f"), tempMP ) ) );
}
