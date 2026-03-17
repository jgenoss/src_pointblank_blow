#include "pch.h"
#include "TournamentSystem.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "RoomManager.h"
#include "Room.h"
#include "GameContextMain.h"
#include "GameProtocol.h"
#include "ServerLog.h"

// ============================================================================
// Tournament System Implementation
// ============================================================================

TournamentSystem* g_pTournament = nullptr;

// ============================================================================
// Packet structures for tournament notifications
// ============================================================================

#pragma pack(push, 1)

struct TOURNEY_INFO_PACKET
{
	uint16_t	ui16Size;
	uint16_t	ui16Protocol;
	int32_t		i32Result;
	int32_t		i32TourneyId;
	char		szName[64];
	uint8_t		ui8State;
	int32_t		i32TeamCount;
	int32_t		i32MaxTeams;
	uint8_t		ui8GameMode;
	int32_t		i32BestOf;
	int32_t		i32CurrentRound;
};

struct TOURNEY_BRACKET_ENTRY
{
	int32_t		i32MatchId;
	int32_t		i32Round;
	int32_t		i32Position;
	int32_t		i32Team1Id;
	int32_t		i32Team2Id;
	int32_t		i32WinnerId;
	int32_t		i32Team1Wins;
	int32_t		i32Team2Wins;
	uint8_t		ui8State;
	char		szTeam1Name[32];
	char		szTeam2Name[32];
};

#pragma pack(pop)

// ============================================================================
// Constructor / Destructor
// ============================================================================

TournamentSystem::TournamentSystem()
	: m_i32NextTourneyId(1)
	, m_i32NextSeasonId(1)
	, m_i32CurrentSeasonIdx(-1)
	, m_dwLastUpdateTime(0)
{
	for (int i = 0; i < MAX_TOURNAMENTS; ++i)
		m_Tournaments[i].Reset();
	for (int i = 0; i < MAX_TOURNEY_SEASONS; ++i)
		m_Seasons[i].Reset();
}

TournamentSystem::~TournamentSystem()
{
}

// ============================================================================
// Initialize
// ============================================================================

bool TournamentSystem::Initialize()
{
	m_dwLastUpdateTime = GetTickCount();

	printf("[Tournament] Tournament System initialized\n");
	return true;
}

// ============================================================================
// CreateTournament
// ============================================================================

int TournamentSystem::CreateTournament(const TournamentConfig& config)
{
	TournamentData* pSlot = FindFreeTournamentSlot();
	if (!pSlot)
	{
		printf("[Tournament] ERROR: No free tournament slots\n");
		return -1;
	}

	pSlot->Reset();
	pSlot->config = config;
	pSlot->config.i32TourneyId = m_i32NextTourneyId++;
	pSlot->config.eState = TOURNEY_REGISTRATION;
	pSlot->bActive = true;

	printf("[Tournament] Created tournament: id=%d, name='%s', maxTeams=%d, bestOf=%d\n",
		pSlot->config.i32TourneyId, pSlot->config.szName,
		pSlot->config.i32MaxTeams, pSlot->config.i32BestOf);

	return pSlot->config.i32TourneyId;
}

// ============================================================================
// StartTournament
// ============================================================================

bool TournamentSystem::StartTournament(int i32TourneyId)
{
	TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
		return false;

	if (pTourney->config.eState != TOURNEY_REGISTRATION)
	{
		printf("[Tournament] Cannot start tournament %d - not in registration state\n", i32TourneyId);
		return false;
	}

	if (pTourney->i32TeamCount < 2)
	{
		printf("[Tournament] Cannot start tournament %d - need at least 2 teams\n", i32TourneyId);
		return false;
	}

	// Generate bracket
	if (!GenerateBracket(i32TourneyId))
	{
		printf("[Tournament] Failed to generate bracket for tournament %d\n", i32TourneyId);
		return false;
	}

	pTourney->config.eState = TOURNEY_IN_PROGRESS;

	printf("[Tournament] Tournament %d started: %d teams, %d rounds\n",
		i32TourneyId, pTourney->i32TeamCount, pTourney->i32TotalRounds);

	// Increment season counter
	if (m_i32CurrentSeasonIdx >= 0 && m_i32CurrentSeasonIdx < MAX_TOURNEY_SEASONS)
		m_Seasons[m_i32CurrentSeasonIdx].i32TournamentsHeld++;

	return true;
}

// ============================================================================
// CancelTournament
// ============================================================================

