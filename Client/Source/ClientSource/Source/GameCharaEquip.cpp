#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaEquip.h"
//#include "GameCharaPlayer.h"
#include "GameCharaBase.h"
#include "BattleServerContext.h"


#define PHYSIX_CONTACTSIZE_HELMET	0.15f

I3_CLASS_INSTANCE( CGameCharaEquip);

CGameCharaEquip::~CGameCharaEquip( void)
{
	I3_SAFE_RELEASE( m_pContactHitBox);
}


void CGameCharaEquip::Create( i3GameResSceneGraph * pRes, T_ItemID ItemID )
{
	I3ASSERT( pRes != nullptr);

	m_bModified = false;

	i3GameObj::BindSgRes( g_pFramework, pRes);

	i3GameObj::OnInitAfterLoad();

	Instantiate();

	m_EquipPart = EQUIP::ItemID2PartsType(ItemID);
	m_ID = EQUIP::ItemID2ResID(ItemID);

	m_pInfo = g_pCharaInfoDataBase->GetInfo( ItemID);
	
	//	노드 상위에 알파처리를 위한 AttrSet을 붙인다.
	{
		m_pAttrSet = i3AttrSet::new_object_ref();
		m_pAttrSet->SetPriority( 0);

		while( getAttachNode()->GetChildCount() > 0)
		{
			i3Node * pChild = getAttachNode()->GetChild( 0);
			pChild->ChangeParent(m_pAttrSet);
		}	

		getAttachNode()->AddChild( m_pAttrSet);		

		m_pBlendEnable = i3BlendEnableAttr::new_object_ref();
		m_pBlendEnable->Set( true);
		m_pAttrSet->AddAttr( m_pBlendEnable);

		i3BlendModeAttr * pBlendMode = i3BlendModeAttr::new_object_ref();
		pBlendMode->SetSource( I3G_BLEND_SRCALPHA);
		pBlendMode->SetDestination( I3G_BLEND_INVSRCALPHA);
		m_pAttrSet->AddAttr( pBlendMode);

		m_pMaterialAttr = i3MaterialAttr::new_object_ref();
		m_pAttrSet->AddAttr( m_pMaterialAttr);

		COLORREAL col;

		i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);
		m_pMaterialAttr->SetDiffuse( &col);
		m_pMaterialAttr->SetSpecular( &col);

		m_pMaterialAttr->setValidEmissive( false);
		m_pMaterialAttr->setValidSpecular( false);
		m_pMaterialAttr->setValidShininess( false);

	}

	m_pBody = getSceneObject()->GetBody();
	I3ASSERT( m_pBody != nullptr);

	//	임의로 변경하기 쉽게 하기 위해 강제로 LOD Slope값을 지정합니다. (기존에 CharaTool에서 지정한 Slope값은 사용하지 않습니다. komet)
	m_pBody->setSlope( EQUIP_LOD_SLOPE);
	m_pBody->removeStyle( I3BS_LOD_AUTO);
	m_pBody->setCurrentLevel( 0);
	
	if( BattleServerContext::i()->IsP2PActivate() )
	{
		// 러프컬리젼 세팅 (먼지 효과에 사용)

		if( g_pEnvSet->m_nEffectLevel > 1)
		{
			VEC3D vZero;
			
			m_pContactHitBox = i3PhysixShapeSet::new_object();
			m_pContactHitBox->CreateSphereShape( &vZero, PHYSIX_CONTACTSIZE_HELMET, CGRP_ITEM);
			m_pContactHitBox->getNxActor()->setGroup( CGRP_ITEM);
#if defined( I3_DEBUG)
			m_pContactHitBox->SetName( "HelmetContactShapeSet" );
			m_pContactHitBox->getNxShape(0)->setName( "HelemtContactShape");
#endif
			m_pContactHitBox->getNxShape(0)->setFlag( NX_SF_POINT_CONTACT_FORCE, true);
			m_pContactHitBox->setEventReceiver( this);
			m_pContactHitBox->modifyDynamicState( I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RAYCAST, 0);
			
			if( g_pFramework->GetCurrentStage() != nullptr)
			{
				i3World * pWorld = g_pFramework->GetCurrentStage()->getWorld();

				if( pWorld->getWorldCollision() != nullptr)
				{
					i3Scene::SetActorPairFlag( pWorld->getWorldCollision()->getInstanceSg(), m_pContactHitBox->getNxActor(), NX_NOTIFY_ON_START_TOUCH);
				}

				if( g_pFramework->GetCurrentStage()->GetStageObjectList() != nullptr )
				{
					for( size_t i = 0; i < g_pFramework->GetCurrentStage()->GetStageObjectList()->size(); i++ )
					{
						i3Object * pObj = (*g_pFramework->GetCurrentStage()->GetStageObjectList())[i];
						i3Scene::SetActorPairFlag( pObj->getSceneObject(), m_pContactHitBox->getNxActor(), NX_NOTIFY_ON_START_TOUCH );
					}
				}
			}
		}
	}
}

