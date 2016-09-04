// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSScoreboardWidget.h"

#include "BSScoreboardEntry.h"
#include "BSGameState.h"

static auto EntrySortPredicate = [](const UBSScoreboardEntry& Lhs, const UBSScoreboardEntry& Rhs) 
{ 
	const ABSPlayerState* const LhsState = Lhs.GetPlayerState().Get();
	const ABSPlayerState* const RhsState = Rhs.GetPlayerState().Get();

	//#bstodo Refine scoreboard ranking K/D
	return LhsState->Score >= RhsState->Score;
};

struct FEntryFindStatePredicate
{
	FEntryFindStatePredicate(TWeakObjectPtr<ABSPlayerState> InMatchingState)
		: MatchingState(InMatchingState) {}

	TWeakObjectPtr<ABSPlayerState> MatchingState;

	bool operator()(UBSScoreboardEntry* Entry)
	{
		return Entry->GetPlayerState() == MatchingState;
	}
};

UBSScoreboardWidget::UBSScoreboardWidget(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	bIsFocusable = false;
	Visibility = ESlateVisibility::HitTestInvisible;
}

void UBSScoreboardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ABSGameState* GameState = GetWorld()->GetGameState<ABSGameState>())
	{
		OnPlayJoinLeaveHandle = GameState->OnPlayerJoinLeave().AddUObject(this, &UBSScoreboardWidget::OnPlayerJoinLeaveGame);
		InitializeScoreboard(*GameState);
	}
}

void UBSScoreboardWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	bool bNeedsSort = false;

	// Check if we need to sort
	for (int i = 1; i < ScoreboardEntries.Num() && !bNeedsSort; ++i)
	{
		if (!EntrySortPredicate(*ScoreboardEntries[i - 1], *ScoreboardEntries[i]))
		{
			bNeedsSort = true;
		}
	}

	if (bNeedsSort)
	{
		// Naive reordering of entries, this only happens when the scoreboard is up and a
		// score event occurs.

		ScoreboardEntries.Sort(EntrySortPredicate);
		ScoreboardEntryPanel->ClearChildren();
		for (auto Entry : ScoreboardEntries)
		{
			ScoreboardEntryPanel->AddChild(Entry);
		}
	}
}

void UBSScoreboardWidget::OnPlayerJoinLeaveGame(APlayerState* PlayerState, bool bIsJoin)
{
	if (ABSPlayerState* BSPlayerState = Cast<ABSPlayerState>(PlayerState))
	{
		if (bIsJoin)
		{
			if (UBSScoreboardEntry* const Entry = CreateWidget<UBSScoreboardEntry>(GetOwningPlayer(), ScoreboardEntryClass))
			{
				Entry->SetPlayerState(BSPlayerState);
				ScoreboardEntries.Add(Entry);
				ScoreboardEntryPanel->AddChild(Entry);
			}
		}
		else
		{
			// Find the entry and remove it
			const FEntryFindStatePredicate EntryFinder{ BSPlayerState };			
			if (UBSScoreboardEntry** EntryPtr = ScoreboardEntries.FindByPredicate(EntryFinder))
			{
				UBSScoreboardEntry* const Entry = *EntryPtr;
				ScoreboardEntryPanel->RemoveChild(Entry);
				ScoreboardEntries.Remove(Entry);
			}
		}
	}
}

void UBSScoreboardWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (ABSGameState* GameState = GetWorld()->GetGameState<ABSGameState>())
	{
		GameState->OnPlayerJoinLeave().Remove(OnPlayJoinLeaveHandle);
	}

	ScoreboardEntries.Empty();
	ScoreboardEntryPanel->ClearChildren();	
}

void UBSScoreboardWidget::InitializeScoreboard(const ABSGameState& GameState)
{
	APlayerController* const OwningPlayer = GetOwningPlayer();

	// Create an entry for each widget
	for (const APlayerState* PlayerState : GameState.PlayerArray)
	{
		checkf(Cast<ABSPlayerState>(PlayerState) != nullptr, TEXT("In-Game PlayerState is incorrect type. Should be ABSPlayerState."));

		auto BSPlayerState = static_cast<const ABSPlayerState* const>(PlayerState);	

		if (UBSScoreboardEntry* const Entry = CreateWidget<UBSScoreboardEntry>(OwningPlayer, ScoreboardEntryClass))
		{
			Entry->SetPlayerState(BSPlayerState);
			ScoreboardEntries.Add(Entry);
		}
	}

	// Sort member array then add to panel
	ScoreboardEntries.Sort(EntrySortPredicate);

	for (auto Entry : ScoreboardEntries)
	{
		ScoreboardEntryPanel->AddChild(Entry);
	}
}