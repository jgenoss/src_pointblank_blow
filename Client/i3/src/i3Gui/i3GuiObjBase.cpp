#include "i3GuiPCH.h"
#include "i3GuiObjBase.h"
#include "i3GuiPopupList.h"
#include "i3GuiButton.h"
#include "i3GuiEditBox.h"


I3_CLASS_INSTANCE(i3GuiObjBase);

i3GuiObjBase::i3GuiObjBase(void)
{
	m_pSceneNode = i3SceneObject::new_object();
	
	i3::fill_n(m_UserDefineStateIndex, (int)MAX_I3GUI_CONTROL_STATE, -1);
	
	ResetEvent();
	SetEnable(true);
}

i3GuiObjBase::~i3GuiObjBase(void)
{
	i3GuiRoot::NoticeGuiObjBaseDestroy(this);

	INT32 i;
	for( i = 0; i< MAX_I3GUI_CONTROL_STATE; ++i)
	{
		if( m_pInfo_EventData[i] != nullptr)
		{
			I3_SAFE_RELEASE( m_pInfo_EventData[i]);
		}
	}

	if( m_pSoundNode != nullptr)
	{
		m_pSoundNode->DeactivateNode();

		m_pSceneNode->RemoveChild( m_pSoundNode);
	}

	I3_SAFE_RELEASE(m_pSceneNode);
	I3_SAFE_RELEASE( m_pSoundNode);
}

void i3GuiObjBase::OnUpdate( REAL32 rDeltaSeconds )
{	
	if(getEvent() != 0)
	{
		_UpdateEvent(rDeltaSeconds);
	}

	i3GameNode::OnUpdate(rDeltaSeconds);
}


void i3GuiObjBase::AddChild(i3GameNode* pObj )
{	
	// Game Graph에 연결
	i3GameNode::AddChild(pObj);

	// Scene Graph에 연결
	GetNode()->AddChild(((i3GuiObjBase*)pObj)->GetNode());

//	m_List.Add(pObj);	
}

void i3GuiObjBase::RemoveChild(i3GameNode* pObj)
{
	// Scene Graph에 연결 해제 및 릴리즈
	GetNode()->RemoveChild(((i3GuiObjBase*)pObj)->GetNode());

	// Game Graph에 연결 해제 및 릴리즈
	i3GameNode::RemoveChild(pObj);


//	m_List.Remove(pObj);
}


void i3GuiObjBase::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{
	if( !i3::same_of<i3GuiWindow*>(this)) return;

	if( m_bInputDisable || !isEnable()) return;

	i3GuiObjBase* pCapturedMouseControl = i3GuiRoot::getCapturedMouseControl();
	if( pCapturedMouseControl != nullptr)
	{
		pCapturedMouseControl->UserInput( fX, fY, state);
		return;
	}

	i3GuiObjBase* pObj = (i3GuiObjBase*)getFirstChild();
	i3GuiObjBase* pDestObj = nullptr;

	while(pObj)
	{
		if( !pObj->getInputDisable() && pObj->getControlState() != I3GUI_CONTROL_STATE_DISABLED && pObj->isEnable())
		{
			REAL32 fParentPosX = pObj->getTotalPosX();
			REAL32 fParentPosY = pObj->getTotalPosY();

			REAL32 fWidth = fParentPosX + (((REAL32)pObj->getWidth()));
			REAL32 fHeight = fParentPosY + (((REAL32)pObj->getHeight()));

			if( (fX >= fParentPosX) && (fX <= fWidth) )
			{
				if( (fY >= fParentPosY) && (fY <= fHeight) )
				{
					{
						// 자기 자신이 Disable되어 있더라도 UserInput을 업데이트 안합니다.
						if(pObj->isEnable() && !pObj->getInputDisable() && pObj->getControlState() != I3GUI_CONTROL_STATE_DISABLED)
						{
							if( pDestObj)
							{
								pDestObj->setControlState( I3GUI_CONTROL_STATE_NORMAL);
							}

							pDestObj = pObj;
						}
					}
				}
				else
				{
					if( pObj)
					{
						pObj->setControlState( I3GUI_CONTROL_STATE_NORMAL);
					}
				} 
			}
			else
			{
				if( pObj)
				{
					pObj->setControlState( I3GUI_CONTROL_STATE_NORMAL);
				}
			}

		}

//		pObj = (i3GuiObjBase*)pObj->getNext();

		if( pObj->getFirstChild() != nullptr && !pObj->getInputDisable() && pObj->isEnable())
		{
			if((i3GuiObjBase*)pObj->getNext() != nullptr)
			{
				m_ObjBackUpList.push_back( (i3GuiObjBase*) pObj->getNext());
			}
			pObj = (i3GuiObjBase*)pObj->getFirstChild();				
		}
		else
		{				
			pObj = (i3GuiObjBase*)pObj->getNext();
			if(pObj == nullptr)
			{
				if( m_ObjBackUpList.size() != 0)
				{					
					pObj = m_ObjBackUpList.back();

					// 이코드 왜있는거삼~!!!! 그래서 주석으로 막았음! (송명일)
//					char szTemp[256] = {0,};
//					if( pObj->hasName())
//					{
//						I3ASSERT( i3::generic_string_size( pObj->GetName()) < 256 );
//						i3::safe_string_copy( szTemp, pObj->GetName());
//					}
					
				//	m_ObjBackUpList.Delete(m_ObjBackUpList.GetCount());
					m_ObjBackUpList.pop_back();
				}
			}	
		}
	}

	if(pDestObj)
	{
		if( pDestObj->getInputMode() == I3GUI_INPUT_MODE_STROKE)
		{
			if( !(state & (I3GUI_MOUSE_STATE_LBUTTON_DOWN | I3GUI_MOUSE_STATE_LBUTTON_DCLICK |
			I3GUI_MOUSE_STATE_RBUTTON_DOWN | I3GUI_MOUSE_STATE_RBUTTON_DCLICK |
			I3GUI_MOUSE_STATE_MBUTTON_DOWN | I3GUI_MOUSE_STATE_MBUTTON_DCLICK ))) state = 0;

			//if( !(state & (I3GUI_MOUSE_STATE_LBUTTON_DOWN | I3GUI_MOUSE_STATE_LBUTTON_DCLICK))) state = 0;
			// else m_OldInputState = state;
			
			// 원본 코드
			//if(m_OldInputState == state) state = 0;
			//else m_OldInputState = state;			
		}
		else
		{
			state = i3GuiRoot::getMouseButtonState();			
		}

		pDestObj->UserInput(fX, fY, state);
	}

/*
	for(i=nCount-1; i>= 0; i--)
	{
		i3GuiObjBase* pObj = (i3GuiObjBase*)m_List.GetItem(i);

		REAL32 fWidth = pObj->getPositionX() + (REAL32)(pObj->getWidth());
		REAL32 fHeight = pObj->getPositionY() + (REAL32)(pObj->getHeight());

		if( (fX >= pObj->getPositionX()) && (fX <= fWidth) )
		{
			if( (fY >= pObj->getPositionY()) && (fY <= fHeight) )
			{
				if(pObj->IsEnable())
				{
					pObj->UserInput(fX, fY, state);
					break;
				}
			}
		}
	}
*/
/*
	i3GameNode * pChild;
	// 모든 Child에 대해서 UserInput을 Recursive하게 전달.	
	for( pChild = m_pChild; pChild != nullptr; pChild = pChild->GetNext() )
	{
		if( pChild->IsEnable() && (! pChild->IsPaused() ) )
		{
			((i3GuiObjBase*)pChild)->UserInput(nX, nY, state);
		}
	}
*/
	
}

