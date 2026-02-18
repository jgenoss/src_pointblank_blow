#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"
#include "BattleHud.h"
#include "ConfigRenderBattle.h"
#include "BattleServerContext.h"
#include "../StageBattle/DinoModeContext.h"

#include "./StageBattle/ScoreContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GameMissionManager.h"

#include "ClientStageInfoUtil.h"
#include "i3Scene/i3XrayScene.h"

#define		SLICE		0.033f
#define		INVINCIBLE_MIN_ALPHA		0.4f

namespace MoS //MasterOfSpeciality : 주특기
{
	const REAL32 Scn_Width = 0.8f;

	const REAL32 Img_w = 0.050f, Img_h = 0.096f;
	
	const REAL32 MG_x = 0.53f, MG_y = 0.38f;
	const REAL32 Sniper_x = 0.58f, Sniper_y = 0.38f;
	const REAL32 Assault_x = 0.63f, Assault_y = 0.38f;
	const REAL32 SMG_x = 0.53f, SMG_y = 0.48f;
	const REAL32 Shotgun_x = 0.58f, Shotgun_y = 0.48f;
}

I3_CLASS_INSTANCE( CGameCharaMaterialContext);//, CGameCharaContextBase);

COLORREAL CGameCharaMaterialContext::COL_BLACK = { 0.0f, 0.0f, 0.0f, 0.0f };
COLORREAL CGameCharaMaterialContext::COL_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
COLORREAL CGameCharaMaterialContext::COL_RED = { 1.0f, 0.0f, 0.0f, 1.0f };
COLORREAL CGameCharaMaterialContext::COL_GREEN = { 0.0f, 1.0f, 0.0f, 1.0f };
COLORREAL CGameCharaMaterialContext::COL_BLUE = { 0.0f, 0.0f, 1.0f, 1.0f };

CGameCharaMaterialContext::~CGameCharaMaterialContext()
{
	OnDestroy();
}

void CGameCharaMaterialContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{
	m_pOwner = pOwner;

	CreateMaterialTable();

	CreateTopAttrSet();
	CreateTeamTag();

	// Penalty System용 Ambient Color
	{
		COLORREAL col;

		m_pPenaltyAmbientAttr = i3AmbientAttr::new_object();

		i3Color::Set( &col, 1.0f, 0.0f, 0.0f, 1.0f);
		m_pPenaltyAmbientAttr->Set( &col);
	}

	// Clan Mark Texture를 찾는다.
	i3::vector<i3Node*> list;
	i3Scene::FindNodeByName( pOwner->getSceneObject(), "Model_Clan_Material", list);

	UINT32 cnt = list.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		i3AttrSet * pAttrSet = static_cast<i3AttrSet*>(list.at(i));

		INT32 attrCnt = pAttrSet->GetAttrCount();
		for( INT32 j = 0; j < attrCnt; j++)
		{
			if( i3::same_of<i3TextureBindAttr*>(pAttrSet->GetAttr( j)) )
			{
				i3TextureBindAttr * pAttr = static_cast<i3TextureBindAttr*>(pAttrSet->GetAttr(j));
				m_ClanMarkTextureBindList.push_back( pAttr);
				I3_MUST_ADDREF(pAttr);
			}
		}
	}

	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);
}

void CGameCharaMaterialContext::OnDestroy( void)
{
	CGameCharaContextBase::OnDestroy();

	const INT32 numTable = m_MaterialTableCount;

	for (INT32  i = 0 ; i < numTable ; ++i)
	{
		CHARA_MATERIAL* pMtrl =	m_pMaterialTable + i;
		pMtrl->m_pMaterial->Release();
	}

	I3MEM_SAFE_FREE( m_pMaterialTable);
#ifdef DOMI_TAG
	I3_SAFE_RELEASE( m_pDominationTag);
#endif
	I3_SAFE_RELEASE( m_pTeamTag);

	I3_SAFE_RELEASE( m_pPenaltyAmbientAttr );

	for( UINT32 i = 0; i < m_ClanMarkTextureBindList.size(); i++)
	{
		I3_SAFE_RELEASE( m_ClanMarkTextureBindList.at(i));
	}
	m_ClanMarkTextureBindList.clear();
}

