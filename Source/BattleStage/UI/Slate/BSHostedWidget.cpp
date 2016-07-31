// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSHostedWidget.h"
#include "UI/BSUIHost.h"


DEFINE_LOG_CATEGORY_STATIC(BSHostedWidget, Warning, All)

bool UBSHostedWidget::SetWidgetHost(UBSUIHost* InHost)
{
	bool bWasSet = false;

	if (HostType == nullptr)
	{
		UE_LOG(BSHostedWidget, Warning,
			TEXT("No HostType provided to %s."), *GetClass()->GetName());
	}
	else if (InHost && InHost->GetClass() != HostType)
	{
		UE_LOG(BSHostedWidget, Warning,
			TEXT("Incompatible UI host type provided to %s. %s was provided, expected %s."), 
			*GetClass()->GetName(), *InHost->GetClass()->GetName(), *HostType->GetName());
	}
	else
	{
		Host = InHost;
		bWasSet = true;
	}

	return bWasSet;
}