bool TournamentSystem::CancelTournament(int i32TourneyId)
{
	TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
		return false;

	pTourney->config.eState = TOURNEY_CANCELLED;

	// Refund entry fees if applicable
	if (pTourney->config.i32EntryFee > 0 && g_pGameSessionManager)
	{
		for (int i = 0; i < MAX_TOURNEY_TEAMS; ++i)
		{
			if (!pTourney->teams[i].bActive)
				continue;

			// Refund captain
			GameSession* pCaptain = g_pGameSessionManager->FindSessionByUID(
				pTourney->teams[i].i64CaptainUID);
			if (pCaptain)
			{
				// Would add GP back - simplified for now
				printf("[Tournament] Refund %d GP to captain UID=%lld\n",
					pTourney->config.i32EntryFee, pTourney->teams[i].i64CaptainUID);
			}
		}
	}

	printf("[Tournament] Tournament %d cancelled\n", i32TourneyId);
	return true;
}

// ============================================================================
// RegisterTeam
// ============================================================================

int TournamentSystem::RegisterTeam(int i32TourneyId, const char* pszTeamName,
								   int64_t i64CaptainUID, const char* pszCaptainNick,
								   int i32ClanId)
{
	TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
		return -1;

	if (pTourney->config.eState != TOURNEY_REGISTRATION)
	{
		printf("[Tournament] Cannot register - tournament %d not in registration\n", i32TourneyId);
		return -1;
	}

	if (pTourney->i32TeamCount >= pTourney->config.i32MaxTeams)
	{
		printf("[Tournament] Tournament %d is full (%d/%d teams)\n",
			i32TourneyId, pTourney->i32TeamCount, pTourney->config.i32MaxTeams);
		return -1;
	}

	// Check for duplicate captain
	for (int i = 0; i < MAX_TOURNEY_TEAMS; ++i)
	{
		if (pTourney->teams[i].bActive && pTourney->teams[i].i64CaptainUID == i64CaptainUID)
		{
			printf("[Tournament] Captain UID=%lld already has a team in tournament %d\n",
				i64CaptainUID, i32TourneyId);
			return -1;
		}
	}

	// Find free team slot
	TournamentTeam* pTeam = nullptr;
	for (int i = 0; i < MAX_TOURNEY_TEAMS; ++i)
	{
		if (!pTourney->teams[i].bActive)
		{
			pTeam = &pTourney->teams[i];
			break;
		}
	}

	if (!pTeam)
		return -1;

	pTeam->Reset();
	pTeam->i32TeamId = pTourney->i32NextTeamId++;
	strncpy_s(pTeam->szTeamName, pszTeamName, _TRUNCATE);
	pTeam->i64CaptainUID = i64CaptainUID;
	strncpy_s(pTeam->szCaptainNick, pszCaptainNick, _TRUNCATE);
	pTeam->i32ClanId = i32ClanId;
	pTeam->i32Seed = pTourney->i32TeamCount + 1;
	pTeam->bActive = true;

	// Add captain as first member
	pTeam->members[0].i64UID = i64CaptainUID;
	strncpy_s(pTeam->members[0].szNickname, pszCaptainNick, _TRUNCATE);
	pTeam->members[0].bActive = true;
	pTeam->i32MemberCount = 1;

	pTourney->i32TeamCount++;

	printf("[Tournament] Team '%s' (id=%d) registered in tournament %d by %s\n",
		pszTeamName, pTeam->i32TeamId, i32TourneyId, pszCaptainNick);

	return pTeam->i32TeamId;
}

// ============================================================================
// AddTeamMember / RemoveTeamMember
// ============================================================================

bool TournamentSystem::AddTeamMember(int i32TourneyId, int i32TeamId,
									  int64_t i64UID, const char* pszNickname,
									  int i32Level, int i32RankId)
{
	TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
		return false;

	TournamentTeam* pTeam = FindTeam(pTourney, i32TeamId);
	if (!pTeam)
		return false;

	if (pTeam->i32MemberCount >= pTourney->config.i32PlayersPerTeam)
		return false;

	// Check minimum level
	if (pTourney->config.i32MinLevel > 0 && i32Level < pTourney->config.i32MinLevel)
		return false;

	// Check duplicate
	for (int i = 0; i < MAX_TOURNEY_TEAM_MEMBERS; ++i)
	{
		if (pTeam->members[i].bActive && pTeam->members[i].i64UID == i64UID)
			return false;	// Already in team
	}

	// Find free member slot
	for (int i = 0; i < MAX_TOURNEY_TEAM_MEMBERS; ++i)
	{
		if (!pTeam->members[i].bActive)
		{
			pTeam->members[i].i64UID = i64UID;
			strncpy_s(pTeam->members[i].szNickname, pszNickname, _TRUNCATE);
			pTeam->members[i].i32Level = i32Level;
			pTeam->members[i].i32RankId = i32RankId;
			pTeam->members[i].bActive = true;
			pTeam->i32MemberCount++;
			return true;
		}
	}

	return false;
}

