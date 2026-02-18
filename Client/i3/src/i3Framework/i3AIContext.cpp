#include "i3FrameworkPCH.h"
#include "i3AIContext.h"
#include "i3Chara.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( i3AIContext);

i3AIContext::~i3AIContext(void)
{
	// m_pChara는 AddRef 하지 않는다.
	I3_SAFE_RELEASE( m_pAI);
}

void i3AIContext::SetAI( i3AI * pAI)
{
	I3_REF_CHANGE( m_pAI, pAI);
}

void i3AIContext::SetOwnerToAllChildren( i3Chara * pChara)
{
	setOwner( pChara);

	i3GameNode * pChild = getFirstChild();

	while( pChild != nullptr)
	{
		if( i3::same_of<i3AIContext* >(pChild))
		{
			i3AIContext * pCtx = (i3AIContext *) pChild;

			pCtx->SetOwnerToAllChildren( pChara);
		}

		pChild = pChild->getNext();
	}
}

void i3AIContext::OnChangeTime( REAL32 tm)
{
	i3GameNode::OnChangeTime( tm);

	m_timeLocal = tm;
	m_timeLocalMulScale = tm;

	if( m_pCurAIState != nullptr)
	{
		if( tm == 0.0f)
		{
			Reset( false);
		}

		m_pCurAIState->OnChangeTime( this, tm);
	}
}

void i3AIContext::RemoveAIStateAll( void)
{
	m_pAI->RemoveAIStateAll();
	m_pInitialAIState = nullptr;
	m_pCurAIState = nullptr;
	m_NextAIStateIndex = 0;
}

void i3AIContext::RemoveAIStateFromId( INT32 id)
{
	INT32 idx = FindAIStateByID( id);

	if( idx == -1)
		return;

	RemoveAIStateFromIndex( idx);
}

void i3AIContext::RemoveAIStateFromIndex( INT32 idx)
{
	i3AIState * pAIState;

	pAIState = getAIState( idx);

	if( m_pInitialAIState == pAIState)
		m_pInitialAIState = nullptr;

	if( m_pCurAIState == pAIState)
	{
		m_pCurAIState = nullptr;
		m_CurAIStateIndex = -1;
	}

	if( m_NextAIStateIndex == idx)
		m_NextAIStateIndex = -1;

	m_pAI->RemoveAIStateFromIndex( idx);
}

void i3AIContext::RemoveAIState( i3AIState * pAIState)
{
	INT32 idx = m_pAI->FindAIState( pAIState);
	
	if( idx == -1)
		return;

	RemoveAIStateFromIndex( idx);
}

void i3AIContext::Reset( bool bFirst)
{
	bool bRepeat;

	m_timeLocalMulScale = m_timeLocal = m_timeStart;

	if( m_pCurAIState != nullptr)
	{
		bRepeat = (m_pCurAIState->getStyle() & (I3_AISTATE_STYLE_FIN_TIME | I3_AISTATE_STYLE_FIN_ANIM)) == 0;

		if( m_pCurAIState->getAnim() == nullptr)
		{
			// Load가 안된 경우
			if( i3::generic_string_size( m_pCurAIState->getAnimNameString()) > 0)
			{
				i3AnimationResManager * pManager = (i3AnimationResManager *) i3NamedResourceManager::FindByMeta( i3AnimationResManager::static_meta());
				i3Animation * pAnim = (i3Animation*) pManager->GetResource(nullptr, nullptr, m_pCurAIState->getAnimName());
				I3ASSERT( pAnim != nullptr);
				m_pCurAIState->SetAnim( pAnim);
			}
		}

		// Animation 설정
		if( m_pCurAIState->getAnim() != nullptr)
		{
			//REAL32 fDuration = 0.0f;	// C4189
			I3_ANIM_BLEND_MODE	mode;
			I3_ANIM_PLAY_MODE destPlay;

			if( (bFirst == false) && (m_pCurAIState->getBlendTime() > 0.0f) )
				mode = I3_ANIM_BLEND_MODE_BLEND;
			else
				mode = I3_ANIM_BLEND_MODE_REPLACE;

			if( m_pCurAIState->getStyle() & I3_AISTATE_STYLE_ANIM_LOOP)
				destPlay = I3_ANIM_PLAY_MODE_REPEAT;
			else
				destPlay = I3_ANIM_PLAY_MODE_CLAMP;

			m_pChara->PlayAnimation( (i3Animation *) m_pCurAIState->getAnim(), mode, m_timeStart, m_pCurAIState->getBlendTime(), m_pCurAIState->getTimeScale(), 1.0f, destPlay);

			m_pCurAIState->setDuration( m_pCurAIState->getAnim()->GetDuration());
		}
	}
}

