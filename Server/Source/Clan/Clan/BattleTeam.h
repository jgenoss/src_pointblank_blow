#ifndef __BATTLE_TEAM_H__
#define __BATTLE_TEAM_H__

class CBattleTeam : public i3ElementBase
{
	I3_CLASS_DEFINE( CBattleTeam );

private:
	i3BinList*				m_pListChannel;
	i3BinList*				m_pListClan;

	INT32					m_ai32Naming[ MAX_MEMBER_COUNT ];

	INT32					_GetNaming( i3BinList* pClanList );

	INT32					_GetChannelNaming( BATTLE_CHANNE_INFO* pstChannelInfo );

	BATTLE_INFO*			_GetEmptyBattleInfo();
	BATTLE_CLAN_INFO*		_GetEmptyClanInfo( UINT32 ui32CID );
	BATTLE_CHANNE_INFO*		_GetEmptyChannelInfo( UINT32 ui32Channel );


public:
	CBattleTeam();
	virtual ~CBattleTeam();

	BOOL					Create();
	void					OnDestroy();

	BATTLE_INFO*			CreateTeam( UINT32 ui32CID, UINT32 ui32Channel, INT32* pi32Naming );
	void					DestroyTeam( BATTLE_INFO* pstBattleInfo );

	BATTLE_INFO*			FindTeam( UINT32 ui32Idx );

	BATTLE_CHANNE_INFO*		FindChannelTeam( UINT32 ui32Channel );
	BATTLE_CLAN_INFO*		FindClanTeam( UINT32 ui32CID );

	void					SetClanContext( BATTLE_CLAN_CONTEXT* pClanContext );
	void					SetChannelContext( BATTLE_CHANNEL_CONTEXT* pClanContext );
};

#endif