bool TournamentSystem::RemoveTeamMember(int i32TourneyId, int i32TeamId, int64_t i64UID)
{
	TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
		return false;

	TournamentTeam* pTeam = FindTeam(pTourney, i32TeamId);
	if (!pTeam)
		return false;

	// Cannot remove captain
	if (pTeam->i64CaptainUID == i64UID)
		return false;

	for (int i = 0; i < MAX_TOURNEY_TEAM_MEMBERS; ++i)
	{
		if (pTeam->members[i].bActive && pTeam->members[i].i64UID == i64UID)
		{
			pTeam->members[i].Reset();
			pTeam->i32MemberCount--;
			return true;
		}
	}

	return false;
}

// ============================================================================
// UnregisterTeam
// ============================================================================

bool TournamentSystem::UnregisterTeam(int i32TourneyId, int i32TeamId)
{
	TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
		return false;

	if (pTourney->config.eState != TOURNEY_REGISTRATION)
		return false;	// Can only unregister during registration

	TournamentTeam* pTeam = FindTeam(pTourney, i32TeamId);
	if (!pTeam)
		return false;

	printf("[Tournament] Team '%s' (id=%d) unregistered from tournament %d\n",
		pTeam->szTeamName, i32TeamId, i32TourneyId);

	pTeam->Reset();
	pTourney->i32TeamCount--;

	return true;
}

// ============================================================================
// GenerateBracket - single elimination
// ============================================================================

bool TournamentSystem::GenerateBracket(int i32TourneyId)
{
	TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
		return false;

	int teamCount = pTourney->i32TeamCount;
	if (teamCount < 2)
		return false;

	// Calculate bracket size (next power of 2)
	int bracketSize = NextPowerOf2(teamCount);
	int totalRounds = CalculateBracketRounds(bracketSize);
	pTourney->i32TotalRounds = totalRounds;

	// Collect active team IDs sorted by seed
	int teamIds[MAX_TOURNEY_TEAMS];
	int activeCount = 0;
	for (int i = 0; i < MAX_TOURNEY_TEAMS; ++i)
	{
		if (pTourney->teams[i].bActive)
			teamIds[activeCount++] = pTourney->teams[i].i32TeamId;
	}

	// Sort by seed (simple insertion sort)
	for (int i = 1; i < activeCount; ++i)
	{
		int key = teamIds[i];
		const TournamentTeam* pKeyTeam = FindTeam(pTourney, key);
		int j = i - 1;
		while (j >= 0)
		{
			const TournamentTeam* pjTeam = FindTeam(pTourney, teamIds[j]);
			if (pjTeam && pKeyTeam && pjTeam->i32Seed > pKeyTeam->i32Seed)
			{
				teamIds[j + 1] = teamIds[j];
				j--;
			}
			else
				break;
		}
		teamIds[j + 1] = key;
	}

	// Generate first round matches
	int firstRoundMatches = bracketSize / 2;
	int matchCount = 0;

	for (int i = 0; i < firstRoundMatches; ++i)
	{
		TournamentMatch& match = pTourney->matches[matchCount];
		match.Reset();
		match.i32MatchId = pTourney->i32NextMatchId++;
		match.i32BracketRound = 0;
		match.i32BracketPosition = i;
		match.i32BestOf = pTourney->config.i32BestOf;

		// Standard bracket seeding: 1 vs bracketSize, 2 vs bracketSize-1, etc.
		int seedIdx1 = i;
		int seedIdx2 = bracketSize - 1 - i;

		if (seedIdx1 < activeCount)
			match.i32Team1Id = teamIds[seedIdx1];
		else
			match.i32Team1Id = -1;	// BYE

		if (seedIdx2 < activeCount)
			match.i32Team2Id = teamIds[seedIdx2];
		else
			match.i32Team2Id = -1;	// BYE

		// Handle BYEs (auto-advance)
		if (match.i32Team1Id == -1 && match.i32Team2Id != -1)
		{
			match.i32WinnerId = match.i32Team2Id;
			match.eState = TMATCH_BYE;
		}
		else if (match.i32Team2Id == -1 && match.i32Team1Id != -1)
		{
			match.i32WinnerId = match.i32Team1Id;
			match.eState = TMATCH_BYE;
		}
		else if (match.i32Team1Id == -1 && match.i32Team2Id == -1)
		{
			match.eState = TMATCH_BYE;
		}
		else
		{
			match.eState = TMATCH_PENDING;
		}

		matchCount++;
	}

	// Generate subsequent round placeholder matches
	int matchesInRound = firstRoundMatches / 2;
	for (int round = 1; round < totalRounds; ++round)
	{
		for (int pos = 0; pos < matchesInRound; ++pos)
		{
			TournamentMatch& match = pTourney->matches[matchCount];
			match.Reset();
			match.i32MatchId = pTourney->i32NextMatchId++;
			match.i32BracketRound = round;
			match.i32BracketPosition = pos;
			match.i32BestOf = pTourney->config.i32BestOf;
			match.eState = TMATCH_PENDING;
			matchCount++;
		}
		matchesInRound /= 2;
		if (matchesInRound < 1) matchesInRound = 1;
	}

	pTourney->i32MatchCount = matchCount;
	pTourney->i32CurrentRound = 0;

	// Advance BYE winners to next round
	for (int i = 0; i < matchCount; ++i)
	{
		if (pTourney->matches[i].eState == TMATCH_BYE && pTourney->matches[i].i32WinnerId != -1)
		{
			AdvanceBracket(pTourney, pTourney->matches[i].i32MatchId);
		}
	}

	printf("[Tournament] Bracket generated: %d matches, %d rounds, bracket size %d\n",
		matchCount, totalRounds, bracketSize);

	return true;
}

