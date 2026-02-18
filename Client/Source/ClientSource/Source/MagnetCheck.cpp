#include "pch.h"
#include "MagnetCheck.h"

#include "./StageBattle/UserContext.h"
#include "BattleSlotContext.h"

//----------------------------------------------------------//
//Hack Protect//
//----------------------------------------------------------//
CMagnetCheck::CMagnetCheck()
: m_fTime(0.f), m_fWaittingTime(0.f), m_pCharaList(nullptr), m_bDoubtPos(false), 
m_bSendInvalidSpeed(false), m_bStartDetecting(false), m_bIsInit(false)
{	
}

CMagnetCheck::~CMagnetCheck()
{
	Release();
}

void CMagnetCheck::Init( const i3::vector<CGameCharaBase*>& CharaList )
{	
	m_pCharaList = &CharaList;
	m_bIsInit = true;

	InitStartTime();
}

void CMagnetCheck::Release()
{
}

void CMagnetCheck::Update( REAL32 fElapesedTime )
{
#if defined( BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)
	ISNETWORKRETURN(false);
#endif

	if( !m_bIsInit) return;				//리스트 참조 못한 경우
	
	I3ASSERT( m_pCharaList != nullptr );

	if( !m_bStartDetecting )
	{
		m_fWaittingTime += fElapesedTime;
		if( m_fWaittingTime > CHARA_POS_CHECK_START_TIME)
		{	
			m_bStartDetecting = true;
		}
	}
	
	if( !m_bStartDetecting ) return;	//30초 가 아직 안지난 경우
	if( m_bSendInvalidSpeed ) return;	//invalid speed를 한번 보내면 검사를 안한다.

	if( m_bDoubtPos )
	{
		m_fTime += fElapesedTime;
		if( m_fTime > CHARA_POS_CHECK_TIME )
		{
			if( Comfirm() )
			{
				UINT32 nRemainBattleTime = (UINT32)UserContext::i()->fRemainingBattleTime;
				REAL32 rMoveSpeed = CC_MAGNET;

				GameEventSender::i()->SetEvent( EVENT_BATTLE_REMAINTIME_N, &nRemainBattleTime, &rMoveSpeed);

				I3PRINTLOG(I3LOG_DEFALUT, "## Magnet ##"); 
				I3PRINTLOG(I3LOG_DEFALUT, "##  : %.3f	 : %.3f	 : %.3f ##", i3Vector::GetX(&m_Compare.pos), i3Vector::GetY(&m_Compare.pos), i3Vector::GetZ(&m_Compare.pos));
				I3PRINTLOG(I3LOG_DEFALUT, "##  : %.3f	 : %.3f	 : %.3f ##", i3Vector::GetX(&m_Compare.pos2), i3Vector::GetY(&m_Compare.pos2), i3Vector::GetZ(&m_Compare.pos2));

				m_bSendInvalidSpeed = true;
			}
		
			m_bDoubtPos = false;
			m_fTime = 0.f;
		}
	}
	else
	{
		Detect();
	}	
}

bool CMagnetCheck::InRange( const VEC3D* v1, const VEC3D* v2 )
{
	VEC3D vSub;
	i3Vector::Sub( &vSub, v1, v2 );

	REAL32 fLength = i3Vector::Length( &vSub );

	if( fLength <= (CHARA_POS_CHECK_RANGE * 0.01f) ) return true;

	return false;
}

void CMagnetCheck::Detect()
{
	CGameCharaBase* pChara = nullptr;
	CGameCharaBase* pChara2 = nullptr;
	VEC3D* pPos = nullptr;
	VEC3D* pPos2 = nullptr;

	const size_t nCount = m_pCharaList->size();
	for( size_t idx = 0; idx < nCount; ++idx )
	{
		pChara = (*m_pCharaList)[idx];
		if( pChara == nullptr ) continue;

		//예외.
		if( pChara->isAIToClient() ) continue;
		if( pChara->isInvincible() ) continue;
		if( pChara->getCharaNetIndex() == BattleSlotContext::i()->getMySlotIdx() ) continue;
		if( pChara->isCharaStateMask( CHARA_STATE_DEATH ) || pChara->isCharaStateMask(CHARA_STATE_ONOBJECT) ) continue;

		pPos = 	pChara->GetPos();	
 		
		for( size_t idx2 = idx+1; idx2 < m_pCharaList->size(); ++idx2 )
		{
			pChara2 = (*m_pCharaList)[idx2];
			if( pChara2 == nullptr ) continue;

			//예외.
			if( pChara2->isAIToClient() ) continue;
			if( pChara2->isInvincible() ) continue;
			if( pChara2->getCharaNetIndex() == BattleSlotContext::i()->getMySlotIdx() ) continue;
			if( pChara2->isCharaStateMask( CHARA_STATE_DEATH ) || pChara2->isCharaStateMask(CHARA_STATE_ONOBJECT) ) continue;

			pPos2 = pChara2->GetPos();	
			
			if( InRange( pPos, pPos2 ) )
			{
				m_bDoubtPos = true;

				m_Compare.idx = idx;
				m_Compare.idx2 = idx2;

				i3Vector::Copy( &m_Compare.pos, pPos ); 
				i3Vector::Copy( &m_Compare.pos2, pPos2 );

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)			
				I3PRINTLOG(I3LOG_DEFALUT, "## CMagnetDetect		netidx : %d		netidx2 : %d ##", idx, idx2 );
				I3PRINTLOG(I3LOG_DEFALUT, "## CMagnetDetect		Pos x : %.3f	y : %.3f	z : %.3f ##", i3Vector::GetX(pPos), i3Vector::GetY(pPos), i3Vector::GetZ(pPos));
				I3PRINTLOG(I3LOG_DEFALUT, "## CMagnetDetect		Pos2 x : %.3f	y : %.3f	z : %.3f ##", i3Vector::GetX(pPos2), i3Vector::GetY(pPos2), i3Vector::GetZ(pPos2));
#endif
				break;
			}
		}

		if( m_bDoubtPos ) break;
	}
}
bool CMagnetCheck::Comfirm()
{
	CGameCharaBase* pChara = nullptr;
	CGameCharaBase* pChara2 = nullptr;
	VEC3D* pPos = nullptr;
	VEC3D* pPos2 = nullptr;

	pChara = (*m_pCharaList)[m_Compare.idx];
	if( pChara == nullptr ) return false;

	pPos = 	pChara->GetPos();	


	pChara2 = (*m_pCharaList)[m_Compare.idx2];	
	if( pChara2 == nullptr ) return false;

	pPos2 = pChara2->GetPos();	

	return InRange( pPos, pPos2 ) && ( (m_Compare.pos) == (*pPos) ) && ( (m_Compare.pos2) == (*pPos2) );
}