#ifndef __TOURNAMENTSYSTEM_H__
#define __TOURNAMENTSYSTEM_H__

#pragma once
#include "RoomDef.h"

// ============================================================================
// Tournament System
// ============================================================================
// Competitive bracket system for organized play.
// Supports:
// - Single elimination brackets
// - Team registration and management
// - Automatic bracket generation
// - Match scheduling and result tracking
// - Prize distribution at tournament end
// - Season tracking for competitive rankings
// ============================================================================

class GameSession;

// ============================================================================
// Tournament State
// ============================================================================

enum TournamentState
{
	TOURNEY_NONE = 0,
	TOURNEY_REGISTRATION,			// Accepting team registrations
	TOURNEY_IN_PROGRESS,			// Tournament active, matches in progress
	TOURNEY_COMPLETE,				// Tournament finished, results finalized
	TOURNEY_CANCELLED,				// Tournament cancelled
};

// ============================================================================
// Tournament Config
// ============================================================================

struct TournamentConfig
{
	int			i32TourneyId;
	char		szName[64];
	TournamentState eState;
	int			i32MaxTeams;			// Max teams allowed (power of 2: 4,8,16,32)
	int			i32PlayersPerTeam;		// Players per team (1-8)
	uint8_t		ui8GameMode;			// StageMode for matches
	uint8_t		ui8MapIndex;			// Map for matches (0 = varies per round)
	int			i32BestOf;				// Best of 3, 5, etc.
	uint32_t	ui32StartTime;			// Unix timestamp for start
	uint32_t	ui32EndTime;			// Unix timestamp for end
	int			i32MinLevel;			// Minimum player level to register
	int			i32EntryFee;			// GP entry fee per team (0 = free)

	TournamentConfig()
		: i32TourneyId(0)
		, eState(TOURNEY_NONE)
		, i32MaxTeams(8)
		, i32PlayersPerTeam(5)
		, ui8GameMode(STAGE_MODE_BOMB)
		, ui8MapIndex(0)
		, i32BestOf(3)
		, ui32StartTime(0)
		, ui32EndTime(0)
		, i32MinLevel(0)
		, i32EntryFee(0)
	{
		szName[0] = '\0';
	}
};

// ============================================================================
// Tournament Team
// ============================================================================

#define MAX_TOURNEY_TEAM_MEMBERS	8
#define MAX_TOURNEY_TEAM_NAME		32

struct TournamentTeam
{
	int			i32TeamId;
	char		szTeamName[MAX_TOURNEY_TEAM_NAME];
	int64_t		i64CaptainUID;
	char		szCaptainNick[64];
	int			i32ClanId;					// Optional clan association (0 = none)

	struct Member
	{
		int64_t		i64UID;
		char		szNickname[64];
		int			i32Level;
		int			i32RankId;
		bool		bActive;

		void Reset()
		{
			i64UID = 0;
			szNickname[0] = '\0';
			i32Level = 0;
			i32RankId = 0;
			bActive = false;
		}
	};

	Member		members[MAX_TOURNEY_TEAM_MEMBERS];
	int			i32MemberCount;
	int			i32Seed;					// Seeding position (1 = top seed)
	int			i32MatchWins;				// Total match wins in this tournament
	int			i32MatchLosses;
	int			i32RoundWins;				// Total round wins
	int			i32RoundLosses;
	bool		bEliminated;				// Knocked out of bracket
	bool		bActive;					// Is this team slot in use

	void Reset()
	{
		i32TeamId = 0;
		szTeamName[0] = '\0';
		i64CaptainUID = 0;
		szCaptainNick[0] = '\0';
		i32ClanId = 0;
		for (int i = 0; i < MAX_TOURNEY_TEAM_MEMBERS; ++i)
			members[i].Reset();
		i32MemberCount = 0;
		i32Seed = 0;
		i32MatchWins = 0;
		i32MatchLosses = 0;
		i32RoundWins = 0;
		i32RoundLosses = 0;
		bEliminated = false;
		bActive = false;
	}
};

// ============================================================================
// Tournament Match
// ============================================================================

enum TourneyMatchState
{
	TMATCH_PENDING = 0,				// Not yet started
	TMATCH_IN_PROGRESS,				// Currently playing
	TMATCH_COMPLETE,				// Finished
	TMATCH_BYE,						// Bye (odd bracket)
};