void i3GuiObjBase::OnKillFocus(void)
{
	i3GuiRoot::getFocusControl()->SetFocus(false);

	if (m_bExclusiveEnableControl)
	{
		SetEnable(false);
	}
}

INT32 i3GuiObjBase::OnGuiEvent( i3GUI::EVENT event, UINT32 param1, UINT32 param2)
{
	switch( event)
	{
		case i3GUI::EVT_LEAVEMOUSE :
			/*
			if( isEnable())
				setControlState(I3GUI_CONTROL_STATE_NORMAL);
				*/
			break;
	}

	return 0;
}

void i3GuiObjBase::RemoveAllChild()
{
	i3GameNode::RemoveAllChild();
}

void i3GuiObjBase::SetControlResizeWidth( REAL32 fWidth)
{
	m_nWidth = (INT32)(m_nWidth * fWidth);
}

void i3GuiObjBase::SetControlResizeHeight( REAL32 fHeight)
{
	m_nHeight = (INT32)(m_nHeight * fHeight);
}

REAL32 i3GuiObjBase::getTotalPosX()
{
	REAL32 fPosX = 0.0f;
	i3GameNode * pNode = (i3GameNode *)this;

	while( pNode)
	{
		if( i3::kind_of<i3GuiObjBase*>(pNode))
		{
			fPosX += ((i3GuiObjBase*)pNode)->getPositionX();
		}
		pNode = pNode->getParent();
	}

//	if( i3GuiRoot::getGuiResolutionWidth() > 0.0f)
//	{
//		return fPosX * i3GuiRoot::getGuiResolutionWidth();
//	}

	return (fPosX * i3GuiRoot::getGuiResolutionWidth());
}

REAL32 i3GuiObjBase::getTotalPosY()
{
	REAL32 fPosY = 0.0f;
	i3GameNode * pNode = (i3GameNode *)this;

	while( pNode)
	{
		if( i3::kind_of<i3GuiObjBase*>(pNode))
		{
			fPosY += ((i3GuiObjBase*)pNode)->getPositionY();
		}
		pNode = pNode->getParent();
	}

//	if( i3GuiRoot::getGuiResolutionHeight() > 0.0f)
//	{
//		return fPosY * i3GuiRoot::getGuiResolutionHeight();
//	}

	return (fPosY * i3GuiRoot::getGuiResolutionHeight());
}

bool i3GuiObjBase::IsOnMouse(REAL32 fx,REAL32 fy)
{
	REAL32 fOrigX = getTotalPosX();
	REAL32 fOrigY = getTotalPosY();
    REAL32 fWidth = fOrigX + (((REAL32)m_nWidth));
	REAL32 fHeight = fOrigY + (((REAL32)m_nHeight));

	if( fx >= fOrigX && fx <= fWidth )
	{
		if( fy >= fOrigY && fy <= fHeight )
		{
			return true;
		}
	}

	return false;
}

void i3GuiObjBase::_Rec_FindOnMouse( INT32 depth, I3GUI_OVERLAP_INFO * pInfo)
{
	if( IsOnMouse( pInfo->x, pInfo->y) == false)
		return;

	if( (pInfo->m_pFind == nullptr) || (pInfo->m_FindDepth < depth))
	{
		pInfo->m_pFind = this;
		pInfo->m_FindDepth = depth;
	}

	i3GuiObjBase * pTemp = (i3GuiObjBase *) getFirstChild();

	while( pTemp != nullptr)
	{
		pTemp->_Rec_FindOnMouse( depth + 1, pInfo);

		pTemp = (i3GuiObjBase *) pTemp->getNext();
	}
}

