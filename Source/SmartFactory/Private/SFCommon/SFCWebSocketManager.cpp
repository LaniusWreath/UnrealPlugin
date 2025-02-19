// Fill out your copyright notice in the Description page of Project Settings.

#include "SFCommon/SFCWebSocketManager.h"
#include "SFCommon/SFCLog.h"
#include "WebSocketsModule.h"

USFCWebSocketManager::USFCWebSocketManager()
{
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}
}

// 웹소켓 매니저 인스턴스 생성
USFCWebSocketManager* USFCWebSocketManager::CreateWebSocketManagerInstance(UObject* Outer, TSubclassOf<USFCWebSocketManager> ManagerClass)
{
	// 클래스 지정 안한 경우, CPP기반 StaticClass
	if (!ManagerClass)
	{
		return NewObject<USFCWebSocketManager>(Outer, USFCWebSocketManager::StaticClass());
	}
	// 클래스 지정 한 경우, BP 기반 상속 클래스
	return NewObject<USFCWebSocketManager>(Outer, ManagerClass);
}

bool USFCWebSocketManager::IsConnected()
{
	if (WebSocket)
	{
		return WebSocket->IsConnected();
	}
	else
	{
		return false;
	}
}

// 웹소켓 연결 초기화 및 연결 함수 바인딩.
void USFCWebSocketManager::Connect(const FString& ServerAddress)
{
	if (WebSocket && WebSocket->IsConnected())
	{
		Disconnect();
	}

	WebSocket = FWebSocketsModule::Get().CreateWebSocket(ServerAddress);

	// 함수 바인딩.
	WebSocket->OnConnected().AddUObject(this, &USFCWebSocketManager::OnConnected);
	WebSocket->OnConnectionError().AddUObject(this, &USFCWebSocketManager::OnError);
	WebSocket->OnClosed().AddUObject(this, &USFCWebSocketManager::OnClosed);
	WebSocket->OnMessage().AddUObject(this, &USFCWebSocketManager::OnMessageReceived);

	WebSocket->Connect();
}

// 연결 해제.
void USFCWebSocketManager::Disconnect()
{
	if (!WebSocket)
	{
		return;
	}

	if (WebSocket->IsConnected())
	{
		WebSocket->Close();
	}
}

void USFCWebSocketManager::OnConnected()
{
	UE_LOG(SFClog, Log, TEXT("WebSocket connected."));
}

void USFCWebSocketManager::OnError(const FString& ErrorMessage)
{
	UE_LOG(SFClog, Error, TEXT("WebSocket error: %s"), *ErrorMessage);
}

void USFCWebSocketManager::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	UE_LOG(SFClog, Log, TEXT("WebSocket closed. Reason: %s"), *Reason);
	// 연결 해제 후 안전하게 WebSocket 객체 초기화
	if (WebSocket)
	{
		WebSocket.Reset();
	}

}

// 메시지 받았을 때 바인딩 
void USFCWebSocketManager::OnMessageReceived(const FString& Message)
{
	UE_LOG(LogTemp, Log, TEXT("WebSocketManager: Message received: %s"), *Message);

	// 델리게이트가 바인딩되어 있으면 메시지 전달
	if (OnMessageReceivedDelegate.IsBound())
	{
		OnMessageReceivedDelegate.Execute(Message);
	}

	// 다이나믹 델리게이트는 무조건 브로드캐스트
	if (OnMessageReceivedEvent.IsBound())
	{
		OnMessageReceivedEvent.Broadcast(Message);
	}
	
}

// 서버에 메시지 송신.
void USFCWebSocketManager::SendMessage(const FString& Message)
{
	if (!WebSocket)
	{
		UE_LOG(SFClog, Error, TEXT("WebSocket not initialized"));
		return;
	}
	if (!WebSocket->IsConnected())
	{
		UE_LOG(SFClog, Error, TEXT("WebSocket server is not connected"));
		return;
	}

	WebSocket->Send(Message);
}