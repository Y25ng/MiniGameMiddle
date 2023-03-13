// Fill out your copyright notice in the Description page of Project Settings.


#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignupUI.generated.h"


class UEditableTextBox;
class UButton;
class UTextBlock;


UCLASS()
class MINIGAME_API USignupUI : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY( meta = ( BindWidget ) )
		UEditableTextBox* EditableBox_ID;

	UPROPERTY( meta = ( BindWidget ) )
		UEditableTextBox* EditableBox_PW;

	UPROPERTY( meta = ( BindWidget ) )
		UButton* Btn_SignUp;

	UPROPERTY( meta = ( BindWidget ) )
		UButton* Btn_Exit;

	UPROPERTY( meta = ( BindWidget ) )
		UTextBlock* Text_IDCondition;

	UPROPERTY( meta = ( BindWidget ) )
		UTextBlock* Text_PWCondition;

	UPROPERTY( meta = ( BindWidget ) )
		UTextBlock* Text_SignupCondition;

public:
	void NativeConstruct() override;

	UFUNCTION()
	void Btn_SignUp_Func();

	UFUNCTION()
	void Btn_Exit_Func();	

	UFUNCTION()
	void SetSignupConditionOpacity( float var ); 
};