void CGameCharaMaterialContext::OnReset( void)
{
	CGameCharaContextBase::OnReset();

	i3Color::Set( &m_TargetDiffuse, 0.5f, 0.5f, 0.5f, 1.0f);
	i3Color::Set( &m_OldDiffuse, 0.5f, 0.5f, 0.5f, 1.0f);

	m_timeVolLit = 0.f;
	m_timeVolLitCheck = 0.f;

	m_pOwner->Cmd_SetColor( nullptr, nullptr, 1.f);

	SetBlendEnable( false);
	SetZWriteEnable(true);
}

void CGameCharaMaterialContext::OnChange1PV()
{
	m_pTexFilterAttr->SetMinFilter(I3G_TEXTURE_FILTER_LINEAR);

	if( m_pOwner->IsElite())
		setFaceCull(false);
}

void CGameCharaMaterialContext::OnChange3PV()
{
	m_pTexFilterAttr->SetMinFilter(I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR);
}


void CGameCharaMaterialContext::CreateMaterialTable()
{
	i3::vector<i3RenderAttr*> mtlList;
	mtlList.reserve( 32);

	i3Scene::FindAttrByExactType( m_pOwner->getAttachNode(), i3MaterialAttr::static_meta(), mtlList);
	I3ASSERT( INT32(mtlList.size()) > 0);
	if( m_pMaterialTable != nullptr )
	{

		const INT32 numTable = m_MaterialTableCount;

		for (INT32  i = 0 ; i < numTable ; ++i)
		{
			CHARA_MATERIAL* pMtrl =	m_pMaterialTable + i;
			pMtrl->m_pMaterial->Release();
		}

		I3MEM_SAFE_FREE( m_pMaterialTable);
	}

	m_pMaterialTable = (CHARA_MATERIAL *) i3MemAlloc( sizeof( CHARA_MATERIAL) * mtlList.size());
	I3ASSERT( m_pMaterialTable != nullptr);

	m_MaterialTableCount = INT32(mtlList.size());
	
	// AttrSet의 Attr을 검사
	UINT32 cnt = mtlList.size();
	for( UINT32 i = 0; i < cnt; i++ )
	{
		i3MaterialAttr * pAttr = (i3MaterialAttr*) mtlList[i];

		m_pMaterialTable[i].m_pMaterial = pAttr;		pAttr->AddRef();			// 큰 문제가 있는듯해서 여기서 addRef();

		i3Color::Set( &	m_pMaterialTable[i].diffuse, pAttr->GetDiffuse());
		i3Color::Set( &	m_pMaterialTable[i].specular, pAttr->GetSpecular());
		i3Color::Set( &	m_pMaterialTable[i].emissive, pAttr->GetEmissive());
		m_pMaterialTable[i].shininess = pAttr->GetShininess();
	}
}

void CGameCharaMaterialContext::CreateTopAttrSet()
{
	m_pAttrSet = i3AttrSet::new_object_ref();
	m_pAttrSet->SetPriority( 5);


	// Zwrite 및 Blend 등의 Attr들은 항상 동작할 필요가 없다.
	//m_pAttrSet->AddFlag( I3_NODEFLAG_NO_WORK);

	{
		m_pBlendCtrlAttrSet = i3AttrSet::new_object_ref();
		//m_pBlendCtrlAttrSet->SetPriority( I3_DEF_ATTR_PRIORITY);
		m_pBlendCtrlAttrSet->AddFlag( I3_NODEFLAG_NO_WORK);
		m_pBlendCtrlAttrSet->SetPriority( 0);

		{
			m_pBlendEnableAttr = i3BlendEnableAttr::new_object_ref();
			m_pBlendEnableAttr->Set( false);
			m_pBlendCtrlAttrSet->AddAttr( m_pBlendEnableAttr);
		}

		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
			pAttr->SetSource( I3G_BLEND_SRCALPHA);
			pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
			m_pBlendCtrlAttrSet->AddAttr( pAttr);
		}

		{
			m_pBlendMtlAttr = i3MaterialAttr::new_object_ref();

			m_pBlendMtlAttr->setValidEmissive( false);

			m_pBlendMtlAttr->SetSpecular( 0.3f, 0.3f, 0.3f, 1.0f);
			m_pBlendMtlAttr->SetShininess( 128.0f);
			
			m_pBlendCtrlAttrSet->AddAttr( m_pBlendMtlAttr);
		}

		{	
			// KillCam용
			m_pZWriteEnable = i3ZWriteEnableAttr::new_object_ref();
			m_pAttrSet->AddAttr(m_pZWriteEnable);
		}

		{
			m_pTexFilterAttr = i3TextureFilterAttr::new_object_ref();
			m_pAttrSet->AddAttr( m_pTexFilterAttr);

			m_pTexFilterAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
			m_pTexFilterAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT);
		}

		m_pAttrSet->AddChild( m_pBlendCtrlAttrSet);
	}

	{
		m_pFaceCullModeAttr = i3FaceCullModeAttr::new_object_ref();
	
		m_pFaceCullModeAttr->Set( I3G_FACE_CULL_BACK);
		
		m_pAttrSet->AddAttr( m_pFaceCullModeAttr);
	}

	{
		m_pFresnelAttrSet = i3AttrSet::new_object_ref();
		m_pBlendCtrlAttrSet->AddChild( m_pFresnelAttrSet);

		m_pFresnelAttr = i3FresnelAttr::new_object_ref();
		//m_pFresnelAttr->setIOR( 1.33f);
		m_pFresnelAttr->setIOR( 1.f);
		//m_pFresnelAttr->setColor( 0.8f, 0.8f, 0.8f, 1.0f);
		m_pFresnelAttr->setEnable( true);

		m_pFresnelAttrSet->AddAttr( m_pFresnelAttr);
	}

	for( ; m_pOwner->getAttachNode()->GetChildCount() > 0;)
	{
		i3Node * pChild = m_pOwner->getAttachNode()->GetChild( 0);
		pChild->ChangeParent(m_pFresnelAttrSet);
	}

	m_pOwner->getAttachNode()->AddChild( m_pAttrSet);

	SetBlendEnable( false);
}

