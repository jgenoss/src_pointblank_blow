#pragma once

#ifdef	DOMI_CORE

#include "StageBattle.h"
#include "../../GameMode.h"

#include "Domination_FlowBase.h"

#include "../../ui/UISecCount.h"

enum TAKEBACK_AREA
{
	TAKEBACK_AREA_0		= 0,
	TAKEBACK_AREA_1,
	TAKEBACK_AREA_2,
	TAKEBACK_AREA_3,

	MAX_TAKEBACK_AREA_COUNT,
	TAKEBACK_AREA_UNKNOWN,
};

/* 쓰이지 않아 일단 주석(13.03.07.수빈)
struct LayerObjects
{
	i3::string				layerName;
	i3::vector<i3Object*>	layerObjects;

	void Clear()
	{
		layerName.clear();
		layerObjects.clear();
	}
};
*/


namespace Domination_Util
{
	class CNaviMesh
	{
		CDomiStageCli*		m_pMesh;
		INT32				m_nVertexCount;
		CUSTOM_VERTEX		m_Vertex[MAX_NAVIMESH_COUNT];
	public:
		CNaviMesh( const STAGE_ID id);
		~CNaviMesh();
		//INT16 GetNaviMeshIdx( VEC3D& v);
		void SetNaviMeshAICollision( PACKET_DOMINATION_NAVIMESH_AI_COLLISION_ACK* pData );
		INT32 GetVertexCount() const { return m_nVertexCount;}
		CUSTOM_VERTEX* GetVertexList() { return m_Vertex; }
	};

	void RenderNaviMesh(i3Viewer* pViewer, void* pData);
};
namespace du = Domination_Util;

class UIHUDDominationMgr;
//class IModeScript;
class i3Object;

class gmode_domination : public gmode
{
private:
	du::CNaviMesh*				m_pNaviMesh;

	UIHUDDominationMgr*			m_pHud;

public:
	virtual void ProcessCreateStage( CStageBattle* pStage);

	virtual	void ProcessEventPreStartRound(void);
	virtual void ProcessEventRoundStart(void);
	virtual void ProcessEventRoundEnd(void);

	virtual void ProcessUpdate( REAL32 rDeltaSec);		// 전체 업데이트
	virtual void OnUpdateEndGame( REAL32 rDeltaSec);		// 스테이지끝

	virtual void InitialIzeStage(void);
	virtual void InitLayer(void);
	virtual void ResetObjectStatus(void) {}

	void ProcessEventRespawn( INT32);
	void ProcessEventGiveUpBattle( INT32);

	i3::vector<i3RespawnObj*>&	GetRespawnObjectList() { return m_pStage->getRespawnObjList();}
	du::CNaviMesh*				GetNaviMesh() const { return m_pNaviMesh;}

	virtual void ProcessLoadProperties(i3RegKey* stage_key);
	
	gmode_domination();
	virtual ~gmode_domination();


private:
	void cb_window_minimized(void*, void*);
	void cb_window_restored(void*, void*);

public:
	void	 OnSecCount_Start(INT32 result_time) {}
	void OnSecCount_Change(INT32 result_time) {}
	void	 OnSecCount_Stop(INT32 result_time) {}
	void OnSecCount_Done(INT32 result_time);

private:
	typedef gmode_domination my_type;
	TUISecCallback<my_type, &my_type::OnSecCount_Start, &my_type::OnSecCount_Change, &my_type::OnSecCount_Done, &my_type::OnSecCount_Stop> m_SecCountCB;
};

#endif