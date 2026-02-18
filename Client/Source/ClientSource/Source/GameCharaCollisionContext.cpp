#include "pch.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"
#include "GlobalFunc.h"

#include "UI/UIBattlePopupBase.h"
#include "UI/UIBattleFrame.h" // 강퇴제안팝업시 닉네임마크를 삭제하기 위해 포함시킴.(11.06.09.수빈)
#include "UI/UIUtil.h"
#include "CrosshairMgr.h"
#include "Crosshair.h"

#include "GameMode.h"
#include "./Mode/Domination/GameMode_Domination.h"
#include "./Mode/Domination/DominationSkillSystem.h"
#include "./UI/HUD/UIHUD_Domination_Manager.h"
#include "Weapon/WeaponItemID.h"
#include "MyRoomInfoContext.h"
#include "GameMissionManager.h"
#include "BattleObjContext.h"

#define HUD_CROSSNICKCHECK_TIME		0.05f	// 크로스헤어 표시 검색( 초당 20회 )
#define PHYSIX_CHARA_CONTACT_TIME	5.f

I3_CLASS_INSTANCE( CGameCharaCollisionContext);

CGameCharaCollisionContext::CGameCharaCollisionContext()
{
}

CGameCharaCollisionContext::~CGameCharaCollisionContext()
{
	I3_SAFE_DELETE(m_CrossHitResult);
	I3_SAFE_DELETE(m_FloorHitResult);
}

void CGameCharaCollisionContext::OnDestroy( void)
{
	CGameCharaContextBase::OnDestroy();

	DetachCollisionNode();
}

void CGameCharaCollisionContext::OnReset( void)
{
	INT32 i;
	CGameCharaContextBase::OnReset();

	m_DropCount = 0;
	for( i = 0; i < CHARA_CONTACT_BONE_COUNT; i++)
	{
		m_bContactBones[ i] = false;
		i3Vector::Zero( &m_vBonesForceDir[ i]);
	}

	m_vecContactReport.clear();
//	m_nContactReportCount		= 0;
//	m_nContactReportReadPos		= 0;
//	for( i = 0; i < PHYSIX_CONTACT_REPORT_COUNT; i++)
//	{
//		i3Physix::ContactReportClear( &m_ContactReportBuf[ i] );
//	}

	i3Physix::HitResultClear( m_CrossHitResult );
	m_rCrossHitTimer			= 0.f;
	m_pCrossHitChara			= nullptr;

	m_rContactTimer				= 0.f;
}



void CGameCharaCollisionContext::SetCollisionGroup( UINT16 group, UINT32 mask)
{	
	m_nCollisionGroup = group;
	m_pOwner->InitColliderSet( group, mask );
}

void CGameCharaCollisionContext::AttachRoughHitCollision()
{
	UINT32 nColGroup = CGRP_TEAM_BLUE_ROUGH;
	if( (m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED) ||
		(m_pOwner->getCharaTeam() & NPC_TEAM_RED) )
		nColGroup = CGRP_TEAM_RED_ROUGH;
	
	m_pRoughHitTrans = i3Transform2::new_object_ref();
#if defined( I3_DEBUG)//노드 이름 세팅
	m_pRoughHitTrans->SetName( "Rough_Coll_Sphere");
#endif

	m_pOwner->getAttachNode()->AddChild( m_pRoughHitTrans);

	{
		i3PhysixShapeSet *	pShapeSet = i3PhysixShapeSet::new_object_ref();

		VEC3D	vZero;
		i3Vector::Set( &vZero, 0.0f, 0.f, 0.f);

		MATRIX mat;
		
		i3Matrix::SetPos( &mat, 0.f, m_rColRadius, 0.f, 1.f );
		
		m_pRoughHitTrans->SetMatrix( &mat );

		pShapeSet->CreateSphereShape( &vZero, m_rColRadius, nColGroup);
		pShapeSet->setEventReceiver( m_pOwner);

		m_pRoughHitTrans->setShapeSet( pShapeSet);
		m_pRoughHitTrans->modifyDynamicState( I3_PHYSIX_STATE_KINEMATIC | I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_RESPONSE , 0);
	}
    
	//렌더링용 충돌 Mesh생성
#if defined( I3_DEBUG) && defined( SHOW_CHARCOL_MESH)//주석으로 되어 있는 디버그
	//m_pRenderableRoughHitCol = i3SceneUtil::BuildRendableCollider( m_pRoughHitColSet, m_pSceneObj, true, true);
#endif
}

void CGameCharaCollisionContext::DetachRoughHitCollision( void)
{
	if( m_pRoughHitTrans != nullptr)
	{
		m_pOwner->getAttachNode()->RemoveChild( m_pRoughHitTrans);
	}
}

void CGameCharaCollisionContext::AttachCollisionNode( i3Node * pParentNode, UINT32 nColGroupIndex)
{
	DetachCollisionNode();

	if( (m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED) ||
		(m_pOwner->getCharaTeam() & NPC_TEAM_RED) )
		SetCollisionGroup( CGRP_TEAM_RED, CGRP_MASK_TEAM_RED );
	else
		SetCollisionGroup( CGRP_TEAM_BLUE, CGRP_MASK_TEAM_BLUE );

	//World Collision
	getMoveContext()->CreateController();

	AttachRoughHitCollision();
}