void	i3AIContext::SetEnable( bool bTrue, REAL32 tm)
{
	i3GameNode::SetEnable( bTrue, tm);
}

////////////////////////////////////////////////////////////////////////////
void i3AIContext::SetExclusiveEnable( bool bEnable, bool bExclusive)
{
	_ForceEnable( bEnable);

	if( bEnable && bExclusive)
	{
		// 자신 이외의 Sibling AI들을 모두 Disable 시킨다.
		i3GameNode * pParent = getParent();
		i3GameNode * pChild = pParent->getFirstChild();

		I3ASSERT( pChild != nullptr);

		while( pChild != nullptr)
		{
			if( i3::same_of<i3AIContext*>(pChild)
				&& (pChild != this))
			{
				pChild->SetEnable(false);
			}

			pChild = pChild->getNext();
		}
	}

	if( !bEnable)
	{
		StopAI( 0.0f);
	}
}

bool	i3AIContext::StopAI( REAL32 tm)
{
	// 다른 AI State로 Transition해야 한다.
	if( m_pCurAIState != nullptr)
	{
		if( m_pAI->getAIDriver() != nullptr)
		{
			I3_AISTATE_PROC pProc = m_pAI->getAIDriver()->getFinishAIProc( m_CurAIStateIndex);

			if( pProc != nullptr)
				pProc( this, tm);
		}

		if( m_pCurAIState != nullptr)
		{
			m_pCurAIState->OnFinishState( this, tm);

			// TimeSeq node를 제거한다.
			if( m_pCurAIState->getTimeSeq() != nullptr)
			{
				m_pCurAIState->getTimeSeq()->OnChangeTime( 0.0f);
				m_pChara->getAttachNode()->RemoveChild( m_pCurAIState->getTimeSeq());
			}
		}
	}

	ResetCurrentAI();

	return true;
}

void i3AIContext::ResetCurrentAI( void)
{
	m_PrevAIStateIndex	= m_CurAIStateIndex;
	m_CurAIStateIndex	= -1;
	m_NextAIStateIndex	= -1;
	m_pCurAIState		= nullptr;
}

bool i3AIContext::SetAIStateImmediately( REAL32 tm)
{
	if( m_NextAIStateIndex != -1)
	{
		// 다른 AI State로 Transition해야 한다.
		if( m_pCurAIState != nullptr)
		{
			if( m_pAI->getAIDriver() != nullptr)
			{
				I3_AISTATE_PROC pProc = m_pAI->getAIDriver()->getFinishAIProc( m_CurAIStateIndex);

				if( pProc != nullptr)
					pProc( this, tm);
			}

			m_pCurAIState->OnFinishState( this, tm);

			// TimeSeq node를 제거한다.
			if( m_pCurAIState->getTimeSeq() != nullptr)
			{
				m_pCurAIState->getTimeSeq()->OnChangeTime( 0.0f);
				m_pChara->getAttachNode()->RemoveChild( m_pCurAIState->getTimeSeq());
			}
		}

		m_PrevAIStateIndex = m_CurAIStateIndex;
		m_CurAIStateIndex = m_NextAIStateIndex;
		m_NextAIStateIndex = -1;
		m_pCurAIState = getAIState( m_CurAIStateIndex);
		
		if( ( m_pCurAIState != nullptr) && (m_pCurAIState->getTimeSeq() != nullptr) )
		{
			m_pChara->getAttachNode()->AddChild( m_pCurAIState->getTimeSeq());
			m_pCurAIState->getTimeSeq()->OnChangeTime( m_timeStart);
		}

		m_pChara->OnChangeAIState( this);

		// OnChangeTime( 0.f);
		// 애니메이션을 직접 설정하는 경우 초기화시 replace되도록 설정해야 한다.
		i3GameNode::OnChangeTime( 0.f);

		m_timeLocal = 0.f;
		m_timeLocalMulScale = 0.f;

		if( m_pCurAIState != nullptr)
		{
			Reset( true);

			m_pCurAIState->OnChangeTime( this, 0.f);
		}
	}

	return true;
}