i3GuiObjBase *	i3GuiObjBase::GetOnMouse( REAL32 x, REAL32 y, bool bRecursive)
{
	I3GUI_OVERLAP_INFO info;

	info.m_FindDepth = -1;
	info.m_pFind = nullptr;
	info.x = x;
	info.y = y;

	_Rec_FindOnMouse( 0, &info);

	return info.m_pFind;
}

void i3GuiObjBase::setControlState(GUI_CONTROL_STATE state)	
{ 	
	if( m_ControlState == state)
		return;

	// strock 버튼 용 - QB 일반 버튼
	if (I3GUI_CONTROL_STATE_CLICKED == m_OldControlState)
	{
		m_bSoundSkip = true;
	}

	m_OldControlState = m_ControlState;
	m_ControlState = state;

	// press 버튼 용 - PB 일반 버튼
	if (I3GUI_CONTROL_STATE_CLICKED == m_OldControlState)
	{
		m_bSoundSkip = true;
	}

	switch(m_ControlState)
	{
	case I3GUI_CONTROL_STATE_DRAG:
	case I3GUI_CONTROL_STATE_CLICKED:
	case I3GUI_CONTROL_STATE_R_CLICKED:
		{	
			if (getParent() && !i3::same_of<i3GuiPopupList*>(getParent()))
			{
				if (false == IsComboButton())
				{
					SetFocus(true);
				}
			}
		}
		break;
	case I3GUI_CONTROL_STATE_ONENABLE:
		{
		}
		break;
	case I3GUI_CONTROL_STATE_ONDISABLE:
		{
		}
		break;
	default:
		break;
	}

	_OnSetControlState( state);
}

void i3GuiObjBase::SetFocus( bool bTrue)	
{
	if( m_bIsFocus == bTrue) return;

	m_bIsFocus = bTrue;	

// 1회성 코드로 반드시 수정되어야 합니다. (송명일) ㅎㅎ 죄송!
#if defined( I3G_DX)
	if( m_bIsFocus)
	{
		if( i3GuiRoot::getFocusControl() != nullptr && 
			i3GuiRoot::getFocusControl() != this)
		{
			i3GuiObjBase* pObjBase= i3GuiRoot::getFocusControl();
			pObjBase->OnKillFocus();
		}

		if( i3GuiRoot::getFocusControl() != this)
		{		
			i3GuiRoot::setFocusControl( this);

			if (m_bExclusiveEnableControl)
			{
				SetEnable(true);
			}
		}		
	}
	else
	{
		if (m_bExclusiveEnableControl)
		{
			if (this == i3GuiRoot::getFocusControl())
			{
				i3GuiRoot::getFocusControl()->OnKillFocus();
				i3GuiRoot::setFocusControl(nullptr);
			}

			SetEnable(false);
		}
	}

#else
	if( m_bIsFocus)
	{
		if( i3GuiRoot::getFocusControl() != nullptr && 
			i3GuiRoot::getFocusControl() != this)
		{
			setControlState(I3GUI_CONTROL_STATE_NORMAL);

			if (false == IsComboButton())
			{
				i3GuiRoot::getFocusControl()->OnKillFocus();
			}
		}

		if( i3GuiRoot::getFocusControl() != this)
		{
			setControlState(I3GUI_CONTROL_STATE_FOCUS);	
			i3GuiRoot::setFocusControl( this);

			if (m_bExclusiveEnableControl)
			{
				SetEnable(true);
			}
		}
		else
		{
			setControlState(I3GUI_CONTROL_STATE_FOCUS);	
		}
	}
	else
	{
		setControlState(I3GUI_CONTROL_STATE_NORMAL);
		
		if (m_bExclusiveEnableControl)
		{
			if (this == i3GuiRoot::getFocusControl())
			{
				i3GuiRoot::setFocusControl(nullptr);
			}

			SetEnable(false);
		}
	}
#endif
}

void i3GuiObjBase::setControlDisable( bool bTrue)
{ 
	m_bDisable = bTrue; 

	if( m_bDisable)
	{		
		setControlState(I3GUI_CONTROL_STATE_DISABLED);
	}
	else
	{
		if( getControlState() == I3GUI_CONTROL_STATE_DISABLED)
		{
			setControlState(I3GUI_CONTROL_STATE_NORMAL);
		}
	}
}

