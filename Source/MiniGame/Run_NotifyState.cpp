// Fill out your copyright notice in the Description page of Project Settings.


#include "Run_NotifyState.h"
#include "MiniGameCharacter.h"

void URun_NotifyState::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration ) // ���� ������ Begin �������� ����
{
}

void URun_NotifyState::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) // ���� ������ End �������� ����
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
