// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/BSUIHost.h"
#include "BSServerBrowserHost.generated.h"

USTRUCT(BlueprintType)
struct FServerBrowserRecord
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
};

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class BATTLESTAGE_API UBSServerBrowserHost : public UBSUIHost
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = ServerBrowser)
	bool SearchForSessions();
	
	UFUNCTION(BlueprintCallable, Category = ServerBrowser)
	TArray<FServerBrowserRecord> GetSearchResults();

	//UFUNCTION(BlueprintCallable, Category = ServerBrowser)
	//bool JoinSession(); 

	/** UBSUIHost Interface Begin */
	virtual void Open() override;
	virtual void Close() override;
	/** UBSUIHost Interface End */

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ServerBrowser)
	TSubclassOf<UBSHostedWidget> BrowserWidgetClass;

private:
	UBSHostedWidget* BrowserWidget;
};