void i3GuiObjBase::SetUserDefine( GUI_CONTROL_STATE state, INT32 index)
{	
	m_UserDefineStateIndex[ (INT32)state] = index;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Event관련함수 모음입니다. (시작)
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void i3GuiObjBase::SetEventData( GUI_CONTROL_STATE state, I3GUI_EVENT_DATA * pData)
{
	I3_REF_CHANGE( m_pInfo_EventData[state], pData);
}

void i3GuiObjBase::_OnSetControlState( GUI_CONTROL_STATE nState)
{
	//ResetEvent();

	I3GUI_EVENT_DATA *	pAction		= m_pInfo_EventData[nState];
	UINT32				nEventStyle	= 0;

	if( pAction != nullptr)
	{
		nEventStyle = pAction->m_nEventStyle;
	}

	if( nEventStyle & I3GUI_EVENT_MOVE)
	{
		I3GUI_EVENT_MOVE_DATA * p = m_pInfo_EventData[nState]->m_pMoveData;

		I3ASSERT( p != nullptr);

		MoveTo(	p->m_fDestPosX, p->m_fDestPosY, p->m_fCountTime, p->m_nFactorType);
	}
	
	if( nEventStyle & I3GUI_EVENT_ROTATE)
	{
		I3GUI_EVENT_ROTATE_DATA * p = m_pInfo_EventData[nState]->m_pRotateData;

		I3ASSERT( p != nullptr);

		RotateTo( p->m_nSpinDirection, p->m_bLoopRotate ? true : false, p->m_fCountTime, p->m_nFactorType);
	}

	if( nEventStyle & I3GUI_EVENT_SCALE)
	{
		I3GUI_EVENT_SCALE_DATA * p = m_pInfo_EventData[nState]->m_pScaleData;

		I3ASSERT( p != nullptr);

		ScaleTo( p->m_fDestScaleX, p->m_fDestScaleY, p->m_bLoopScale ? true : false, p->m_fCountTime, p->m_nFactorType);
	}

	if (false == m_bSoundSkip)
	{
		if( nEventStyle & I3GUI_EVENT_SOUND)
		{
			if( m_pSceneNode) 
			{
				i3SoundNode * pNode = m_pInfo_EventData[nState]->m_pSoundNode;

				I3ASSERT( pNode != nullptr);

				PlaySound( pNode);
			}
		}
	}

	m_bSoundSkip = false;
}

void i3GuiObjBase::_UpdateEvent( REAL32 rDeltaSeconds )
{
	UINT32 state = getEvent();
	
	// 복수개의 Event가 동시적으로 발생할수 있다고 가정합니다. (그렇기 때문에 else if로 묶지 않습니다)
	{
		if(state & I3GUI_EVENT_MOVE)
		{
			if(_ProcessMoveTo(rDeltaSeconds))
			{
				removeEvent(I3GUI_EVENT_MOVE);
			}
		}
		if(state & I3GUI_EVENT_ROTATE)
		{
			if(_ProcessRotateTo(rDeltaSeconds))
			{
				removeEvent(I3GUI_EVENT_ROTATE);
			}
		}
		if(state & I3GUI_EVENT_SCALE)
		{
			if(_ProcessScaleTo(rDeltaSeconds))
			{
				removeEvent(I3GUI_EVENT_SCALE);
			}
		}
		if( state & I3GUI_EVENT_SOUND)
		{
			if( !getControlDisable() && !getInputDisable())
			{
				if(_ProcessSound( rDeltaSeconds))
				{
					removeEvent( I3GUI_EVENT_SOUND);
				}
			}
		}
	}
}

void i3GuiObjBase::ResetEvent()
{
	m_nEventFlag = 0;

	m_fEventScaleX = 1.0f;
	m_fEventScaleY = 1.0f;

	m_fRotation = 0.0f;

	m_Event_Move.Init();
	m_Event_Rotate.Init();
	m_Event_Scale.Init();

	if( m_pSoundNode != nullptr)
	{
		m_pSoundNode->OnDeactivate();
		m_pSoundNode->RemoveFromParent();

		I3_SAFE_RELEASE( m_pSoundNode);
	}
}

void i3GuiObjBase::MoveTo(REAL32 fX, REAL32 fY, REAL32 fTime, INT32 nFactorType)
{
	addEvent(I3GUI_EVENT_MOVE);
	m_Event_Move.m_fDestPosX = fX;
	m_Event_Move.m_fDestPosY = fY;
	m_Event_Move.m_fCountTime = fTime;
	m_Event_Move.m_nFactorType = nFactorType;

	// 초기 설정시 한번만 실행됩니다.
	{
		REAL32 fGapX = m_fStartX - m_Event_Move.m_fDestPosX;
		REAL32 fGapY = m_fStartY - m_Event_Move.m_fDestPosY;
		fGapX = i3Math::abs(fGapX);
		fGapY = i3Math::abs(fGapY);
	
		switch(m_Event_Move.m_nFactorType)
		{
		case I3GUI_EVENT_FACTOR_TYPE_NORMAL:
			{
				m_Event_Move.m_fSpeedX = i3Math::div(fGapX, m_Event_Move.m_fCountTime);
				m_Event_Move.m_fSpeedY = i3Math::div(fGapY, m_Event_Move.m_fCountTime);		
			}
			break;
		case I3GUI_EVENT_FACTOR_TYPE_ACCEL:
			{
			}
		default:
			break;
		}		
	}
}

void i3GuiObjBase::RotateTo(INT32 Direction, bool bLoop, REAL32 fTime, INT32 nFactorType)
{
	addEvent(I3GUI_EVENT_ROTATE);
	m_Event_Rotate.m_nSpinDirection = Direction;
	m_Event_Rotate.m_fCountTime = fTime;
	m_Event_Rotate.m_nFactorType = nFactorType;
	m_Event_Rotate.m_bLoopRotate = bLoop;

	// 초기 설정시 한번만 실행됩니다.
	{
		REAL32 fGap = m_fRotation - I3_2PI;
		fGap = i3Math::abs(fGap);

		switch(m_Event_Rotate.m_nFactorType)
		{
		case I3GUI_EVENT_FACTOR_TYPE_NORMAL:
			{
				m_Event_Rotate.m_fSpeed = i3Math::div(fGap, m_Event_Rotate.m_fCountTime);
			}
			break;
		case I3GUI_EVENT_FACTOR_TYPE_ACCEL:
			{
			}
		default:
			break;
		}		
	}
}

void i3GuiObjBase::ScaleTo(REAL32 sX, REAL32 sY, bool bLoop, REAL32 fTime, INT32 nFactorType)
{
	if( isEvent( I3GUI_EVENT_SCALE))
	{
		return;
	}

	addEvent( I3GUI_EVENT_SCALE);
	m_Event_Scale.m_fDestScaleX = sX;
	m_Event_Scale.m_fDestScaleY = sY;
	m_Event_Scale.m_fCountTime = fTime;
	m_Event_Scale.m_nFactorType = nFactorType; 
	m_Event_Scale.m_bLoopScale = bLoop;
	
	m_Event_Scale.m_fOldScaleX = m_fEventScaleX;
	m_Event_Scale.m_fOldScaleY = m_fEventScaleY;

	m_nScaleDir = I3GUI_SCALE_DIR_NONE;

	// 초기 설정시 한번만 실행됩니다.
	{
		REAL32 fGapX = m_fEventScaleX - m_Event_Scale.m_fDestScaleX;
		REAL32 fGapY = m_fEventScaleY - m_Event_Scale.m_fDestScaleY;
		fGapX = i3Math::abs(fGapX);
		fGapY = i3Math::abs(fGapY);
	
		switch(m_Event_Scale.m_nFactorType)
		{
		case I3GUI_EVENT_FACTOR_TYPE_NORMAL:
			{
				m_Event_Scale.m_fSpeedScaleX = i3Math::div(fGapX, m_Event_Scale.m_fCountTime);
				m_Event_Scale.m_fSpeedScaleY = i3Math::div(fGapY, m_Event_Scale.m_fCountTime);		
			}
			break;
		case I3GUI_EVENT_FACTOR_TYPE_ACCEL:
			{
			}
		default:
			break;
		}		
	}
}

void i3GuiObjBase::ScaleToRight(REAL32 scale,  bool bLoop,REAL32 fTime, INT32 nFactorType)
{
	addEvent(I3GUI_EVENT_SCALE);
	m_Event_Scale.m_fDestScaleX = scale;
	m_Event_Scale.m_fDestScaleY = 1.0f;
	m_Event_Scale.m_fCountTime = fTime;
	m_Event_Scale.m_nFactorType = nFactorType; 
	m_Event_Scale.m_bLoopScale = bLoop;
	
	m_Event_Scale.m_fOldScaleX = m_fEventScaleX;
	m_Event_Scale.m_fOldScaleY = m_fEventScaleY;

	m_nScaleDir = I3GUI_SCALE_DIR_RIGHT;

	// 초기 설정시 한번만 실행됩니다.
	{
		REAL32 fGapX = m_fEventScaleX - m_Event_Scale.m_fDestScaleX;
		REAL32 fGapY = m_fEventScaleY - m_Event_Scale.m_fDestScaleY;
		fGapX = i3Math::abs(fGapX);
		fGapY = i3Math::abs(fGapY);
	
		switch(m_Event_Scale.m_nFactorType)
		{
		case I3GUI_EVENT_FACTOR_TYPE_NORMAL:
			{
				m_Event_Scale.m_fSpeedScaleX = i3Math::div(fGapX, m_Event_Scale.m_fCountTime);
				m_Event_Scale.m_fSpeedScaleY = i3Math::div(fGapY, m_Event_Scale.m_fCountTime);		
			}
			break;
		case I3GUI_EVENT_FACTOR_TYPE_ACCEL:
			{
			}
		default:
			break;
		}		
	}
}

void i3GuiObjBase::ScaleToLeft(REAL32 scale, bool bLoop, REAL32 fTime, INT32 nFactorType)
{
	addEvent(I3GUI_EVENT_SCALE);
	m_Event_Scale.m_fDestScaleX = scale;
	m_Event_Scale.m_fDestScaleY = 1.0f;
	m_Event_Scale.m_fCountTime = fTime;
	m_Event_Scale.m_nFactorType = nFactorType; 
	m_Event_Scale.m_bLoopScale = bLoop;
	
	m_Event_Scale.m_fOldScaleX = m_fEventScaleX;
	m_Event_Scale.m_fOldScaleY = m_fEventScaleY;

	m_nScaleDir = I3GUI_SCALE_DIR_LEFT;

	// 초기 설정시 한번만 실행됩니다.
	{
		REAL32 fGapX = m_fEventScaleX - m_Event_Scale.m_fDestScaleX;
		REAL32 fGapY = m_fEventScaleY - m_Event_Scale.m_fDestScaleY;
		fGapX = i3Math::abs(fGapX);
		fGapY = i3Math::abs(fGapY);
	
		switch(m_Event_Scale.m_nFactorType)
		{
		case I3GUI_EVENT_FACTOR_TYPE_NORMAL:
			{
				m_Event_Scale.m_fSpeedScaleX = i3Math::div(fGapX, m_Event_Scale.m_fCountTime);
				m_Event_Scale.m_fSpeedScaleY = i3Math::div(fGapY, m_Event_Scale.m_fCountTime);		
			}
			break;
		case I3GUI_EVENT_FACTOR_TYPE_ACCEL:
			{
			}
		default:
			break;
		}		
	}
}

void i3GuiObjBase::ScaleToUp(REAL32 scale, bool bLoop, REAL32 fTime, INT32 nFactorType)
{
	addEvent(I3GUI_EVENT_SCALE);
	m_Event_Scale.m_fDestScaleX = 1.0f;
	m_Event_Scale.m_fDestScaleY = scale;
	m_Event_Scale.m_fCountTime = fTime;
	m_Event_Scale.m_nFactorType = nFactorType; 
	m_Event_Scale.m_bLoopScale = bLoop;
	
	m_Event_Scale.m_fOldScaleX = m_fEventScaleX;
	m_Event_Scale.m_fOldScaleY = m_fEventScaleY;

	m_nScaleDir = I3GUI_SCALE_DIR_UP;

	// 초기 설정시 한번만 실행됩니다.
	{
		REAL32 fGapX = m_fEventScaleX - m_Event_Scale.m_fDestScaleX;
		REAL32 fGapY = m_fEventScaleY - m_Event_Scale.m_fDestScaleY;
		fGapX = i3Math::abs(fGapX);
		fGapY = i3Math::abs(fGapY);
	
		switch(m_Event_Scale.m_nFactorType)
		{
		case I3GUI_EVENT_FACTOR_TYPE_NORMAL:
			{
				m_Event_Scale.m_fSpeedScaleX = i3Math::div(fGapX, m_Event_Scale.m_fCountTime);
				m_Event_Scale.m_fSpeedScaleY = i3Math::div(fGapY, m_Event_Scale.m_fCountTime);		
			}
			break;
		case I3GUI_EVENT_FACTOR_TYPE_ACCEL:
			{
			}
		default:
			break;
		}		
	}
}

void i3GuiObjBase::ScaleToDown(REAL32 scale, bool bLoop, REAL32 fTime, INT32 nFactorType)
{	
	addEvent(I3GUI_EVENT_SCALE);
	m_Event_Scale.m_fDestScaleX = 1.0f;
	m_Event_Scale.m_fDestScaleY = scale;
	m_Event_Scale.m_fCountTime = fTime;
	m_Event_Scale.m_nFactorType = nFactorType; 
	m_Event_Scale.m_bLoopScale = bLoop;
	
	m_Event_Scale.m_fOldScaleX = m_fEventScaleX;
	m_Event_Scale.m_fOldScaleY = m_fEventScaleY;

	m_nScaleDir = I3GUI_SCALE_DIR_DOWN;

	// 초기 설정시 한번만 실행됩니다.
	{
		REAL32 fGapX = m_fEventScaleX - m_Event_Scale.m_fDestScaleX;
		REAL32 fGapY = m_fEventScaleY - m_Event_Scale.m_fDestScaleY;
		fGapX = i3Math::abs(fGapX);
		fGapY = i3Math::abs(fGapY);
	
		switch(m_Event_Scale.m_nFactorType)
		{
		case I3GUI_EVENT_FACTOR_TYPE_NORMAL:
			{
				m_Event_Scale.m_fSpeedScaleX = i3Math::div(fGapX, m_Event_Scale.m_fCountTime);
				m_Event_Scale.m_fSpeedScaleY = i3Math::div(fGapY, m_Event_Scale.m_fCountTime);		
			}
			break;
		case I3GUI_EVENT_FACTOR_TYPE_ACCEL:
			{
			}
		default:
			break;
		}		
	}
}

void i3GuiObjBase::PlaySound(i3SoundNode * pNode)
{
	I3ASSERT( pNode != nullptr);

	addEvent(I3GUI_EVENT_SOUND);

	if( pNode == m_pSoundNode)
	{
		m_pSoundNode->OnChangeTime( 0.0f);
	}
	else
	{
		if( m_pSoundNode != nullptr)
		{
			m_pSoundNode->DeactivateNode();

			m_pSceneNode->RemoveChild( m_pSoundNode);
		}

		I3_REF_CHANGE( m_pSoundNode, pNode);

		if( m_pSoundNode != nullptr)
		{
			m_pSoundNode->ActivateNode();

			m_pSceneNode->AddChild( m_pSoundNode);
		}
	}
}

bool i3GuiObjBase::_ProcessMoveTo( REAL32 rDeltaSeconds )
{
	REAL32 fScrollX = 0.0f;
	REAL32 fScrollY = 0.0f;

	switch(m_Event_Move.m_nFactorType)
	{
	case I3GUI_EVENT_FACTOR_TYPE_NORMAL:
		{
			fScrollX = i3Math::Mul(rDeltaSeconds, m_Event_Move.m_fSpeedX);
			fScrollY = i3Math::Mul(rDeltaSeconds, m_Event_Move.m_fSpeedY);			
		}
		break;
	case I3GUI_EVENT_FACTOR_TYPE_ACCEL:
		{
		}
	default:
		break;
	}

	// X축으로의 이동
	if(m_fStartX > m_Event_Move.m_fDestPosX)
	{				
		m_fStartX -= fScrollX;
		if(m_fStartX <= m_Event_Move.m_fDestPosX)
		{
			m_fStartX = m_Event_Move.m_fDestPosX;
		}
	}
	else if(m_fStartX < m_Event_Move.m_fDestPosX)
	{
		m_fStartX += fScrollX;
		if(m_fStartX >= m_Event_Move.m_fDestPosX)
		{
			m_fStartX = m_Event_Move.m_fDestPosX;
		} 
	}

	// Y축으로의 이동
	if(m_fStartY > m_Event_Move.m_fDestPosY)
	{
		m_fStartY -= fScrollY;
		if(m_fStartY <= m_Event_Move.m_fDestPosY)
		{
			m_fStartY = m_Event_Move.m_fDestPosY;
		}
	}
	else if(m_fStartY < m_Event_Move.m_fDestPosY)
	{
		m_fStartY += fScrollY;
		if(m_fStartY >= m_Event_Move.m_fDestPosY)
		{
			m_fStartY = m_Event_Move.m_fDestPosY;
		}
	}

	// 목표지점 도착에대한 검사
	if( (m_fStartX == m_Event_Move.m_fDestPosX) && (m_fStartY == m_Event_Move.m_fDestPosY) )
	{
		return true;
	}

	return false;
}

bool i3GuiObjBase::_ProcessRotateTo( REAL32 rDeltaSeconds )
{
	REAL32 fRotate = 0.0f;

	switch(m_Event_Rotate.m_nFactorType)
	{
	case I3GUI_EVENT_FACTOR_TYPE_NORMAL:
		{
			fRotate = i3Math::Mul(rDeltaSeconds, m_Event_Rotate.m_fSpeed);
		}
		break;
	case I3GUI_EVENT_FACTOR_TYPE_ACCEL:
		{
		}
	default:
		break;
	}

	if(m_Event_Rotate.m_nSpinDirection == I3GUI_SPIN_CW)
	{
		m_fRotation -= fRotate;
	}
	else if(m_Event_Rotate.m_nSpinDirection == I3GUI_SPIN_CCW)
	{
		m_fRotation += fRotate;
	}

	
	if(!m_Event_Rotate.m_bLoopRotate)
	{				
		if( i3Math::abs(m_fRotation) >= (I3_2PI))
		{
			m_fRotation = 0.0f;
			return true;
		}
	}
	
	return false;
}

bool i3GuiObjBase::_ProcessScaleTo( REAL32 rDeltaSeconds )
{
	REAL32 fScaleX = 0.0f;
	REAL32 fScaleY = 0.0f;

	switch(m_Event_Scale.m_nFactorType)
	{
	case I3GUI_EVENT_FACTOR_TYPE_NORMAL:
		{
			fScaleX = i3Math::Mul(rDeltaSeconds, m_Event_Scale.m_fSpeedScaleX);
			fScaleY = i3Math::Mul(rDeltaSeconds, m_Event_Scale.m_fSpeedScaleY);			
		}
		break;
	case I3GUI_EVENT_FACTOR_TYPE_ACCEL:
		{
		}
	default:
		break;
	}

	// X축으로의 Scale
	if(m_fEventScaleX > m_Event_Scale.m_fDestScaleX)
	{				
		m_fEventScaleX -= fScaleX;
		if(m_fEventScaleX <= m_Event_Scale.m_fDestScaleX)
		{
			m_fEventScaleX = m_Event_Scale.m_fDestScaleX;
		}
	}
	else if(m_fEventScaleX < m_Event_Scale.m_fDestScaleX)
	{
		m_fEventScaleX += fScaleX;
		if(m_fEventScaleX >= m_Event_Scale.m_fDestScaleX)
		{
			m_fEventScaleX = m_Event_Scale.m_fDestScaleX;
		}
	}

	// Y축으로의 Scale
	if(m_fEventScaleY > m_Event_Scale.m_fDestScaleY)
	{
		m_fEventScaleY -= fScaleY;
		if(m_fEventScaleY <= m_Event_Scale.m_fDestScaleY)
		{
			m_fEventScaleY = m_Event_Scale.m_fDestScaleY;
		}
	}
	else if(m_fEventScaleY < m_Event_Scale.m_fDestScaleY)
	{
		m_fEventScaleY += fScaleY;
		if(m_fEventScaleY >= m_Event_Scale.m_fDestScaleY)
		{
			m_fEventScaleY = m_Event_Scale.m_fDestScaleY;
		}
	}

	// 목표Scale에 대한 조사
	if( (m_fEventScaleX == m_Event_Scale.m_fDestScaleX) && (m_fEventScaleY == m_Event_Scale.m_fDestScaleY) )
	{
		if(m_Event_Scale.m_bLoopScale)
		{
			removeEvent( I3GUI_EVENT_SCALE);

			ScaleTo(m_Event_Scale.m_fOldScaleX,
					m_Event_Scale.m_fOldScaleY,
					m_Event_Scale.m_bLoopScale ? true : false,
					m_Event_Scale.m_fCountTime,
					m_Event_Scale.m_nFactorType);
		}
		else
		{
			return true;
		}
	}

	return false;
}

bool i3GuiObjBase::_ProcessSound( REAL32 rDeltaSeconds)
{
	if( m_pSoundNode == nullptr)	return true;

	if( m_pSoundNode->GetFlag() & I3_NODEFLAG_DISABLE)
	{
		m_pSoundNode->RemoveFromParent();

		I3_SAFE_RELEASE( m_pSoundNode);

		return true;
	}
	else
		return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Event관련함수 모음입니다. (끝)
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

void i3GuiObjBase::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method )
{
	i3GameObjBase::CopyTo( pDest, method );

	if( !i3::kind_of<i3GuiObjBase*>(pDest) )
		return;

	i3GuiObjBase * pObjBase = (i3GuiObjBase*)pDest;

	pObjBase->setPosition( m_fStartX, m_fStartY);
	pObjBase->setSize( m_nWidth, m_nHeight );

	if( hasName())
	{
		pObjBase->SetName( GetNameString());
	}
}

void	i3GuiObjBase::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	INT32 i;
	for( i = 0; i< MAX_I3GUI_CONTROL_STATE; ++i)
	{
		if( m_pInfo_EventData[i] != 0)
		{
			m_pInfo_EventData[i]->OnBuildObjectList( List);
		}
	}

	i3GameObjBase::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUIOBJBASE
	{
		REAL32			m_fStartX = 0.0f;
		REAL32			m_fStartY = 0.0f;
		INT32			m_nWidth = 0;
		INT32			m_nHeight = 0;

		OBJREF64		m_EventData[MAX_I3GUI_CONTROL_STATE] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32	i3GuiObjBase::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	
	Result = i3GameObjBase::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	pack::GUIOBJBASE data;
	data.m_fStartX	= m_fStartX;
	data.m_fStartY	= m_fStartY	;
	data.m_nWidth	= m_nWidth;
	data.m_nHeight	= m_nHeight;

	for(INT32 i = 0; i< MAX_I3GUI_CONTROL_STATE; ++i)
	{
		data.m_EventData[i] = (OBJREF64)pResFile->GetObjectPersistID( m_pInfo_EventData[i]);
	}
	
	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3GameObjBase::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32	i3GuiObjBase::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Rc,Result = 0;
	pack::GUIOBJBASE data;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3GameObjBase::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}
	
	Rc = pStream->Read( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3GuiObjBase::OnLoad()", "Could not load data.");
		return STREAM_ERR;
	}
	Result += Rc;

	//GameData
	m_fStartX = data.m_fStartX;
	m_fStartY = data.m_fStartY;
	m_nWidth  = (INT32)(data.m_nWidth * i3GuiRoot::getGuiResolutionWidth());
	m_nHeight = (INT32)(data.m_nHeight * i3GuiRoot::getGuiResolutionHeight());
	m_nOriginalWidth = data.m_nWidth;
	m_nOriginalHeight = data.m_nHeight;

	INT32 i;
	for( i = 0; i< MAX_I3GUI_CONTROL_STATE; ++i)
	{
		if( (data.m_EventData[i] != 0) && (data.m_EventData[i] != 0xFFFFFFFF))
		{
			SetEventData( (GUI_CONTROL_STATE)i, ( I3GUI_EVENT_DATA*)pResFile->FindObjectByID( (OBJREF) data.m_EventData[i]));
		}
	}

	return Result;
}