// ============================================================================
// ReportMatchResult
// ============================================================================

bool TournamentSystem::ReportMatchResult(int i32TourneyId, int i32MatchId,
										  int i32Team1Wins, int i32Team2Wins)
{
	TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
		return false;

	TournamentMatch* pMatch = FindMatch(pTourney, i32MatchId);
	if (!pMatch)
		return false;

	if (pMatch->eState != TMATCH_IN_PROGRESS && pMatch->eState != TMATCH_PENDING)
		return false;

	pMatch->i32Team1Wins = i32Team1Wins;
	pMatch->i32Team2Wins = i32Team2Wins;

	int winsNeeded = (pMatch->i32BestOf / 2) + 1;

	if (i32Team1Wins >= winsNeeded)
	{
		return AdvanceWinner(i32TourneyId, i32MatchId, pMatch->i32Team1Id);
	}
	else if (i32Team2Wins >= winsNeeded)
	{
		return AdvanceWinner(i32TourneyId, i32MatchId, pMatch->i32Team2Id);
	}

	// Match still in progress
	pMatch->eState = TMATCH_IN_PROGRESS;
	return true;
}

// ============================================================================
// AdvanceWinner
// ============================================================================

bool TournamentSystem::AdvanceWinner(int i32TourneyId, int i32MatchId, int i32WinnerTeamId)
{
	TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
		return false;

	TournamentMatch* pMatch = FindMatch(pTourney, i32MatchId);
	if (!pMatch)
		return false;

	pMatch->i32WinnerId = i32WinnerTeamId;
	pMatch->eState = TMATCH_COMPLETE;

	// Mark loser as eliminated
	int loserId = (i32WinnerTeamId == pMatch->i32Team1Id) ?
		pMatch->i32Team2Id : pMatch->i32Team1Id;

	TournamentTeam* pWinner = FindTeam(pTourney, i32WinnerTeamId);
	TournamentTeam* pLoser = FindTeam(pTourney, loserId);

	if (pWinner)
	{
		pWinner->i32MatchWins++;
		pWinner->i32RoundWins += (i32WinnerTeamId == pMatch->i32Team1Id) ?
			pMatch->i32Team1Wins : pMatch->i32Team2Wins;
		pWinner->i32RoundLosses += (i32WinnerTeamId == pMatch->i32Team1Id) ?
			pMatch->i32Team2Wins : pMatch->i32Team1Wins;
	}

	if (pLoser)
	{
		pLoser->i32MatchLosses++;
		pLoser->bEliminated = true;
		pLoser->i32RoundWins += (loserId == pMatch->i32Team1Id) ?
			pMatch->i32Team1Wins : pMatch->i32Team2Wins;
		pLoser->i32RoundLosses += (loserId == pMatch->i32Team1Id) ?
			pMatch->i32Team2Wins : pMatch->i32Team1Wins;
	}

	printf("[Tournament] Match %d: Team %d wins, Team %d eliminated\n",
		i32MatchId, i32WinnerTeamId, loserId);

	// Advance winner to next round
	AdvanceBracket(pTourney, i32MatchId);

	// Check if round is complete
	if (IsRoundComplete(pTourney, pMatch->i32BracketRound))
	{
		pTourney->i32CurrentRound = pMatch->i32BracketRound + 1;
		printf("[Tournament] Round %d complete, advancing to round %d\n",
			pMatch->i32BracketRound, pTourney->i32CurrentRound);
	}

	// Check if tournament is complete
	CheckTournamentComplete(pTourney);

	return true;
}

// ============================================================================
// AdvanceBracket - place winner into next round match
// ============================================================================