void CGameCharaMaterialContext::CreateTeamTag( void)
{
	if( (g_pEnvSet->m_bEnableFrindlyMark == false) || (g_pFramework->IsBattleStage() == false))
		return;

	if( m_pOwner->isLocal() )
		return;

	bool bCreateTag = false;

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) == true || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT) == true)
	{
		// 보스 미션에서는 게임 중에 Team이 변경될 수 있기 때문에
		// 모든 캐릭터에 대해 TeamTag가 우선 생성되어야 한다
		bCreateTag = true;
	}
	else
	{
		// 보스 미션이 아닌 경우에는 Team이 게임 중에 변경되는 경우가 없다.
		if( BattleSlotContext::i()->getMyTeam() == m_pOwner->getCharaTeam() )
		{
			bCreateTag = true;
		}
	}

	if( bCreateTag)
	{
		if( m_pTeamTag)
		{
			m_pTeamTag->RemoveFromParent();
			I3_SAFE_RELEASE( m_pTeamTag);
		}

		m_pTeamTag = i3Quad::new_object();

		m_pTeamTag->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
		m_pTeamTag->Create( 1, I3SG_QUAD_SPACE_WORLD, true, false, true );
		m_pTeamTag->SetColor( 0, 255, 255, 255, 255 );
		m_pTeamTag->SetSize( 0, MoS::Scn_Width, MoS::Scn_Width );
		m_pTeamTag->SetCenter( 0, 0.0f, 2.2f, 0.0f );
		m_pTeamTag->SetEnable( 0, false );

		if (CHud::i() != nullptr)
		{
			m_pTeamTag->SetTexture( CHud::i()->getHud3Texture() );
		}

		{
			i3LightingEnableAttr *pLightEnable = i3LightingEnableAttr::new_object_ref();
			pLightEnable->Set( false );
			m_pTeamTag->AddAttr( pLightEnable );		

			i3TextureFunctionAttr * pTexFuncAttr = i3TextureFunctionAttr::new_object_ref();
			pTexFuncAttr->SetFunction( I3G_TEXTURE_FUNC_ADD);
			m_pTeamTag->AddAttr( pTexFuncAttr);

			i3ZWriteEnableAttr * pZWirteAttr = i3ZWriteEnableAttr::new_object_ref();
			pZWirteAttr->Set( false);
			m_pTeamTag->AddAttr( pZWirteAttr);
		}

		m_pOwner->getAttachNode()->AddChild( m_pTeamTag );
	}
}

void CGameCharaMaterialContext::SetBlendEnable( bool bEnable)
{
	if (!m_pOwner->getSceneObject()->GetBody())
		return;
	
	if( m_pBlendEnableAttr->Get() != bEnable)
	{
		m_pBlendEnableAttr->Set( bEnable);

		if( bEnable)
		{
			//m_pBlendCtrlAttrSet->SetPriority( 0);
			//m_pFaceCullModeAttr->Set( I3G_FACE_CULL_BACK);
			m_pBlendCtrlAttrSet->RemoveFlag( I3_NODEFLAG_NO_WORK);
		}
		else
		{
			//m_pBlendCtrlAttrSet->SetPriority( I3_DEF_ATTR_PRIORITY);
			//m_pFaceCullModeAttr->Set( I3G_FACE_CULL_NONE);
			m_pBlendCtrlAttrSet->AddFlag( I3_NODEFLAG_NO_WORK);
		}

		CHARA_MATERIAL * pMtl = m_pMaterialTable;

		m_pBlendMtlAttr->SetDiffuse( pMtl->m_pMaterial->GetDiffuse());
	}
}

