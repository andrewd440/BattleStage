// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSHUDWidget.h"


UBSHUDWidget::UBSHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsFocusable = false;
	Visibility = ESlateVisibility::HitTestInvisible;
}
