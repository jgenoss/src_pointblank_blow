#pragma once

class NxShapeSet;

class CMapData;

// global game 리소스를 관리한다
class CMapMgr
{
	// check boundary
	BOOL	CB( INT32 i32Idx) const		{ return ( i32Idx >= 0 && i32Idx < (INT32) m_ui32Count );		 }
	BOOL	SB(STAGEORD stageOrd) const	{ return ( stageOrd > STAGE_ORD_NA && stageOrd < STAGE_ORD_MAX); }


public:
	// simulation called by main
	void	OnUpdate_Main();		

public:
	BOOL			Init();
	

	UINT32			FillMapNames();			// commonDef.cpp로부터 map name과 map id를 얻는다
	
	UINT32			GetCount()	const	{	return m_ui32Count; }
	CMapData*		GetMap( STAGEORD mapId )		
										{ 
											if( SB( mapId ) ) return m_pMaps[ GetIndex(mapId) ]; 
											else return NULL; 
										}
	char*			GetMapName( STAGEORD mapId )	
										{	
											if( SB( mapId ) ) return m_pTabMapNames[ GetIndex( mapId ) ]; 
											else return NULL;
										}
	
	// map의 stage_id 가 필요할 경우를 위함
	STAGEORD		GetMapID ( INT32 i32Index )  
										{	
											if( CB( i32Index ) ) return m_pTabMapIDs[ i32Index ];
											else return STAGE_ORD_NA;
										}
	INT32			GetIndex( STAGEORD stageOrd ) 
										{	return (INT32)stageOrd - 1; }

	
	// weapon list
	BOOL			InsertWeaponList( UINT32* ui32WeaponList, UINT32 ui32ItemCount, INT32 i32StartIndex, UINT32 ui32TotalItemCount );
	


	CMapMgr (void);
	~CMapMgr(void);

private:
	void		_Release();

	// map object
	CMapData*	m_pMaps			[DS_MAX_MAP-1];	// map objects 
	char*		m_pTabMapNames	[DS_MAX_MAP-1];	// map name : code name  (resource directory를 얻기 위함)
												//	note. map name은 중복될 수 있다
	STAGEORD	m_pTabMapIDs	[DS_MAX_MAP-1];	// map id   : ord number (stage_id가 필요한 경우를 위함)
	UINT32		m_ui32Count;					// map count
public:
	// WeaponID의 유효성 검사
	BOOL CheckWeaponID (UINT32 ui32WeaponID);
	// WeaponID로부터 WeaponIndex를 얻음 
	INT16 GetWeaponIndex( UINT32 ui32WeaponID );
	// WeaponIndex로부터 WeaponID를 얻음
	UINT32 GetWeaponID( INT16 i16WeaponIdx );

};

extern CMapMgr* g_pMapMgr;