void TournamentSystem::AdvanceBracket(TournamentData* pTourney, int i32MatchId)
{
	if (!pTourney)
		return;

	TournamentMatch* pMatch = FindMatch(pTourney, i32MatchId);
	if (!pMatch || pMatch->i32WinnerId == -1)
		return;

	// Find the next round match
	int nextRound = pMatch->i32BracketRound + 1;
	int nextPos = pMatch->i32BracketPosition / 2;

	// Find the match in the next round at the right position
	for (int i = 0; i < pTourney->i32MatchCount; ++i)
	{
		TournamentMatch& nextMatch = pTourney->matches[i];
		if (nextMatch.i32BracketRound == nextRound &&
			nextMatch.i32BracketPosition == nextPos)
		{
			// Place winner in correct slot
			if (pMatch->i32BracketPosition % 2 == 0)
				nextMatch.i32Team1Id = pMatch->i32WinnerId;
			else
				nextMatch.i32Team2Id = pMatch->i32WinnerId;

			// If both teams are set and one is a BYE, auto-advance
			if (nextMatch.i32Team1Id != -1 && nextMatch.i32Team2Id == -1)
			{
				// Wait for opponent
			}
			else if (nextMatch.i32Team1Id == -1 && nextMatch.i32Team2Id != -1)
			{
				// Wait for opponent
			}

			break;
		}
	}
}

// ============================================================================
// Query methods
// ============================================================================

const TournamentConfig* TournamentSystem::GetTournamentConfig(int i32TourneyId) const
{
	const TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
		return nullptr;
	return &pTourney->config;
}

const TournamentTeam* TournamentSystem::GetTeam(int i32TourneyId, int i32TeamId) const
{
	const TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
		return nullptr;
	return FindTeam(pTourney, i32TeamId);
}

const TournamentMatch* TournamentSystem::GetMatch(int i32TourneyId, int i32MatchId) const
{
	const TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
		return nullptr;
	return FindMatch(pTourney, i32MatchId);
}

int TournamentSystem::GetTeamCount(int i32TourneyId) const
{
	const TournamentData* pTourney = FindTournament(i32TourneyId);
	return pTourney ? pTourney->i32TeamCount : 0;
}

int TournamentSystem::GetMatchCount(int i32TourneyId) const
{
	const TournamentData* pTourney = FindTournament(i32TourneyId);
	return pTourney ? pTourney->i32MatchCount : 0;
}

int TournamentSystem::GetCurrentRound(int i32TourneyId) const
{
	const TournamentData* pTourney = FindTournament(i32TourneyId);
	return pTourney ? pTourney->i32CurrentRound : 0;
}

bool TournamentSystem::IsTournamentComplete(int i32TourneyId) const
{
	const TournamentData* pTourney = FindTournament(i32TourneyId);
	return pTourney ? (pTourney->config.eState == TOURNEY_COMPLETE) : false;
}

int TournamentSystem::GetWinnerTeamId(int i32TourneyId) const
{
	const TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney || pTourney->config.eState != TOURNEY_COMPLETE)
		return -1;

	// Find the final match (last round, position 0)
	for (int i = 0; i < pTourney->i32MatchCount; ++i)
	{
		if (pTourney->matches[i].i32BracketRound == pTourney->i32TotalRounds - 1 &&
			pTourney->matches[i].i32BracketPosition == 0)
		{
			return pTourney->matches[i].i32WinnerId;
		}
	}

	return -1;
}

// ============================================================================
// Rewards
// ============================================================================

void TournamentSystem::SetReward(int i32TourneyId, int i32Place, const TournamentReward& reward)
{
	TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
		return;

	if (i32Place < 1 || i32Place > MAX_TOURNEY_REWARDS)
		return;

	pTourney->rewards[i32Place - 1] = reward;
	pTourney->rewards[i32Place - 1].i32Place = i32Place;
}