void CGameCharaCollisionContext::DetachCollisionNode( void)
{
	if( getMoveContext() != nullptr)
		getMoveContext()->ReleaseController();

	//렌더링용 충돌 Mesh제거
#if defined( I3_DEBUG) && defined( SHOW_CHARCOL_MESH)//주석으로 되어 있는 디버그
	//m_pOwner->getAttachNode()->RemoveChild( m_pRenderableWorldCol);
#endif
}

void CGameCharaCollisionContext::SetContactShapeSetEnable( bool bVal)
{
	/*
	if( m_pContactShapeSetList == nullptr )
		return;

	for( INT32 i = 0; i < m_pContactShapeSetList->GetCount(); i++ )
	{
		if( i < 2)
		{
			i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) m_pContactShapeSetList->GetItem( i );
			if( bVal )
				pShapeSet->modifyDynamicState( 0, I3_PHYSIX_STATE_NO_COLLISION);
			else
				pShapeSet->modifyDynamicState( I3_PHYSIX_STATE_NO_COLLISION, 0);
		}
	}
	*/
}

void CGameCharaCollisionContext::QueContactReport( const I3_PHYSIX_CONTACT_REPORT_PTR& spReport)
{
//	UINT32 nCount = m_nContactReportCount - m_nContactReportReadPos;
	if( m_vecContactReport.size() >= PHYSIX_CONTACT_REPORT_COUNT )
	{
		I3TRACE( "CGameCharaCollisionContext::QueContactReport() - buffer over flow.\n" );
		return;
	}
	
	m_vecContactReport.push_back(spReport);

//	UINT32 i = m_nContactReportCount % PHYSIX_CONTACT_REPORT_COUNT;
//
//	i3Physix::ContactReportCopyTo( &m_ContactReportBuf[ i ], pReport );
//
//	m_nContactReportCount++;
}

void CGameCharaCollisionContext::ParseContactReport( void )
{
	const UINT32 nCount = (UINT32)m_vecContactReport.size();	// m_nContactReportCount - m_nContactReportReadPos;
	if( nCount == 0 )
		return;

	UINT32 i;

	bool	bCheck = false;
	INT32	nEffAndSndCnt	= 0;
	VEC3D	vUp;

	i3Vector::Set( &vUp, 0.f, 1.f, 0.f );

	for( i = 0; i < nCount; i++ )
	{
//		UINT32 nPos = (m_nContactReportReadPos + i) % PHYSIX_CONTACT_REPORT_COUNT;
//		I3_PHYSIX_CONTACT_REPORT * pReport = &m_ContactReportBuf[ nPos ];
		I3_PHYSIX_CONTACT_REPORT * pReport = m_vecContactReport[i].get();

		i3PhysixShapeSet * pShapeSet = pReport->m_pOwnShapeSet;
		const char * pszName = pShapeSet->GetName();

		if( pszName[12] == 'R' && pszName[15] == 't')
		{
			bCheck = true;
		}

		if( pReport->m_Events == NX_NOTIFY_ON_END_TOUCH )
		{
			if( bCheck )
				m_pOwner->removeCharaStateMask( CHARA_STATE_PHYSIXCONTACT);
		}
		else if( (pReport->m_Events == NX_NOTIFY_ON_START_TOUCH) )
		{
			UINT32 contactCnt = pReport->m_ContactList.size();
			for( UINT32 j = 0; j < contactCnt; j++ )
			{
				I3_PHYSIX_CONTACT_INFO * pPointInfo = pReport->m_ContactList.at(j);

				if( !m_pOwner->isCharaStateMask( CHARA_STATE_PHYSIXCONTACT) )
				{
					if( i3Vector::Dot( &pPointInfo->m_Normal, &vUp ) > I3_EPS + 0.9f )
					{
						m_pOwner->addCharaStateMask( CHARA_STATE_PHYSIXCONTACT);
					}
				}

				{	
					if( pszName[0] == 'C' && (pszName[12] == 'L' || pszName[12] == 'R') &&
						!m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
					{	// Foot contact
						g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_WALK_COMMON, &pPointInfo->m_Intersect, &pPointInfo->m_Normal );
					}
					else
					{	// Body contact
						g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, &pPointInfo->m_Intersect, &pPointInfo->m_Normal );
					}

					if( bCheck )
					{	
						if( nEffAndSndCnt  <= 0 && pReport->m_pDestShapeSet != nullptr && (m_DropCount == 0 || pPointInfo->m_rNormalForce > 0.f) )
						{
							I3_PHYSIX_TERRAIN * pTri = pReport->m_pDestShapeSet->getTerrain(0);
							if( pTri != nullptr)
							{
								g_pSndMng->StartCharacterActionSnd( GTBCA_ON_THUD, i3Math::Rand()%2, &pPointInfo->m_Intersect, m_pOwner->is1PV() );
								m_DropCount++;
								//I3TRACE("터어어얼~~~쓰~~~어~~~억~~~..!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
							}

							nEffAndSndCnt++;
						}
					}
				}
			}
		}
	}

	m_vecContactReport.clear();

