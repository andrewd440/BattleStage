// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OnlineSessionInterface.h"
#include "GameFramework/GameSession.h"
#include "BSOnlineSessionSettings.h"
#include "BSGameSession.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESTAGE_API ABSGameSession : public AGameSession
{
	GENERATED_BODY()
	
public:
	static const int32 DEFAULT_MAX_PLAYERS = 8;

public:
	ABSGameSession(const FObjectInitializer& ObjectInitializer);

	/*
	 * Creates an online session. This is an sync operation that, upon completion, will broadcast
	 * the OnSessionCreated event.
	 * 
	 * @param LocalPlayer		The player that is creating the session.
	 * @param SessionName		The name for the session.
	 * @param MaxConnections	The max number of connection allowed in the session.
	 * @param bIsLan			If this is a LAN session.
	 * @param GameType			The gametype for the session.
	 * @param MapName			The name of the map for the session.
	 * 
	 * @return True if session creation was successful, false otherwise.
	 */
	bool CreateSession(ULocalPlayer* LocalPlayer, const FName SessionName, const int32 MaxConnections, const bool bIsLan, const FString& GameType, const FString& MapName);

	/**
	* Broadcasted when a session has been created, or failed to be created.
	*
	* @param SessionName		The name of the session that was created.
	* @param bWasSuccessful	If the session was created successfully or not.
	*/
	DECLARE_EVENT_TwoParams(ABSGameSession, FOnSessionCreatedEvent, FName /*SessionName*/, bool /*bWasSuccessful*/)
	FOnSessionCreatedEvent& OnSessionCreated() { return OnSessionCreatedEvent; }

	/*
	* Searches for online sessions. This is an sync operation that, upon completion, will broadcast
	* the OnFindSessionsComplete event.
	*
	* @param LocalPlayer		The player that is searching for sessions.
	*
	* @return True if session creation was successful, false otherwise.
	*/
	bool FindSessions(ULocalPlayer* LocalPlayer);

	/**
	* Broadcasted when a session has been created, or failed to be created.
	*
	* @param bWasSuccessful	True if any sessions were found.
	* @param SearchResults	If successful, the search results.
	*/
	DECLARE_EVENT_OneParam(ABSGameSession, FOnSessionsFoundEvent, bool /*bWasSuccessful*/)
	FOnSessionsFoundEvent& OnFindSessionsComplete() { return OnSessionsFoundEvent; }

	const TArray<FOnlineSessionSearchResult>& GetSearchResults() const;

	const EOnlineAsyncTaskState::Type GetSearchStatus() const;

	bool JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult);

	/**
	* Delegate fired when the joining process for an online session has completed
	*
	* @param SessionName	The name of the session this callback is for
	* @param Result			The result of the join operation.
	*/
	DECLARE_EVENT_TwoParams(ABSGameSession, FOnJoinSessionEvent, FName /*SessionName*/, EOnJoinSessionCompleteResult::Type /*Result*/);
	FOnJoinSessionEvent& OnJoinSessionComplete() { return OnJoinedSessionEvent; }

	/** AGameSession Interface Begin */
	virtual void ReturnToMainMenuHost() override;
	/** AGameSession Interface End */

private:	
	/**
	* Delegate fired when a session create request has completed
	*
	* @param SessionName the name of the session this callback is for
	* @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnCreateDelegateComplete(FName SessionName, bool bWasSuccessful);

	/**
	* Delegate fired when the online session search has completed.
	*
	* @param bWasSuccessful True if the search was successful, false otherwise.
	*/
	void OnFindSessionsComplete(bool bWasSuccessful);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);	

protected:
	// The current sessions search settings
	TSharedPtr<FBSOnlineSessionSearch> SearchSettings;

	// Delegates for network operations
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompletedDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	// Events broadcasted when network operations are completed
	FOnSessionCreatedEvent OnSessionCreatedEvent;
	FOnSessionsFoundEvent OnSessionsFoundEvent;
	FOnJoinSessionEvent OnJoinedSessionEvent;

private:
	// Handles for network delegates
	FDelegateHandle OnCreateSessionCompleteHandle;	
	FDelegateHandle OnFindSessionsCompleteHandle;
	FDelegateHandle OnJoinSessionCompleteHandle;
};