bool TournamentSystem::DistributeRewards(int i32TourneyId)
{
	TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney || pTourney->config.eState != TOURNEY_COMPLETE)
		return false;

	if (!g_pGameSessionManager)
		return false;

	// Find winner (1st place)
	int winnerTeamId = GetWinnerTeamId(i32TourneyId);
	if (winnerTeamId < 0)
		return false;

	// Find finalist (2nd place) - loser of final match
	int finalistTeamId = -1;
	for (int i = 0; i < pTourney->i32MatchCount; ++i)
	{
		const TournamentMatch& match = pTourney->matches[i];
		if (match.i32BracketRound == pTourney->i32TotalRounds - 1 &&
			match.i32BracketPosition == 0)
		{
			finalistTeamId = (match.i32WinnerId == match.i32Team1Id) ?
				match.i32Team2Id : match.i32Team1Id;
			break;
		}
	}

	// Distribute rewards to team members
	struct PlaceEntry { int teamId; int place; };
	PlaceEntry places[MAX_TOURNEY_REWARDS];
	int placeCount = 0;

	if (winnerTeamId >= 0)
	{
		places[placeCount].teamId = winnerTeamId;
		places[placeCount].place = 1;
		placeCount++;
	}
	if (finalistTeamId >= 0)
	{
		places[placeCount].teamId = finalistTeamId;
		places[placeCount].place = 2;
		placeCount++;
	}

	// Find semi-finalists (3rd/4th)
	for (int i = 0; i < pTourney->i32MatchCount; ++i)
	{
		const TournamentMatch& match = pTourney->matches[i];
		if (match.i32BracketRound == pTourney->i32TotalRounds - 2 &&
			match.eState == TMATCH_COMPLETE && placeCount < MAX_TOURNEY_REWARDS)
		{
			int loserId = (match.i32WinnerId == match.i32Team1Id) ?
				match.i32Team2Id : match.i32Team1Id;
			if (loserId >= 0)
			{
				places[placeCount].teamId = loserId;
				places[placeCount].place = 3;
				placeCount++;
			}
		}
	}

	// Award rewards
	for (int p = 0; p < placeCount; ++p)
	{
		int place = places[p].place;
		if (place < 1 || place > MAX_TOURNEY_REWARDS)
			continue;

		const TournamentReward& reward = pTourney->rewards[place - 1];
		if (reward.i32GPReward == 0 && reward.i32CashReward == 0 &&
			reward.i32ExpReward == 0 && reward.ui32ItemId == 0)
			continue;

		const TournamentTeam* pTeam = FindTeam(pTourney, places[p].teamId);
		if (!pTeam)
			continue;

		for (int m = 0; m < MAX_TOURNEY_TEAM_MEMBERS; ++m)
		{
			if (!pTeam->members[m].bActive)
				continue;

			GameSession* pSession = g_pGameSessionManager->FindSessionByUID(
				pTeam->members[m].i64UID);
			if (pSession)
			{
				// Reward distribution would go through the normal reward pipeline
				printf("[Tournament] Reward: Place %d, Team '%s', Player '%s' - GP=%d, Cash=%d, EXP=%d\n",
					place, pTeam->szTeamName, pTeam->members[m].szNickname,
					reward.i32GPReward, reward.i32CashReward, reward.i32ExpReward);
			}
		}
	}

	printf("[Tournament] Rewards distributed for tournament %d\n", i32TourneyId);
	return true;
}

// ============================================================================
// Season
// ============================================================================

int TournamentSystem::CreateSeason(const char* pszName, uint32_t ui32Start, uint32_t ui32End)
{
	for (int i = 0; i < MAX_TOURNEY_SEASONS; ++i)
	{
		if (!m_Seasons[i].bActive)
		{
			m_Seasons[i].Reset();
			m_Seasons[i].i32SeasonId = m_i32NextSeasonId++;
			strncpy_s(m_Seasons[i].szSeasonName, pszName, _TRUNCATE);
			m_Seasons[i].ui32StartDate = ui32Start;
			m_Seasons[i].ui32EndDate = ui32End;
			m_Seasons[i].bActive = true;
			m_i32CurrentSeasonIdx = i;

			printf("[Tournament] Season '%s' (id=%d) created\n",
				pszName, m_Seasons[i].i32SeasonId);
			return m_Seasons[i].i32SeasonId;
		}
	}
	return -1;
}

const TournamentSeason* TournamentSystem::GetCurrentSeason() const
{
	if (m_i32CurrentSeasonIdx < 0 || m_i32CurrentSeasonIdx >= MAX_TOURNEY_SEASONS)
		return nullptr;
	if (!m_Seasons[m_i32CurrentSeasonIdx].bActive)
		return nullptr;
	return &m_Seasons[m_i32CurrentSeasonIdx];
}

// ============================================================================
// Update
// ============================================================================

void TournamentSystem::Update()
{
	DWORD dwNow = GetTickCount();

	// Update every 5 seconds
	if (dwNow - m_dwLastUpdateTime < 5000)
		return;

	m_dwLastUpdateTime = dwNow;

	// Check for scheduled tournament starts (based on ui32StartTime)
	// In a full implementation, this would compare against current Unix time
}

// ============================================================================
// SendBracketInfo
// ============================================================================

