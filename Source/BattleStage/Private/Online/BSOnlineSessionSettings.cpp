// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSOnlineSessionSettings.h"

FBSOnlineSessionSettings::FBSOnlineSessionSettings(const int32 PublicConnections, const bool bIsLan, const FString& MapName, const FString& GameType)
	: FOnlineSessionSettings()
{
	NumPublicConnections = PublicConnections;
	bShouldAdvertise = true;
	bAllowJoinInProgress = false;
	bIsLANMatch = bIsLan;
	bUsesPresence = true;
	bAllowJoinViaPresence = true;

	Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);
	Set(SETTING_GAMEMODE, GameType, EOnlineDataAdvertisementType::ViaOnlineService);
}

FBSOnlineSessionSettings::~FBSOnlineSessionSettings()
{
}

FBSOnlineSessionSearch::FBSOnlineSessionSearch(const bool bIsLan, const int32 MaxResults)
	: FOnlineSessionSearch()
{
	MaxSearchResults = MaxResults;
	bIsLanQuery = bIsLan;
}

FBSOnlineSessionSearch::~FBSOnlineSessionSearch()
{

}
