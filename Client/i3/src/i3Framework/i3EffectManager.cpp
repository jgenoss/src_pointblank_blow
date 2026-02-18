#include "i3FrameworkPCH.h"
#include "i3EffectManager.h"
#include "i3EffectTypeInfo.h"
#include "i3ResourceManager.h"
#include "i3Framework.h"
#include "i3ActionEmitEffect.h"
#include "i3Scene.h"
#include "i3Base/itl/algorithm_util.h"
#include "i3Base/itl/vector_set_util.h"
#include "i3Base/string/algorithm/to_upper.h"

I3_CLASS_INSTANCE( i3EffectManager);

/*
static INT32 _CompareEffectTypeProc( i3EffectTypeInfo * p1, i3EffectTypeInfo * p2)
{
	if( p1->getHashCode() > p2->getHashCode())
		return -1;
	else if( p1->getHashCode() < p2->getHashCode())
		return 1;

	return 0;
}
*/
bool	operator<(const i3EffectTypeInfo& a1, const i3EffectTypeInfo& a2)
{
	return a1.getHashCode() > a2.getHashCode();				// 내림차순..
}

/*
static INT32 _FindEffectTypeProc( i3EffectTypeInfo * p1, UINT32 hashcode)
{
	if( p1->getHashCode() > hashcode)
		return -1;
	else if( p1->getHashCode() < hashcode)
		return 1;

	return 0;
}
*/

struct _FindEffectTypeProc
{
	// 내림 차순..
	bool operator()(const i3EffectTypeInfo * p1, UINT32 hashcode) const { return (hashcode < p1->getHashCode() );	}
	bool operator()(UINT32 hashcode, const i3EffectTypeInfo * p1) const {	return (p1->getHashCode() < hashcode);	}
};


i3EffectManager::i3EffectManager(void)
{
	m_EffectList.reserve( 256);
	m_EmitterList.reserve( 32);

	m_pRenderer = i3ParticleRender::new_object();
}

i3EffectManager::~i3EffectManager(void)
{
	RemoveAllEmitter();
	RemoveAllEffect();

	I3_SAFE_RELEASE( m_pRenderer);
}

void i3EffectManager::RemoveAllEffect(void)
{
	i3EffectTypeInfo * pInfo;

	for(size_t i = 0; i < m_EffectList.size(); i++)
	{
		pInfo = m_EffectList.get_vector()[i];

		pInfo->getRoot()->RemoveFromParent();
		I3_MUST_RELEASE( pInfo);
	}

	m_EffectList.clear();
}

INT32 i3EffectManager::FindEffect( const char * pszName)
{
	if( pszName == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Effect is Null");
		return -1;
	}

	char conv[MAX_PATH];
	i3::safe_string_copy( conv, pszName, MAX_PATH);
	i3::to_upper( conv);

	UINT32 hash = CRC32( 0xFFFFFFFF, (UINT8 *) conv, i3::generic_string_size( conv));

	return FindEffect( hash);
}

INT32 i3EffectManager::FindEffect( UINT32 hash)
{
//	INT32 idx;
//	m_EffectList.SetCompareProc( (COMPAREPROC) _FindEffectTypeProc);
//	idx = m_EffectList.FindItem( (void *) hash);
	i3::pair<i3::vector_set<i3EffectTypeInfo*>::iterator, bool> pr = i3::au::binary_find(m_EffectList.begin(), m_EffectList.end(), hash, _FindEffectTypeProc());
	
	if (pr.second == false) return -1;

	return (INT32)i3::distance(m_EffectList.begin(), pr.first);
//	m_EffectList.SetCompareProc( (COMPAREPROC) _CompareEffectTypeProc);
//	return idx;
}

void i3EffectManager::AddEffectType( i3EffectTypeInfo * pInfo)
{
	I3_MUST_ADDREF( pInfo);

	GetNode()->AddChild( pInfo->getRoot());

	m_EffectList.insert( pInfo);
}

void i3EffectManager::RemoveEffectType( i3EffectTypeInfo * pInfo)
{
	i3::vsu::linear_remove(m_EffectList, pInfo);		// 2진검색을 통해 지울수도 있지만..일단 이렇게....
//	m_EffectList.Remove( pInfo);

	GetNode()->RemoveChild( pInfo->getRoot());

	I3_MUST_RELEASE( pInfo);
}

// 생성
bool i3EffectManager::Create(void)
{
	m_pRenderer->Create();

	return true;
}

