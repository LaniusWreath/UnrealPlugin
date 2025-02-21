// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IWebSocket.h"
#include "SFCWebSocketManager.generated.h"

/**
 * 
 */

DECLARE_DELEGATE_OneParam(FOnMessageReceivedDelegate, const FString&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceivedDynamicDelegateOneParam, const FString&, ReceivedMessage);

UCLASS(BlueprintAble)
class SMARTFACTORY_API USFCWebSocketManager : public UObject
{
	GENERATED_BODY()

private:
	TSharedPtr<IWebSocket> WebSocket;

protected:

	FString TempResultResponseString;

	// ----------------------------- Binding functions ---------------------------------
	
	// Connection event binding
	virtual void OnConnected();
	// Closing event binding
	virtual void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
	// WebSocket message handling
	virtual void OnMessageReceived(const FString& Message);
	virtual void OnError(const FString& ErrorMessage);

public:
	USFCWebSocketManager();

	// Call function to send message
	UFUNCTION(BlueprintCallable, Category = "SFC|WebSocket")
	void SendMessage(const FString& NotifyString);

	// Delete socket object
	UFUNCTION(BlueprintCallable, Category = "SFC|WebSocket")
	void Disconnect();
	
	UFUNCTION(BlueprintCallable, Category = "SFC|WebSocket")
	void Connect(const FString& ServcerAddress);

	UFUNCTION(BlueprintCallable, Category = "SFC|WebSocket")
	static USFCWebSocketManager* CreateWebSocketManagerInstance(UObject* Outer, TSubclassOf<USFCWebSocketManager> ManagerClass );

	UFUNCTION(BlueprintPure, Category = "SFC|WebSocket")
	bool IsConnected();

	UFUNCTION(BlueprintPure, Category = "SFC|HTTP")
	const FString& GetTempResultResponseString()
	{
		return TempResultResponseString;
	}

public:
	// Blueprint Expose Delegate
	UPROPERTY(BlueprintAssignable)
	FOnMessageReceivedDynamicDelegateOneParam OnMessageReceivedEvent;

	// Inner Delegate
	FOnMessageReceivedDelegate OnMessageReceivedDelegate;


};
