#include "i3FrameworkPCH.h"
#include "i3Object.h"
#include "i3ActionBreakObject.h"
#include "i3ActionVisible.h"
#include "i3ActionDisable.h"
#include "i3ActionPlaySound.h"
#include "i3GameResObject.h"
#include "i3Framework.h"

//I3_CLASS_INSTANCE( i3Object, i3GameObj);
I3_CLASS_INSTANCE( i3Object);

i3Object::i3Object(void)
{
}

i3Object::~i3Object(void)
{
	RemoveAllState();

	I3_SAFE_RELEASE( m_pObjRes);
}

void i3Object::Create( i3Framework * pFramework, i3GameResObject * pRes)
{	

	i3Object * pSrc = (i3Object *) pRes->getGameObject();

	I3_REF_CHANGE( m_pObjRes, pRes);

	pSrc->CopyTo( this, I3_COPY_INSTANCE);

	pRes->Bind( pFramework, this, false, true);

	// 여기 수정해야 됩니다. dfly79
	SetInitialState( 0, true);
	//SetName( pRes->GetName());

#if defined( I3_DEBUG)
	GetNode()->SetName( pRes->GetNameString());
#endif
}

void i3Object::SetInitialState( INT32 nState, bool bInit)
{	
	if( nState > m_StateCount)	return;

	m_nInitialState		= nState;
}

void i3Object::OnChangeTime( REAL32 tm)
{
	i3GameObj::OnChangeTime( tm);

	if( tm == 0.0f)
	{
		setCurrentHP( m_MaxHP);

		m_OldState		= m_CurState;
					
		m_CurState		= m_nInitialState;

		for( INT32 i = 0; i < m_StateCount; i++)
		{
			if( m_pState[i].m_pTimeEvent != nullptr)
			{
				m_pState[i].m_pTimeEvent->Reset();

				m_pState[i].m_pTimeEvent->OnChangeTime( 0.0f);
			}
		}

		SetEnable();

		m_bChangedSg	= false;
				
	}
}

void i3Object::SetStateCount( INT32 cnt)
{
	RemoveAllState();

	m_pState = new I3_OBJECT_STATE[cnt];		// 배열임.. 
	I3ASSERT( m_pState != nullptr);
	NetworkDump_Malloc( m_pState, sizeof( I3_OBJECT_STATE) * cnt, __FILE__, __LINE__);

	m_StateCount = cnt;
	m_CurState = 0;
}

void	i3Object::SetVisible( bool bTrue, REAL32 tm)
{
	i3GameObj::SetVisible( bTrue, tm);
}

void i3Object::RemoveAllState(void)
{
	INT32 i;
	
	for( i = 0; i < m_StateCount; i++)
	{
		I3_SAFE_RELEASE( m_pState[i].m_pTimeEvent);
		I3_SAFE_RELEASE( m_pState[i].m_pSg);
		I3_SAFE_RELEASE( m_pState[i].m_pRes);
	}

	I3_SAFE_DELETE_ARRAY( m_pState);
}

void i3Object::Damage( REAL32 dmg, i3GameObj * pEventSender, i3PhysixShapeSet * pHitShapeSet)
{
	INT32 old;
	REAL32 ratio;

	m_pEventSender = pEventSender;

	m_HP -= dmg;
	if( m_HP < 0.0f)	m_HP = 0.0f;

	ratio = m_HP / m_MaxHP;

	old = m_CurState;

	while( ((m_CurState + 1) < m_StateCount) && (m_pState[ m_CurState + 1].m_HPRatio >= ratio))
	{
		if( m_pState[ m_CurState].m_pTimeEvent != nullptr)
		{
			m_pState[ m_CurState].m_pTimeEvent->Stop();
			m_pState[ m_CurState].m_pTimeEvent->OnChangeTime( 0.0f);
		}

		m_CurState++;
		
		if( m_pState[ m_CurState].m_pTimeEvent != nullptr)
		{
			m_pState[ m_CurState].m_pTimeEvent->OnUpdate( 0.0f);
		}
			
	}

}

