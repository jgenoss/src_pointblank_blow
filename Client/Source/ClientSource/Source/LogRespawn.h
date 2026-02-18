#if !defined(__LOG_RESPAWN_H)
#define __LOG_RESPAWN_H

#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
#define LOG_RESPAWN_ENABLE		true
#else
#define LOG_RESPAWN_ENABLE		false
#endif


template<bool enable> class tlog_respawn;

template<>
class tlog_respawn<false>
{
public:
	typedef  tlog_respawn<false>				this_type;

	static this_type*	i() { return tinst<this_type>(); }
	void		ProcessCreateStage() {}
	void		SetMyDeathAck(const DEATH_INFO_CHARA& add_info ) {}
	void		AddDeathAckTime(REAL32 fDeltaTime) {}
	void		AddRespawnTime(REAL32 fDeltaTime) {}
	void		Reset() {}
	void		Print() {}
};

typedef tlog_respawn<LOG_RESPAWN_ENABLE>		log_respawn;

template<bool enable>
class tlog_respawn
{
public:
	typedef  tlog_respawn<enable>				this_type;

	static this_type*	i() { return tinst<this_type>(); }

	tlog_respawn() { Reset(); }
	
	void		ProcessCreateStage() { Reset(); }

	void		SetMyDeathAck(const DEATH_INFO_CHARA& death_info_chara )
	{
		if ( BattleSlotContext::i()->getMySlotIdx() == (INT32)GET_DEATHINFO_IDX_TYPE(death_info_chara._DeathInfo) )
			m_bRecvDeathAck = true;
	}

	void		AddDeathAckTime(REAL32 fDeltaTime)
	{
		if( m_bRecvDeathAck )
			m_fTimeDeathAckToRepawnReq += fDeltaTime;
	}
			
	void		AddRespawnTime(REAL32 fDeltaTime)
	{
		m_fCheckRespawnTime += fDeltaTime;		
	}
			
	void		Reset()
	{
		m_fCheckRespawnTime = 0.f;
		m_fTimeDeathAckToRepawnReq = 0.f;
		m_bRecvDeathAck = false;
		m_bSendRespawnReq = false;
	}

	void		Print()
	{
		//REAL32 limit_time = MyRoomInfoContext::i()->GetRespawnTime( BattleSlotContext::i()->getMySlotIdx());

		//I3PRINTLOG(I3LOG_DEFALUT, "## Limit Respawn Time : %.3f ##\n", limit_time);
		//I3PRINTLOG(I3LOG_DEFALUT, "## Check Respawn Time : %.3f ##\n", (m_fCheckRespawnTime - 3.f));		//RespawnDelayTime -> 3.f
		//I3PRINTLOG(I3LOG_DEFALUT, "## Time DeathAck to ReapawnReq: %.3f ##\n", m_fTimeDeathAckToRepawnReq);

		if( m_bSendRespawnReq )
		{
			I3ASSERT(0);
			I3PRINTLOG(I3LOG_DEFALUT, "RESPAWN을 다시 요청 합니다.");
		}
		m_bSendRespawnReq = true;			
	}
	
private:

	REAL32					m_fCheckRespawnTime;
	REAL32					m_fTimeDeathAckToRepawnReq;
	bool					m_bRecvDeathAck;
	bool					m_bSendRespawnReq;
};




#endif