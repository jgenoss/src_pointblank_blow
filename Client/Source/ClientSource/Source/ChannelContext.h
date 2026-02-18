#pragma once

#include "UI/UIDefine.h"

class ChannelContext : public i3::shared_ginst<ChannelContext>
{
public:
	ChannelContext();

	INT32				GetChannelCount(void) const						{ return _nChannelCount;}
	INT32				GetChannelMaxUser(void)	const					{ return _nMaxUserForChannel;}
	INT32				GetChannelUserCount( INT32 idxChannel) const	{ I3_BOUNDCHK( idxChannel, GAME_CHANNEL_COUNT); return _nUserCount[idxChannel];}


	void				SetChannelCount( INT32 count)							{ I3_BOUNDCHK( count, GAME_CHANNEL_COUNT+1); _nChannelCount = count; }
	void				SetChannelMaxUser( INT32 count)							{ _nMaxUserForChannel = count; }
	void				SetChannelUserCount( INT32 idxChannel, INT32 count)		{ I3_BOUNDCHK( idxChannel, GAME_CHANNEL_COUNT); _nUserCount[ idxChannel] = count; }
	void				SetChannelUserCountArray( const UINT16* user_count_array, INT32 num);
	void				Reset();
		
	bool				IsMoveChannel( void) const			{ return m_bMoveChannel; }
	void				SetMoveChannel()					{ m_bMoveChannel = true; }
	void				ClearMoveChannel()					{ m_bMoveChannel = false; }

	// 현재 입장한 채널조건에 맞는지 확인
	bool				IsValidChannelType(ChannelType channel) const;
	void				ClearChannelType(void)							{ i3mem::FillZero(m_channelTypeList, sizeof(m_channelTypeList)); }
	const UINT8*		GetChannelTypePtr(INT32 serverIdx) const		{ return m_channelTypeList[serverIdx]; }
	UINT8*				SetChannelTypePtr(INT32 serverIdx)				{ return m_channelTypeList[serverIdx]; }
	ChannelType			GetChannelType(INT32 serverIdx, INT32 channelIdx) const;		
	ChannelType			GetConnectedChannelType(void) const;	

	// revision 59499 Warnet Clan 서버 타입 예외 추가.
	bool				GetBattleChannel() const		{ return GetConnectedChannelType() == CHANNEL_TYPE_CLAN || GetConnectedChannelType() == CHANNEL_TYPE_PCCAFE_CLAN;	}
	bool				IsBeginnerChannel1(void) const	{ return GetConnectedChannelType() == CHANNEL_TYPE_BEGIN1;	}
	bool				IsBeginnerChannel2(void) const	{ return GetConnectedChannelType() == CHANNEL_TYPE_BEGIN2;	}
	
private:
		
	INT32 _nChannelCount = 0;
	INT32 _nMaxUserForChannel;							// [initialize at constructor]
	INT32 _nUserCount[GAME_CHANNEL_COUNT] = { 0 };
	
	bool   m_bMoveChannel = false;		// 고수/초보 채널 이동

	// 채널 타입 정보
	UINT8	m_channelTypeList[MAX_GAME_SERVER_COUNT][GAME_CHANNEL_COUNT] = { 0 };
	

	// 채널 제목을 Context로 올립니다.
	i3::wstring m_wstrChannelTypeString[CHANNEL_TYPE_MAX][SS_SUBTYPE_COUNT];

public:

	void				GetServerChannelText(i3::rc_wstring& output, ChannelType channel, SS_SUBTYPE subtype);
	void				LoadServerTextFromFile();
};