void CGameCharaEquip::ReturnInstance( void)
{
	Detach();

	//I3_SAFE_NODE_RELEASE( m_pAttrSet);

	I3_SAFE_RELEASE( m_pContactHitBox);

	getAttachNode()->RemoveAllChild();

	RemoveAllChild();

	i3GameObj::ReturnInstance();
}

void CGameCharaEquip::Reset( void)
{
	m_fDropElapseTime = 0.0f;
	m_fAlphaElapseTime = 0.0f;

	SetBlendAlpha( 1.0f);

	SetEnable( true);

	//Attach할때만 사용한다.
	//SetMatrix( &I3_IDENTITYMATRIX);

//	m_nContactReportCount = 0;
//	m_nContactReportReadPos = 0;
	m_vecContactReport.clear();

	// Contact report
	//m_bContact = false;
	m_DropCount = 0;

	// xgkick
	setDynamicActiveState( true, false);
	SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true);

	//I3TRACE( "Equip Reset...\n");
}

void CGameCharaEquip::SetCollisionGroup( UINT16 nGroup, UINT32 nMask )
{
	nGroup = CGRP_ITEM;
	m_nCollisionGroup = nGroup;

	InitColliderSet( nGroup, nMask);
}

void CGameCharaEquip::OnBindResource(void)
{
}

void CGameCharaEquip::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GameObj::OnUpdate( rDeltaSeconds );

	if( m_pContactHitBox != nullptr )
	{
		MATRIX mat;
		i3Skeleton * pSkel = m_pBody->getFrameLOD()->getSkeleton();
		i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( 0);
		pShapeSet->getShapeGlobalPose( &mat);
		m_pContactHitBox->setShapeGlobalPose( &mat);
		_ParseContactReport();
	}

	//	캐릭터에서 떨어져 있다.
	if( !m_bOwnerChara)
	{
		m_fDropElapseTime += rDeltaSeconds;

		//	일정시간이 지나면 사라진다.
		if( m_fDropElapseTime > 5.0f)
		{
			if( m_fAlphaElapseTime < 2.0f)
			{	//	사라지는 중...
				REAL32 a = m_fAlphaElapseTime / 2.0f;
				a = 1.0f - i3Math::Clamp( a, 0.0f, 1.0f);

				SetBlendAlpha( a);

				//I3TRACE( "Equip Visible : %.4f\n", a);

				m_fAlphaElapseTime += rDeltaSeconds;
			}
			else
			{	//	완전히 사라졌다.
				SetEnable( false);

				//I3TRACE( "Equip Disabled...\n");
			}
		}

		//	월드에 떨어지면 자신이 직접 LOD 계산을 합니다.		
		{
			REAL32 fRatio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();
			calcLOD( GetPos(), i3Matrix::GetPos( g_pCamera->getCamMatrix()), fRatio);
		}
	}
}