INT32 i3Object::GetStateByHP(REAL32 rHP)
{
	REAL32 ratio;

	ratio = rHP / m_MaxHP;
	INT32 iState = m_CurState;

	while( ((iState + 1) < m_StateCount) && (m_pState[ iState + 1].m_HPRatio >= ratio))
	{
		iState++;
	}

	return iState;
}

void i3Object::SetDamageNoEffect( REAL32 dmg, REAL32 tm)
{
	REAL32 ratio;

	m_HP -= dmg;
	if( m_HP < 0.f)	m_HP = 0.f;

	ratio = m_HP / m_MaxHP;

	while( ((m_CurState + 1) < m_StateCount) && (m_pState[ m_CurState + 1].m_HPRatio >= ratio))
	{
		//I3TRACE( "DAMAGE : %d, %f\n", m_CurState, m_pState[ m_CurState + 1].m_HPRatio);
		m_CurState++;
		if( m_pState[ m_CurState].m_pTimeEvent != nullptr)
		{
			m_pState[ m_CurState].m_pTimeEvent->OnUpdate( tm);
		}
	}

}

void i3Object::OnUpdate( REAL32 tm)
{
	if( m_CurState == -1)
	{
		m_CurState = 0;
	}

	if( m_CurState < m_StateCount)
	{
		I3_OBJECT_STATE * pState = getState( m_CurState);

		if( pState->m_pTimeEvent != nullptr)
		{
			pState->m_pTimeEvent->OnUpdate( tm);
		}
			
	}

	if( isFading())
	{
		if( m_pMaterialAttr != nullptr )
			m_pMaterialAttr->SetDiffuse( 1.f, 1.f, 1.f, getFadeRate());
	}

	i3GameObj::OnUpdate( tm);
	
}

bool i3Object::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if( event == I3_EVT_ACTION)
		{
			if( i3::same_of<i3ActionBreakObject* >(pObj))
			{
				_SetCurrentSg( false);

				return true;
			}
		}
	}

	return i3GameObj::OnEvent(event, pObj, param, code);
}

void i3Object::_SetCurrentSg( bool bInit)
{
#if defined( I3_PHYSX)
	bool bDynamicActive = getDynamicActiveState();

	if( m_OldState != -1)
	{
		setDynamicActiveState( false);
	}
#endif

	if( m_CurState != -1)
	{
		i3SceneGraphInfo * pSg = getSceneGraphInfo( m_CurState);
		I3_OBJECT_STATE * pState = getState( m_CurState);
		I3_OBJECT_STATE * pOldState = nullptr;
		
		if( m_OldState != -1)
		{
			pOldState = getState( m_OldState);

			BindSgRes( i3Framework::getCurrentFramework(), nullptr, false, false);

			if( pOldState->m_pRes != pState->m_pRes )
				m_bChangedSg = true;
		}

		if( pState->m_pRes != nullptr)
		{
			// Common Sg.와의 연결을 올바르게 하기 위해 BindSgRes를 호출해야 한다.
			// 단, Instance Sg.의 Clone은 i3SceneGraphInfo에 있기 때문에
			// 여기에서는 Clone을 생성하지 않는다.
			BindSgRes( i3Framework::getCurrentFramework(), pState->m_pRes, false);

			if( IsAttachedScene() == false)
				AttachScene();
		}

		{
			// 현재 SceneObject에 붙어 있는 Instance Sg는 Resource의 원본 Sg.이므로
			// Clone을 생성해둔 Sg로 대체한다.
			getAttachNode()->RemoveAllChild();

			SetInstanceSg( pSg->getInstanceSg());
			//getAttachNode()->AddChild( pSg->getInstanceSg());
		}

		MATRIX oldWorldSpace;

		getSceneObject()->GetWorldSpaceSourceMatrix( &oldWorldSpace);

		// SceneObject의 Context를 전환.
		getSceneObject()->setBoneList( pState->m_pBoneList);
		getSceneObject()->SetBody( pState->m_pBody);

		getSceneObject()->FindWorldTransformSource();

		if( getSceneObject()->getCurrentAnim() != nullptr)
		{
			getSceneObject()->RefreshAnim();
		}

		// NOTE
		// setDynamicActiveState() 함수와 UpdateLastWorldTransform() 함수의 호출 순서가 바뀌면 안된다.
		// Kinematic 상태에서의 setShapeGlobalPose()는 내부적으로 moveGlobalPose를 사용하는데,
		// 이 경우 Actor가 올바르게 위치 이동하지 않는 결과를 보인다.
		// 그런 이유로, Dynamic Sttae로 변경이 된 후, setGlobalPose() 함수로 위치 이동하도록
		// 해야한다.
		#if defined( I3_PHYSX)
		if( bInit == false)
		{
			setDynamicActiveState( bDynamicActive, true);
		}

		//if( getDynamicActiveState())
		{
			if (m_CurState == 1)
			{
				g_pDebugNode = getSceneObject()->getWorldSpaceSource();
			}

			i3SceneObject* pScnObj = getSceneObject();

			pScnObj->UpdateLastWorldTransform(&oldWorldSpace);

			i3Node* pNewBoundNode = pScnObj->getWorldSpaceSource();

			if (pNewBoundNode)
			{
				i3Node* pInstanceNode = pState->m_pSg->getInstanceSg();
		
				if ( pInstanceNode != nullptr && pInstanceNode->IsFlag(I3_NODEFLAG_ALPHASORT) )
				{
					i3BoundBox* pCurBound = pInstanceNode->GetBound();
			
					if (pCurBound)
					{
						pCurBound->AddRef();
						pInstanceNode->SetBound(nullptr);
						pInstanceNode->RemoveFlag(I3_NODEFLAG_ALPHASORT);	// 
						pNewBoundNode->AddFlag(I3_NODEFLAG_ALPHASORT);

						i3BoundBox* pNewBound = pNewBoundNode->GetBound();
						if (pNewBound == nullptr)
						{
							pNewBoundNode->SetBound(pCurBound);
							pNewBound = pCurBound;
						}
						pCurBound->Release();
					}
				}

				if (pNewBoundNode->IsFlag(I3_NODEFLAG_ALPHASORT))
				{
					pNewBoundNode->GetWrappingBound(pNewBoundNode->GetBound());
				}
			}
						
		}
#endif
	}

	m_OldState = m_CurState;
}