void CGameCharaMaterialContext::SetZWriteEnable(bool bEnable)
{
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	if (!pFramework->getSgContext()->GetXrayScene()->IsXrayState())
		m_pZWriteEnable->Set(bEnable);
}

void CGameCharaMaterialContext::SetBlendAlpha( REAL32 val)
{
	if( val >= 0.f)
	{
		REAL32 alphaval = i3Math::Clamp( val, 0.0f, 1.0f);

		// revision 29616
		if( BattleSlotContext::i()->IsGMObserverForClient( m_pOwner->getCharaNetIndex()))
		{//GM 옵저버 모드 일 경우 시체가 안보이도록 함.
			alphaval = 0.f;
			m_pBlendMtlAttr->GetDiffuse()->a = alphaval;
		}
		
		if( m_fAlphaVal != alphaval)
		{
			m_pBlendMtlAttr->GetDiffuse()->a = alphaval;
			m_fAlphaVal = alphaval;

			if(m_pOwner->IsDino() == false)
				m_pOwner->Cmd_SetColor( nullptr, nullptr, m_fAlphaVal);

			/*if( m_fAlphaVal < 1.f)
				SetBlendEnable( true);
			else
				SetBlendEnable( false);*/
		}
	}
}

void CGameCharaMaterialContext::SetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	INT32 i;

	if( pfDiffuse != nullptr )
	{
		for( i = 0; i < m_MaterialTableCount; i++ )
		{
			m_pMaterialTable[i].m_pMaterial->GetDiffuse()->r = pfDiffuse->r;
			m_pMaterialTable[i].m_pMaterial->GetDiffuse()->g = pfDiffuse->g;
			m_pMaterialTable[i].m_pMaterial->GetDiffuse()->b = pfDiffuse->b;
		}
	}

	/*if( pfSpecular != nullptr )
	{
		for( i = 0; i < m_MaterialTableCount; i++)
		{
			m_pMaterialTable[i].m_pMaterial->GetSpecular()->r = pfSpecular->r;
			m_pMaterialTable[i].m_pMaterial->GetSpecular()->g = pfSpecular->g;
			m_pMaterialTable[i].m_pMaterial->GetSpecular()->b = pfSpecular->b;
		}
	}*/
}

