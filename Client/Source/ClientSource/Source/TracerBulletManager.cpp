#include "pch.h"
#include "GlobalVariables.h"
#include "TracerBulletManager.h"
#include "WeaponGlobal.h"
#include "i3Base/string/ext/utf16_to_mb.h"

const INT32 g_TracerBulletInstanceCount = 300;

I3_CLASS_INSTANCE( CTracerBulletManager);//, i3GameObj);

CTracerBulletManager::CTracerBulletManager( void)
{
#ifdef ENABLE_TRACER_BULLET
	m_bActive = true;
#else
	m_bActive = false;
#endif 
}

CTracerBulletManager::~CTracerBulletManager( void)
{	
	Destroy();

	I3_SAFE_RELEASE( m_pEffectAttrSet);
}

void CTracerBulletManager::Destroy( void)
{
	if( m_pEffectAttrSet != nullptr)
		m_pEffectAttrSet->RemoveFromParent();

	for( size_t i = 0; i < m_StructContainer.size(); i++)
	{
		CTracerBullet * pUnit = m_StructContainer[i];

		pUnit->ReturnInstance();
	}

	m_StructContainer.clear();
	m_nActiveCount = 0;
}

bool CTracerBulletManager::Create( i3Node * pRenderRoot)
{
	I3ASSERT( pRenderRoot != nullptr);

	Destroy();
	I3_SAFE_RELEASE( m_pEffectAttrSet);

	i3Stage * pStage = g_pFramework->GetCurrentStage();
	m_pInstanceManager = pStage->GetInstanceManager( CTracerBullet::static_meta());
	I3ASSERT( m_pInstanceManager != nullptr);

	m_pInstanceManager->setInstanceMeta( CTracerBullet::static_meta());
	
	//	Billboard
	{
		I3ASSERT( g_pWeaponInfoDataBase != nullptr);
		char szPath[MAX_PATH];		//	이펙트 파일 지정

		m_pEffectAttrSet = i3AttrSet::new_object();

		i3LightingEnableAttr * pLightEnable = i3LightingEnableAttr::new_object_ref();
		pLightEnable->Set( false);
		m_pEffectAttrSet->AddAttr( pLightEnable);

		i3FaceCullModeAttr * pFaceCullModeAttr = i3FaceCullModeAttr::new_object_ref();
		pFaceCullModeAttr->Set( I3G_FACE_CULL_NONE);
		m_pEffectAttrSet->AddAttr( pFaceCullModeAttr);

		i3TextureFunctionAttr * pTexFuncAttr = i3TextureFunctionAttr::new_object_ref();
		pTexFuncAttr->SetFunction( I3G_TEXTURE_FUNC_ADD);
		m_pEffectAttrSet->AddAttr( pTexFuncAttr);

		i3AlphaTestEnableAttr * pAlphaTestEnableAttr = i3AlphaTestEnableAttr::new_object_ref();
		pAlphaTestEnableAttr->Set(true);
		m_pEffectAttrSet->AddAttr( pAlphaTestEnableAttr);

		i3AlphaFuncAttr * pAlphaFuncAttr = i3AlphaFuncAttr::new_object_ref();
		pAlphaFuncAttr->SetFunction( I3G_COMP_ALWAYS);
		m_pEffectAttrSet->AddAttr( pAlphaFuncAttr);

		pRenderRoot->AddChild( m_pEffectAttrSet);

		for( INT32 i = 0; i < TRACER_TYPE_MAX; i++)
		{
			m_pBulletBillboard[ i] = i3Billboard::new_object_ref();
			I3ASSERT( m_pBulletBillboard[ i] != nullptr);

			switch( i)
			{
			case TRACER_TYPE_BULLET:
				{
					i3::snprintf( szPath, MAX_PATH, "Effect/%s.i3i", g_pWeaponInfoDataBase->getGlobal()->m_strTracerBulletEffectFile);		// 자동 형변환..

					//	반드시 여기서 해야 적용
					m_pBulletBillboard[ i]->SetSrcBlend( I3G_BLEND_SRCALPHA);	
					m_pBulletBillboard[ i]->SetDestBlend( I3G_BLEND_ONE);
				}
				break;
			case TRACER_TYPE_HAND_GRENADE:
				{
					i3::snprintf( szPath, MAX_PATH, "Effect/%s.i3i", g_pWeaponInfoDataBase->getGlobal()->m_strTracerHandGrenadeEffectFile);		// 자동 형변환..

					//	반드시 여기서 해야 적용
					m_pBulletBillboard[ i]->SetSrcBlend( I3G_BLEND_SRCALPHA);	
					m_pBulletBillboard[ i]->SetDestBlend( I3G_BLEND_INVSRCALPHA);	
				}
				break;
			case TRACER_TYPE_SHELL_GRENADE:
				{
					i3::snprintf( szPath, MAX_PATH, "Effect/%s.i3i", g_pWeaponInfoDataBase->getGlobal()->m_strTracerShellGrenadeEffectFile);		// 자동 형변환..

					//	반드시 여기서 해야 적용
					m_pBulletBillboard[ i]->SetSrcBlend( I3G_BLEND_SRCALPHA);	
					m_pBulletBillboard[ i]->SetDestBlend( I3G_BLEND_INVSRCALPHA);	
				}
				break;
			case TRACER_TYPE_ARROW:
				{
					i3::snprintf( szPath, MAX_PATH, "Effect/%s.i3i", g_pWeaponInfoDataBase->getGlobal()->m_strTracerArrowEffectFile);		// 자동 형변환..

					//	반드시 여기서 해야 적용
					m_pBulletBillboard[ i]->SetSrcBlend( I3G_BLEND_SRCALPHA);	
					m_pBulletBillboard[ i]->SetDestBlend( I3G_BLEND_INVSRCALPHA);	
				}
				break;
			case TRACER_TYPE_SMOKE:
				{
					i3::snprintf( szPath, MAX_PATH, "Effect/%s.i3i", g_pWeaponInfoDataBase->getGlobal()->m_strTracerSmokeEffectFile);		// 자동 형변환..

					//	반드시 여기서 해야 적용
					m_pBulletBillboard[ i]->SetSrcBlend( I3G_BLEND_SRCALPHA);	
					m_pBulletBillboard[ i]->SetDestBlend( I3G_BLEND_INVSRCALPHA);	
				}
				break;
			case TRACER_TYPE_FLASH:
				{
					i3::snprintf( szPath, MAX_PATH, "Effect/%s.i3i", g_pWeaponInfoDataBase->getGlobal()->m_strTracerFlashEffectFile);		// 자동 형변환..

					//	반드시 여기서 해야 적용
					m_pBulletBillboard[ i]->SetSrcBlend( I3G_BLEND_SRCALPHA);	
					m_pBulletBillboard[ i]->SetDestBlend( I3G_BLEND_INVSRCALPHA);	
				}
				break;
			case TRACER_TYPE_BULLET_SMOKE:
				{
					i3::snprintf(szPath, MAX_PATH, "Effect/TracerBulletSmoke.i3i");				

					//	반드시 여기서 해야 적용
					m_pBulletBillboard[ i]->SetSrcBlend( I3G_BLEND_SRCALPHA);	
					m_pBulletBillboard[ i]->SetDestBlend( I3G_BLEND_INVSRCALPHA);
				}
				break;
				
			}

#if defined( NO_GRENADE_TRACE_EFFECT)
			if( i == TRACER_TYPE_GRENADE || i == TRACER_TYPE_SMOKE || i == TRACER_TYPE_FLASH)
				continue;
#endif

			i3ResourceManager * pMng = g_pFramework->GetResourceManager();

			i3Texture * pTex = pMng->LoadTexture( szPath);
			I3ASSERT( pTex != nullptr);
			
			//	궤적 빌보드 생성
			m_pBulletBillboard[ i]->SetCount( g_TracerBulletInstanceCount);
			m_pBulletBillboard[ i]->SetTexture( pTex);	
			m_pBulletBillboard[ i]->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);

			m_pEffectAttrSet->AddChild( m_pBulletBillboard[ i]);

			I3_MUST_RELEASE(pTex);
		}
	}

	//	Stack Instance Bullet in container
	for( INT32 i = 0; i < g_TracerBulletInstanceCount; i++)
	{
		CTracerBullet * pNew = (CTracerBullet *) m_pInstanceManager->GetFreeInstance();

		if( pNew == nullptr)
		{
			I3TRACE("Warning! Failed to create tracer bullet (idx: %d).\n", i);
		}
		else
		{
			I3ASSERT( pNew->GetRefCount() >= 1);

			pNew->Create( i);
			m_StructContainer.push_back( pNew);
		}
	}

	return true;
}