void i3Object::setSceneGraphInfo( INT32 idx, i3SceneGraphInfo * pSg, i3GameResSceneGraph * pRes)
{
	// pSg는 pRes의 Instance Sg가 아니라 Clone이다.

	I3_REF_CHANGE( m_pState[idx].m_pSg, pSg);
	I3_REF_CHANGE( m_pState[idx].m_pRes, pRes);

	if( pSg != nullptr)
	{
		// Sg가 변경될 때, 미리 Bone List 등의 상태를 확인해 보관한다.
		getAttachNode()->RemoveAllChild();
		getSceneObject()->setBoneList( BoneListPtr() );

		//getAttachNode()->AddChild( pSg->getInstanceSg());

		SetInstanceSg( pSg->getInstanceSg());
		getSceneObject()->BuildBoneList();

//		I3_REF_CHANGE( m_pState[idx].m_pBoneList, getSceneObject()->getBoneList());
		m_pState[idx].m_pBoneList = getSceneObject()->getBoneList();
		m_pState[idx].m_pBody = getSceneObject()->GetBody();

		m_OldState = idx;

		// 원래의 상태로 전이

		i3Scene::InitColliderSet( pSg->getInstanceSg(), -1, 0, 0, this );
	}
}

void i3Object::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	INT32 i;
	i3SceneGraphInfo * pSg;
	MATRIX mtx;

	I3ASSERT( pInfo != nullptr);
	I3ASSERT( pInfo->_pMatrixStack != nullptr);

	pInfo->_pMatrixStack->PushPre( &mtx, getSceneObject()->GetMatrix());
	
	for( i = 0; i < m_StateCount; i++)
	{
		pSg = m_pState[i].m_pSg;

		if( pSg != nullptr)
		{
			if( pSg->getCommonSg() != nullptr )
			{	
				pSg->getCommonSg()->OnInstancing( pInfo);
			}
			
			if( pSg->getInstanceSg() != nullptr )
			{
				pSg->getInstanceSg()->OnInstancing( pInfo);
			}
		}
		//사운드 메모리에 미리 올리기위해 업데이트를 한번 돌립니다. 
		//m_pState[i].m_pTimeEvent->OnUpdate( 1.0f);

		for( UINT32 j = 0; j < m_pState[i].m_pTimeEvent->getEventCount(); j++)
		{
			i3TimeEventInfo * pEventInfo = m_pState[i].m_pTimeEvent->getEvent( j);
			if( i3::same_of<i3ActionPlaySound*>(pEventInfo->getAction()))
			{
				i3ActionPlaySound * pActionPlaySound = static_cast<i3ActionPlaySound*>(pEventInfo->getAction());
				i3SoundPlayStateFMOD * pPlayStateFMOD = static_cast<i3SoundPlayStateFMOD*>(pActionPlaySound->getSoundNode()->getSoundPlayState());
				if( pPlayStateFMOD != nullptr)
					pPlayStateFMOD->PreLoadSound();
			}
		}
	}

	pInfo->_pMatrixStack->Pop();

	i3GameObj::OnInstancing( pInfo);
}