//	m_nContactReportReadPos += nCount;
}

void CGameCharaCollisionContext::ProcessCollisionEffect( void)
{	
	VEC3D vec;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_PHYSIXCONTACT))
		return;

	m_rContactTimer += g_pViewer->getDeltaTime();
	if( m_rContactTimer > PHYSIX_CHARA_CONTACT_TIME)
	{
		m_pOwner->addCharaStateMask( CHARA_STATE_PHYSIXCONTACT);
		return;
	}

	CGameCharaBoneContext * pBoneCtx = getBoneContext();
	
	i3Skeleton * pSkel = m_pOwner->getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();
	i3PhysixShapeSet * pShapeSet = nullptr;
	REAL32 force = 0.f;
	REAL32 dot = 0.f;
	INT32 iBoneRoot = 0;
	INT32 iBoneSpine3 = 0;

	//렙터스팅
	if(m_pOwner->IsRaptorBase())
	{
		iBoneRoot = pBoneCtx->getBoneIndex( RAPTOR_BONE_ROOT);
		iBoneSpine3 = pBoneCtx->getBoneIndex( RAPTOR_BONE_SPINE1);
	}
	else if(m_pOwner->IsRex())
	{
		iBoneRoot = pBoneCtx->getBoneIndex( TREX_BONE_ROOT);
		iBoneSpine3 = pBoneCtx->getBoneIndex( TREX_BONE_SPINE1);
	}
#ifdef DOMI_DINO
	else if (m_pOwner->IsDomiRaptorBoneBase())
	{
		iBoneRoot = pBoneCtx->getBoneIndex(DOMI_RAPTOR_BONE_ROOT);
		iBoneSpine3 = pBoneCtx->getBoneIndex(DOMI_RAPTOR_BONE_SPINE1);
	}
	else if (m_pOwner->IsDomiMutantRex())
	{
		iBoneRoot = pBoneCtx->getBoneIndex(DOMI_MUTANTREX_BONE_ROOT);
		iBoneSpine3 = pBoneCtx->getBoneIndex(DOMI_MUTANTREX_BONE_SPINE1);
	}
#endif
	else
	{
		iBoneRoot = pBoneCtx->GetRootBone();
		iBoneSpine3 = pBoneCtx->GetSpine3Bone();
	}

	if( !m_bContactBones[ CHARA_CONTACT_BONE_ROOT] )
	{	// Root
		pShapeSet = pSkel->getShapeSet( iBoneRoot );
		if( pShapeSet != nullptr)
		{
			pShapeSet->getLinearVelocity( &vec);

			force = i3Vector::Length( &vec);

			//if( force > I3_EPS)
			{
				dot = i3Vector::Dot( &m_vBonesForceDir[ CHARA_CONTACT_BONE_ROOT], &vec);

				if( dot < 0.3f && force < 0.1f)
				//if( force < 0.3f)
				{
					//pShapeSet->getShapeGlobalPose( &mat);
					//g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
					m_pOwner->addCharaStateMask( CHARA_STATE_PHYSIXCONTACT);
					m_bContactBones[ CHARA_CONTACT_BONE_ROOT] = true;
				}

				i3Vector::Copy( &m_vBonesForceDir[ CHARA_CONTACT_BONE_ROOT], &vec);
			}
		}
	}

	if( !m_bContactBones[ CHARA_CONTACT_BONE_SPINE3] )
	{	// Spine3
		pShapeSet = pSkel->getShapeSet( iBoneSpine3 );
		if( pShapeSet != nullptr)
		{
			pShapeSet->getLinearVelocity( &vec);

			force = i3Vector::Length( &vec);

			//if( force > I3_EPS )
			{
				dot = i3Vector::Dot( &m_vBonesForceDir[ CHARA_CONTACT_BONE_SPINE3], &vec);
				if( dot < 0.3f && force < 0.1f)
				//if( force < 0.3f)
				{
					//pShapeSet->getShapeGlobalPose( &mat);
					//g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
					m_pOwner->addCharaStateMask( CHARA_STATE_PHYSIXCONTACT);
					m_bContactBones[ CHARA_CONTACT_BONE_SPINE3] = true;
				}

				i3Vector::Copy( &m_vBonesForceDir[ CHARA_CONTACT_BONE_SPINE3], &vec);
			}
		}
	}
}

void CGameCharaCollisionContext::ContactShapeSetUpdate( void)
{
	/*
	if( m_pContactShapeSetList != nullptr)
	{
		for( INT32 i = 0; i < m_pContactShapeSetList->GetCount(); i++ )
		{
			if( i < 2)
			{
				i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) m_pContactShapeSetList->GetItem( i );
				//pShapeSet->modifyDynamicState( 0, I3_PHYSIX_STATE_NO_COLLISION);
			}
		}
	}
	*/
}
CHARA_HIT_PART CGameCharaCollisionContext::GetHitPart( i3PhysixShapeSet * pDestShapeSet)
{
	if(m_pOwner->IsRaptorBase() 
#ifdef DOMI_DINO
		|| m_pOwner->IsDomiRaptorBoneBase()
#endif
		)
	{
		return _GetHitPartRaptor(pDestShapeSet);
	}	
	else if(m_pOwner->IsRex() 
#ifdef DOMI_DINO
		|| m_pOwner->IsDomiMutantRex()
#endif
		)
	{
		return _GetHitPartRex(pDestShapeSet);
	}
	else
	{
		return _GetHitHuman(pDestShapeSet);
	}
}