void CGameCharaEquip::DumpState( void)
{
	I3PRINTLOG(I3LOG_NOTICE, "<< Dump Equip >>");
	I3PRINTLOG(I3LOG_NOTICE, "- ID : %d", m_ID);

	switch( m_EquipPart)
	{
	case EQUIP::ePART_UNKNOWN:		I3PRINTLOG(I3LOG_NOTICE, "- Equip Part : EQUIP::ePART_UNKNOWN");	break;
	case EQUIP::ePART_HEADGEAR:		I3PRINTLOG(I3LOG_NOTICE, "- Equip Part : EQUIP::ePART_HEADGEAR");	break;
	default:						I3PRINTLOG(I3LOG_NOTICE, "- Equip Part : (null)");					break;
	}	

	I3PRINTLOG(I3LOG_NOTICE, "- Visible Node : %d", isVisible());
	I3PRINTLOG(I3LOG_NOTICE, "- Enable Node : %d", isEnable());
	I3PRINTLOG(I3LOG_NOTICE, "- Current Pos : %.4f, %.4f, %.4f", GetPos()->x, GetPos()->y, GetPos()->z);
	I3PRINTLOG(I3LOG_NOTICE, "- Attached : %d", m_bAttached);
	I3PRINTLOG(I3LOG_NOTICE, "- Owner Chara : %d", m_bOwnerChara);
	I3PRINTLOG(I3LOG_NOTICE, "- Owner Addr : 0x%p", m_pOwner);
	I3PRINTLOG(I3LOG_NOTICE, "- Material");
	I3PRINTLOG(I3LOG_NOTICE, " +Diffuse %.4f %.4f %.4f %.4f",  i3Color::GetR( m_pMaterialAttr->GetDiffuse()),
												i3Color::GetG( m_pMaterialAttr->GetDiffuse()),
												i3Color::GetB( m_pMaterialAttr->GetDiffuse()),
												i3Color::GetA( m_pMaterialAttr->GetDiffuse()));
	I3PRINTLOG(I3LOG_NOTICE, " +Specular %.4f %.4f %.4f %.4f", i3Color::GetR( m_pMaterialAttr->GetSpecular()),
												i3Color::GetG( m_pMaterialAttr->GetSpecular()),
												i3Color::GetB( m_pMaterialAttr->GetSpecular()),
												i3Color::GetA( m_pMaterialAttr->GetSpecular()));
}

void CGameCharaEquip::Attach( i3Node * pSceneNode, i3GameNode * pOwner, bool bTakeChara)
{	//	m_bAttached가 true이고 m_pOwner가 nullptr이면 월드에 붙어 있는 것이다.
	I3ASSERT( pSceneNode != nullptr);
	I3ASSERT( pOwner != nullptr);

	//	캐릭터에 붙는다.
	if( bTakeChara)
	{
		m_bOwnerChara = true;	
		m_fDropElapseTime = 0.0f;
	}
	else
	{
		m_bOwnerChara = false;				
	}

	//	렌더링될 곳에 붙인다.
	if( m_pParentScene != pSceneNode)
	{
		if( m_pParentScene != nullptr )
			getSceneObject()->RemoveFromParent();

		pSceneNode->AddChild( getSceneObject() );
		m_pParentScene = pSceneNode;
	}

	//	업데이트될 곳에 붙인다.
	if( m_pOwner != pOwner )
	{
		if( m_pOwner != nullptr )
			m_pOwner->RemoveChild( this );

		pOwner->AddChild( this );
		m_pOwner = pOwner;	
	}

	// Equipment.pef 의 _Transform값으로 셋팅해준다.
	ReloadTransform();

	m_bAttached = true;
}

void CGameCharaEquip::Detach( void)
{	
	if( m_pParentScene != nullptr)
	{
		m_pParentScene->RemoveChild( getSceneObject() );
		m_pParentScene = nullptr;
	}
	
	if( m_pOwner != nullptr)
	{
		m_pOwner->RemoveChild( this);
		m_pOwner = nullptr;
	}

	m_bAttached = false;
}