INT32 i3Object::_findSg( i3SceneGraphInfo * pSg)
{
	INT32 i;

	for( i = 0; i < m_StateCount; i++)
	{
		if( getSceneGraphInfo( i) == pSg)
			return i;
	}

	return -1;
}

void i3Object::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameObj::CopyTo( pDest, method);

	i3Object * pObj = (i3Object *) pDest;
	INT32 i;

	pObj->SetStateCount( getStateCount());

	pObj->setMaxHP( getMaxHP());
	pObj->setFirstParam( getFirstParam());
	pObj->setSecondParam( getSecondParam());
	pObj->setThirdParam( getThirdParam());

	for( i = 0; i < getStateCount(); i++)
	{
		pObj->setHP( i, getHP( i));

		if( getTimeEvent( i) != nullptr)
		{
			i3TimeEventGen * pTimeEvent = i3TimeEventGen::new_object_ref();
			
			getTimeEvent( i)->CopyTo( pTimeEvent, I3_COPY_INSTANCE);
			pObj->setTimeEvent( i, pTimeEvent);
		}

		if( getSceneGraphInfo( i) != nullptr)
		{
			// 이전 State에서 사용된 적이 있는 Sg.인지를 확인한다.
			i3SceneGraphInfo * pSg;

			INT32 idx = _findSg( getSceneGraphInfo( i));
					
			if (idx == -1)
			{
				I3ASSERT(idx != -1);
				continue;
			}

			if( idx < i)
			{
				// 이전에 사용된 적이 있다.
				pSg = pObj->getSceneGraphInfo( idx);
			}
			else
			{
				pSg = i3SceneGraphInfo::new_object_ref();

				getSceneGraphInfo(i)->CopyTo( pSg, I3_COPY_INSTANCE);
				
			}

			pObj->setSceneGraphInfo( i, pSg, getRes( i));
		}

		I3_REF_CHANGE( 	pObj->m_pState[i].m_pRes, m_pState[i].m_pRes);
	}

	// 로딩시에 업데이트를 돌게 되면 사운드 등의 이벤트가 발생한다.
	//pObj->OnUpdate( 0.0f);
}

void i3Object::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	INT32 i;

	for( i = 0; i < getStateCount(); i++)
	{
		if( getTimeEvent( i) != nullptr)
		{
			getTimeEvent(i)->OnBuildObjectList( List);
		}

		if( getSceneGraphInfo( i) != nullptr)
		{
			getSceneGraphInfo( i)->OnBuildObjectList( List);
		}
	}

	i3GameObj::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 OBJECT_STATE
	{
		REAL32		m_HPRatio = 0.0f;
		OBJREF64	m_pTimeEvent = 0;
		OBJREF64	m_pSg = 0;
		UINT32		pad[4] = { 0 };
	};

	struct ALIGN4 OBJECT
	{
		UINT32		m_ID[4] = { 'G', 'O', 'I', '1' };
		UINT32		m_StateCount = 0;
		REAL32		m_MaxHP = 0.0f;
		UINT32		m_nFirstParam = 0;
		UINT32		m_nSecondParam = 0;
		UINT32		m_nThirdParam = 0;
		UINT32		pad = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Object::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::OBJECT data;
	INT32 i;

	result = i3GameObj::OnSave( pResFile);
	I3_CHKIO( result);

	//i3mem::Copy( data.m_ID, "GOI1", 4);
	data.m_StateCount = getStateCount();
	data.m_MaxHP = getMaxHP();

	data.m_nFirstParam	= (UINT32)getFirstParam();
	data.m_nSecondParam = (UINT32)getSecondParam();
	data.m_nThirdParam	= (UINT32)getThirdParam();

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	for( i = 0; i < getStateCount(); i++)
	{
		pack::OBJECT_STATE state;

		//i3mem::FillZero( &state, sizeof(state));

		state.m_HPRatio		= getHP( i);
		state.m_pTimeEvent	= (OBJREF64) pResFile->GetObjectPersistID( getTimeEvent( i));
		state.m_pSg			= (OBJREF64) pResFile->GetObjectPersistID( getSceneGraphInfo( i));

		rc = pResFile->Write( &state, sizeof(state));
		I3_CHKIO( rc);
		result += rc;
	}

	

	return result;
}