CHARA_HIT_PART CGameCharaCollisionContext::_GetHitHuman( i3PhysixShapeSet * pDestShapeSet)
{	
	I3ASSERT( pDestShapeSet != nullptr);

	CHARA_HIT_PART	part = CHARA_HIT_UNKNOWN;
	const char * pszCollName = pDestShapeSet->GetName();
	if( pszCollName == nullptr ) return part;

	switch( pszCollName[0])
	{
		case 'M' : //Model_Helmet
			if( pszCollName[2] == 'd')
				part = CHARA_HIT_HEAD;
			break;
		case 'H' :
			if( pszCollName[1] == 'N' )
			{
				if( pszCollName[3] == 'L' )		part = CHARA_HIT_LHAND;		// Left Hand
				else							part = CHARA_HIT_RHAND;		// Right Hand
			}
			else
			{
				part = CHARA_HIT_HEAD;			// Head
			}
			break;
		case 'B' :
			if( pszCollName[2] == 'N' )			part = CHARA_HIT_NECK;		// Neck
			else if( pszCollName[2] == 'S' )
			{
				switch( pszCollName[ 7 ] )
				{
				case '3' :	part = CHARA_HIT_BODY1;		break;				// Body1
				case '2' :	part = CHARA_HIT_BODY2;		break;				// Body2
				case '1' :	part = CHARA_HIT_BODY3;		break;				// Body3
				default :	part = CHARA_HIT_ROOT;		break;
				}
			}
			else if( pszCollName[4] == 'C' )
			{
				if( pszCollName[2] == 'L' ) part = CHARA_HIT_LCLAVICLE;		// Left Clavicle
				else						part = CHARA_HIT_RCLAVICLE;		// Right Clavicle
			}
			else if( pszCollName[ 6] == 'H')
			{	// NPC Giran
				part = CHARA_HIT_HEAD;
			}
			else
				part = CHARA_HIT_ROOT;			// Root
			break;
		case 'A' :
			if( pszCollName[1] == 'U' )
			{
				if( pszCollName[3] == 'L' ) part = CHARA_HIT_LUARM;		// Left UpperArm
				else						part = CHARA_HIT_RUARM;		// Right UpperArm
			}
			else
			{
				if( pszCollName[3] == 'L' )	part = CHARA_HIT_LDARM;		// Left ForeArm
				else						part = CHARA_HIT_RDARM;		// Right ForeArm
			}
			break;
		case 'L' :
			if( pszCollName[1] == 'U' )
			{
				if( pszCollName[3] == 'L' )	part = CHARA_HIT_LULEG;		// Left Thigh
				else						part = CHARA_HIT_RULEG;		// Right Thigh
			}
			else
			{
				if( pszCollName[3] == 'L' )	part = CHARA_HIT_LDLEG;		// Left Calf
				else						part = CHARA_HIT_RDLEG;		// Right Calf
			}
			break;
		case 'F' :
			if( pszCollName[2] == 'L' )		part = CHARA_HIT_LFOOT;		// Left Foot
			else							part = CHARA_HIT_RFOOT;		// Right Foot
			break;
		default:
			{
				//I3TRACE( "[%s] WARNING! Invalid Hit Part (%s)\n", __FUNCTION__, pszCollName);
			}
			break;
	}

		
	//I3TRACE( "HIT NAME : %s\n", pszCollName);

	return part;
}

