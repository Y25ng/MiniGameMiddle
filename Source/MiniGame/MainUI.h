// Fill out your copyright notice in the Description page of Project Settings.


#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"


class UImage;
class UTextBlock;


UCLASS()
class MINIGAME_API UMainUI : public UUserWidget
{
	GENERATED_BODY()

private:
	// 미니맵을 보여줄 이미지
	UPROPERTY( meta = ( BindWidget ) )
		UImage* miniMap_Image = nullptr;

	UPROPERTY( meta = ( BindWidget ) )
		UTextBlock* MP;

public:
	UMainUI( const FObjectInitializer& ObjectInitializer );
	void NativeConstruct() override;

	// 메인 UI를 생성하고 화면에 출력하는 함수
	void CreateUI();

	UTextBlock* GetMPTextBlock() { return MP; }

	void UpdateMP();
};