bool i3EffectManager::Load( const char * szIni, bool bCommon)
{
	INT32	i;
	bool	bRet = false;
	i3Stream * pStream;
	i3IniParser * pParser = i3IniParser::new_object();
	char szName[256];

	if( szIni == nullptr)
	{
		I3_SAFE_RELEASE( pParser);
		return true;
	}

	i3ResourceManager * pMng = m_pFramework->GetResourceManager();

	pStream = pMng->OpenStream( szIni);
	if( pStream == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "Could not open %s", szIni );
		goto ExitLabel;
	}

	if( !pParser->Open( pStream))
	{
		goto ExitLabel;
	}

	// Type 개수만큼 이름과 속성값을 얻어온다.
	for( i = 0; i < pParser->GetSectionCount(); ++i)
	{
		INT32 idx;
		i3EffectTypeInfo * pInfo;
		i3IniSectionInfo * pSection;

		pSection = pParser->GetSection( i);

		idx = FindEffect( pSection->Name);

		if( idx != -1)
		{
			pInfo = getEffectType( idx);
		}
		else
		{
			pInfo = i3EffectTypeInfo::new_object_ref();
			
			pInfo->SetName( pSection->Name);
			pInfo->setCommonRes( bCommon);

			AddEffectType( pInfo);
		}

		if( !pParser->ReadSectionByIndex( i))
		{
			I3TRACE("ERROR : ReadSection. ( i = %d)\n", i);
			goto ExitLabel;
		}
		else
		{
			// 최대 개수
			INT32 maxCount;
			GET_IMPORTANT_VALUE( "Count", 0, &maxCount);
			I3ASSERT( maxCount >= 0);

			// Use decal
			bool	bDecal = false;
			char	szCheck[MAX_PATH];
			GET_IMPORTANT_STRING( "Decal", 0, szCheck, MAX_PATH -1 );

			// Priority
			INT32	priority;
			if( pParser->GetValue( "Priority", 30, &priority) == false)
			{
				priority = 50;
			}

			if( maxCount != pInfo->getInstanceCount())
			{
				sprintf( szName, "%s.i3s", pInfo->getName() );

				i3GameResSceneGraph * pRes = (i3GameResSceneGraph *) m_pFramework->FindResourceByOnlyNameAndClassMeta( szName, i3TimeSequence::static_meta());
				//I3ASSERT( pRes != nullptr);	// 불필요한 로그 삭제
				if ( pRes )
				{
					I3ASSERT( i3::same_of<i3TimeSequence*>(pRes->getScene()));

					if( szCheck[0] == 'Y' )
					{
						bDecal = true;
						pInfo->setDecal( true);

						//m_DecalList.Add( pInfo );
					}

					m_pRenderer->AddParticleScene( (i3TimeSequence *) pRes->getScene(), maxCount, priority, bDecal);

					pInfo->Create( pRes, maxCount, bDecal );
				}
			}
			else
			{
				I3TRACE( "!!!!! EFFECT : Could not found %s effect.\n", pSection->Name);
			}
		}
	}

#if defined ( I3_DEBUG)
	I3TRACE("==========================================================\n");
	I3TRACE( "Number of Effect Type = %d\n", (INT32)m_EffectList.size());
	I3TRACE("==========================================================\n");
#endif
	bRet = true;

	m_pRenderer->ArrangeRenderOrder();		//  이 함수를 호출하는 대신 다른 방법을 써야한다..(2012.05.22)

	OnCreate();

ExitLabel:
	I3_SAFE_RELEASE( pParser);
	I3_SAFE_RELEASE( pStream);
	return bRet;
}

i3TimeStamp *	i3EffectManager::EmitEffect( INT32 nType, MATRIX * pEmitTx, MATRIX * pBoneMatrix, MATRIX * pLocalOffset, VEC3D * pSrcPos)
{
	i3EffectTypeInfo * pInfo;

	if( nType < 0 || nType >= getEffectTypeCount())
	{
		I3TRACE("EmitEffect : nType : %d is Invalid\n");
		return nullptr;
	}

	pInfo = getEffectType( nType);

	i3TimeStamp * pEffect = pInfo->Emit( m_timeLocal, m_bForceEmit, m_pCancelEffectProc);
	if( pEffect == nullptr)
	{
		return nullptr;
	}

	i3MatrixObjectProxy * pProxy = (i3MatrixObjectProxy*)pEffect->GetMatrixObject();

	pEffect->SetMatrix( pEmitTx);
	
	pProxy->SetSourceMatrix( pBoneMatrix);
	pProxy->SetLocalOffsetTransform( pLocalOffset);
	pProxy->SetPositionSource( pSrcPos);

	return pEffect;
}