CHARA_HIT_PART CGameCharaCollisionContext::_GetHitPartRex( i3PhysixShapeSet * pDestShapeSet)
{	
	I3ASSERT( pDestShapeSet != nullptr);

	CHARA_HIT_PART	part = CHARA_HIT_UNKNOWN;
	const char * pszCollName = pDestShapeSet->GetName();
	if( pszCollName == nullptr ) return part;

	switch( pszCollName[0])
	{
		case 'H' :
			if( pszCollName[1] == 'E' )				// H_H
			{
				part = CHARA_HIT_VITAL_EYE;				// H_Head
				//I3TRACE( "HIT CHARA_HIT_VITAL_EYE : %s\n", pszCollName);
			}
			else if( pszCollName[2] == 'H' )				// H_H
			{
				part = CHARA_HIT_HEAD;				// H_Head
				//I3TRACE( "HIT CHARA_HIT_HEAD : %s\n", pszCollName);
			}
			else if( pszCollName[1] == 'J' )
			{
				part = CHARA_HIT_JAW;				// H_Jaw
				//I3TRACE( "HIT CHARA_HIT_JAW : %s\n", pszCollName);
			}
			else if( pszCollName[3] == 'L' )
			{
				part = CHARA_HIT_LHAND;				// HN_L Hand
				//I3TRACE( "HIT CHARA_HIT_LHAND : %s\n", pszCollName);
			}
			else if( pszCollName[3] == 'R' )
			{
				part = CHARA_HIT_RHAND;				// HN_R Hand
				//I3TRACE( "HIT CHARA_HIT_RHAND : %s\n", pszCollName);
			}
			else
			{
				I3ASSERT_0;
			}
			break;
		case 'B' :
			if( pszCollName[2] == 'N' )				// B_N
			{
				if( pszCollName[8] == 'L' )
				{
					part = CHARA_HIT_NECK_LOWER;			// B_Neck1
					//I3TRACE( "HIT CHARA_HIT_VITAL_NECK : %s\n", pszCollName);
				}
				else if( pszCollName[6] == '1' )
				{
					part = CHARA_HIT_NECK;			// B_Neck1
					//I3TRACE( "HIT CHARA_HIT_NECK : %s\n", pszCollName);
				}
				else if(pszCollName[6] == '2' )
				{
					part = CHARA_HIT_NECK2;			// B_Neck2
					//I3TRACE( "HIT CHARA_HIT_NECK2 : %s\n", pszCollName);
				}
				else if(pszCollName[3] == 'e' )
				{
					part = CHARA_HIT_NECK2;			// B_Neck
					//I3TRACE( "HIT CHARA_HIT_NECK2 : %s\n", pszCollName);
				}
				else
				{
					I3ASSERT_0;
				}
				
			}
			else if( pszCollName[2] == 'S' )		// B_S
			{
				if( pszCollName[7] == '2' )			// B_Spine2
				{
					part = CHARA_HIT_BODY1;
					//I3TRACE( "HIT CHARA_HIT_BODY1 : %s\n", pszCollName);
				}
				else if( pszCollName[7] == '1' )	// B_Spine3
				{
					part = CHARA_HIT_BODY2;
					//I3TRACE( "HIT CHARA_HIT_BODY2 : %s\n", pszCollName);
				}
				else if( pszCollName[7] == '3' )	// B_Spine1
				{
					part = CHARA_HIT_BODY3;
					//I3TRACE( "HIT CHARA_HIT_BODY3 : %s\n", pszCollName);
				}
				else
				{
					I3ASSERT_0;
				}
			}
			else if( pszCollName[2] == 'P' )		// B_P
			{
				part = CHARA_HIT_ROOT;
			}
			else if( pszCollName[2] == 'R' )		// B_Root
			{
				part = CHARA_HIT_ROOT;
				//I3TRACE( "HIT CHARA_HIT_ROOT : %s\n", pszCollName);
			}
			else
			{
				I3ASSERT_0;
			}
			break;
		case 'T' :
			if( pszCollName[6] == '1' )				
			{
				part = CHARA_HIT_TAIL1;				// T_Tail1
				//I3TRACE( "HIT CHARA_HIT_TAIL1 : %s\n", pszCollName);
			}
			else if( pszCollName[6] == '2' )				
			{
				part = CHARA_HIT_TAIL2;				// T_Tail2
				//I3TRACE( "HIT CHARA_HIT_TAIL2 : %s\n", pszCollName);
			}
			else if( pszCollName[6] == '3' )				
			{
				part = CHARA_HIT_TAIL3;				// T_Tail3
				//I3TRACE( "HIT CHARA_HIT_TAIL3 : %s\n", pszCollName);
			}
			else if( pszCollName[6] == '4' )				
			{
				part = CHARA_HIT_TAIL4;				// T_Tail4
				//I3TRACE( "HIT CHARA_HIT_TAIL4 : %s\n", pszCollName);
			}
			else
			{
				I3ASSERT_0;
			}
			break;
		case 'F' :
			if( pszCollName[1] == 'N')
			{
				if( pszCollName[3] == 'L' )				
				{
					part = CHARA_HIT_LHAND;				// F_L Foot
					//I3TRACE( "HIT CHARA_HIT_LHAND : %s\n", pszCollName);
				}
				else if( pszCollName[3] == 'R' )				
				{
					part = CHARA_HIT_RHAND;				// F_R Foot
					//I3TRACE( "HIT CHARA_HIT_RHAND : %s\n", pszCollName);
				}
				else
				{
					I3ASSERT_0;
				}
			}
			else 
			{
				if( pszCollName[2] == 'L' )				
				{
					part = CHARA_HIT_LFOOT;				// F_L Foot
					//I3TRACE( "HIT CHARA_HIT_LFOOT : %s\n", pszCollName);
				}
				else if( pszCollName[2] == 'R' )				
				{
					part = CHARA_HIT_RFOOT;				// F_R Foot
					//I3TRACE( "HIT CHARA_HIT_RFOOT : %s\n", pszCollName);
				}
				else
				{
					I3ASSERT_0;
				}
			}
			break;
			
		case 'A' :
			if( pszCollName[1] == 'U' )				// AU
			{
				if( pszCollName[3] == 'L' ) 
				{
					part = CHARA_HIT_LUARM;			// AU_L UpperArm
					//I3TRACE( "HIT CHARA_HIT_LUARM : %s\n", pszCollName);
				}
				else if( pszCollName[3] == 'R' ) 
				{
					part = CHARA_HIT_RUARM;			// AU_R UpperArm
					//I3TRACE( "HIT CHARA_HIT_RUARM : %s\n", pszCollName);
				}
				else
				{
					//I3ASSERT_0;
				}
			}
			else if( pszCollName[1] == 'D' )		// AD
			{
				if( pszCollName[3] == 'L' ) 
				{
					part = CHARA_HIT_LDARM;			// AD_L Forearm
					//I3TRACE( "HIT CHARA_HIT_LDARM : %s\n", pszCollName);
				}
				else if( pszCollName[3] == 'R' ) 
				{
					part = CHARA_HIT_RDARM;			// AD_R Forearm
					//I3TRACE( "HIT CHARA_HIT_RDARM : %s\n", pszCollName);
				}
				else
				{
					I3ASSERT_0;
				}
			}
			else
			{
				I3ASSERT_0;
			}
			break;
		case 'L' :
			if( pszCollName[1] == 'U' )				// LU
			{
				if( pszCollName[3] == 'L' ) 
				{
					part = CHARA_HIT_LULEG;			// LU_L Thigh
					//I3TRACE( "HIT CHARA_HIT_LULEG : %s\n", pszCollName);
				}
				else if( pszCollName[3] == 'R' ) 
				{
					part = CHARA_HIT_RULEG;			// LU_R Thigh
					//I3TRACE( "HIT CHARA_HIT_RULEG : %s\n", pszCollName);
				}
				else
				{
					I3ASSERT_0;
				}
			}
			else if( pszCollName[1] == 'D' )		// LD
			{
				if( pszCollName[3] == 'L' ) 
				{
					part = CHARA_HIT_LDLEG;			// LD_L Calf
					//I3TRACE( "HIT CHARA_HIT_LDLEG : %s\n", pszCollName);
				}
				else if( pszCollName[3] == 'R' ) 
				{
					part = CHARA_HIT_RDLEG;			// LD_R Calf
					//I3TRACE( "HIT CHARA_HIT_RDLEG : %s\n", pszCollName);
				}
				else
				{
					I3ASSERT_0;
				}
			}
			else if( pszCollName[1] == 'H' )		// LH
			{
				if( pszCollName[3] == 'L' ) 
				{
					part = CHARA_HIT_LHHORSE_LINK;	// LH_L HorseLink
					//I3TRACE( "HIT CHARA_HIT_LHHORSE_LINK : %s\n", pszCollName);
				}
				else if( pszCollName[3] == 'R' ) 
				{
					part = CHARA_HIT_RHHORSE_LINK;	// AD_R Forearm
					//I3TRACE( "HIT CHARA_HIT_RHHORSE_LINK : %s\n", pszCollName);
				}
				else
				{
					I3ASSERT_0;
				}
			}
			else
			{
				I3ASSERT_0;
			}
			break;
		default:
			{
				I3ASSERT_0;
			}
			break;
	}

	//I3TRACE( "HIT NAME : %s\n", pszCollName);

	return part;
}