EQUIP::ePART CGameCharaEquip::GetEquipPart(void)
{
	if( m_EquipPart == EQUIP::ePART_BERET )
		return EQUIP::ePART_HEADGEAR;
	
	return m_EquipPart;
}

bool CGameCharaEquip::IsPhysixPart(void)
{
	switch( m_EquipPart)
	{
	case EQUIP::ePART_HEADGEAR	: return true;
	case EQUIP::ePART_FACEGEAR	: return (g_pEnvSet->IsV2Version() == false);
	case EQUIP::ePART_BERET		: return true;
	}

	return false;
}

void CGameCharaEquip::SetBlendAlpha( REAL32 val)
{
	val = i3Math::Clamp( val, 0.0f, 1.0f);

	if( m_fAlphaVal != val)
	{
		i3Color::SetA( m_pMaterialAttr->GetDiffuse(), val);
		i3Color::SetA( m_pMaterialAttr->GetSpecular(), val);
		m_fAlphaVal = val;
	}
}

void CGameCharaEquip::SetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	if( pfDiffuse != nullptr)	
	{
		REAL32 fDiffuseAlpha = (REAL32) i3Color::GetA( m_pMaterialAttr->GetDiffuse());
		m_pMaterialAttr->SetDiffuse( pfDiffuse->r, pfDiffuse->g, pfDiffuse->b, fDiffuseAlpha);
	}

	if( pfSpecular != nullptr)	
	{
		REAL32 fSepcularAlpha = (REAL32) i3Color::GetA( m_pMaterialAttr->GetSpecular());
		m_pMaterialAttr->SetSpecular( pfSpecular->r, pfSpecular->g, pfSpecular->b, fSepcularAlpha);
	}
}

void CGameCharaEquip::_QueContactReport(  const I3_PHYSIX_CONTACT_REPORT_PTR& spReport)
{
//	UINT32 nCount = m_nContactReportCount - m_nContactReportReadPos;
	if( m_vecContactReport.size() >= PHYSIX_CONTACT_REPORT_COUNT )
	{
		I3TRACE( "CGameCharaEquip::_QueContactReport() - Contactreport Buffer over flow.\n" );
		return;
	}

	m_vecContactReport.push_back(spReport);

//	UINT32 i = m_nContactReportCount % PHYSIX_CONTACT_REPORT_COUNT;
//	i3Physix::ContactReportCopyTo( &m_ContactReportBuf[ i ], pReport );
//	m_nContactReportCount++;
}

void CGameCharaEquip::_ParseContactReport( void )
{
	const UINT32 nCount = m_vecContactReport.size();	//	m_nContactReportCount - m_nContactReportReadPos;
	if( nCount == 0 )	return;
	
	UINT32 i;
	INT32	nSndCnt = 0;

	VEC3D vUp;
	i3Vector::Set( &vUp, 0.f, 1.f, 0.f );

	for( i = 0; i < nCount; i++ )
	{
//		UINT32 nPos = (m_nContactReportReadPos + i) % PHYSIX_CONTACT_REPORT_COUNT;
//		I3_PHYSIX_CONTACT_REPORT * pReport = &m_ContactReportBuf[ nPos ];
		I3_PHYSIX_CONTACT_REPORT * pReport = m_vecContactReport[i].get();

		if( pReport->m_Events == NX_NOTIFY_ON_START_TOUCH )
		{
			for( UINT32 j = 0; j < pReport->m_ContactList.size(); j++ )
			{
				I3_PHYSIX_CONTACT_INFO * pPointInfo = pReport->m_ContactList.at(j);

				g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_HELMET_DOWN_COMMON, &pPointInfo->m_Intersect, &pPointInfo->m_Normal );

				if( (nSndCnt <= 0) && (m_DropCount == 0 || pPointInfo->m_rNormalForce > 0.0f ))
				{
					I3_PHYSIX_TERRAIN * pTri = pReport->m_pDestShapeSet->getTerrain(0);
					if( pTri != nullptr)
					{
						g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, pTri->m_Terrain, GTBT_BOUND_HELMET, &pPointInfo->m_Intersect, false );

						nSndCnt++;
						m_DropCount++;
					}
				}
			}
		}
	}
	
	m_vecContactReport.clear();
