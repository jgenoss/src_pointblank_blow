#ifndef _QUICK_JOIN_CONTEXT_H_
#define _QUICK_JOIN_CONTEXT_H_

struct QuickJoinInfo
{
	UINT32 m_ui32StageID = 0;
};

struct QuickJoinSecondBestInfo			 
{
	UINT32	m_i32Score[TEAM_COUNT] = { 0 };		// TEAM_RED : 레드팀, TEAM_BLUE : 블루팀
	INT32	m_i32PlayTime = 0;				// 배틀 진행 시간.
	INT32	m_i32ScoreType = 0;
	UINT32 m_ui32StageID = 0;
};

class QuickJoinContext : public i3::shared_ginst<QuickJoinContext>
{
private:
	QuickJoinInfo m_aQuickJoinInfo[MAX_QUICK_JOIN_INFO_COUNT];

	QuickJoinSecondBestInfo m_2ndBestRoomInfo;

	UINT32  m_ui32RoomIdx;
	INT32	m_i32Rv;
	UINT8   m_ui8QuickJoinIdx;
	UINT8	m_ui8ChannelIdx;
	UINT8   m_ui8ChannelType;
	UINT8	m_ui8RoomState;
	INT32	m_i32ServerIdx;
	bool	m_bPassthroughLobby;

	
public:
	QuickJoinContext() : m_ui32RoomIdx(0), m_i32Rv(0), m_ui8QuickJoinIdx(0), m_ui8ChannelIdx(0), m_i32ServerIdx(-1), m_bPassthroughLobby(false) {}

	const QuickJoinInfo *	getQuickJoinInfo( INT32 idx) const
	{
		I3_BOUNDCHK( idx, MAX_QUICK_JOIN_INFO_COUNT);
		return &m_aQuickJoinInfo[ idx];
	}

	QuickJoinInfo *	setQuickJoinInfo( INT32 idx)
	{
		I3_BOUNDCHK( idx, MAX_QUICK_JOIN_INFO_COUNT);
		return &m_aQuickJoinInfo[ idx];
	}

	const QuickJoinSecondBestInfo * getQuickJoinSecondInfo( void) const		{ return &m_2ndBestRoomInfo; }
	QuickJoinSecondBestInfo * setQuickJoinSecondInfo( void)					{ return &m_2ndBestRoomInfo; }

	UINT32	getRoomIndex( void) const			{ return m_ui32RoomIdx; }
	void	setRoomIndex( UINT32 idx)			{ m_ui32RoomIdx = idx; }

	INT32	getResult( void) const				{ return m_i32Rv; }
	void	setResult( INT32 rv)				{ m_i32Rv = rv; }

	UINT8	getQuickJoinIndex( void) const		{ return m_ui8QuickJoinIdx; }
	void	setQuickJoinIndex( UINT8 idx)		{ m_ui8QuickJoinIdx = idx; }

	UINT8	getChannelIndex( void) const		{ return m_ui8ChannelIdx; }
	void	setChannelIndex( UINT8 idx)			{ m_ui8ChannelIdx = idx; }

	UINT8	getChannelType( void) const			{ return m_ui8ChannelType; }
	void	setChannelType( UINT8 type)			{ m_ui8ChannelType = type; }

	UINT8	getRoomState( void) const			{ return m_ui8RoomState; }
	void	setRoomState( UINT8 state)			{ m_ui8RoomState = state; }

	INT32	getServerIndex( void) const			{ return m_i32ServerIdx; }
	void	setServerIndex( INT32 idx)			{ m_i32ServerIdx = idx; }

	bool	wasPassingthroughLobby( void) const			{ return m_bPassthroughLobby; }
	void	setPassingthroughLobby( bool ok)			{ m_bPassthroughLobby = ok; }
};

#endif