CHARA_HIT_PART CGameCharaCollisionContext::_GetHitPartRaptor( i3PhysixShapeSet * pDestShapeSet)
{	
	I3ASSERT( pDestShapeSet != nullptr);

	return GlobalFunc::GetHitPartRaptor(pDestShapeSet->GetName());
}

bool CGameCharaCollisionContext::IsOverlappedBy( UINT32 nGroupIdx)
{	
	VEC3D vec;

	i3Vector::Copy( &vec, m_pOwner->GetPos() );
	i3Vector::Add( &vec, 0.f, RADIUS+0.05f, 0.f );
	NxVec3 v1( getX(&vec), getY(&vec), getZ(&vec) );
	i3Vector::Add( &vec, 0.f, CAPSULE_FULL_HEIGHT-0.05f, 0.f );
	NxVec3 v2( getX(&vec), getY(&vec), getZ(&vec) );
	NxSegment	seg( v1, v2 );
	NxCapsule Capsule( seg, RADIUS );
	
	NxScene * pScene = g_pPhysixCtx->getScene();
 	NxU32	activeGrp = g_pPhysixCtx->getGroupMask( nGroupIdx );

	activeGrp &= ~(( 1 << CGRP_TRIGGER ) | ( 1 << CGRP_TRIGGER_ALL));
	
	return pScene->checkOverlapCapsule( Capsule, NX_ALL_SHAPES, activeGrp, nullptr);
}

