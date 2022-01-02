#include "SessionsOnlineSubsystem.h"
#include "Public/OnlineSubsystem.h"

#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"

SessionsOnlineSubsystem::SessionsOnlineSubsystem(UWorld* world) :
m_World{world}
{
	check(world);
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::SessionsOnlineSubsystem"));
	
	OnCreateSessionCompleteInternalDelegate = FOnCreateSessionCompleteDelegate::CreateRaw(this, &SessionsOnlineSubsystem::OnCreateSessionComplete);
	OnStartSessionCompleteInternalDelegate = FOnStartSessionCompleteDelegate::CreateRaw(this, &SessionsOnlineSubsystem::OnStartOnlineGameComplete);
	OnEndSessionCompleteInternalDelegate = FOnEndSessionCompleteDelegate::CreateRaw(this, &SessionsOnlineSubsystem::OnEndOnlineGameComplete);
	OnFindSessionsCompleteInternalDelegate = FOnFindSessionsCompleteDelegate::CreateRaw(this, &SessionsOnlineSubsystem::OnFindSessionsComplete);
	OnJoinSessionCompleteInternalDelegate = FOnJoinSessionCompleteDelegate::CreateRaw(this, &SessionsOnlineSubsystem::OnJoinSessionComplete);
	OnDestroySessionCompleteInternalDelegate = FOnDestroySessionCompleteDelegate::CreateRaw(this, &SessionsOnlineSubsystem::OnDestroySessionComplete);
}

SessionsOnlineSubsystem::~SessionsOnlineSubsystem()
{
	OnCreateSessionCompleteInternalDelegate.Unbind();
	OnStartSessionCompleteInternalDelegate.Unbind();
	OnEndSessionCompleteInternalDelegate.Unbind();
	OnFindSessionsCompleteInternalDelegate.Unbind();
	OnJoinSessionCompleteInternalDelegate.Unbind();
	OnDestroySessionCompleteInternalDelegate.Unbind();
	
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::~SessionsOnlineSubsystem"));
}

bool SessionsOnlineSubsystem::CreateSession(TSharedPtr<const FUniqueNetId> userId, FName sessionName, bool isLan, bool isPresence, int32 maxNumPlayers)
{
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::CreateSession"));
	IOnlineSessionPtr sessions = GetSession();

	if (!sessions || !userId.IsValid())
	{
		return false;
	}

	m_SessionSettings = MakeShareable(new FOnlineSessionSettings());

	m_SessionSettings->bIsLANMatch = isLan;
	m_SessionSettings->bUsesPresence = isPresence;
	m_SessionSettings->NumPublicConnections = maxNumPlayers;
	m_SessionSettings->NumPrivateConnections = 0;
	m_SessionSettings->bAllowInvites = true;
	m_SessionSettings->bAllowJoinInProgress = true;
	m_SessionSettings->bShouldAdvertise = true;
	m_SessionSettings->bAllowJoinViaPresence = true;
	m_SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

	OnCreateSessionCompleteInternalDelegateHandle = sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteInternalDelegate);

	return sessions->CreateSession(*userId, sessionName, *m_SessionSettings);
}

void SessionsOnlineSubsystem::DestroySession(FName sessionName)
{
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::DestroySession"));
	IOnlineSessionPtr sessions = GetSession();

	if (!sessions)
	{
		return;
	}
	
	OnDestroySessionCompleteDelegateHandle = sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteInternalDelegate);
	sessions->DestroySession(sessionName);
}

void SessionsOnlineSubsystem::StartSession(FName sessionName)
{
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::StartSession"));
	IOnlineSessionPtr sessions = GetSession();

	if (sessions)
	{
		OnStartSessionCompleteDelegateHandle = sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteInternalDelegate);
		sessions->StartSession(sessionName);
	}
}

void SessionsOnlineSubsystem::EndSession(FName sessionName)
{
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::EndSession"));
	IOnlineSessionPtr sessions = GetSession();

	if (sessions)
	{
		OnEndSessionCompleteDelegateHandle = sessions->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteInternalDelegate);
		sessions->EndSession(sessionName);
	}
}

void SessionsOnlineSubsystem::FindSessions(TSharedPtr<const FUniqueNetId> userId, bool isLan, bool isPresence)
{
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::FindSessions"));
	IOnlineSessionPtr sessions = GetSession();

	if (!sessions || !userId.IsValid())
	{
		OnFindSessionsComplete(false);
		return;
	}
	
	m_SessionSearch = MakeShareable(new FOnlineSessionSearch());
	m_SessionSearch->bIsLanQuery = isLan;
	m_SessionSearch->MaxSearchResults = 20;
	m_SessionSearch->PingBucketSize = 50;
	
	if (isPresence)
	{
		m_SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, isPresence, EOnlineComparisonOp::Equals);
	}

	const TSharedRef<FOnlineSessionSearch> SearchSettingsRef = m_SessionSearch.ToSharedRef();
	OnFindSessionsCompleteDelegateHandle = sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteInternalDelegate);
	sessions->FindSessions(*userId, SearchSettingsRef);
}