struct TournamentMatch
{
	int			i32MatchId;
	int			i32BracketRound;		// Round in bracket (0 = first round)
	int			i32BracketPosition;		// Position in round (0-based)

	int			i32Team1Id;				// -1 = TBD
	int			i32Team2Id;				// -1 = TBD (or BYE)
	int			i32WinnerId;			// -1 = not decided

	int			i32Team1Wins;			// Rounds won by team 1
	int			i32Team2Wins;			// Rounds won by team 2
	int			i32BestOf;				// Best of N

	TourneyMatchState eState;

	int			i32RoomIdx;				// Game room index (-1 = not created)
	int			i32ChannelNum;			// Channel for the room

	void Reset()
	{
		i32MatchId = 0;
		i32BracketRound = 0;
		i32BracketPosition = 0;
		i32Team1Id = -1;
		i32Team2Id = -1;
		i32WinnerId = -1;
		i32Team1Wins = 0;
		i32Team2Wins = 0;
		i32BestOf = 3;
		eState = TMATCH_PENDING;
		i32RoomIdx = -1;
		i32ChannelNum = -1;
	}
};

// ============================================================================
// Tournament Reward
// ============================================================================

struct TournamentReward
{
	int			i32Place;				// 1st, 2nd, 3rd, etc.
	int			i32GPReward;			// GP reward
	int			i32CashReward;			// Cash reward
	int			i32ExpReward;			// EXP reward
	uint32_t	ui32ItemId;				// Special item reward (0 = none)
	char		szTitle[32];			// Title awarded (empty = none)

	void Reset()
	{
		i32Place = 0;
		i32GPReward = 0;
		i32CashReward = 0;
		i32ExpReward = 0;
		ui32ItemId = 0;
		szTitle[0] = '\0';
	}
};

// ============================================================================
// Season tracking
// ============================================================================

struct TournamentSeason
{
	int			i32SeasonId;
	char		szSeasonName[64];
	uint32_t	ui32StartDate;			// YYYYMMDD
	uint32_t	ui32EndDate;
	int			i32TournamentsHeld;
	bool		bActive;

	void Reset()
	{
		i32SeasonId = 0;
		szSeasonName[0] = '\0';
		ui32StartDate = 0;
		ui32EndDate = 0;
		i32TournamentsHeld = 0;
		bActive = false;
	}
};

// ============================================================================
// TournamentSystem
// ============================================================================

#define MAX_TOURNAMENTS				4		// Max concurrent tournaments
#define MAX_TOURNEY_TEAMS			32		// Max teams per tournament
#define MAX_TOURNEY_MATCHES			64		// Max matches per tournament (bracket size)
#define MAX_TOURNEY_REWARDS			4		// Reward tiers (1st, 2nd, 3rd/4th)
#define MAX_TOURNEY_SEASONS			4		// Season history

// Protocol for tournament notifications
#define PROTOCOL_TOURNEY_INFO_ACK		0x0F30
#define PROTOCOL_TOURNEY_REGISTER_ACK	0x0F31
#define PROTOCOL_TOURNEY_BRACKET_ACK	0x0F32
#define PROTOCOL_TOURNEY_MATCH_ACK		0x0F33
#define PROTOCOL_TOURNEY_RESULT_ACK		0x0F34

class TournamentSystem
{
public:
	TournamentSystem();
	~TournamentSystem();

	// Initialize
	bool		Initialize();

	// Tournament lifecycle
	int			CreateTournament(const TournamentConfig& config);	// Returns tourney ID, -1 on error
	bool		StartTournament(int i32TourneyId);
	bool		CancelTournament(int i32TourneyId);

	// Team management
	int			RegisterTeam(int i32TourneyId, const char* pszTeamName,
							  int64_t i64CaptainUID, const char* pszCaptainNick,
							  int i32ClanId = 0);		// Returns team ID, -1 on error
	bool		AddTeamMember(int i32TourneyId, int i32TeamId,
							  int64_t i64UID, const char* pszNickname,
							  int i32Level, int i32RankId);
	bool		RemoveTeamMember(int i32TourneyId, int i32TeamId, int64_t i64UID);
	bool		UnregisterTeam(int i32TourneyId, int i32TeamId);