void CGameCharaCollisionContext::_CheckCrossHitMissionBomb( void)
{
	VEC3D vAt;
	REAL32 rLen = 0.F;
	WeaponBase* pWeapon = nullptr;
	CWeaponInfo* pInfo = nullptr;

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);

	pWeapon = m_pOwner->GetCurrentCharaWeapon();

	if( pWeapon == nullptr)	return;	//	무기가 없을 수도 있다.
	pInfo = pWeapon->getWeaponInfo();
	if( pInfo == nullptr)		return;	//	무기가 없을 수도 있다.

	MATRIX mtx;
	VEC3D pos;

	Crosshair * pCrosshair = CrosshairMgr::i()->getCrosshair();
	if (pCrosshair->getTargetChara() && pCrosshair->getTargetChara()->getCameraContext())
		pCrosshair->getTargetChara()->getCameraContext()->getVirtualCamPosition(&pos, &mtx);
	else
		getCameraContext()->getVirtualCamPosition( &pos, &mtx);

	i3Vector::Scale( &vAt, i3Matrix::GetAt( &mtx), - 1000.f);

	m_CrossHitLine.SetStart( i3Matrix::GetPos( g_pCamera->getCamMatrix()));
	i3Vector::Add( &vAt, &vAt, &pos);
	m_CrossHitLine.SetEnd( &vAt );

	//World에 대해서 한번 테스트
	I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &m_CrossHitLine, CGRP_MASKGROUP_ATTACK);
	if( pResult != nullptr)
	{	
		i3Physix::HitResultCopyTo( m_CrossHitResult, pResult );
	}
	else
	{	
		m_CrossHitResult->m_pDestShapeSet = nullptr;
		m_CrossHitResult->m_pHitShape = nullptr;
		m_CrossHitResult->m_pTerrain = nullptr;
		m_CrossHitResult->m_T = 100000.f;
	}

	if( pMissionMng->IsPossibleDisarmBomb())	// 해체 가능한지
	{
		i3::vector<i3GameObj *> ObjectList;
		REAL32 rMaxDot = 0.6f;
		REAL32 rAddWidth = 0;		//폭이큰 오브젝트를 위해 추가폭

		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB))
		{
		#if defined( USE_EVENT_SHUTTLEX)
			ObjectList.push_back( g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SHUTTLEX)));
		#else
			ObjectList.push_back( (i3GameObj *)g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4) ));
		#endif
		}
		else if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
		{
			ObjectList.push_back( (i3GameObj *)g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4) ));
		}
		else if ( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY))
		{
			BattleObjContext::i()->getObjectMission(ObjectList);
			rAddWidth = 3;
		}

		//BattleObjContext::i()->CreateBattle();

		for(size_t i = 0; i < ObjectList.size(); i++)
		{
			i3GameObj *pObject = ObjectList.at(i);
			if( pObject != nullptr )
			{
				if(pMissionMng->IsEnableMy() == false) 
				{
					return;
				}

				MATRIX matWeaponPos;

				GlobalFunc::GetNowFrameMatrix(&matWeaponPos, pObject->getSceneObject());

				VEC3D vDir;

				// i3Vector::Sub( &vDir, pWeapon->GetPos(), i3Matrix::GetPos( g_pCamera->getCamMatrix()) );
				I3ASSERT(pObject->getSceneObject());
				I3ASSERT(pObject->getSceneObject()->GetCacheMatrix());
				i3Vector::Sub( &vDir, i3Matrix::GetPos(&matWeaponPos), i3Matrix::GetPos( g_pCamera->getCamMatrix()) );

				rLen = i3Vector::Length( &vDir);
				i3CollideeLine cLine;
				cLine.SetStart(i3Matrix::GetPos( g_pCamera->getCamMatrix()));
				cLine.SetDir(&vDir);
				//World에 대해서 한번 테스트
				I3_PHYSIX_HIT_RESULT * pResultBomb = g_pFramework->raycastClosest( &cLine, CGRP_MASKGROUP_ATTACK);
				REAL32 rCollisionLen = 1000000.f;
				if( pResultBomb != nullptr)
				{	
					rCollisionLen = pResultBomb->m_T;
				}

				// 거리 계산은 카메라의 시점부터 계산합니다.
				// if( rLen < m_CrossHitResult.m_T + 0.1f)
				//I3TRACE("rLen:rCollisionLen[%f, %f]\n", rLen, rCollisionLen);

				//폭탄과 캐릭터의 높이 차이도 고려해야하기 때문에 카메라 포지션 -> 캐릭터 포지션으로 변경
				i3Vector::Sub(&vDir, i3Matrix::GetPos(&matWeaponPos), m_pOwner->GetPos());

 				if( rLen < rCollisionLen+rAddWidth && 
					(i3Math::abs(getY(&vDir)) < 0.8f))	//높이도 고려해야함 서버 1.1m 보다 약간 작은 0.8m로
				{
					// 화면에 대한 각도 계산은 캐릭터 포지션으로 합니다.
					// 카메라 포지션으로 하면 캐릭터가 앉으면서 해체가 풀리게 됩니다.
					i3Vector::Normalize( &vDir, &vDir );
					REAL32 rDot = i3Vector::Dot( &vDir, i3Matrix::GetAt( m_pOwner->GetMatrix() ));
					//I3TRACE("rDot:rMaxDot[%f, %f]\n", rDot, rMaxDot);
					if( rDot > rMaxDot )
					{
						CGameMissionManager::i()->setEnableCrossHit( true);
						break;
					}
					else
					{
						CGameMissionManager::i()->setEnableCrossHit( false);
					}
				}
				else
				{	// 벽 뒤에 있는 경우를 체크합니다. 벽 뒤에 있으면 해체 불가능으로 체크
					CGameMissionManager::i()->setEnableCrossHit( false);
				}
			}
			else
			{
				CGameMissionManager::i()->setEnableCrossHit( false);
			}
		}

	}
}