bool i3GuiObjBase::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3GameObjBase::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	pXML->addAttr("StartX", m_fStartX);
	pXML->addAttr("startY", m_fStartY);
	pXML->addAttr("Width",	m_nWidth);
	pXML->addAttr("Height", m_nHeight);
	pXML->addAttr("OriginalWidth", m_nOriginalWidth);
	pXML->addAttr("OriginalHeight", m_nOriginalHeight);

	pXML->addAttr("ControlState", m_ControlState);

	INT32 i, iVal;
	char conv[MAX_PATH];
	for( i = 0;i < MAX_I3GUI_CONTROL_STATE; ++i)
	{
		I3GUI_EVENT_DATA * pData = m_pInfo_EventData[i];

		iVal = 0;
		sprintf(conv, "EventData_%d", i);

		if( pData != nullptr)
		{
			iVal = (INT32)pFile->GetObjectPersistID( pData);
		}

		pXML->addAttr( conv, iVal);
	}

	return true;
}

bool i3GuiObjBase::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3GameObjBase::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return bRv;
	INT32 state;

	pXML->getAttr("StartX", &m_fStartX);
	pXML->getAttr("StartX", &m_fStartY);
	pXML->getAttr("Width", &m_nWidth);
	pXML->getAttr("Height", &m_nHeight);
	pXML->getAttr("OriginalWidth", &m_nOriginalWidth);
	pXML->getAttr("OriginalHeight", &m_nOriginalHeight);

	pXML->getAttr("ControlState", &state);
	m_ControlState = (GUI_CONTROL_STATE)state;

	INT32 i, iVal;
	char conv[MAX_PATH];
	for( i = 0;i < MAX_I3GUI_CONTROL_STATE;++i)
	{
		iVal = 0;
		sprintf(conv, "EventData_%d", &i);

		pXML->getAttr( conv, &iVal);
		if( iVal > 0)
		{
			I3GUI_EVENT_DATA * pData = (I3GUI_EVENT_DATA*)pFile->FindObjectByID((OBJREF)iVal);
			I3ASSERT( pData);

			SetEventData( (GUI_CONTROL_STATE)i, pData);
		}
	}

	return true;
}