	// Bracket
	bool		GenerateBracket(int i32TourneyId);			// Single elimination
	bool		AdvanceWinner(int i32TourneyId, int i32MatchId, int i32WinnerTeamId);
	bool		ReportMatchResult(int i32TourneyId, int i32MatchId,
								   int i32Team1Wins, int i32Team2Wins);

	// Query
	const TournamentConfig*	GetTournamentConfig(int i32TourneyId) const;
	const TournamentTeam*	GetTeam(int i32TourneyId, int i32TeamId) const;
	const TournamentMatch*	GetMatch(int i32TourneyId, int i32MatchId) const;
	int			GetTeamCount(int i32TourneyId) const;
	int			GetMatchCount(int i32TourneyId) const;
	int			GetCurrentRound(int i32TourneyId) const;
	bool		IsTournamentComplete(int i32TourneyId) const;
	int			GetWinnerTeamId(int i32TourneyId) const;

	// Rewards
	void		SetReward(int i32TourneyId, int i32Place, const TournamentReward& reward);
	bool		DistributeRewards(int i32TourneyId);

	// Season
	int			CreateSeason(const char* pszName, uint32_t ui32Start, uint32_t ui32End);
	const TournamentSeason* GetCurrentSeason() const;

	// Periodic update (check for scheduled tournaments, timeout matches)
	void		Update();

	// Send bracket info to a player
	void		SendBracketInfo(int i32TourneyId, GameSession* pSession);
	void		SendTournamentList(GameSession* pSession);

private:
	// Internal tournament data
	struct TournamentData
	{
		TournamentConfig	config;
		TournamentTeam		teams[MAX_TOURNEY_TEAMS];
		TournamentMatch		matches[MAX_TOURNEY_MATCHES];
		TournamentReward	rewards[MAX_TOURNEY_REWARDS];
		int					i32TeamCount;
		int					i32MatchCount;
		int					i32NextTeamId;
		int					i32NextMatchId;
		int					i32CurrentRound;
		int					i32TotalRounds;
		bool				bActive;

		void Reset()
		{
			config = TournamentConfig();
			for (int i = 0; i < MAX_TOURNEY_TEAMS; ++i) teams[i].Reset();
			for (int i = 0; i < MAX_TOURNEY_MATCHES; ++i) matches[i].Reset();
			for (int i = 0; i < MAX_TOURNEY_REWARDS; ++i) rewards[i].Reset();
			i32TeamCount = 0;
			i32MatchCount = 0;
			i32NextTeamId = 1;
			i32NextMatchId = 1;
			i32CurrentRound = 0;
			i32TotalRounds = 0;
			bActive = false;
		}
	};

	// Find tournament data by ID
	TournamentData*		FindTournament(int i32TourneyId);
	const TournamentData* FindTournament(int i32TourneyId) const;
	TournamentData*		FindFreeTournamentSlot();

	// Find team within a tournament
	TournamentTeam*		FindTeam(TournamentData* pTourney, int i32TeamId);
	const TournamentTeam* FindTeam(const TournamentData* pTourney, int i32TeamId) const;

	// Find match within a tournament
	TournamentMatch*	FindMatch(TournamentData* pTourney, int i32MatchId);
	const TournamentMatch* FindMatch(const TournamentData* pTourney, int i32MatchId) const;

	// Calculate number of bracket rounds needed
	int			CalculateBracketRounds(int i32TeamCount) const;

	// Get next power of 2 >= n
	int			NextPowerOf2(int n) const;

	// Advance bracket after a match completes
	void		AdvanceBracket(TournamentData* pTourney, int i32MatchId);

	// Check if all matches in a round are complete
	bool		IsRoundComplete(const TournamentData* pTourney, int i32Round) const;

	// Check if tournament is complete
	void		CheckTournamentComplete(TournamentData* pTourney);

private:
	TournamentData		m_Tournaments[MAX_TOURNAMENTS];
	TournamentSeason	m_Seasons[MAX_TOURNEY_SEASONS];
	int					m_i32NextTourneyId;
	int					m_i32NextSeasonId;
	int					m_i32CurrentSeasonIdx;
	DWORD				m_dwLastUpdateTime;
};

extern TournamentSystem* g_pTournament;

#endif // __TOURNAMENTSYSTEM_H__