i3TimeStamp * i3EffectManager::AddEffect( INT32 nType, VEC3D * pPos, VEC3D * pNormal, MATRIX * pMatrix, REAL32 fDeg, i3GameObjBase * pObj)
{	
	i3TimeStamp * pEffect = nullptr;
	MATRIX mtx;

	if( pObj != nullptr)	// 물리를 적용받는 Effct(위치가 변경되어짐)
	{
		MATRIX InverseMat; 
		MATRIX ResultMat;

		// Effect의 월드매트릭스
		{
			if( i3Math::abs( 1.0f - i3Vector::Dot( pNormal, &I3_YAXIS)) < 0.0001f)
			{
				i3Matrix::MakeWithAt( &mtx, pNormal, &I3_ZAXIS);
			}
			else
			{
				i3Matrix::MakeWithAt( &mtx, pNormal, &I3_YAXIS);
			}

			if( fDeg != 0.0f)
				i3Matrix::PostRotateAxisDeg( &mtx, pNormal, fDeg); 

			i3Matrix::SetPos( &mtx, pPos);
		}

		if( pMatrix)
		{
			i3Matrix::Inverse( &InverseMat, 0, pMatrix);
			i3Matrix::Mul( &ResultMat, &mtx, &InverseMat);

			pEffect = EmitEffect( nType, &I3_IDENTITYMATRIX, pMatrix, &ResultMat, nullptr);
		}
		else
		{
			pEffect = EmitEffect( nType, &I3_IDENTITYMATRIX, nullptr, nullptr, nullptr);
		}
	}
	else	// 물리를 적용받지 않는 Effct(위치가 고정)
	{
		//Normal 방향이 Up Vector가 되도록 조정한다.

		if( pNormal != nullptr)
		{
			i3Matrix::MakeWithUp( &mtx, pNormal, &I3_YAXIS);
			if( fDeg != 0.0f)
				i3Matrix::PostRotateAxisDeg( &mtx, pNormal, fDeg); 
			i3Matrix::SetPos( &mtx, pPos);
		}
		else
		{
			i3Matrix::SetTranslate( &mtx, pPos);
		}

		pEffect = EmitEffect( nType, &mtx, pMatrix, nullptr, nullptr);
	}

	if( pEffect != nullptr)
	{
		pEffect->setRefObject( pObj);
	}

	return pEffect;
}

i3TimeStamp * i3EffectManager::AddEffect( INT32 nType, VEC3D * pPos, VEC3D * pNormal, REAL32 fDeg)
{
	return AddEffect( nType, pPos, pNormal, fDeg, nullptr, 0 );
}

i3TimeStamp *	i3EffectManager::AddEffect( i3ActionEmitEffect * pAction, MATRIX * pMatrix)
{
	if( pAction->getEffectIndex() == -1)
	{
		INT32 idx = FindEffect( (const char *) pAction->getEffectName());
		if( idx == -1)
			return nullptr;

		pAction->setEffectIndex( idx);
	}

	MATRIX mtx, * pAttachMtx = nullptr;
	VEC3D * pNormal = i3Matrix::GetUp( pMatrix);

	if( pAction->getEmitToWorldState())
	{
		i3Matrix::SetScale( &mtx, pAction->getScale(), pAction->getScale(), pAction->getScale());

		if( pAction->getAttachBoneMatrix() != nullptr)
			i3Matrix::Mul( &mtx, &mtx, pAction->getAttachBoneMatrix());
	}
	else
	{
		pAttachMtx = pAction->getAttachBoneMatrix();

		if( i3Math::abs( 1.0f - i3Vector::Dot( pNormal, &I3_YAXIS)) < 0.0001f)
		{
			i3Matrix::MakeWithAt( &mtx, pNormal, &I3_ZAXIS);
		}
		else
		{
			i3Matrix::MakeWithAt( &mtx, pNormal, &I3_YAXIS);
		}
	}

	i3TimeStamp * pEffect;

	pEffect = EmitEffect( pAction->getEffectIndex(), &mtx, pAttachMtx, pAction->getLocalOffsetMatrix(), nullptr);

	if( pAction->isRepeat())
	{
		m_EmitterList.push_back( pAction);
		I3_MUST_ADDREF( pAction);
		pAction->reset();
	}

	return pEffect;
}

i3TimeStamp * i3EffectManager::AddEffect( INT32 nType, VEC3D * pPos, VEC3D * pNormal, REAL32 fDeg, VEC2D * pUVs, INT32 nCount, UINT16 * pIndices, INT32 nIndexCount )
{
	MATRIX mtx;
	i3TimeStamp * pEffect;

	// New vertex set
	if( i3Math::abs( 1.0f - i3Vector::Dot( pNormal, &I3_YAXIS)) < 0.0001f)
	{
		i3Matrix::MakeWithAt( &mtx, pNormal, &I3_ZAXIS);
	}
	else
	{
		i3Matrix::MakeWithAt( &mtx, pNormal, &I3_YAXIS);
	}

	if( fDeg != 0.0f)
		i3Matrix::PostRotateAxisDeg( &mtx, pNormal, fDeg); 
	
	i3Matrix::SetPos( &mtx, pPos);

	pEffect = EmitEffect( nType, &mtx, nullptr, nullptr, nullptr);

	return pEffect;
}