UINT32 i3Object::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::OBJECT data;
	INT32 i;

	result = i3GameObj::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setMaxHP( data.m_MaxHP);
	setFirstParam( (I3_OBJECT_PARAM_FIRST)data.m_nFirstParam);
	setSecondParam( (I3_OBJECT_PARAM_SECOND)data.m_nSecondParam);
	setThirdParam( (I3_OBJECT_PARAM_THIRD)data.m_nThirdParam);

	if( data.m_StateCount > 0)
	{
		SetStateCount( data.m_StateCount);

		for( i = 0; i < (INT32) data.m_StateCount; i++)
		{
			pack::OBJECT_STATE state;

			rc = pResFile->Read( &state, sizeof(state));
			I3_CHKIO( rc);
			result += rc;

			setHP( i, state.m_HPRatio);
			
			if( state.m_pTimeEvent != 0)
			{
				i3TimeEventGen * pTimeEvent = (i3TimeEventGen *) pResFile->FindObjectByID( (OBJREF) state.m_pTimeEvent);
				I3ASSERT( pTimeEvent != nullptr);

				setTimeEvent( i, pTimeEvent);
			}

			if( state.m_pSg != 0)
			{
				i3SceneGraphInfo * pSg = (i3SceneGraphInfo *) pResFile->FindObjectByID( (OBJREF) state.m_pSg);

				if( pSg != nullptr)
				{
														
					i3GameResSceneGraph *	pRes = nullptr;

					if( i3Framework::getCurrentFramework() != nullptr)
					{
						pRes = (i3GameResSceneGraph *) i3Framework::getCurrentFramework()->FindResourceByPath( pSg->GetName());
					}

					setSceneGraphInfo( i, pSg, pRes);

					// pSg의 인스턴스 노드에 대해서 알파블렌딩 여부를 조사하고, 존재하면 플래그를 올리도록 해봄..
					i3Node* pNodeCommon = pSg->getCommonSg();
					i3Node* pNodeInstance = pSg->getInstanceSg();

					i3RenderAttr* pAttr = nullptr;
					if (pNodeCommon)
						pAttr = i3Scene::FindAttrByExactType(pNodeCommon, i3BlendEnableAttr::static_meta());

					if (pAttr == nullptr && pNodeInstance)
						pAttr = i3Scene::FindAttrByExactType(pNodeInstance, i3BlendEnableAttr::static_meta());

					if (pAttr != nullptr)
					{
						if (((i3BlendEnableAttr*)pAttr)->Get())
						{
							pNodeInstance->AddFlag(I3_NODEFLAG_ALPHASORT);
							//	pNodeInstance->AddFlag(I3_NODEFLAG_STATIC);
 
							i3BoundBox* pBound = pNodeInstance->GetBound();
							if (pBound == nullptr)
							{
								pBound = i3BoundBox::new_object_ref();
								pNodeInstance->SetBound(pBound);
							}
							pNodeInstance->GetWrappingBound(pBound);			// 계산으로 직접 bound_box구하기..
						}
					}

				}
				else
				{
					I3TRACE( "i3Object::OnLoad() : Could not found state scene-graph resource.!!!\n");
					return STREAM_ERR;
				}
			}
		}
	}

	return result;
}
