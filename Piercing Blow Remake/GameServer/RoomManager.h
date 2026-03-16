#ifndef __ROOMMANAGER_H__
#define __ROOMMANAGER_H__

#pragma once
#include "i3ElementBase.h"
#include "i3List.h"
#include "i3Mutex.h"
#include "RoomDef.h"

class Room;
class GameSession;

// Port simplificado de CRoomManager
// Patron de double-buffered room lists del original (m_InfoSide swap)
// Cada canal tiene su propia lista de rooms con mutex independiente
class RoomManager : public i3ElementBase
{
	I3_CLASS_DEFINE(RoomManager);

public:
	RoomManager();
	virtual ~RoomManager();

	void		OnCreate();
	void		OnUpdate();
	void		OnDestroy();

	// Room operations
	int			OnCreateRoom(GameSession* pSession, GameRoomCreateInfo* pInfo);
	int			OnJoinRoom(GameSession* pSession, int i32ChannelNum, int i32RoomIdx, int i32Team);
	int			OnLeaveRoom(GameSession* pSession, int i32ChannelNum);
	int			OnQuickJoinRoom(GameSession* pSession, int i32ChannelNum);

	// QuickJoin with filtering (Phase 8)
	struct QuickJoinResult
	{
		int		i32ChannelIdx;
		int		i32RoomIdx;
		bool	bFound;
		// 2nd best room info (fallback)
		int		i32FallbackChannelIdx;
		int		i32FallbackRoomIdx;
		bool	bHasFallback;

		QuickJoinResult() : i32ChannelIdx(-1), i32RoomIdx(-1), bFound(false),
			i32FallbackChannelIdx(-1), i32FallbackRoomIdx(-1), bHasFallback(false) {}
	};
	QuickJoinResult SearchQuickJoinRoom(uint32_t ui32StageId, int i32CurrentChannel);

	// Room access
	Room*		GetRoom(int i32ChannelNum, int i32Idx);
	int			GetChannelUseRoomCount(int i32ChannelNum);
	uint32_t	GetChannelCount() const		{ return m_ui32ChannelCount; }

	// Room list for lobby
	void		OnSendRoomList(GameSession* pSession, int i32Channel);

private:
	void		UpdateRoomInfo(int i32ChannelNum);

private:
	// Resource pool: rooms pre-allocated per channel
	i3List**		m_pChannelResRoomList;

	// Active room lists per channel (with mutex protection)
	i3List**		m_pChannelRoomList;
	i3Mutex**		m_pcsChannelRoom;

	uint32_t		m_ui32EachChannelMaxCount;
	uint32_t		m_ui32ChannelCount;

	// Double-buffered room info (patron m_InfoSide del original)
	int*			m_InfoSide;					// Swap index per channel (0 or 1)
	int**			m_pRoomInfoIdx0;			// Room indices buffer 0
	uint32_t*		m_pRoomCount0;
	int**			m_pRoomInfoIdx1;			// Room indices buffer 1
	uint32_t*		m_pRoomCount1;

	// Change tracking
	uint32_t*		m_pChangeRoomListTime;

	// Stats
	int				m_i32UseRoomCount;
};

extern RoomManager* g_pRoomManager;

#endif // __ROOMMANAGER_H__