void i3EffectManager::RemoveAllEmitter(void)
{
	for(size_t i = 0; i < m_EmitterList.size(); i++)
	{
		I3_MUST_RELEASE( m_EmitterList[i]);
	}

	m_EmitterList.clear();
}

void i3EffectManager::OnUpdate( REAL32 rDeltaSeconds)
{
	INT32 cnt = 0;
	i3EffectTypeInfo * pInfo;

	m_timeLocal += rDeltaSeconds;


	for(INT32 i = 0; i < getEffectTypeCount(); ++i)
	{
		pInfo = getEffectType( i);

		cnt += pInfo->OnUpdate( rDeltaSeconds);
	}

	// Emitter 처리
	for(size_t i = 0; i < m_EmitterList.size(); i++)
	{
		i3ActionEmitEffect * pAction = m_EmitterList[i];

		pAction->addLifeTime( rDeltaSeconds);

		if( pAction->getCurrentLifeTime() < pAction->getRepeatDuration())
		{
			pAction->addIntervalTime( rDeltaSeconds);

			if( pAction->getCurrentInterval() > pAction->getRepeatInterval())
			{
				pAction->setCurrentInterval( 0.0f);

				MATRIX mtx, * pAttachMtx = nullptr;

				if( pAction->getEmitToWorldState())
				{
					i3Matrix::SetScale( &mtx, pAction->getScale(), pAction->getScale(), pAction->getScale());
					i3Matrix::Mul( &mtx, &mtx, pAction->getAttachBoneMatrix());
				}
				else
				{
					pAttachMtx = pAction->getAttachBoneMatrix();
				}

				EmitEffect( pAction->getEffectIndex(), &mtx, pAttachMtx, pAction->getLocalOffsetMatrix(), nullptr);
			}
		}
		else
		{
			// 끝났다.
			m_EmitterList.erase( m_EmitterList.begin() + i);

			i--;
		}
	}

	if( cnt == 0)
	{
		m_timeLocal = 0.0f;
	}

	//_ParseContactReport();
}

void i3EffectManager::Reset()
{
	INT32 i;
	i3EffectTypeInfo * pInfo;

	m_timeLocal = 0.0f;

	//RemoveAllNonCommonEffects();

	for( i = 0; i < getEffectTypeCount(); i++)
	{
		pInfo = getEffectType( i);

		pInfo->Reset();
	}

	RemoveAllEmitter();
}

void i3EffectManager::RemoveAllNonCommonEffects(void)
{
	INT32 i;
	i3EffectTypeInfo * pInfo;

	for( i = 0; i < getEffectTypeCount(); i++)
	{
		pInfo = getEffectType( i);

		if( pInfo->isCommonRes())
			continue;

		I3TRACE( "[%s] - Removed\n", pInfo->getName());
		RemoveEffectType( pInfo);

		i--;
	}
}

void i3EffectManager::OnCreate(void)
{
	getSceneObject()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
}

void i3EffectManager::AttachToLayer( i3SgLayer * pSgLayer)
{
	getSceneObject()->ChangeParent(pSgLayer->getLayerRoot());
}

bool i3EffectManager::OnLostDevice( bool bLostDevice)
{
	m_pRenderer->OnLostDevice( bLostDevice);
	return true;
}

bool i3EffectManager::OnRevive( i3RenderContext * pCtx)
{
	m_pRenderer->OnRevive( pCtx);
	return true;
}

void i3EffectManager::EnterStage( void)
{
	m_pRenderer->CreatePhysXShapes();
}

void i3EffectManager::LeaveStage( void)
{
	m_pRenderer->ReleasePhysXShapes();
}

void	i3EffectManager::AddWorldRenderAttrToEffect(i3RenderAttr* worldRenderAttr)
{
	m_pRenderer->AddWorldRenderAttrToEffect(worldRenderAttr);
}

void	i3EffectManager::RemoveWorldRenderAttrsFromEffect()
{
	m_pRenderer->RemoveWorldRenderAttrsFromEffect();
}





#if defined( I3_DEBUG)
void i3EffectManager::Dump(void)
{
	INT32 i;
	i3EffectTypeInfo * pInfo;

	I3TRACE( "------------------------- Effects -------------------------\n");

	for( i = 0; i < getEffectTypeCount(); i++)
	{
		pInfo = getEffectType( i);

		I3TRACE( "%5d [%08X] %s - %d instances\n", i, pInfo->getHashCode(), pInfo->getName(), pInfo->getRoot()->GetChildCount());
	}

	I3TRACE( "-----------------------------------------------------------\n");
}

#endif