void TournamentSystem::SendBracketInfo(int i32TourneyId, GameSession* pSession)
{
	if (!pSession)
		return;

	const TournamentData* pTourney = FindTournament(i32TourneyId);
	if (!pTourney)
	{
		// Send error
		char buf[16];
		uint16_t pktSize = SOCKET_HEAD_SIZE + sizeof(int32_t);
		uint16_t proto = PROTOCOL_TOURNEY_BRACKET_ACK;
		int32_t result = 1;	// Not found
		int offset = 0;
		memcpy(buf + offset, &pktSize, 2); offset += 2;
		memcpy(buf + offset, &proto, 2); offset += 2;
		memcpy(buf + offset, &result, 4); offset += 4;
		pSession->SendGamePacket(buf, offset);
		return;
	}

	// Build bracket packet
	int payloadSize = sizeof(int32_t) * 3 + // result + matchCount + currentRound
					  pTourney->i32MatchCount * (int)sizeof(TOURNEY_BRACKET_ENTRY);

	uint16_t pktSize = (uint16_t)(SOCKET_HEAD_SIZE + payloadSize);
	char* pBuf = new char[pktSize];
	int offset = 0;

	memcpy(pBuf + offset, &pktSize, 2); offset += 2;
	uint16_t proto = PROTOCOL_TOURNEY_BRACKET_ACK;
	memcpy(pBuf + offset, &proto, 2); offset += 2;

	int32_t result = 0;
	memcpy(pBuf + offset, &result, 4); offset += 4;
	memcpy(pBuf + offset, &pTourney->i32MatchCount, 4); offset += 4;
	memcpy(pBuf + offset, &pTourney->i32CurrentRound, 4); offset += 4;

	for (int i = 0; i < pTourney->i32MatchCount; ++i)
	{
		const TournamentMatch& match = pTourney->matches[i];

		TOURNEY_BRACKET_ENTRY entry;
		memset(&entry, 0, sizeof(entry));
		entry.i32MatchId = match.i32MatchId;
		entry.i32Round = match.i32BracketRound;
		entry.i32Position = match.i32BracketPosition;
		entry.i32Team1Id = match.i32Team1Id;
		entry.i32Team2Id = match.i32Team2Id;
		entry.i32WinnerId = match.i32WinnerId;
		entry.i32Team1Wins = match.i32Team1Wins;
		entry.i32Team2Wins = match.i32Team2Wins;
		entry.ui8State = (uint8_t)match.eState;

		// Fill team names
		const TournamentTeam* pTeam1 = FindTeam(pTourney, match.i32Team1Id);
		const TournamentTeam* pTeam2 = FindTeam(pTourney, match.i32Team2Id);
		if (pTeam1) strncpy_s(entry.szTeam1Name, pTeam1->szTeamName, _TRUNCATE);
		if (pTeam2) strncpy_s(entry.szTeam2Name, pTeam2->szTeamName, _TRUNCATE);

		memcpy(pBuf + offset, &entry, sizeof(entry)); offset += sizeof(entry);
	}

	pSession->SendGamePacket(pBuf, offset);
	delete[] pBuf;
}

// ============================================================================
// SendTournamentList
// ============================================================================

void TournamentSystem::SendTournamentList(GameSession* pSession)
{
	if (!pSession)
		return;

	// Count active tournaments
	int count = 0;
	for (int i = 0; i < MAX_TOURNAMENTS; ++i)
	{
		if (m_Tournaments[i].bActive)
			count++;
	}

	int payloadSize = sizeof(int32_t) * 2 +	// result + count
					  count * (int)sizeof(TOURNEY_INFO_PACKET);
	uint16_t pktSize = (uint16_t)(SOCKET_HEAD_SIZE + payloadSize);

	char* pBuf = new char[pktSize];
	int offset = 0;

	memcpy(pBuf + offset, &pktSize, 2); offset += 2;
	uint16_t proto = PROTOCOL_TOURNEY_INFO_ACK;
	memcpy(pBuf + offset, &proto, 2); offset += 2;

	int32_t result = 0;
	memcpy(pBuf + offset, &result, 4); offset += 4;
	memcpy(pBuf + offset, &count, 4); offset += 4;

	for (int i = 0; i < MAX_TOURNAMENTS; ++i)
	{
		if (!m_Tournaments[i].bActive)
			continue;

		const TournamentData& t = m_Tournaments[i];
		TOURNEY_INFO_PACKET info;
		memset(&info, 0, sizeof(info));
		info.ui16Size = sizeof(TOURNEY_INFO_PACKET);
		info.ui16Protocol = PROTOCOL_TOURNEY_INFO_ACK;
		info.i32Result = 0;
		info.i32TourneyId = t.config.i32TourneyId;
		strncpy_s(info.szName, t.config.szName, _TRUNCATE);
		info.ui8State = (uint8_t)t.config.eState;
		info.i32TeamCount = t.i32TeamCount;
		info.i32MaxTeams = t.config.i32MaxTeams;
		info.ui8GameMode = t.config.ui8GameMode;
		info.i32BestOf = t.config.i32BestOf;
		info.i32CurrentRound = t.i32CurrentRound;

		memcpy(pBuf + offset, &info, sizeof(info)); offset += sizeof(info);
	}

	pSession->SendGamePacket(pBuf, offset);
	delete[] pBuf;
}

