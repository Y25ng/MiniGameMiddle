// Fill out your copyright notice in the Description page of Project Settings.


#include "Run_NotifyState.h"
#include "MiniGameCharacter.h"

void URun_NotifyState::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration ) // 지정 구간의 Begin 지점에서 실행
{
}

void URun_NotifyState::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) // 지정 구간의 End 지점에서 실행
{
	if ( MeshComp != nullptr && MeshComp->GetOwner() != nullptr )
	{
		AMiniGameCharacter* Player = Cast< AMiniGameCharacter >( MeshComp->GetOwner() );

		if ( Player != nullptr )
		{
			// Player->SetIsRun( false );
		}
	}
}