void i3GuiObjBase::SetEnable( bool bTrue)
{
	if( bTrue )	
	{
		if( !this->isEnable())
		{
			//
			setControlState( I3GUI_CONTROL_STATE_ONENABLE);					
		}

		m_pSceneNode->RemoveFlag(I3_NODEFLAG_DISABLE);
	}
	else
	{
		//
		if( this->isEnable())
		{
			m_pSceneNode->OnChangeTime( 0.0f);
			setControlState( I3GUI_CONTROL_STATE_ONDISABLE);		
			RemoveAllChildEvents();
		}

		m_pSceneNode->AddFlag(I3_NODEFLAG_DISABLE);
	}

	m_LastEvent = 0;

	i3GameNode::SetEnable( bTrue);
}

void i3GuiObjBase::RemoveAllChildEvents( void)
{
	i3GameNode * pChild = m_pChild;

	ResetEvent();

	while( pChild != nullptr)
	{
		if( i3::kind_of<i3GuiObjBase*>(pChild))
		{
			((i3GuiObjBase*)pChild)->RemoveAllChildEvents();
		}

		pChild = pChild->getNext();
	}
}

void i3GuiObjBase::SetControlID(INT32 nControlID)
{
	m_nControlID = nControlID;	// non-zero
}

void i3GuiObjBase::PushControlNotify(INT32 nEvent,INT32 nPriParam,INT32 nSecParam)
{
	// 마지막 인자에 현재 Control의 포인터를 넘깁니다.(2008년 10월 10일-송명일)
	g_pGuiRoot->PushControlNotify(m_nControlID, nEvent, nPriParam, nSecParam, this);
}

bool i3GuiObjBase::IsComboButton(void)
{
	// 버튼과 팝업이 같은 부모로 에디트박스를 가지고 있다면, 컴보박스의 용도이다.
	// 이때 버튼이 눌려도 팝업의 활성화에 영향을 주어서는 안된다.
	if (nullptr == i3GuiRoot::getFocusControl()) return false;

	// CGameButton 주의
	if (i3::kind_of< i3GuiButton* >(this))
	{
		if (i3::same_of<i3GuiEditBox*>(getParent()))
		{
			if (i3::same_of<i3GuiPopupList*>(i3GuiRoot::getFocusControl()))
			{
				if (getParent() == i3GuiRoot::getFocusControl()->getParent())
				{
					return true;
				}
			}
		}
	}

	return false;
}
