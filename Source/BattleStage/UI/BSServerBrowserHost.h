// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/BSUIHost.h"
#include "OnlineSessionSettings.h"
#include "BSServerBrowserHost.generated.h"

USTRUCT(BlueprintType)
struct FServerListEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString CurrentMap;

	UPROPERTY(BlueprintReadOnly)
	FString GameMode;

	/** Ping of the session */
	UPROPERTY(BlueprintReadOnly)
	int32 PingInMs;

	UPROPERTY(BlueprintReadOnly)
	int32 AvailableConnections;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxConnections;

	/** Search result this entry was derived from. */
	FOnlineSessionSearchResult SearchResult;
};

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class BATTLESTAGE_API UBSServerBrowserHost : public UBSUIHost
{
	GENERATED_UCLASS_BODY()

public:

	//-----------------------------------------------------------------
	// Session Search
	//-----------------------------------------------------------------
	
	UFUNCTION(BlueprintCallable, Category = ServerBrowser)
	bool SearchForSessions();
	
	const TArray<FServerListEntry>& GetSearchResults() const;

	/**
	* Updates the current search status.
	* 
	* @returns True if the search status changed.
	*/
	UFUNCTION(BlueprintCallable, Category = ServerBrowser)
	bool UpdateSearchStatus();

	UFUNCTION(BlueprintCallable, Category = ServerBrowser)
	bool IsSearchInProgress() const;

	UFUNCTION(BlueprintCallable, Category = ServerBrowser)
	bool IsSearchFinished() const;

	UFUNCTION(BlueprintCallable, Category = ServerBrowser)
	bool IsSearchedFailed() const;

	//-----------------------------------------------------------------
	// Session Join
	//-----------------------------------------------------------------
	
	UFUNCTION(BlueprintCallable, Category = ServerBrowser)
	bool JoinSession(const FServerListEntry& Entry); 

	/** UBSUIHost Interface Begin */
	virtual void Open() override;
	virtual void Close() override;
	/** UBSUIHost Interface End */

protected:
	void UpdateServerList(class ABSGameSession* GameSession);

protected:
	// The widget class used for the server browser UI
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ServerBrowser)
	TSubclassOf<UBSHostedWidget> BrowserWidgetClass;

private:
	// Reference to the hosted browser UI widget
	UBSHostedWidget* BrowserWidget = nullptr;

	// Lastest server list retrieved on last call to UpdateSearchStatus
	UPROPERTY(BlueprintReadOnly, Category = ServerBrowser, meta = (AllowPrivateAccess = true))
	TArray<FServerListEntry> CurrentServerList;

	// The current search status. May not match GameSession search status
	// if UpdateSearchStatus() has not been called.
	EOnlineAsyncTaskState::Type SearchStatus;
};