void CGameCharaMaterialContext::SetTeamMarkEnable(  bool bEnable, INT32 Attack, INT32 Speed)
{
	if(m_pOwner->IsDino())
	{
		return;
	}

	if( !g_pEnvSet->m_bEnableFrindlyMark )
	{
		if( m_pTeamTag != nullptr)
		{
			m_pTeamTag->SetEnable( 0, false);
		}

		return;
	}

	if( m_pTeamTag == nullptr)
	{
#if defined( DEF_OBSERVER_MODE)
		if( BattleSlotContext::i()->isObserverMe() == false)
#endif
		CreateTeamTag();
	}

	if( m_pTeamTag != nullptr)
	{
		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) ||
			MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT) )
		{
			// 유저가 공룡이라면 다른 모든 캐릭터의 TeamTag는 보이지 않아야 한다.
			// 유저가 공룡이 아니라면, 공룡이 아닌 다른 모든 유저의 Team Tag가 활성화되어야 한다.
			if (BattleSlotContext::i()->getMyTeam() == CHARACTER_TEAM_RED)
				bEnable = false;
			else
				bEnable = bEnable && !m_pOwner->IsDino();
		}
		else
		{
			bEnable = bEnable && (BattleSlotContext::i()->getMyTeam() == m_pOwner->getCharaTeam());
		}

		m_pTeamTag->SetEnable( 0, bEnable);

		if( bEnable && getWeaponContext())
		{
			WeaponBase * pWeapon = getWeaponContext()->getWeapon( WEAPON_SLOT_PRIMARY);
			if( pWeapon == nullptr)
			{
				m_pTeamTag->SetEnable( 0, false); return;
			}

			CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

			if (Attack == SKILL_UNKNOWN && Speed == SKILL_UNKNOWN)
			{
				// 무기에 따른 UV변경
				switch( pWeaponInfo->GetTypeClass())
				{
				case WEAPON_CLASS_SHOTGUN:
					m_pTeamTag->SetTextureCoord( 0, MoS::Shotgun_x, MoS::Shotgun_y, MoS::Shotgun_x + MoS::Img_w, MoS::Shotgun_y + MoS::Img_h );				
					break;
				case WEAPON_CLASS_SNIPER:
					m_pTeamTag->SetTextureCoord( 0, MoS::Sniper_x, MoS::Sniper_y, MoS::Sniper_x + MoS::Img_w, MoS::Sniper_y+ MoS::Img_h);
					break;
				case WEAPON_CLASS_MG:
					m_pTeamTag->SetTextureCoord( 0, MoS::MG_x, MoS::MG_y, MoS::MG_x + MoS::Img_w, MoS::MG_y + MoS::Img_h);
					break;
				case WEAPON_CLASS_ASSAULT:
				case WEAPON_CLASS_BOW:
					m_pTeamTag->SetTextureCoord( 0, MoS::Assault_x, MoS::Assault_y, MoS::Assault_x + MoS::Img_w, MoS::Assault_y + MoS::Img_h);
					break;
				case WEAPON_CLASS_SMG:
					m_pTeamTag->SetTextureCoord( 0, MoS::SMG_x, MoS::SMG_y, MoS::SMG_x + MoS::Img_w, MoS::SMG_y + MoS::Img_h);
					break;

				default:
					m_pTeamTag->SetEnable( 0, false);
					break;
				}
			}
			else
			{
				if (Speed == SKILL_SPEED_ON || Attack == SKILL_ATTACK_ON )
				{
					I3TRACE("스피드 온 \n");

					m_pTeamTag->SetEnable( 0, false);

				#ifdef DOMI_TAG
					if(m_pDominationTag)
					{
						m_pDominationTag->SetEnable( 0, true);
						m_pDominationTag->SetTextureCoord( 0, 0.0f, 0.0f, 0.5f, 0.5f );	
					}
				#endif
				}
			}
		}	
	}
}

void CGameCharaMaterialContext::SetClanMark( i3Texture * pTex)
{
	UINT32 cnt = m_ClanMarkTextureBindList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		m_ClanMarkTextureBindList.at(i)->SetTexture( pTex);
	}
}

void CGameCharaMaterialContext::DriveLightVolume( REAL32 rDeltatime)
{
	REAL32 rLimit = SLICE;
	m_timeVolLit += rDeltatime;

	if( m_timeVolLit < rLimit)
	{
		REAL32 val = i3Math::vsin_libm( m_timeVolLit * (1.0f / SLICE));
		COLORREAL diff, mtl;

		i3Color::Sub( &diff, &m_TargetDiffuse, &m_OldDiffuse);
		i3Color::Scale( &mtl, &diff, val);
		i3Color::Add( &mtl, &mtl, &m_OldDiffuse);

		m_pOwner->Cmd_SetColor( &mtl, &mtl);
	}
	else
	{
		// 반원으로 Collistion Test를 한다. 매 Frame마다 하는 것이 아니라
		// 간헐적으로 한다.
		i3CollideeSphere sphere;

		VEC3D pos;
		INT32 count, i;
		COLORREAL target;

		i3Vector::Copy( &pos, m_pOwner->GetPos());
		i3Vector::AddY( &pos, getCollisionContext()->getCollisionRadius());
		sphere.SetCenter( &pos);
		sphere.SetRadius( getCollisionContext()->getCollisionRadius());

		//I3TRACE( "%f %f %f %f\n", pos.x, pos.y, pos.z, m_rColRadius);
		
		count = g_pFramework->getCollisionContext()->CollisionTest( &sphere, CGRP_CHARA_LIGHTVOL, 0); //I3_COLLISION_STYLE_NEAREST_ONE);

		I3_COLLISION_RESULT * pResult = g_pFramework->getCollisionContext()->GetResult();

		if( count > 0)
		{
			i3Color::Set( &target, 0.0f, 0.0f, 0.0f, 0.0f);

			for( i = 0; i < count; i++)
			{
				REAL32 r, g, b;

				r = (REAL32)pResult->m_pTri->m_Color[0] * (1.0f / 255.0f) * 2.0f;
				g = (REAL32)pResult->m_pTri->m_Color[1] * (1.0f / 255.0f) * 2.0f;
				b = (REAL32)pResult->m_pTri->m_Color[2] * (1.0f / 255.0f) * 2.0f;

				i3Color::Add( &target, r, g, b, 1.0f);
				pResult ++;
			}

			i3Color::Scale( &target, &target, (1.0f / count));
		}
		else
		{
			if( crb::i() != nullptr)
				i3Color::Set( &target, crb::i()->getDefaultDiffuse());
			else
				i3Color::Set( &target, 0.8f, 0.8f, 0.8f, 1.f);
		}

		{
			m_timeVolLit = 0.0f;
			i3Color::Set( &m_OldDiffuse, &m_TargetDiffuse);
			i3Color::Set( &m_TargetDiffuse, &target);
		}
	}
}

