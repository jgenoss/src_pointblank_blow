#include "pch.h"
#include "GameCharaFullbodyDinosaur.h"
#include "../GameDinoContexts.h"
#include "../../GameCharaMoveContext.h"
#include "../../GameCharaMaterialContext.h"
#include "../../GameCharaUpdateContext.h"
#include "../../GameCharaInputContext.h"
#include "../../GameCharaCameraContext.h"
#include "../../GameCharaSoundContext.h"
#include "../../GameCharaCollisionContext.h"
#include "../../GameCharaHUDContext.h"
#include "../../GameCharaWeaponContext.h"

// 기획 문서 참조
// http://tiki.fps-pb.com/tiki-download_wiki_attachment.php?attId=3293

I3_CLASS_INSTANCE(CGameCharaFullbodyDinosaur);

void CGameCharaFullbodyDinosaur::Create(GLOBAL_CHARA_INFO* global, const CCharaInfo* db)
{
	CGameCharaFullbody::Create(global, db);
}

void CGameCharaFullbodyDinosaur::CreateFirst(const CCharaInfo* db, bool bInLobby)
{
	m_bReturnedInstance = false;

	Destroy();

	m_pDBInfo = const_cast<CCharaInfo*>(db);

	i3GameResChara* res = (i3GameResChara*)g_pCharaManager->QueryCharaResource(getCharaInfo()->GetResType());

	i3Chara::Create(g_pFramework, res); // 엔진에서 OnBindResource()를 호출하므로 별도로 호출하지 않음에 유의!

	m_pMaterialContext = CGameCharaMaterialContext::new_object();
	m_pMaterialContext->OnCreate(this);

	getSceneObject()->GetBody()->setSlope(CHARA_LOD_SLOPE);

	m_pUpdateContext = CGameCharaUpdateContext::new_object();
	m_pUpdateContext->OnCreate(this);

	m_pMoveContext = CGameCharaMoveContext::new_object();
	m_pMoveContext->OnCreate(this);

	m_pInputContext = CGameCharaInputContext::new_object();
	m_pInputContext->OnCreate(this);

	m_pCameraContext = CGameCharaCameraContext::new_object();
	m_pCameraContext->OnCreate(this);

	m_pBoneContext = CGameDinoBoneContext::new_object();
	m_pBoneContext->OnCreate(this);

	m_pSoundContext = CGameCharaSoundContext::new_object();
	m_pSoundContext->OnCreate(this);

	m_pCollisionContext = CGameDinoCollisionContext::new_object();
	m_pCollisionContext->OnCreate(this);

	m_pHUDContext = CGameCharaHUDContext::new_object();
	m_pHUDContext->OnCreate(this);

	m_pWeaponContext = CGameCharaWeaponContext::new_object();
	m_pWeaponContext->OnCreate(this);

	m_pAnimContext		= 0;
	m_pActionContext	= 0;
	m_pEquipContext		= 0;

	_SetControlsByCharaType();

	setFullHP();
}

void CGameCharaFullbodyDinosaur::CreateLater(const CCharaInfo* db, bool bLoadWeapon)
{
	SetDynamicState(I3GAME_DYNAMIC_STATE_KINEMATIC);

	MATRIX mat;
	I3_ONINSTANCING_INFO info;
	info._pBase = this;
	info._pMatrixStack = g_pFramework->GetViewer()->GetSceneTracer()->GetModelViewMatrixStack();
	
	/*캐릭터 생성시 겹치지 않도록 합니다.*/
	i3Matrix::SetPos(&mat, (REAL32)getCharaNetIndex(), g_pConfigEx->GetPhysix().HeightChara, (REAL32)-getCharaNetIndex());

	info._pMatrixStack->Push(&mat, nullptr);
	OnInstancing(&info);
	info._pMatrixStack->Pop();

	_PHYSIX_SetDefaultVariable();
	getCollisionContext()->AttachCollisionNode(getSceneObject(), 0);

	Cmd_Visible( false);
}

void CGameCharaFullbodyDinosaur::ResetChara()
{
	CGameCharaFullbody::ResetChara();
	for( UINT32 i = 0 ; i < CHARA_STATE_MAX  ; i++)
	{
		if( i == CHARA_STATE_FIRSTRESPAWN || i == CHARA_STATE_DEATH )
			continue;
		
		removeCharaStateMask( i );
	}
	removeEnableStateMask(0xffffffff);
	removeCharaFlag(CHARA_FLAG_RESETABLE);

	if (m_pUpdateContext)		m_pUpdateContext->OnReset();
	if (m_pMaterialContext)		m_pMaterialContext->OnReset();
	if (m_pInputContext)		m_pInputContext->OnReset();
	if (m_pMoveContext)			m_pMoveContext->OnReset();
	if (m_pSoundContext)		m_pSoundContext->OnReset();
	if (m_pCollisionContext)	m_pCollisionContext->OnReset();
	if (m_pCameraContext)		m_pCameraContext->OnReset();
	if (m_pHUDContext)			m_pHUDContext->OnReset();
	if (m_pBoneContext)			m_pBoneContext->OnReset();
	if (m_pWeaponContext)		m_pWeaponContext->OnReset();

	setFullHP();
}

void CGameCharaFullbodyDinosaur::OnBindResource()
{
	CGameCharaFullbody::OnBindResource();
}

void CGameCharaFullbodyDinosaur::OnUpdate(REAL32 tm)
{
	CGameCharaFullbody::OnUpdate(tm);

	m_pInputContext->OnFirstUpdate(tm);
	_UpdateOnlyAI(tm);
	m_pMoveContext->OnUpdate(tm);
	m_pUpdateContext->OnFirstUpdate(tm);
	m_pUpdateContext->OnUpdate(tm);
}