void i3AIContext::setCurrentAIStateDirect( INT32 idx, REAL32 timeStart, REAL32 timeScale)
{
	if((m_pCurAIState != nullptr) && isGNodeState( I3_GAMENODE_STATE_LOCKED))
		return;

	i3AIState * pAIState = getAIState( idx);
	I3ASSERT_RETURN( pAIState != nullptr);

	pAIState->setTimeScale( timeScale);

	m_NextAIStateIndex = idx;
	m_timeStart = timeStart;

	OnUpdate( 0.0f);
}

void i3AIContext::OnUpdate( REAL32 tm)
{
	bool bDoRun;

	if( m_NextAIStateIndex != -1)
	{
		// 다른 AI State로 Transition해야 한다.
		if( m_pCurAIState != nullptr)
		{
			if( m_pAI->getAIDriver() != nullptr)
			{
				I3_AISTATE_PROC pProc = m_pAI->getAIDriver()->getFinishAIProc( m_CurAIStateIndex);

				if( pProc != nullptr)
					pProc( this, tm);
			}

			m_pCurAIState->OnFinishState( this, tm);

			// TimeSeq node를 제거한다.
			if( m_pCurAIState->getTimeSeq() != nullptr)
			{
				m_pCurAIState->getTimeSeq()->OnChangeTime( 0.0f);
				m_pChara->getAttachNode()->RemoveChild( m_pCurAIState->getTimeSeq());
			}
		}

		m_PrevAIStateIndex = m_CurAIStateIndex;
		m_CurAIStateIndex = m_NextAIStateIndex;
		m_NextAIStateIndex = -1;
		m_pCurAIState = getAIState( m_CurAIStateIndex);

		if( (m_pCurAIState != nullptr) && (m_pCurAIState->getTimeSeq() != nullptr) )
		{
			m_pChara->getAttachNode()->AddChild( m_pCurAIState->getTimeSeq());
			m_pCurAIState->getTimeSeq()->OnChangeTime( m_timeStart);
		}

		m_pChara->OnChangeAIState( this);

		OnChangeTime( 0.0f);
	}

	if( m_pCurAIState != nullptr)
	{
		bDoRun = true;

		if(m_pCurAIState->getStyle() & I3_AISTATE_STYLE_FIN_TIME)
		{
			// 시간 조건을 갖고 있는 AIState라면...
			if( getElapsedTime() > m_pCurAIState->getDuration())
			{
				if( m_pCurAIState->getNext() != nullptr)
					setCurrentAIState( m_pCurAIState->getNext());
				else
				{
					if( m_pAI->getAIDriver() != nullptr)
					{
						I3_AISTATE_PROC pProc = m_pAI->getAIDriver()->getFinishAIProc( m_CurAIStateIndex);

						if( pProc != nullptr)
							pProc( this, tm);
					}

					m_pCurAIState->OnFinishState( this, tm);

					// TimeSeq node를 제거한다.
					if( m_pCurAIState->getTimeSeq() != nullptr)
					{
						m_pCurAIState->getTimeSeq()->OnChangeTime( 0.0f);
						m_pChara->getAttachNode()->RemoveChild( m_pCurAIState->getTimeSeq());
					}

					m_pCurAIState = nullptr;
					m_CurAIStateIndex = -1;
					bDoRun = false;
				}
			}
		}

		if( m_pCurAIState != nullptr && m_pCurAIState->getStyle() & I3_AISTATE_STYLE_FIN_ANIM)
		{
			if( getElapsedTime() > m_pCurAIState->getAnimDuration())
			{
				if( m_pCurAIState->getNext() != nullptr)
					setCurrentAIState( m_pCurAIState->getNext());
				else
				{
					if( m_pAI->getAIDriver() != nullptr)
					{
						I3_AISTATE_PROC pProc = m_pAI->getAIDriver()->getFinishAIProc( m_CurAIStateIndex);
						
						if( pProc != nullptr)
							pProc( this, tm);
						else
						{
							char tmp[256] = "";
							i3::snprintf( tmp, 256, "알수없는 AI State를 호출하려고 합니다.\ni3Chara: %s\nCalling AI index: %d\nTotal AI Count: %d\n[해결방안]\ni3CharaEditor에서 Generate C Source로 생성된 AIDriver 파일과\n클라이언트의 사용하는 AIDriver 파일의 AI state가 일치하는지 확인하세요.\n문의: 백성민, 정순구", 
								m_pAI->GetNameString(), m_CurAIStateIndex, m_pAI->getAIDriver()->getAIProcCount());

							I3PRINTLOG(I3LOG_FATAL,  tmp);
						}
					}

					m_pCurAIState->OnFinishState( this, tm);

					// TimeSeq node를 제거한다.
					if( m_pCurAIState->getTimeSeq() != nullptr)
					{
						m_pCurAIState->getTimeSeq()->OnChangeTime( 0.0f);
						m_pChara->getAttachNode()->RemoveChild( m_pCurAIState->getTimeSeq());
					}

					m_pCurAIState = nullptr;
					m_CurAIStateIndex = -1;
					bDoRun = false;
				}
			}
		}

		// 주기적으로 반복되는 Style이 있어야할듯...
		if( bDoRun &&  m_pCurAIState != nullptr)
		{
			if( m_pAI->getAIDriver() != nullptr)
			{
				I3_AISTATE_PROC pProc = m_pAI->getAIDriver()->getRunAIProc( m_CurAIStateIndex);

				if( pProc != nullptr)
					pProc( this, tm);
				else
				{
					char tmp[512] = "";
					i3::snprintf( tmp, 256, "알수없는 AI State를 호출하려고 합니다.\ni3Chara: %s\nCalling AI index: %d\nTotal AI Count: %d\n[해결방안]\ni3CharaEditor에서 Generate C Source로 생성된 AIDriver 파일과\n클라이언트의 사용하는 AIDriver 파일의 AI state가 일치하는지 확인하세요.\n문의: 백성민, 정순구\n", 
								m_pAI->GetNameString(), m_CurAIStateIndex, m_pAI->getAIDriver()->getAIProcCount());

					I3TRACE( tmp);
				}
			}
			m_pCurAIState->OnActiveState( this, tm);

			if( m_pCurAIState->getStyle() & I3_AISTATE_STYLE_ANIM_LOOP)
			{
				if( m_timeLocal >= m_pCurAIState->getDuration())
				{
					m_timeLocal = i3Math::fmod( m_timeLocal, m_pCurAIState->getDuration());

					//I3TRACE( "LOOP : %f\n", m_timeLocal);

					m_pCurAIState->OnChangeTime( this, m_timeLocal);
				}
			}
		}
	}
	else
	{
		setCurrentAIState( m_pInitialAIState);
	}

	m_timeLocal += tm;

	if(m_pCurAIState != nullptr) {
		m_timeLocalMulScale += (tm * m_pCurAIState->getTimeScale());
	}

	i3GameNode::OnUpdate( tm);

}