void CGameCharaMaterialContext::ProcessMaterial( REAL32 rDeltatime)
{
	REAL32 fAlpha = 1.0f;

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);

	if( BattleServerContext::i()->IsP2PActivate() )
	{
		//////////////////////////////////////////////////////////////////////
		//	리스폰 타임
		REAL32 fCurrentTime = m_pOwner->getRespawnElapseTime();
		if( fCurrentTime > 0.0f)	
		{
			if( CStageInfoUtil::IsEternerlRespawn(*MyRoomInfoContext::i()->getStage()) )
			{
				if( fCurrentTime <= 2.0f)
				{
					fAlpha = fCurrentTime / 2.0f;

					if( m_pOwner->isLocal() &&	i3::same_of<CGameCamera_Kill*>(g_pCamera))
					{	// 자기 자신
						CGameCamera_Kill * pKillCam = (CGameCamera_Kill*)g_pCamera;
						
						if( pKillCam->isFPS())
						{
							SetZWriteEnable(false);
							fAlpha = 0.f;
						}
						else
							SetZWriteEnable( true);
					}

					m_pOwner->Cmd_SetColor( nullptr, nullptr, fAlpha);
				}
			}
		}
		else if( BattleSlotContext::i()->IsGMObserverForClient( m_pOwner->getCharaNetIndex()) )
		{
			m_pOwner->Cmd_SetColor( nullptr, nullptr, 0.f);
		}
		else
		{
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
			{
				//////////////////////////////////////////////////////////////////////////////
				//	무적 타임
				if( m_pOwner->getInvincibleTime() > 0.0f)	//	무적시간시에는 깜빡이는 효과
				{
					fAlpha = 1.f - i3Math::abs( i3Math::sin( m_pOwner->getInvincibleTime() * 6.0f));
				}
				
				m_pOwner->Cmd_SetColor( nullptr, nullptr, fAlpha);
			}
		}
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////////
		//	 캐릭터 Fade효과
		/*if( m_rFadeElapseTime != 0.0f)
		{
			fAlpha = m_rFadeElapseTime;

			I3ASSERT( m_pOwner->getAnimContext()->getAILower() != nullptr);
			I3ASSERT( m_pOwner->getAnimContext()->getAIUpper() != nullptr);

			if( m_pOwner->getAnimContext()->getAILower()->getCurrentAIState() == nullptr ||
				m_pOwner->getAnimContext()->getAIUpper()->getCurrentAIState() == nullptr)
				m_pOwner->SetEnable( false);
			else
				m_pOwner->SetEnable( true);

			SetBlendEnable( true);

			if( m_rFadeElapseTime < 0.0f)
			{
				m_rFadeElapseTime += rDeltatime;
				if( m_rFadeElapseTime > 0.0f)	
				{
					m_rFadeElapseTime = 0.0f;
					SetBlendEnable( false);
				}
				fAlpha = i3Math::abs( 1.0f + fAlpha);
			}
			else
			{
				m_rFadeElapseTime -= rDeltatime;
				if( m_rFadeElapseTime < 0.0f)	
				{
					m_rFadeElapseTime = 0.0f;
					SetBlendEnable( false);
				}
				fAlpha = i3Math::abs( fAlpha);
			}

			m_pOwner->Cmd_SetColor( nullptr, nullptr, fAlpha);
		}*/
	}
}

void CGameCharaMaterialContext::EnablePenalty(void)
{
	if( m_bPenalty == false)
	{
		m_pAttrSet->AddAttr( m_pPenaltyAmbientAttr );

		m_bPenalty = true;
	}
}
void CGameCharaMaterialContext::DisablePenalty()
{
	if( m_bPenalty == true)
	{
		m_pAttrSet->RemoveAttr( m_pPenaltyAmbientAttr);
		m_bPenalty = false;
	}
}