// ============================================================================
// Internal helpers
// ============================================================================

TournamentSystem::TournamentData* TournamentSystem::FindTournament(int i32TourneyId)
{
	for (int i = 0; i < MAX_TOURNAMENTS; ++i)
	{
		if (m_Tournaments[i].bActive && m_Tournaments[i].config.i32TourneyId == i32TourneyId)
			return &m_Tournaments[i];
	}
	return nullptr;
}

const TournamentSystem::TournamentData* TournamentSystem::FindTournament(int i32TourneyId) const
{
	for (int i = 0; i < MAX_TOURNAMENTS; ++i)
	{
		if (m_Tournaments[i].bActive && m_Tournaments[i].config.i32TourneyId == i32TourneyId)
			return &m_Tournaments[i];
	}
	return nullptr;
}

TournamentSystem::TournamentData* TournamentSystem::FindFreeTournamentSlot()
{
	for (int i = 0; i < MAX_TOURNAMENTS; ++i)
	{
		if (!m_Tournaments[i].bActive)
			return &m_Tournaments[i];
	}
	return nullptr;
}

TournamentTeam* TournamentSystem::FindTeam(TournamentData* pTourney, int i32TeamId)
{
	if (!pTourney || i32TeamId < 0)
		return nullptr;
	for (int i = 0; i < MAX_TOURNEY_TEAMS; ++i)
	{
		if (pTourney->teams[i].bActive && pTourney->teams[i].i32TeamId == i32TeamId)
			return &pTourney->teams[i];
	}
	return nullptr;
}

const TournamentTeam* TournamentSystem::FindTeam(const TournamentData* pTourney, int i32TeamId) const
{
	if (!pTourney || i32TeamId < 0)
		return nullptr;
	for (int i = 0; i < MAX_TOURNEY_TEAMS; ++i)
	{
		if (pTourney->teams[i].bActive && pTourney->teams[i].i32TeamId == i32TeamId)
			return &pTourney->teams[i];
	}
	return nullptr;
}

TournamentMatch* TournamentSystem::FindMatch(TournamentData* pTourney, int i32MatchId)
{
	if (!pTourney)
		return nullptr;
	for (int i = 0; i < pTourney->i32MatchCount; ++i)
	{
		if (pTourney->matches[i].i32MatchId == i32MatchId)
			return &pTourney->matches[i];
	}
	return nullptr;
}

const TournamentMatch* TournamentSystem::FindMatch(const TournamentData* pTourney, int i32MatchId) const
{
	if (!pTourney)
		return nullptr;
	for (int i = 0; i < pTourney->i32MatchCount; ++i)
	{
		if (pTourney->matches[i].i32MatchId == i32MatchId)
			return &pTourney->matches[i];
	}
	return nullptr;
}

int TournamentSystem::CalculateBracketRounds(int i32TeamCount) const
{
	int rounds = 0;
	int n = 1;
	while (n < i32TeamCount)
	{
		n *= 2;
		rounds++;
	}
	return rounds;
}

int TournamentSystem::NextPowerOf2(int n) const
{
	int p = 1;
	while (p < n) p *= 2;
	return p;
}

bool TournamentSystem::IsRoundComplete(const TournamentData* pTourney, int i32Round) const
{
	if (!pTourney)
		return false;

	for (int i = 0; i < pTourney->i32MatchCount; ++i)
	{
		if (pTourney->matches[i].i32BracketRound == i32Round)
		{
			if (pTourney->matches[i].eState != TMATCH_COMPLETE &&
				pTourney->matches[i].eState != TMATCH_BYE)
				return false;
		}
	}
	return true;
}

void TournamentSystem::CheckTournamentComplete(TournamentData* pTourney)
{
	if (!pTourney)
		return;

	// Check if final match is complete
	for (int i = 0; i < pTourney->i32MatchCount; ++i)
	{
		if (pTourney->matches[i].i32BracketRound == pTourney->i32TotalRounds - 1 &&
			pTourney->matches[i].i32BracketPosition == 0)
		{
			if (pTourney->matches[i].eState == TMATCH_COMPLETE)
			{
				pTourney->config.eState = TOURNEY_COMPLETE;

				const TournamentTeam* pWinner = FindTeam(pTourney,
					pTourney->matches[i].i32WinnerId);

				printf("[Tournament] Tournament %d '%s' COMPLETE! Winner: %s\n",
					pTourney->config.i32TourneyId, pTourney->config.szName,
					pWinner ? pWinner->szTeamName : "Unknown");

				// Auto-distribute rewards
				DistributeRewards(pTourney->config.i32TourneyId);
			}
			break;
		}
	}
}