//	m_nContactReportReadPos += nCount;
}

bool CGameCharaEquip::OnEvent( UINT32 event, i3ElementBase *pObj, UINT32 param, I3_EVT_CODE code)
{	
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		switch( event )
		{
		case I3_EVT_CONTACT :
			if( i3::same_of<i3PhysixShapeSet*>(pObj))
			{
				i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pObj;
				
				for( INT32 i = 0; i < pShapeSet->getContactCount(); i++ )
				{
					const I3_PHYSIX_CONTACT_REPORT_PTR& spReport = pShapeSet->getContactReport( i );

					_QueContactReport( spReport );
				}

				return true;
			}
			break;
		}
	}

	return i3GameObj::OnEvent( event, pObj, param, code);
}

void CGameCharaEquip::Instantiate( void)
{
	MATRIX mat;
	I3_ONINSTANCING_INFO info;
	info._pBase = this;
	info._pMatrixStack = g_pFramework->GetViewer()->GetSceneTracer()->GetModelViewMatrixStack();
	
	i3Matrix::SetPos( &mat, 0.f, g_pConfigEx->GetPhysix().HeightEquip, 0.f);
	info._pMatrixStack->Push( &mat, nullptr);
	OnInstancing( &info);
	info._pMatrixStack->Pop();
}



void CGameCharaEquip::ReloadTransform()
{
	if( !m_pOwner ) return ;
	if( !m_pInfo ) return ;

	Avatar* pAvatar = i3::same_cast<Avatar*>(m_pOwner);
	if( pAvatar )
	{
		SetMatrix( &I3_IDENTITYMATRIX);

		i3::rc_string strName = pAvatar->GetPartName(EQUIP::ePART_CHARA); 
		CEquipTransform * pTransform = m_pInfo->GetTransform( pAvatar->getTeam());
		if( pTransform == nullptr ) return ;
		VEC3D vPos;
		REAL32 fScale = 0.0f;
		i3mem::FillZero( &vPos, sizeof(vPos) );
		if( pTransform->GetPositionScale( strName.c_str(), vPos, fScale ) )
		{
			MATRIX matW, matP, matS;
			
			i3Matrix::SetScale(&matS, fScale,fScale,fScale );
			i3Matrix::SetTranslate(&matP, vPos.x, vPos.y, vPos.z);
			i3Matrix::Mul(&matW,&matS,&matP);
			SetMatrix( &matW );
		}
	}
}

bool CGameCharaEquip::SetEquipMatrix(INT32 idx, VEC3D * pVecPos)
{
	MATRIX mat;
	i3Skeleton * pSkel = m_pBody->getFrameLOD()->getSkeleton();

	if(pSkel != nullptr && idx < pSkel->getBoneCount() )
	{
		if (pSkel->isShapeSet(idx) == false)
		{
			i3::rc_wstring Resname;
			m_pInfo->GetResPostName(Resname);
			I3PRINTLOG(I3LOG_NOTICE, "pShapeSet is nullptr, Equip ID %d, Name %s", m_pInfo->GetITEMID(), Resname.c_str());
			return false;
		}
		i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet(idx);
		
		pShapeSet->getShapeGlobalPose( &mat);
		i3Matrix::SetPos(&mat, pVecPos);	
		pShapeSet->setShapeGlobalPose(&mat);

		i3Matrix::Copy(m_pBody->getCacheMatrix(idx), &mat);
	}

	return true;
}