#if !defined( __SCORE_CONTEXT_H)
#define __SCORE_CONTEXT_H



class ScoreContext : public i3::shared_ginst<ScoreContext>
{
private:
	SCORE_INFO IndividualScore[SLOT_MAX_COUNT];

	UINT8 CurRoundCount;
	UINT8 CurrentDediRound;					// 임시로 이것을 Dedi와 통신시 사용합니다.
											// GameServer와는 m_uCurrentRound를 사용. 서버, Dedi, Client 간에 Round check가 다릅니다.
											// 추후에 반드시 바뀌어야 함..... swoongoo
											// Dedicated Server와의 통신에서는 이것을 사용. 현재 GS - Dedi - Client 간 Round가 다르게 사용되고 있다..
	UINT16	m_AccTrainingScore;
	UINT16	m_AddTrainingScore;

	//서버에서 보내온 팀별 이긴라운드값입니다.(서버에서는 공수교대여부와상관없이 원래팀의값을 보냅니다)
	UINT16 m_redWinRound;
	UINT16 m_blueWinRound;
	SCORE_INFO m_teamScore[TEAM_COUNT];

	bool m_autoTeamChangeExecuted;			// 자동공수교대 수행여부
	bool m_executeShowTeamAutoChangeHudEffect; // 자동공수교대 이펙트 한번만 출력하게하기위해

public:
	ScoreContext();

	UINT8			getCurrentRound( void);
	void			setCurrentRound( UINT8 Round);
	UINT8			getCurrentDediRound( void) const			{ return CurrentDediRound; }
	void			setCurrentDediRound( UINT8 round)			{ CurrentDediRound = round; }

	UINT8			getCurrentRound( bool bDedicated)		{ return bDedicated ? CurrentDediRound : getCurrentRound(); }


	//
	// 자동공수교대 관련
	//
	void			ExecuteAutoTeamChange(); // 자동공수교대 실행, 여러번실행해도 한번만된다.

		// 자동공수교대가 실행되었는지 여부판별
		// MyRoomContext에 넣지않았던이유는 ScoreContext는 Battle의 종료와함께 정보가 리셋되어서.. 나중에 룸컨텍스에 옮기는것도
	bool			IsAutoTeamChangeExecuted() const { return m_autoTeamChangeExecuted; }	
	void			ShowTeamAutoChangeHudEffect();

	//
	// 팀점수관련 (자동공수교대가 실행되면 바뀐 팀점수를 보내줍니다
	//
		// 서버에서 보내온 값입니다. 팀변경유무와 상관없음
	void			InitTeamKillCount(void);

	void			SetSvrTeamWinRound(UINT16 red, UINT16 blue);
	UINT16			GetSvrTeamWinRound(TEAM_TYPE team) const;
	const SCORE_INFO* GetSvrTeamScore(TEAM_TYPE team) const;
	void			SetSvrTeamScore(const SCORE_INFO& redScore, const SCORE_INFO& blueScore);
	
								
		// 자동공수교대에의한 팀변경이 적용 됩니다.
	UINT16				GetRedTeamWinRound( void) const;
	UINT16				GetBlueTeamWinRound( void) const;
	UINT16				GetTeamWinRound(TEAM_TYPE team) const;
	const UINT16		GetDinoEscapeScore(TEAM_TYPE team) const;
	const SCORE_INFO*	GetTeamScore(TEAM_TYPE team) const;


	INT32			getTotalRoundCount( void) const				{ return static_cast<INT32>(m_redWinRound + m_blueWinRound); }

	const SCORE_INFO * getIndividualScore( INT32 userSlot)		{ I3_BOUNDCHK( userSlot, SLOT_MAX_COUNT); return &IndividualScore[ userSlot]; }
	void			setIndividualScores( const SCORE_INFO* pScores, UINT32 size)
	{
		I3ASSERT( sizeof(IndividualScore) == size);
		i3mem::Copy( IndividualScore, pScores, size);
	}

	void			ResetIndividualScore(INT32 userSlot)	{ I3_BOUNDCHK(userSlot, SLOT_MAX_COUNT); IndividualScore[userSlot].Reset();	}

	void			SetAddTrainingScore(UINT16 addScore)		{ m_AddTrainingScore = addScore;	}
	UINT16			GetAddTrainingScore(void) const				{ return m_AddTrainingScore;		}
	void			SetAccTrainingScore(UINT16 accScore)		{ m_AccTrainingScore = accScore;	}
	UINT16			GetAccTrainingScore(void) const				{ return m_AccTrainingScore;		}
	void			ResetTrainingScore()						{ m_AccTrainingScore = m_AddTrainingScore = 0; }

	void			CreateBattle( void);
	void			DestroyBattle(void);

};

#endif