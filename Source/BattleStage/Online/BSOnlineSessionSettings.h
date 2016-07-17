// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "../OnlineSubsystem/Public/OnlineSessionSettings.h"

/**
 * Setting used for describing a BattleStage online session.
 */
class BATTLESTAGE_API FBSOnlineSessionSettings : public FOnlineSessionSettings
{
public:
	FBSOnlineSessionSettings(const int32 PublicConnections, const bool bIsLan, const FString& MapName, const FString& GameType);
	~FBSOnlineSessionSettings();
};

/*
 * Settings used to search for BattleStage online sessions.
 */
class BATTLESTAGE_API FBSOnlineSessionSearch : public FOnlineSessionSearch
{
public:
	FBSOnlineSessionSearch(const bool bIsLan = false, const int32 MaxResults = 10);
	~FBSOnlineSessionSearch();
};