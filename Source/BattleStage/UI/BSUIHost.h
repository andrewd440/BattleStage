// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "BSUIHost.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BATTLESTAGE_API UBSUIHost : public UObject
{
	GENERATED_BODY()

public:
	/**
	* Gives control to the UI host.
	* 
	* Derived classes should show any UI elements in overridden implementations.
	*/
	UFUNCTION(BlueprintCallable, Category = UIHost)
	virtual void Open();

	/**
	* Closes the UI host. 
	* If a return host is assigned, control will be given back the that host.
	* 
	* Derived classes should remove any UI elements in overridden implementations.
	*/
	UFUNCTION(BlueprintCallable, Category = UIHost)
	virtual void Close();

	/** Gets the game instance associated with this UI host*/
	class UBSGameInstance* GetGameInstance() const;

	/** Set the owning local player context for this UI host. */
	UFUNCTION(BlueprintCallable, Category = UIHost)
	void SetLocalPlayerContext(APlayerController* PlayerController);

	/** Get the owning local player context for this UI host. */
	const FLocalPlayerContext& GetLocalPlayerContext() const;

	UFUNCTION(BlueprintCallable, Category = UIHost)
	void SetReturnHost(UBSUIHost* ReturnHost);

protected:
	/**
	* Creates a hosted widget for this UI host.
	* 
	* @param WidgetClass	The type of hosted widget to create.
	* 
	* @returns The created hosted widget.
	*/
	template <typename T>
	T* CreateHostedWidget(TSubclassOf<class UBSHostedWidget> WidgetClass);

	/**
	* Creates a UI subhost under this host. This host will be set as
	* the return host on the created UI host.
	* 
	* @param HostType	The type of host to create.
	* @param Name		The name for the host.
	* 
	* @returns The created UI host.
	*/
	template <typename T>
	T* CreateSubHost(TSubclassOf<UBSUIHost> HostType, FName Name = NAME_None);

private:
	// Context for the player that owners this UI host.
	FLocalPlayerContext PlayerContext;
	
	// Host that control should be returned to when this host is closed.
	UBSUIHost* ReturnHost;
};

template <typename T>
T* UBSUIHost::CreateHostedWidget(TSubclassOf<class UBSHostedWidget> WidgetClass)
{
	T* Widget = nullptr;

	if (WidgetClass)
	{
		Widget = CreateWidget<T>(GetGameInstance(), WidgetClass);

		if (Widget)
		{
			Widget->SetWidgetHost(this);
		}
	}

	return Widget;
}

template <typename T>
T* UBSUIHost::CreateSubHost(TSubclassOf<UBSUIHost> HostType, FName Name)
{
	T* SubHost = NewObject<T>(this, HostType, Name);

	SubHost->SetLocalPlayerContext(PlayerContext.GetPlayerController());
	SubHost->SetReturnHost(this);

	return SubHost;
}