bool CGameCharaCollisionContext::UpdateNickOnTarget( void)
{	
	bool	bNeutralColor = true;
	
	// 캐릭터의 넷 인덱스가 -1인 경우는 해당 캐릭터가 나간경우이다.
	// 업데이트 순서상 이전에 체크된 캐릭터이므로 -1이면 출력하지 말아야합니다. -by dfly 2007.12.20
	if( (m_pCrossHitChara != nullptr) &&
		(m_pCrossHitChara->getCharaInfo() != nullptr) &&
		(m_pCrossHitChara->getCharaNetIndex() != -1) &&
		(m_pCrossHitChara->isNetwork() || m_pCrossHitChara->isAI() ) )
	{
		if( LocaleValue::Enable( "ForciblyRemove" ) )
		{
			UIBattlePopupBase * p = GameUI::GetBattlePopup( UBP_ACCUSE_CRIME);
			if( p != nullptr && p->isEnable() == false)
				m_pCrossHitChara->Cmd_SetCharaNick( false);
		}
		else
		{
			m_pCrossHitChara->Cmd_SetCharaNick( false);
		}
	}

	m_rCrossHitTimer = m_rCrossHitTimer + g_pViewer->getDeltaTime();
	// 초당 20회만 체크
	if( m_rCrossHitTimer < HUD_CROSSNICKCHECK_TIME )
		return false;
		
	Crosshair * pCrosshair = CrosshairMgr::i()->getCrosshair();

	// 폭탄을 바라보고 있는지 확인합니다.
	_CheckCrossHitMissionBomb();

	//
	UIHUDDominationMgr* domi_hud_mgr = UIHUDDominationMgr::i();
	if( domi_hud_mgr)
		domi_hud_mgr->GetHUD_DinoHp().print( false);

	m_pCrossHitChara = nullptr;
	m_CrossHitDist = 0.0f;

	WeaponBase * pWeapon = nullptr;
	UINT32 collGroup;

	if (pCrosshair->getTargetChara())
	{
		pWeapon = pCrosshair->getTargetChara()->GetCurrentCharaWeapon();

		if (pCrosshair->getTargetChara()->getCharaTeam() & CHARACTER_TEAM_BLUE)
			collGroup = CGRP_MASKGROUP_CHARA_RED;
		else
			collGroup = CGRP_MASKGROUP_CHARA_BLUE;
	}
	else
	{
		pWeapon = m_pOwner->GetCurrentCharaWeapon();

		if (m_pOwner->getCharaTeam() & CHARACTER_TEAM_BLUE)
			collGroup = CGRP_MASKGROUP_CHARA_RED;
		else
			collGroup = CGRP_MASKGROUP_CHARA_BLUE;
	}

	if( pWeapon == nullptr )	return false;
	pWeapon = pWeapon->getUsedWeapon();
	I3ASSERT( pWeapon != nullptr);
	CWeaponInfo * pInfo = pWeapon->getWeaponInfo();
	if( pInfo == nullptr )	return false;

 	bool bCheck = pInfo->isUnconditionalNeutralColorCrossHair();

	if(bCheck)
	{
		bNeutralColor = true;
	}
	else
	{
		I3_PHYSIX_HIT_RESULT * pResult;

		REAL32 rMinLength = MIN( m_CrossHitResult->m_T, pWeapon->GetRange());

		UINT32 nResultCount = g_pFramework->raycast( &m_CrossHitLine, collGroup);
		if( nResultCount > 0 )
		{
			pResult = g_pFramework->getRaycastResult();

			CGameCharaBase* _near_chara = nullptr;
			REAL32 _near_dist = rMinLength;
			for( UINT32 i = 0; i < nResultCount; i++ )
			{
				I3ASSERT( pResult[ i ].m_pDestShapeSet != nullptr);

				if( !i3::kind_of<CGameCharaBase*>(pResult[ i ].m_pDestShapeSet->getEventReceiver()))
					continue;

				CGameCharaBase * pChara = (CGameCharaBase *) pResult[ i ].m_pDestShapeSet->getEventReceiver();

				if( rMinLength < pResult[ i].m_T )
					continue;

				if( !(pChara->isCharaStateMask( CHARA_STATE_DEATH)) )
				{
					bNeutralColor = false;
					m_CrossHitDist = rMinLength = pResult[ i].m_T;
					m_pCrossHitChara = pChara;

					if( _near_dist > pResult[ i].m_T)
					{
						_near_chara = pChara;
						_near_dist = pResult[ i].m_T;
					}
				}
			}
			if( _near_chara && domi_hud_mgr)
				domi_hud_mgr->GetHUD_DinoHp().print( true, _near_chara);
		}
	}

	if( bNeutralColor)//보통색
	{
		// 조준된 상대라도 죽엇으면 보통 색상으로 변경
		//pCrosshair->SetCrosshairColor( 0, 255, 0);
		if (pCrosshair)
			pCrosshair->SetCrosshairColor();
		else
			TRACE("CGameCharaCollisionContext::UpdateNickOnTarget pCrosshair=nullptr\n");
	}
	else			  //뻘건색
	{	
		if (pCrosshair)
			pCrosshair->SetCrosshairColor( 255, 0, 0);
		else
			TRACE("CGameCharaCollisionContext::UpdateNickOnTarget pCrosshair=nullptr\n");
	}

	m_rCrossHitTimer = 0.f;
	return true;
}