void	i3AIContext::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameNode::CopyTo( pDest, method);

	i3AIContext * pCtx = (i3AIContext *) pDest;

	//pCtx->setGNodeState( getGNodeState());
	pCtx->setElapsedTime( getElapsedTime());

	if( method == I3_COPY_REF)
	{
		pCtx->SetAI( getAI());
		pCtx->setInitialAIState( getInitialAIState());
		pCtx->setCurrentAIState( getCurrentAIState());
	}
	else
	{
		i3AI * pNewAI;

		if( pCtx->getAI() == nullptr)
		{
			pNewAI = i3AI::new_object_ref();
			getAI()->CopyTo( pNewAI, method);
			pCtx->SetAI( pNewAI);
		}
		else
		{
			pNewAI = pCtx->getAI();
			getAI()->CopyTo( pNewAI, method);
		}

		if( getInitialAIState() != nullptr)
		{
			INT32 idx = pNewAI->FindAIStateByName( getInitialAIState()->GetName());
			I3ASSERT( idx != -1);

			pCtx->setInitialAIState( pNewAI->getAIState( idx));
		}

		if( getCurrentAIState() != nullptr)
		{
			pCtx->setCurrentAIState( getCurrentAIState()->GetName());
		}
	}
}

void	i3AIContext::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{	
	if( m_pAI != nullptr)
		m_pAI->OnBuildObjectList( List);

	i3GameNode::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 AI_CONTEXT
	{
		UINT8		m_ID[4] = { 'A', 'I', 'C', '1' };
		UINT32		m_State = 0;
		OBJREF64	m_pAI = 0;
		OBJREF64	m_pInitialAIState = 0;
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3AIContext::OnSave( i3ResourceFile * pResFile)
{
	pack::AI_CONTEXT data;
	UINT32 Rc, Result;

	Result = i3GameNode::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_State = getGNodeState();
	data.m_pAI = (OBJREF64) pResFile->GetObjectPersistID( getAI());
	data.m_pInitialAIState = (OBJREF64) pResFile->GetObjectPersistID( getInitialAIState());

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3AIContext::OnLoad( i3ResourceFile * pResFile)
{
	pack::AI_CONTEXT data;
	UINT32 Rc, Result = 0;

	Result = i3GameNode::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	setGNodeState( data.m_State);
	
	if( data.m_pAI != 0)
	{
		i3AI * pAI = (i3AI *) pResFile->FindObjectByID( (OBJREF) data.m_pAI);
		I3ASSERT( pAI != nullptr);

		SetAI( pAI);
	}

	if( data.m_pInitialAIState != 0)
	{
		i3AIState * pState = (i3AIState *) pResFile->FindObjectByID( (OBJREF) data.m_pInitialAIState);
		I3ASSERT( pState != nullptr);

		setInitialAIState( pState);
	}

	return Result;
}

bool i3AIContext::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3GameNode::OnSaveXML(pFile, pXML);
	if (bRv == false)
		return false;

	pXML->addAttr("GNodeState", getGNodeState());
	pXML->addAttr("AIID", (OBJREF64)pFile->GetObjectPersistID(getAI()));
	pXML->addAttr("InitialAIStateID", (OBJREF64)pFile->GetObjectPersistID(getInitialAIState()));

	return true;
}

bool i3AIContext::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3GameNode::OnLoadXML(pFile, pXML);
	if (bRv == false)
		return false;

	UINT32 state;
	pXML->getAttr("GNodeState", &state);
	setGNodeState(state);
	
	OBJREF64 temp;
	pXML->getAttr("AIID", &temp);
	if (temp != 0)
	{
		i3AI * pAI = (i3AI *)pFile->FindObjectByID((OBJREF)temp);
		I3ASSERT(pAI != nullptr);
		SetAI(pAI);
	}
	
	pXML->getAttr("InitialAIStateID", &temp);
	if (temp != 0)
	{
		i3AIState * pState = (i3AIState *)pFile->FindObjectByID((OBJREF)temp);
		I3ASSERT(pState != nullptr);
		setInitialAIState(pState);
	}

	return true;
}