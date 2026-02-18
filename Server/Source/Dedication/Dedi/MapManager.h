#pragma once

class NxShapeSet;

class CMapData;

// global game 리소스를 관리한다
class CMapManager
{	

public:

	// simulation function called by main
	void	OnUpdate_Main();

	// init
	BOOL			Init();
	

	BOOL			FillMapNames();			// commonDef.cpp로부터 map name과 map id를 얻는다
	
	BOOL			UnnecessaryMaps(INT8 i8MapIndex);

	inline CMapData* GetMap( STAGE_ID ui32StageID )
	{
		return m_aMaps[StageID::GetStageUID(ui32StageID)];
	}

	inline CMapData* GetMap(STAGE_UID eStageUID)
	{
		return m_aMaps[eStageUID];
	}

	CMapManager (void);
	~CMapManager(void);

private:
	void		_Release();

	// map object
	CMapData*	m_aMaps			[ STAGE_UID_MAX ];	// map objects 
	char*		m_pTabMapNames	[ STAGE_UID_MAX ];	// map name : code name  (resource directory를 얻기 위함)
												//	note. map name은 중복될 수 있다

	UINT32		m_ui32Count;
		
};

extern CMapManager* g_pMapManager;