void CTracerBulletManager::ResetAll( void)
{	//	모든 총알 초기화
	for( size_t i = 0; i < m_StructContainer.size(); i++)
	{
		CTracerBullet * pUnit = m_StructContainer[i];
		if( pUnit != nullptr)
		{
			pUnit->Reset( false);
		}
	}

	m_nActiveCount = 0;
}

void CTracerBulletManager::OnUpdate( REAL32 rDeltaSeconds)
{	
	//REAL_PROFILE( "CTracerBulletManager");

	if( !m_bActive)	return;

	m_nActiveCount = 0;

	for( size_t i = 0; i < m_StructContainer.size(); i++)
	{
		CTracerBullet * pUnit = m_StructContainer[i];
		if( pUnit != nullptr)
		{
			if( pUnit->IsActive())
			{
				m_nActiveCount++;

				pUnit->OnUpdate( rDeltaSeconds);
			} 
		}
	}
}

bool CTracerBulletManager::Fire( TracerBulletInfo * pInfo)
{
	REAL32 dstLen;
	VEC3D dir;

	I3ASSERT( pInfo != nullptr);
	I3_BOUNDCHK( pInfo->_Type, TRACER_TYPE_MAX);

	if( !m_bActive)	return false;

	//	총알 궤적일 경우 환경 설정에서 꺼져 있으면 발사하지 않는다.
	if( pInfo->_Type == TRACER_TYPE_BULLET && !g_pEnvSet->m_bEnableBulletTrace)		return false;

	//	총알 연기일 경우 환경 설정에서 꺼져 있으면 발사하지 않는다.
	if( pInfo->_Type == TRACER_TYPE_BULLET_SMOKE && !g_pEnvSet->m_bEnableBulletSmoke)		return false;

	i3Vector::Sub( &dir, &pInfo->_vDestPos, &pInfo->_vBeginPos);
	dstLen = i3Vector::Length( &dir);
	
	if( pInfo->_Type == TRACER_TYPE_BULLET)
	{
		if( dstLen <= 0.5f )
		{
			return true;		//	거리에 비해 총알이 길어서 발사할 필요가 없다.
		}
	}

#if defined( NO_GRENADE_TRACE_EFFECT)
	if( pInit._Type == TRACER_TYPE_HAND_GRENADE || pInit._Type == TRACER_TYPE_SMOKE || pInit._Type == TRACER_TYPE_FLASH)
		return false;
#endif

	//	궤적을 발사
	for( size_t i = 0; i < m_StructContainer.size(); i++)
	{
		CTracerBullet * pUnit = m_StructContainer[i];
		if( pUnit != nullptr)
		{
			//	사용하지 않는 것이면
			if( pUnit->IsActive() == false)
			{	
				#ifdef DEBUG_TRACER_BULLET 
				I3TRACE("[Cnt:%d] ", GetActiveCount() + 1);
				#endif

				pUnit->Start( m_pBulletBillboard[ pInfo->_Type], pInfo);

				return true;
			}
		}
	}

	//	남는 총알이 없다면 
	//	더이상 궤적을 발사하지 않는다.
	/*for( INT32 i = 0; i < m_StructContainer.GetCount(); i++)
	{
		CTracerBullet * pSearchBullet = GetTracerBullet( i);
		if( pSearchBullet != nullptr)
		{	//	현재 진행중인 총알 (투척계열 궤적을 제외한) 중에 하나를 강제로 활용한다.
			if( pSearchBullet->GetType() == TRACER_TYPE_BULLET)
			{
				pSearchBullet->Reset();	//	진행중 총알 하나 초기화

#ifdef DEBUG_TRACER_BULLET
				I3TRACE( "[%s] 남는 총알이 없어 강제로 총알 하나를 리셋합니다.\n", __FUNCTION__);
#endif

				//	재귀호출
				return Fire( pInfo);
			}
		}
	}*/

#ifdef DEBUG_TRACER_BULLET
	I3TRACE( "[CTracerBulletManager::Fire] Warning! Failed to fire.\n");
	I3TRACE( "                  Not enough empty tracer bullet. all bullet busy!!\n");
#endif

	return false;
}