// Fill out your copyright notice in the Description page of Project Settings.


#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LogInUI.generated.h"


class UEditableTextBox;
class UButton;
class UTextBlock;


enum ECondition
{
	IDLENGTH = 6,
	PWLENGTH = 10,
};


UCLASS()
class MINIGAME_API ULogInUI : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY( meta = ( BindWidget ) )
	UEditableTextBox* EditableBox_ID;

	UPROPERTY( meta = ( BindWidget ) )
	UEditableTextBox* EditableBox_PW;

	UPROPERTY( meta = ( BindWidget ) )
	UButton* Btn_LogIn;

	UPROPERTY( meta = ( BindWidget ) )
	UButton* Btn_SignUp;

	UPROPERTY( meta = ( BindWidget ) )
	UTextBlock* Text_IDCondition;

	UPROPERTY( meta = ( BindWidget ) )
	UTextBlock* Text_PWCondition;

	UPROPERTY( meta = ( BindWidget ) )
	UTextBlock* Text_LogInCondition;

public:
	void NativeConstruct() override;

	UFUNCTION()
	void Btn_LogIn_Func();

	UFUNCTION()
	void Btn_SignUp_Func();

	UFUNCTION()
	void SetLogInConditionOpacity( float var );
};