bool SessionsOnlineSubsystem::JoinSession(TSharedPtr<const FUniqueNetId> userId, FName sessionName, const FString& sessionId)
{
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::JoinSession"));
	IOnlineSessionPtr sessions = GetSession();

	if (!sessions || !userId.IsValid())
	{
		return false;
	}

	OnJoinSessionCompleteDelegateHandle = sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteInternalDelegate);

	FOnlineSessionSearchResult searchResult;
	if(FillWithSessionBySessionId(sessionId, searchResult))
	{
		return sessions->JoinSession(*userId, sessionName, searchResult);
	}
	
	return false;
}

FString SessionsOnlineSubsystem::GetSubsystemName() const
{
	IOnlineSubsystem* const onlineSub = IOnlineSubsystem::Get();
	return onlineSub->GetSubsystemName().ToString();
}

FNamedOnlineSession* SessionsOnlineSubsystem::GetNamedSession(FName sessionName) const
{
	//UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::GetNamedSession"));
	IOnlineSessionPtr sessions = GetSession();

	if (!sessions)
	{
		return nullptr;
	}

	return sessions->GetNamedSession(sessionName);
}

EOnlineAsyncTaskState::Type SessionsOnlineSubsystem::GetFindSessionsStatus() const
{
	return m_SessionSearch->SearchState;
}

IOnlineSessionPtr SessionsOnlineSubsystem::GetSession() const
{
	IOnlineSubsystem* const onlineSub = IOnlineSubsystem::Get();

	if (!onlineSub)
	{
		UE_LOG(LogNet, Error, TEXT("SessionsOnlineSubsystem::GetSession No OnlineSubsystem found!"));
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
		return nullptr;
	}
	
	IOnlineSessionPtr session = onlineSub->GetSessionInterface();

	if (session.IsValid())
	{
		return session;
	}
	UE_LOG(LogNet, Error, TEXT("SessionsOnlineSubsystem::GetSession Return NULLPTR"));
	return nullptr;
}

bool SessionsOnlineSubsystem::FillWithSessionBySessionId(const FString& sessionId, FOnlineSessionSearchResult& searchResult) const
{
	check(m_SessionSearch);

	if(m_SessionSearch == nullptr)
	{
		return false;
	}
	
	for (auto&& session : m_SessionSearch->SearchResults)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Session: %s"), *SessionSearch->SearchResults[i].Session.OwningUserName));

		if (session.Session.GetSessionIdStr() == sessionId)
		{
			searchResult = session;
			return true;
		}
	}
	
	return false;
}

void SessionsOnlineSubsystem::OnCreateSessionComplete(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::OnCreateSessionComplete"));
	IOnlineSessionPtr sessions = GetSession();

	if (sessions)
	{
		sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteInternalDelegateHandle);
	}
		
	if(m_CreateSessionComplete.IsBound())
	{
		m_CreateSessionComplete.Broadcast(sessionName, wasSuccessful);
	}
}

void SessionsOnlineSubsystem::OnDestroySessionComplete(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::OnDestroySessionComplete"));
	IOnlineSessionPtr sessions = GetSession();
	if (sessions)
	{
		sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
	}
	if(m_DestroySessionComplete.IsBound())
	{
		m_DestroySessionComplete.Broadcast(sessionName, wasSuccessful);
	}
}

void SessionsOnlineSubsystem::OnStartOnlineGameComplete(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::OnStartOnlineGameComplete"));
	IOnlineSessionPtr sessions = GetSession();

	if (sessions)
	{
		sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);	
	}
	if(m_StartSessionComplete.IsBound())
	{
		m_StartSessionComplete.Broadcast(sessionName, wasSuccessful);
	}
}

void SessionsOnlineSubsystem::OnEndOnlineGameComplete(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::OnEndOnlineGameComplete"));
	IOnlineSessionPtr sessions = GetSession();

	if (sessions)
	{
		sessions->ClearOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegateHandle);	
	}
	if(m_EndSessionComplete.IsBound())
	{
		m_EndSessionComplete.Broadcast(sessionName, wasSuccessful);
	}
}

void SessionsOnlineSubsystem::OnFindSessionsComplete(bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::OnFindSessionsComplete"));
	IOnlineSessionPtr sessions = GetSession();

	if (sessions)
	{
		sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}
	if(m_FindSessionsComplete.IsBound())
	{
		m_FindSessionsComplete.Broadcast(m_SessionSearch, wasSuccessful);
	}
}

void SessionsOnlineSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	UE_LOG(LogNet, Display, TEXT("SessionsOnlineSubsystem::OnJoinSessionComplete"));
	FString travelURL;
	
	IOnlineSessionPtr sessions = GetSession();
	if (sessions)
	{
		sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		if(result == EOnJoinSessionCompleteResult::Success)
		{
			sessions->GetResolvedConnectString(sessionName, travelURL);
		}
	}

	if(m_JoinSessionComplete.IsBound())
	{
		m_JoinSessionComplete.Broadcast(travelURL, result);
	}
}