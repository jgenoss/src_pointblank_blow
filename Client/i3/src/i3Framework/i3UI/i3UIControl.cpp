#include "i3FrameworkPCH.h"
#include "i3Framework.h"
#include "i3UIControl.h"
#include "i3UIManager.h"
#include "i3UITemplate.h"
#include "i3UIbutton.h"
#include "i3UIRenderer.h"
#include "i3UIScene.h"
#include "i3UI.h"

#include "i3UIControl_Dummy.h"

#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3UIAnimation2DSprite.h"
#include "i3Base/string/compare/generic_compare.h"
//#define i3_profile_enable
#include "i3Base/profile/profile.h"

#include <set>

I3_CLASS_INSTANCE(i3UIControl);

REAL32 i3UIControl::s_fTtDelayTime = 0.7f;		//그래서 정적 멤버 변수는 외부에서 별도로 선언및 초기화해야 한다. 
REAL32 i3UIControl::s_fTtTimer = 0.0f;
POINT i3UIControl::s_CursorPos = {0,0};

void i3UIControl::_recModifyStyle( i3UIControl * pControl, I3UI_STYLE addStyle, I3UI_STYLE removeStyle)
{
	pControl->addStyle( addStyle);
	pControl->removeStyle( removeStyle);

	i3UIControl * pChild = (i3UIControl*)pControl->getFirstChild();
	while( pChild != nullptr)
	{
		_recModifyStyle( pChild, addStyle, removeStyle);

		pChild = (i3UIControl*)pChild->getNext();
	}
}

void i3UIControl::_recsetModifyState( i3UIControl * pControl, bool bModify)
{
	i3_prof_func();
	pControl->setModifyState( bModify);

	i3UIControl * pChild = (i3UIControl*)pControl->getFirstChild();
	while( pChild != nullptr)
	{
		_recsetModifyState( pChild, bModify);

		pChild = (i3UIControl*)pChild->getNext();
	}
}

void i3UIControl::_recEnableCtrl( i3UIControl * pControl, bool bEnable)
{
	pControl->EnableCtrl( bEnable);

	i3UIControl * pChild = (i3UIControl*)pControl->getFirstChild();
	while( pChild != nullptr)
	{
		if( pChild->isStyle( I3UI_STYLE_SLAVE) == false)
			_recEnableCtrl( pChild, bEnable);

		pChild = (i3UIControl*)pChild->getNext();
	}
}

i3UIControl::i3UIControl(void)
{
	addState( I3UI_STATE_NEED_UPDATE);
	
	if (i3UI::getToolDevMode())
	{
		i3UIManager * pMng = i3UI::getManager();
		pMng->RegisterControlForTool(this);
	}
	m_bScissors = false;
	InitScissorsRect();
}

i3UIControl::~i3UIControl(void)
{
	if (i3UI::getToolDevMode())
	{
		i3UIManager * pMng = i3UI::getManager();
		pMng->UnregisterControlForTool(this);
	}
	
	I3_SAFE_RELEASE( m_pTemplate);
	I3_SAFE_RELEASE( m_pReceiver);

	_DestroySprites();

	//
	i3UI::releaseControl( this);

	I3MEM_SAFE_FREE( m_pTtMsg);

	if (m_pSpriteAnimation)
	{
		for (INT32 i = 0; i < m_nShapeCount; i++)
		{
			I3_SAFE_RELEASE(m_pSpriteAnimation[i]._pSprite);
		}
		I3_SAFE_DELETE_ARRAY(m_pSpriteAnimation);
	}
}

void i3UIControl::_DestroySprites()
{
	resetSprites( true);
	I3MEM_SAFE_FREE( m_pRects);
}

void i3UIControl::setScript( const char * pszScript)
{
	m_strScript.clear();

	if( pszScript == nullptr)
		return;

	INT32 length = i3::generic_string_size( pszScript);
	if( length > 0)
	{
		m_strScript = pszScript;
	}
}

void i3UIControl::SetEventHandler( const char * szEventHandler)
{
	m_strEventHandler.clear();

	INT32 length = i3::generic_string_size( szEventHandler);
	if( length > 0)
	{
		m_strEventHandler = szEventHandler;
	}
}

void i3UIControl::SetTemplate(i3UITemplate* pShape)		// Template과 Control 연결..
{
	I3ASSERT(pShape != nullptr);

	I3_REF_CHANGE( m_pTemplate, pShape);

	CreateBaseSceneObjects();

	OnBindTemplate();

	AdjustTemplate();
}

void i3UIControl::AdjustTemplate( void)
{
	if( m_pTemplate != nullptr)
		m_pTemplate->AdjustLayout( m_Size.x, m_Size.y, m_pRects, m_nShapeCount);

	if( i3UI::getToolDevMode())
		AdjustSpriteOrder();

	_recsetModifyState( this, true);
}

INT32 i3UIControl::getEndSpriteHandle(void)
{
	if( m_nShapeCount == 0 || m_hSprites == nullptr)	return -1;

	return m_hSprites[m_nShapeCount - 1];
}

void i3UIControl::SetTemplateByName( const char* pszTemplateName)
{
	i3UILibrary* lib = i3UI::getUILibrary();

	if( lib == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "Library가 존재하지 않습니다.");
		return;
	}

	i3UITemplate* pNewTemplate = lib->FindByName( pszTemplateName);

	if( pNewTemplate == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "%s(이)라는 이름을 가진 템플릿이 없으므로 기존의 템플릿을 유지합니다.", pszTemplateName);
		return;
	}

	SetTemplate( pNewTemplate);
}

void i3UIControl::_AdjustOwnSpriteOrder(void)
{
	if( g_pUIRenderer == nullptr)	return;
	if( m_hSprites == nullptr)	return;

	INT32 idxEnd = -1;

	i3GameNode * pParent = getParent();
	if( pParent != nullptr)
	{
		i3UIControl * pCtrl = (i3UIControl*)pParent->getFirstChild();
		if( pCtrl == this)
		{
		}
		else
		{
			while( pCtrl != nullptr && pCtrl->getNext() != this)
			{
				pCtrl = (i3UIControl*)pCtrl->getNext();
			}

			I3ASSERT_RETURN( pCtrl != nullptr && pCtrl->getNext() == this);

			idxEnd = pCtrl->getEndSpriteHandle();
		}

		if( idxEnd == -1)
		{
			if( i3::kind_of< i3UIControl* >(pParent))
			{
				idxEnd = ((i3UIControl *)pParent)->getEndSpriteHandle();
			}
		}
	}

	if( idxEnd != -1)
	{
		g_pUIRenderer->AdjustOrder( idxEnd, m_hSprites, m_nShapeCount);
	}
}

void i3UIControl::prepareSprites(void)
{
	if( m_pTemplate == nullptr)
	{
		resetSprites( true);
		return;
	}

	if( m_hSprites != nullptr)
	{
		if( m_nShapeCount != m_pTemplate->getShapeCount())
		{
			resetSprites( true);
		}
		else if( !m_bResetSprites)
			return;
	}

	m_nShapeCount = m_pTemplate->getShapeCount();

	INT32 i;

	if( m_nShapeCount > 0)
	{
		if( m_hSprites == nullptr)
		{
			m_hSprites = (INT32*)i3MemAlloc( m_nShapeCount * sizeof( INT32));
			i3mem::Fill( m_hSprites, -1, m_nShapeCount * sizeof( INT32));
		}

		for( i = 0;i < m_nShapeCount; ++i)
		{
			/*if( m_hSprites[i] != -1)
				g_pUIRenderer->FreeSprite( m_hSprites[i]);*/

			if( m_hSprites[i] == -1)
			{
				m_hSprites[i] = g_pUIRenderer->AllocSprite(
					(isStyle(I3UI_STYLE_BLOCK_AUTO_TRASH_TEX)) ? I3UI_SPRITE_STYLE_BLOCK_AUTO_TRASH_TEX : I3UI_SPRITE_STYLE_INSTANCE, this);

				I3ASSERT( m_hSprites[i] >= 0);
			}
		}
	}

	m_bResetSprites = false;
}

void i3UIControl::resetSprites( bool bRelease)
{
	INT32 i;
	if( m_hSprites != nullptr && g_pUIRenderer != nullptr)
	{
		for( i = 0;i < m_nShapeCount; ++i)
		{
			if( m_hSprites[i] != -1)
			{
				g_pUIRenderer->FreeSprite( m_hSprites[i]);
				m_hSprites[i] = -1;
			}
		}
	}

	if( bRelease )
	{
		I3MEM_SAFE_FREE( m_hSprites);
	}

	m_bResetSprites = true;
}

bool i3UIControl::CreateBaseSceneObjects()
{
	if( g_pUIRenderer == nullptr)
		return false;

	prepareSprites();

	if( m_nShapeCount > 0)
	{
		I3MEM_SAFE_FREE( m_pRects);
		m_pRects = (i3::pack::RECT*)i3MemAlloc( m_nShapeCount * sizeof( i3::pack::RECT));
	}

	return true;
}

// Align을 고려한 좌표 계산 (Offset은 Parent Window의 Position값)
void i3UIControl::CalcAlignedPos( const i3::pack::RECT * pRect, VEC3D * pResult)
{
	UINT32 alignX = getStyle() & I3UI_STYLE_ALIGN_HMASK;
	switch( alignX)
	{
	case I3UI_STYLE_ALIGN_LEFT:		i3Vector::SetX( pResult, m_AccPos.x + pRect->left);							break;
	case I3UI_STYLE_ALIGN_RIGHT:	i3Vector::SetX( pResult, m_AccPos.x - getWidth() + pRect->left);			break;
	case I3UI_STYLE_ALIGN_MIDDLE:	i3Vector::SetX( pResult, m_AccPos.x - (getWidth() * 0.5f) + pRect->left);	break; 
	default:
		{
			I3ASSERT_0;
			i3Vector::SetX( pResult, m_AccPos.x + pRect->left);
		}
		break;
	}

	UINT32 alignY = getStyle() & I3UI_STYLE_ALIGN_VMASK;
	switch( alignY)
	{
	case I3UI_STYLE_ALIGN_TOP:		i3Vector::SetY( pResult, m_AccPos.y + pRect->top);							break;
	case I3UI_STYLE_ALIGN_BOTTOM:	i3Vector::SetY( pResult, m_AccPos.y - getHeight() + pRect->top);			break;
	case I3UI_STYLE_ALIGN_CENTER:	i3Vector::SetY( pResult, m_AccPos.y - (getHeight() * 0.5f) + pRect->top);	break;
	default:
		{
			i3Vector::SetY( pResult, m_AccPos.y + pRect->left);

			I3ASSERT_0;
		}
		break;
	}

	pResult->z = m_Pos.z;
}

void i3UIControl::OnUpdate( REAL32 rDeltaSeconds)
{	
	OnSpriteAnimation(rDeltaSeconds);

	if( isState( I3UI_STATE_NEED_UPDATE) )
	{
		_UpdateShapeState();

		UpdateRenderObjects();

		removeState( I3UI_STATE_NEED_UPDATE);
	}

	//ToolTip
	//Timer를 가지고 있는 컨트롤은 timer를 증가시키거나 감소시킬수 있다. 
	if( m_bHasTtTimer && !isState( I3UI_STATE_DEACTIVATED))
	{
		if( m_pTtMsg != nullptr || isEvtEnable( I3UI_EVT_TOOLTIP ) )
		{
			if( s_fTtTimer < s_fTtDelayTime)
			{
				s_fTtTimer = min( s_fTtDelayTime, s_fTtTimer + rDeltaSeconds);
			}
			else
			{
				Notify( I3UI_EVT_TOOLTIP, 1, 0);

				if( m_pTtMsg != nullptr && m_pAncestor != nullptr)	//when s_fTtTimer reaches s_fTtDelayTime
				{
					i3UIToolTip* pToolTip = m_pAncestor->getDefToolTip();
					
					if( pToolTip != nullptr && !pToolTip->isEnable())
					{
						pToolTip->EnableCtrl( true, true);
						pToolTip->SetText( m_pTtMsg);
						pToolTip->ChangePos( s_CursorPos);
					}
				}
			}
		}
	}

	i3GameNode::OnUpdate( rDeltaSeconds);
}

bool i3UIControl::OnLostDevice( bool bLostDevice)
{
	if( i3GameNode::OnLostDevice( bLostDevice) == false)
		return false;

	resetSprites( false);

	return true;
}

bool i3UIControl::OnRevive( i3RenderContext * pCtx)
{
	if( i3GameNode::OnRevive( pCtx) == false)
		return false;

	if( isEnable() || isStyle( I3UI_STYLE_AUTOALIGN) )
	{
		prepareSprites();
		_UpdateAlignedPos();
		return true;
	}

	return false;
}

void i3UIControl::OnRecreate( void)
{
	if( isEnable() || isStyle( I3UI_STYLE_AUTOALIGN) ) 
	{
		prepareSprites();
		_UpdateAlignedPos();
	}
}

void i3UIControl::UpdateRenderObjects() 
{
	INT32 i = 0;
	if( m_pTemplate == nullptr || g_pUIRenderer == nullptr)
		return;

	if( isEnable() == false || isVisible() == false)
		return;

	if( m_hSprites == nullptr || m_hSprites[0] == -1)
	{
		return;
	}

	{
		VEC3D	Position;
 		for(i=0; i< m_nShapeCount; i++)
		{
			// 이미지 설정
			i3UIShape * pShape	= getShape(i);
			i3::pack::RECT	*	pRect	= &m_pRects[i];

			//I3TRACE("%d pRect: %f, %f, %f, %f\n", i, pRect->left, pRect->right, pRect->top, pRect->bottom);

			i3::pack::RECT rt;
			rt.Normalize();
			
			if( g_pUIRenderer->getEnableSprite( m_hSprites[i]))
			{
				// Texture의  UV값을 설정합니다.
				REAL32 offX2, offY2;
				REAL32 baseX, baseY, r;

				r = 1.0f / I3VT_VTEX_SIZE;

				if (m_bAnimationState&&m_pSpriteAnimation[i]._pSprite)
				{
					baseX = (REAL32)m_pSpriteAnimation[i]._pSprite->GetVTexOffset().x;
					baseY = (REAL32)m_pSpriteAnimation[i]._pSprite->GetVTexOffset().y;
				}
				else
				{
					baseX = (REAL32)pShape->getTexCoordOffset()->x;
					baseY = (REAL32)pShape->getTexCoordOffset()->y;
				}

				if( !m_bUseCustomTexCoord)
				{
					if (m_bAnimationState&&m_pSpriteAnimation[i]._pSprite)
					{
						rt = m_pSpriteAnimation[i]._pSprite->GetTexCood();
					}
					else
					{
						rt = *pShape->getTextureCoord();
					}

					REAL32 u1 = baseX + rt.left;
					REAL32 v1 = baseY + rt.top;
					REAL32 u2 = u1 + rt.right;
					REAL32 v2 = v1 + rt.bottom;

					// 어서트 대신 메세지 박스를 띄움
					// Cancel시에 값 0으로 강제 재설정...
					//I3ASSERT( pShape->getCoordU() > -1);
					//I3ASSERT( pShape->getCoordV() > -1);
					if (!m_bAnimationState)
					{
						if (pShape->getCoordU() < 0 || pShape->getCoordV() < 0)
						{
							char szTemp[1024];
							char szPath[MAX_PATH] = { 0, };
							if (pShape->hasTexture())
								i3::safe_string_copy(szPath, pShape->getTextureResPath(), MAX_PATH);

							sprintf(szTemp, "## %s: U:%.4f V:%.4f \n## Tex UVCoord Setting Error \n## if you are Click Cancel Button, Set Adjust Zero", szPath, pShape->getCoordU(), pShape->getCoordV());
							int nFunc = ::MessageBox(NULL, szTemp, "Error", MB_RETRYCANCEL);

							if (nFunc == IDCANCEL)
							{
								if (pShape->getCoordU() < 0)
									pShape->setCoordU(0);
								if (pShape->getCoordV() < 0)
									pShape->setCoordV(0);
							}
						}
					}

					g_pUIRenderer->setTexCoord( m_hSprites[i], u1 * r, v1 * r, u2 * r, v2 * r);
				}

				if (m_bAnimationState&&m_pSpriteAnimation[i]._pSprite)
				{
					offX2 = m_pSpriteAnimation[i]._pSprite->GetVTexOffset().x + m_pSpriteAnimation[i]._pSprite->GetTexCood().right;
					offY2 = m_pSpriteAnimation[i]._pSprite->GetVTexOffset().y + m_pSpriteAnimation[i]._pSprite->GetTexCood().bottom;
				}
				else
				{
					offX2 = pShape->getTexCoordOffset()->x + pShape->getCoordW();
					offY2 = pShape->getTexCoordOffset()->y + pShape->getCoordH();
				}
				
				g_pUIRenderer->setTexRange( m_hSprites[i], baseX * r, baseY * r, offX2 * r, offY2 * r);
				//g_pUIRenderer->setTexRange( m_hSprites[i], baseX, baseY, offX2, offY2);

				g_pUIRenderer->setColor( m_hSprites[i], &m_Color);

				CalcAlignedPos( pRect, &Position);

				bool bClip = false;

				i3Rect rtIntersect;
				i3Rect rtLetter;
				if (m_rtScissors.right > 0 && m_rtScissors.bottom > 0)
				{
					rtLetter.set((INT32)Position.x, (INT32)Position.y,
						(INT32)Position.x + (INT32)pRect->right - 1, (INT32)Position.y + (INT32)pRect->bottom - 1);

					//rect영역을 벗어난것은 무조건 hide처리
					if (m_rtScissors.left > m_rtScissors.right || m_rtScissors.top > m_rtScissors.bottom)
					{
						setShapeVisible(i, false);
						continue;
					}

					TOUCH_TYPE touch = m_rtScissors.Intersect(&rtLetter, &rtIntersect);

					if (touch == TOUCH_NONE)
					{
						//설정된 구역을 이탈한 Shape은 출력하지 않는다
						setShapeVisible(i, false);
						continue;
					}
					else if (touch == TOUCH_PARTIAL)
					{
						bClip = true;
					}

					setShapeVisible(i, true);
				}
			

 				if (bClip)
 				{
					//위치설정
 					g_pUIRenderer->setPos(m_hSprites[i], (REAL32)rtIntersect.left, (REAL32)rtIntersect.top, 0.f);
 
 					// Size설정
 					g_pUIRenderer->setSizeAndRotate(m_hSprites[i], (REAL32)rtIntersect.getWidth(), (REAL32)rtIntersect.getHeight(), 0.0f);
 	
					if (m_bAnimationState&&m_pSpriteAnimation[i]._pSprite)
						rt = m_pSpriteAnimation[i]._pSprite->GetTexCood();
					else
						rt = *pShape->getTextureCoord();
					

					REAL32 u1 = baseX + rt.left;
					REAL32 v1 = baseY + rt.top;
					REAL32 u2 = u1 + rt.right;
					REAL32 v2 = v1 + rt.bottom;
 
 					REAL32 scalex = max(I3_EPS, pRect->right / rt.right);
 					REAL32 scaley = max(I3_EPS, pRect->bottom / rt.bottom);
 
 					REAL32 value = max(I3_EPS, m_rtScissors.left - rtLetter.left);
 					u1 += value / scalex;
 
 					value = max(I3_EPS, m_rtScissors.top - rtLetter.top);
 					v1 += value / scaley;
 
 					value = min(I3_EPS, m_rtScissors.right - rtLetter.right);
 					u2 += value / scalex;
 
 					value = min(I3_EPS, m_rtScissors.bottom - rtLetter.bottom);
 					v2 += value / scaley;
 
					//uv설정
 					g_pUIRenderer->setTexCoord(m_hSprites[i], u1 * r, v1 * r, u2 * r, v2 * r);
 				}
 				else
				{
					g_pUIRenderer->setPos(m_hSprites[i], &Position);
					g_pUIRenderer->setSizeAndRotate( m_hSprites[i], pRect->right, pRect->bottom, pShape->getRotation());
				}
			}
		}

		removeState( I3UI_STATE_INVALIDSPRITE);
	}
}


i3UIShape * i3UIControl::getShape( INT32 idx)
{
	I3ASSERT( m_pTemplate != nullptr);
	return m_pTemplate->getShape( idx);
}

void i3UIControl::SetAlign( UINT32 AlignX, UINT32 AlignY )
{
	m_nControlStyle &= ~I3UI_STYLE_ALIGN_HMASK;
	m_nControlStyle &= ~I3UI_STYLE_ALIGN_VMASK;

	addStyle( AlignX);
	addStyle( AlignY);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIControl::setPos( REAL32 x, REAL32 y, REAL32 z)
{
	i3Vector::Set( &m_Pos, x, y, z);

	CalcAccumulatedPos();

	_recCalcScissorsRect(this);

	OnMove( x, y);

	_recsetModifyState( this, true);
}

void i3UIControl::SetPosNoNotify( REAL32 x, REAL32 y, REAL32 z)
{
	i3Vector::Set( &m_Pos, x, y, z);

	CalcAccumulatedPos();

	_recCalcScissorsRect(this);

	_recsetModifyState( this, true);
}

void i3UIControl::setSize( REAL32 cx, REAL32 cy)
{
	i3Vector::Set( &m_Size, cx, cy);

	OnSize( cx, cy);

	_recCalcScissorsRect(this);

	_recsetModifyState( this, true);
}

void i3UIControl::SetSizeNoNotify( REAL32 cx, REAL32 cy)
{
	i3Vector::Set( &m_Size, cx, cy);

	_recsetModifyState( this, true);
}


void i3UIControl::setColor( COLOR* pColor )
{
	m_Color.r = pColor->r;
	m_Color.g = pColor->g;
	m_Color.b = pColor->b;
	m_Color.a = pColor->a;

	_recsetModifyState( this, true);
}


bool i3UIControl::isReadyToShow(void)
{
	return (isState( I3UI_STATE_WAITLOAD) == false);
}


void i3UIControl::setAllShapeVisible( bool bVisible)
{
	if( g_pUIRenderer == nullptr || m_hSprites == nullptr)
		return;

	bVisible = bVisible && isReadyToShow();

	INT32 i;
	for( i = 0;i < m_nShapeCount; ++i)
	{
		if( m_hSprites[i] != -1)
			g_pUIRenderer->setEnableSprite( m_hSprites[i], bVisible);
	}
}

void i3UIControl::setShapeVisible( INT32 index, bool bVisible)
{
	//I3_BOUNDCHK( index, m_nShapeCount);

	if( index >= m_nShapeCount || index < 0)
	{
//		I3PRINTLOG(I3LOG_WARN,  "Shape이 없거나 잘못된 index입니다. \n");
//		I3TRACE( "Shape이 없거나 잘못된 index입니다. \n");	//11.06.01.수빈.테스트시 잦은 오류박스로 버거워서 TRACE로 교체합니다.
		return;
	}

	bVisible = bVisible && isReadyToShow();

	if( g_pUIRenderer != nullptr && m_hSprites != nullptr)
	{
		if( m_hSprites[index] != -1)
			g_pUIRenderer->setEnableSprite( m_hSprites[index], bVisible);
		//I3TRACE( "Shape %d visible state : %d\n", index, bVisible);
	}
}


bool i3UIControl::isValidShape( INT32 index)
{ 
	//I3_BOUNDCHK( index, m_nShapeCount);
	if( index >= m_nShapeCount || index < 0 )
	{
//		I3PRINTLOG(I3LOG_WARN,  "Shape이 없거나 잘못된 index입니다. \n");		
//		I3TRACE( "Shape이 없거나 잘못된 index입니다. \n");	// 2011.06.01.수빈 // 테스트시 잦은 오류박스로 버거워서 트레이스로 돌립니다.
		return false;
	}

	if( m_pTemplate != nullptr)
	{
		i3UIShape * pShape = getShape( index);

		if( pShape != nullptr)
		{
			return pShape->getEnable();
		}
	}

	return false;
}

void i3UIControl::HideShapes( bool bHide)
{
	//Recursive..
	i3::vector<i3UIControl*> ChildList;

	GetChildControls( ChildList, false);
	
	for( size_t i=0; i< ChildList.size(); i++)
	{
		ChildList.at(i)->HideShapes( bHide);		//i3UIControl이라고 가정해도 무방?
	}

	if( bHide)
	{
		addState( I3UI_STATE_HIDED);
		setAllShapeVisible( !bHide);
	}
	else
		removeState( I3UI_STATE_HIDED);
}

bool i3UIControl::_PrepareTexForSprite()
{
	// 자신이 사용하는 Sprite들에 대해, 해당 Texture가 준비되었는가를 확인하고
	// 모든 준비가 완료되었다면 true를, 그렇지 않다면 false를 반환한다.
	// 이 함수는 Child에 대해 Recursive하게 호출된다.
	
	INT32 i;
	bool bResult = true;
	VEC4D	vCoord, vTexRange;

	for( i = 0; i < getShapeCount(); i++)
	{
		if( isValidShape( i))
		{
			i3UIShape * pShape = getShape(i);

			if( calcTexCoordAndRange( pShape, &vCoord, &vTexRange))
			{
				bResult = bResult && g_pUIRenderer->PrepareTextureForSprite( &vCoord, &vTexRange);
			}
		}
	}

	return bResult;
}

bool i3UIControl::_PrepareShow(void)
{
	bool bResult = _PrepareTexForSprite();

	// Recursive Check
	if( bResult)
	{
		i3UIControl * pChild = (i3UIControl*)getFirstChild();

		while( pChild != nullptr)
		{
			//if( pChild->isStyle( I3UI_STYLE_SLAVE) == false)
			{
				bResult = bResult && pChild->_PrepareShow();
			}
		
			pChild = (i3UIControl*)pChild->getNext();
		}
	}

	return bResult;
}

void i3UIControl::EnableCtrl( bool bTrue, bool bRecursive)
{
	if( bTrue )
	{
		prepareSprites();
		if( isEnable() == false)
		{
			i3GameNode::SetEnable(true);

			// OnVisible
			if( !bRecursive )
			{	// 자식에 대해서 visible을 켜줘야 한다.
				i3UIControl * pChild = (i3UIControl*) getFirstChild();

				while( pChild)
				{
					pChild->OnVisibleScene();
					pChild = (i3UIControl*) pChild->getNext();
				}
			}

			OnEnabled(true);

			addState( I3UI_STATE_NEED_UPDATE);

			if( m_pTopFrameWnd != nullptr)
				m_pTopFrameWnd->addState( I3UI_STATE_REORDERING);
		}
	}
	else if( isEnable() )
	{
		_LoseTimer();

		i3GameNode::SetEnable(false);

		if( !bRecursive )
		{	// 자식에 대해서 visible을 꺼줘야 한다.
			i3UIControl * pChild = (i3UIControl*) getFirstChild();

			while( pChild)
			{
				pChild->OnInvisibleScene();
				pChild = (i3UIControl*) pChild->getNext();
			}
		}

		OnEnabled(false);

		if( i3UI::getToolDevMode() == false)
		{
			if( isStyle(I3UI_STYLE_BLOCK_AUTO_TRASH_TEX) == false)	// 메모리 해제전까지는 유지하도록 한다.
				resetSprites( false);
		}

		addState( I3UI_STATE_INVALIDSPRITE);

		// 컨트롤이 Select / pushed / onMouse 된 상태에서 Enable = false 될 경우
		// 다시 Enable 될때 상태가 남아있는 문제로, 해당 상태를 모두 제거해줍니다. 2012.09.04 양승천
		{
			removeState(I3UI_STATE_PUSHED | I3UI_STATE_SELECTED | I3UI_STATE_ONMOUSE | I3UI_STATE_LBTN_DRAG);
		}

		// focus 처리
		i3UI::releaseControl( this);
	}

	if( bRecursive)
	{
		i3UIControl * pChild = (i3UIControl*)getFirstChild();
		while( pChild != nullptr)
		{
			if( pChild->isStyle( I3UI_STYLE_SLAVE) == false)
			{
				pChild->EnableCtrl( bTrue, bRecursive);
			}
			else					// 예외적인 처리로 Control에 따로 함수를 두는게 가독성에는 더 좋을것 같다..
			{
				pChild->_EnableSprite_IfSlave_Recursive(bTrue);
			}
			pChild = (i3UIControl*)pChild->getNext();
		}
	}
}

void i3UIControl::_ForceVisible( bool bFlag)
{
	i3GameNode::_ForceVisible( bFlag );

	if( bFlag)	OnVisibleScene();
	else		OnInvisibleScene();
}

void i3UIControl::OnVisibleScene(void)
{
	addState( I3UI_STATE_NEED_UPDATE);

	removeStyle( I3UI_STYLE_HIDE);
}

void i3UIControl::OnInvisibleScene(void)
{
	addStyle( I3UI_STYLE_HIDE);

	setAllShapeVisible( false);
}

void i3UIControl::OnCompletedLoading(void)
{	
	removeState( I3UI_STATE_WAITLOAD);
	setModifyState( true);

	i3UIControl * pTemp = (i3UIControl *) getFirstChild();
	while( pTemp != nullptr)
	{
		pTemp->OnCompletedLoading();
		pTemp = (i3UIControl *) pTemp->getNext();
	}

	Notify( I3UI_EVT_COMPLETE_LOAD, 0, 0);
}

void i3UIControl::_RecalcCenterPosToSave( VEC3D * pOut)
{
	i3Vector::Copy( pOut, &m_Pos);

	if( isStyle( I3UI_STYLE_AUTOALIGN) )
	{
		// Scale을 하지 않는다면 Align값을 기준으로 Position을 조절한다.
		if( isStyle( I3UI_STYLE_POS_ALIGN_CENTER))
		{
			REAL32 center = (REAL32)(i3UI::getManager()->getWidth()) * 0.5f;

			pOut->x -= center;
			pOut->x += getX( getSize()) * 0.5f;
		}
		else if( isStyle( I3UI_STYLE_POS_ALIGN_RIGHT))
		{
			REAL32 rightPos = getX( &m_Pos) + getX( &m_Size);
			REAL32 rangeFromRight = (REAL32)(i3UI::getManager()->getWidth()) - rightPos;

			pOut->x = rangeFromRight;
		}

		if( isStyle( I3UI_STYLE_POS_ALIGN_MIDDLE))
		{
			REAL32 middle = (REAL32)(i3UI::getManager()->getHeight()) * 0.5f;

			pOut->y -= middle;
			pOut->y += getY( getSize()) * 0.5f;
		}
		else if( isStyle( I3UI_STYLE_POS_ALIGN_BOTTOM))
		{
			REAL32 bottomPos = getY( &m_Pos) + getY( &m_Size);
			REAL32 rangeFromBottom = (REAL32)(i3UI::getManager()->getHeight()) - bottomPos;

			pOut->y = rangeFromBottom;
		}
	}
}

// Template가 수정되었을 때 호출됩니다.
void i3UIControl::OnChangedTemplate(void)
{
	addState( I3UI_STATE_INVALIDSPRITE);
	setModifyState( true);

	UpdateRenderObjects();

	/*
	i3UIControl * pChild = (i3UIControl *) getFirstChild();

	while( pChild != nullptr)
	{
		pChild->OnChangedTemplate();

		pChild = (i3UIControl *) pChild->getNext();
	}
	*/
}

void i3UIControl::Notify( I3UI_EVT nEvent,INT32 nPriParam,INT32 nSecParam, bool bNotifySelf)
{
	// 자기 자신에게도 Notify. (Button->Scroll로는 Notify 하는데 Editbox->Editbox의 Notify는 처리하지 못해서 추가) ;jisoo
	if( bNotifySelf)
		this->OnNotify( this, nEvent, nPriParam, nSecParam);

	ParentNotify( nEvent, nPriParam, nSecParam);

	if( isState( I3UI_STATE_NO_NOTIFY) == false)
	{
		i3UI::Notify(this, nEvent, nPriParam, nSecParam);
	}
}

void i3UIControl::ParentNotify( I3UI_EVT event, INT32 param1, INT32 param2)
{
	// 부모 Window에게 Notify한다.
	i3UIControl * pParent = (i3UIControl *) this->getParent();

	if( pParent == nullptr)
		return;

	if( i3::kind_of< i3UIControl* >(pParent))
	{
		i3UI::ParentNotify( pParent, this, event, param1, param2);
	}
}

void i3UIControl::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameNode::CopyTo( pDest, method );

	if( !i3::kind_of<i3UIControl*>(pDest))
		return;

	i3UIControl * pCtrl = (i3UIControl*)pDest;

	pCtrl->setState( m_nControlState );
	pCtrl->setTemplateHashcode( m_hashTemplate);

	// copyto 종료후 initafterload에서 bind
	if( m_pTemplate != nullptr)
	{
		pCtrl->SetTemplate( m_pTemplate);
	}

	//initafter load에서 template를 bind한후 처리할 위치값만 기억하고 있도록 한다.
	//i3Vector::Copy( pCtrl->getPos(), &m_Pos);
	//i3Vector::Copy( pCtrl->getSize(), &m_Size);
	pCtrl->setPos( &m_Pos);
	pCtrl->setSize( &m_Size);
	pCtrl->setTempPos( getX(&m_Pos), getY( &m_Pos));

	// 이게 좋은 선택일까? 
	pCtrl->setModifyState();

	pCtrl->SetScissors(IsScissors());
}

INT32 i3UIControl::OnCommand(LuaState * pState)
{
	char szCommand[ 260];

	if( i3Lua::GetStringArg( pState, 2, szCommand, sizeof( szCommand)) != nullptr)
	{
		if( getParent() != nullptr)
		{
			I3TRACE("OnCommand[parent: %s][this: %s]: %s\n", getParent()->GetName(), meta()->class_name(), szCommand);
		}
		else
		{
			I3TRACE("OnCommand[parent: %s][this: %s]: %s\n", "no parent", meta()->class_name(), szCommand);
		}

	}

	return 1;
}

void i3UIControl::SetNode( i3Node * pNode)
{
	I3_REF_CHANGE( m_pSceneNode, pNode);
}


void i3UIControl::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3GameNode::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 UI_CONTROL
	{
		UINT8			m_ID[4] = { 'U', 'I', 'C', '1' };
		INT32			m_nStyle = 0;

		i3::pack::VEC3D	m_Pos;
		i3::pack::VEC2D	m_Size;

		UINT64			m_hashTemplate[2] = { 0 };
		INT32			m_scriptLength = 0;

		i3::pack::COLORREAL	m_Color;

		INT32		Reserved[24] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32	i3UIControl::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result;
	pack::UI_CONTROL data;

	Result = i3GameNode::OnSave( pResFile );
	I3_CHKIO( Result);

	data.m_nStyle		= m_nControlStyle;
	data.m_Size.x = getX( getSize());
	data.m_Size.y = getY( getSize());

	VEC3D vConv;
	_RecalcCenterPosToSave( &vConv);

	data.m_Pos.x = getX( &vConv);
	data.m_Pos.y = getY( &vConv);
	data.m_Pos.z = getZ( &vConv);

	data.m_hashTemplate[0]	= (UINT64)m_hashTemplate[0];
	data.m_hashTemplate[1]	= (UINT64)m_hashTemplate[1];

	if( !m_strScript.empty() ) 
	{
		data.m_scriptLength = m_strScript.length();
	}

	data.m_Color.r = (REAL32) i3Color::GetR( &m_Color) / 255;
	data.m_Color.g = (REAL32) i3Color::GetG( &m_Color) / 255;
	data.m_Color.b = (REAL32) i3Color::GetB( &m_Color) / 255;
	data.m_Color.a = (REAL32) i3Color::GetA( &m_Color) / 255;

	data.Reserved[0] = m_bAnimationState;
	data.Reserved[1] = m_bScissors;

	Rc = pResFile->Write( &data, sizeof( data ));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_scriptLength > 0)
	{
		Rc = pResFile->Write( &m_strScript[0], data.m_scriptLength);
		I3_CHKIO( Rc);
		Result += Rc;
	}
	
	return Result;
}

UINT32	i3UIControl::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result;
	pack::UI_CONTROL data;

	Result = i3GameNode::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data ));
	I3_CHKIO( Rc);
	Result += Rc;
	

	setStyle( data.m_nStyle);

	m_hashTemplate = UINT128( data.m_hashTemplate[1], data.m_hashTemplate[0]);

	if( getParent() != nullptr)
	{
		i3UIControl * pParent = (i3UIControl *) getParent();

		pParent->AddControl( this, false);
	}

	setPos( data.m_Pos.x, data.m_Pos.y, data.m_Pos.z);
	setTempPos( data.m_Pos.x, data.m_Pos.y);
	setSize( data.m_Size.x, data.m_Size.y);

	if( data.m_scriptLength > 0)
	{
		char szScriptPath[ MAX_PATH];
		Rc = pResFile->Read( szScriptPath, data.m_scriptLength);
		I3_CHKIO( Rc);
		Result += Rc;

		setScript( szScriptPath);
	}

	COLOR color;
	i3Color::Set( &color, data.m_Color.r, data.m_Color.g, data.m_Color.b, data.m_Color.a);
	setColor( &color);

	m_bAnimationState = data.Reserved[0] ? true : false;
	m_bScissors = data.Reserved[1] ? true : false;
	return Result;
}

bool	i3UIControl::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3GameNode::OnSaveXML( pFile, pXML );
	if( bRv == false)
		return false;

	VEC3D vConv;
	_RecalcCenterPosToSave( &vConv);

	i3Vector::SetToXML( pXML, "controlpos", &vConv);
	i3Vector::SetToXML( pXML, "controlsize", &m_Size);

	pXML->addAttr("ControlStyle", m_nControlStyle );

	// Control State의 값은 저장되어선 안되는 값들이다.
	// 만약 저장될 필요가 있다면, State가 아니라, Style로 정의되어야 한다.
	//pXML->addAttr("ControlState", m_nControlState & ~I3UI_STATE_VOLATILE_MASK);

	UINT64 a0,a1;
	a0 = (UINT64)m_hashTemplate[0];
	a1 = (UINT64)m_hashTemplate[1];

	pXML->addAttr("hash0", a0);
	pXML->addAttr("hash1", a1);

	if( !m_strScript.empty() )
		pXML->addAttr("script", m_strScript.c_str());

	pXML->addAttr( "ColorR", (INT32) i3Color::GetR( &m_Color));
	pXML->addAttr( "ColorG", (INT32) i3Color::GetG( &m_Color));
	pXML->addAttr( "ColorB", (INT32) i3Color::GetB( &m_Color));
	pXML->addAttr( "ColorA", (INT32) i3Color::GetA( &m_Color));

	INT32 ntemp = m_bAnimationState;
	pXML->addAttr("AnimationPlay", ntemp);
	pXML->addAttr("Scissors", m_bScissors);
	return true;
}

bool	i3UIControl::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3GameNode::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	i3Vector::GetFromXML( pXML, "controlpos", &m_Pos);
	setTempPos( m_Pos.x, m_Pos.y);
	i3Vector::GetFromXML( pXML, "controlsize", &m_Size);

	pXML->getAttr( "ControlStyle", &m_nControlStyle);

	// Control State의 값은 저장되어선 안되는 값들이다.
	// 만약 저장될 필요가 있다면, State가 아니라, Style로 정의되어야 한다.
	//pXML->getAttr( "ControlState", &m_nControlState);

	UINT64 a0,a1;

	pXML->getAttr("hash0", &a0);
	pXML->getAttr("hash1", &a1);

	m_hashTemplate = UINT128( a1, a0);

	const char * pszScript = pXML->Attribute("script");
	setScript( pszScript);

	INT32 r = 255,g = 255,b = 255,a = 255;

	pXML->getAttr( "ColorR", &r);
	pXML->getAttr( "ColorG", &g);
	pXML->getAttr( "ColorB", &b);
	pXML->getAttr( "ColorA", &a);

	if( a == 0)
	{
		r = g = b = a = 255;
	}

	COLOR color;
	i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
	setColor( &color);

	INT32 temp;
	pXML->getAttr("AnimationPlay", &temp);
	m_bAnimationState = temp ? true : false;

	pXML->getAttr("Scissors", &temp);
	m_bScissors = temp ? true : false;

	return true;
}

void i3UIControl::_UpdateAlignedPos( void)
{
	VEC3D	pos;
	VEC2D	size;

	if( isStyle( I3UI_STYLE_SIZE_RATIO))
	{
		i3Vector::Set( &pos, m_Pos.x * i3UI::getUIResolutionWidth(), m_Pos.y * i3UI::getUIResolutionHeight(), m_Pos.z);
		i3Vector::Set( &size, m_Size.x * i3UI::getUIResolutionWidth(), m_Size.y * i3UI::getUIResolutionHeight());
	}
	else
	{
		i3Vector::Copy( &pos, &m_Pos);
		i3Vector::Copy( &size, &m_Size);
	}

	i3UIManager * pMng = i3UI::getManager();

	if( isStyle( I3UI_STYLE_AUTOALIGN) )
	{
		// Scale을 하지 않는다면 Align값을 기준으로 Position을 조절한다.
		if( isStyle( I3UI_STYLE_POS_ALIGN_CENTER))
		{
			REAL32 halfsize = getX( &m_Size) * 0.5f;

			REAL32 center = (REAL32)(i3UI::getManager()->getWidth()) * 0.5f;
			center += getX( &m_TempPos);	// 중심과의 거리

			pos.x = center - halfsize;
		}
		else if( isStyle( I3UI_STYLE_POS_ALIGN_RIGHT))
		{
			REAL32 rightPos = getX( &m_TempPos) + getX( &m_Size);
			REAL32 rangeFromRight = (REAL32)(i3UI::getManager()->getWidth()) - rightPos;

			pos.x = rangeFromRight;

			REAL32 ratio = (REAL32)pMng->getWidth() / (REAL32)pMng->getActualWidth();
				
			pos.x += pMng->getDisplayOffsetX() * ratio;
		}
		else
		{	// Screen 해상도 변경에 따른 Offset을 LeftAlign으로 맞춘다.
			if( isStyle( I3UI_STYLE_POS_ALIGN_FORCE_LEFT ) || this->same_of( i3UIFrameWnd::static_meta()) )
			{
				pos.x = getX( &m_TempPos);

				REAL32 ratio = (REAL32)pMng->getWidth() / (REAL32)pMng->getActualWidth();
				pos.x -= pMng->getDisplayOffsetX() * ratio;
			}
		}


		if( isStyle( I3UI_STYLE_POS_ALIGN_MIDDLE))
		{
			REAL32 halfSizeY = getY( &m_Size) * 0.5f;

			REAL32 middle = (REAL32)(i3UI::getManager()->getHeight()) * 0.5f;
			middle += getY( &m_TempPos);

			pos.y = middle - halfSizeY;
		}
		else if( isStyle( I3UI_STYLE_POS_ALIGN_BOTTOM))
		{	
			REAL32 bottomPos = getY( &m_TempPos) + getY( &m_Size);
			REAL32 rangeFromBottom = (REAL32)(i3UI::getManager()->getHeight()) - bottomPos;

			pos.y = rangeFromBottom;

			REAL32 ratio = (REAL32)pMng->getHeight() / (REAL32)pMng->getActualHeight();
				
			pos.y += pMng->getDisplayOffsetY() * ratio;
		}
		else
		{
			if( isStyle( I3UI_STYLE_POS_ALIGN_FORCE_TOP ) || this->same_of( i3UIFrameWnd::static_meta()) )
			{
				pos.y = getY( &m_TempPos);

				REAL32 ratio = (REAL32)pMng->getHeight() / (REAL32)pMng->getActualHeight();
				pos.y -= pMng->getDisplayOffsetY() * ratio;
			}
		}
	}

	setPos( &pos);
	setSize( &size);

	addState( I3UI_STATE_NEED_UPDATE);

}

void i3UIControl::OnInitAfterLoad()
{
	if( isStyle( I3UI_STYLE_SLAVE))
		return;

	i3UITemplate * pTemplate = i3UI::FindTemplateByHash( m_hashTemplate);
	if( pTemplate == nullptr)
	{
		i3GameNode * parentscene = getParent();
		while(parentscene != nullptr)
		{
			if( i3::same_of<i3UIScene*>(parentscene) == false)
			{
				parentscene = parentscene->getParent();
			}
			else
			{
				break;
			}
		}

//		I3ASSERT_0;
		if(parentscene != nullptr && this->getOwner() != nullptr)
		{
			I3PRINTLOG(I3LOG_NOTICE, "Scene : %, Owner : %s, 템플릿 없음: %s %s", parentscene->GetName(), this->getOwner()->GetName(), meta()->class_name(), this->GetName() );
		}
		else
		{
			I3PRINTLOG(I3LOG_NOTICE, "템플릿 없음: %s %s", meta()->class_name(), this->GetName());
		}

		{
			i3GameNode * pParent = getParent();
			if( pParent != nullptr)
			{
				if( i3::same_of<i3UIScene*>(pParent))
				{
					((i3UIScene *)pParent)->AddControl( this, false);
				}
				else
				{
					((i3UIControl *)pParent)->AddControl( this, false);
				}
			}
		}
		return;
	}
	
	// 생성하는 동안 Notify가 되는 것을 방지하기 위해
	// 임시로 State를 설정함.
	m_nControlState |= I3UI_STATE_NO_NOTIFY;	

	{
		SetTemplate( pTemplate);

		if( getParent() != nullptr)
		{
			i3GameNode * pParent = getParent();
			if( i3::same_of<i3UIScene* >(pParent))
			{
				((i3UIScene*)pParent)->AddControl( this, false);

				setAncestor((i3UIScene*)pParent);

				if( kind_of( i3UIFrameWnd::static_meta()))
				{
					((i3UIFrameWnd*)this)->setTopFrameWndState( true);
				}
			}
			else
			{
				((i3UIControl*)pParent)->AddControl( this, false);

				i3GameNode * pAncestor = pParent->getParent();
				while( pAncestor != nullptr)
				{
					if( i3::same_of<i3UIScene*>(pAncestor))
					{
						setAncestor((i3UIScene*)pAncestor);
						break;
					}

					pAncestor = pAncestor->getParent();
				}
			}
		}
	}

	// loading 시에 template를 설정하면 
	// template 설정시 적절한 크기를 맞추기위해 size를 변경하기 때문에
	// load되는 당시의 값들을 기억 하였다가 template를 설정한후, loading된 값으로 재설정 해준다.
	_UpdateAlignedPos();

	m_nControlState &= ~I3UI_STATE_NO_NOTIFY;
}

i3UIFrameWnd *	i3UIControl::FindOwner(void)
{
	i3UIControl * pTemp = this;

	while((pTemp != nullptr) && ( !i3::kind_of<i3UIFrameWnd*>(pTemp)))
	{
		pTemp = (i3UIControl *) pTemp->getParent();
	}

	return (i3UIFrameWnd *) pTemp;
}

void	i3UIControl::_recCalcAccumulatedPos( i3UIControl * pParent)
{
	i3GameNode * pNode = pParent->getFirstChild();
	while( pNode != nullptr)
	{
		//if( i3::kind_of<i3UIControl*>(pNode))
		{
			i3UIControl * pChildCtrl = (i3UIControl*)pNode;

			i3Vector::Add( pChildCtrl->getAccumulatedPos(), pChildCtrl->getPos(), pParent->getAccumulatedPos());

			_recCalcAccumulatedPos( pChildCtrl);
		}

		pNode = pNode->getNext();
	}
}



void	i3UIControl::CalcAccumulatedPos(void)
{
	i3Vector::Copy( &m_AccPos, getPos());

	i3GameNode * pNode = getParent();
	while( pNode != nullptr)
	{
		if( i3::same_of<i3UIScene*>(pNode))
			break;

		//if( i3::kind_of<i3UIControl*>(pNode))
		if( !((i3UIControl*)pNode)->m_bCalcDoneAccumPos )
		{
			i3Vector::Add( &m_AccPos, &m_AccPos, ((i3UIControl*)pNode)->getPos());
		}
		else
		{	// 이미 계산되었다면
			i3Vector::Add( &m_AccPos, &m_AccPos, ((i3UIControl*)pNode)->getAccumulatedPos());
			break;
		}

		pNode = pNode->getParent();
	}

	m_bCalcDoneAccumPos = true;
	
	_recCalcAccumulatedPos( this);
}

bool i3UIControl::isOnMouse( I3UI_OVERLAP_INFO * pInfo)
{
	REAL32 x1 = m_AccPos.x;
	REAL32 y1 = m_AccPos.y;
	REAL32 x2	= m_AccPos.x + m_Size.x;
	REAL32 y2	= m_AccPos.y + m_Size.y;

	REAL32 left = MIN( x1, x2 );
	REAL32 right = MAX( x1, x2 );

	REAL32 top = MIN( y1, y2 );
	REAL32 bottom = MAX( y1, y2 );

	if( pInfo->x >= left && pInfo->x <= right )
	{
		if( pInfo->y >= top && pInfo->y <= bottom )
		{
			return true;
		}
	}

	return false;
}

void i3UIControl::_Rec_FindOnMouse( INT32 depth, I3UI_OVERLAP_INFO * pInfo)
{
	if( isOnMouse( pInfo) == false)
		return;

	if( isIgnoreInput() )
		return;

	if( (pInfo->m_pFind == nullptr) || (pInfo->m_FindDepth <= depth))
	{
		pInfo->m_pFind = this;
		pInfo->m_FindDepth = depth;
	}

	i3UIControl * pTemp = (i3UIControl *) getFirstChild();
	while( pTemp != nullptr)
	{
		if( pTemp->isEnable())
		{
			if( i3::same_of<i3UIListBox*>(pTemp) )
				pTemp->_Rec_FindOnMouse( depth + 10, pInfo);
			else
				pTemp->_Rec_FindOnMouse( depth + 1, pInfo);
		}

		pTemp = (i3UIControl *) pTemp->getNext();
	}
}

bool	i3UIControl::HitTest( I3UI_PICK_INFO * pInfo, bool bRecursive)
{
	I3UI_OVERLAP_INFO info;

	info.m_FindDepth = -1;
	info.m_pFind = nullptr;
	info.x = pInfo->m_X;
	info.y = pInfo->m_Y;

	_Rec_FindOnMouse( 0, &info);

	if( info.m_pFind != nullptr)
		pInfo->m_pPickedCtrl = info.m_pFind;

	return (info.m_pFind != nullptr);
}

void i3UIControl::GetChildControls( i3::vector<i3UIControl*>& List, bool bRecursive)
{
	i3GameNode * pTemp = getFirstChild();

	while( pTemp != nullptr)
	{
		if( i3::kind_of<i3UIControl*>(pTemp))
		{
			List.push_back( static_cast<i3UIControl*>(pTemp));
		
			if( bRecursive)
			{
				((i3UIControl *) pTemp)->GetChildControls( List, true);
			}
		}

		pTemp = pTemp->getNext();
	}
}

#if defined( I3_DEBUG)
void i3UIControl::setSpreadShape( bool bSpread)
{
	if( m_pTemplate == nullptr)
		return;

	if( bSpread)
	{
		REAL32 width = 100.f, height = 100.f;

		m_pTemplate->SpreadLayout( &width, &height, m_pRects, m_nShapeCount);

		//_recEnableCtrl( this, false);
		
		// 
		i3Vector::Set( &m_Size, width, height);

		m_bSpreaded = true;
	}
	else
	{
		//_recEnableCtrl( this, true);

		m_bSpreaded = false;
	}
}

void i3UIControl::GetTDKGridProperties(i3::vector<PROPERTYINFO*>& List)
{
	i3GameNode::GetTDKGridProperties( List);	// 먼저 Base Class의 GetTDKGridProperties를 호출

	AddMFCProperty_BeginGroup( List, "i3UIControl");

	AddMFCProperty_String( List, (char*)GetName(), MAX_PATH -1, "Name", "트리노드 클릭 후, F2키를 눌러 컨트롤의 이름을 변경할 수 있습니다", true);

	// temporary
	// volatile 컨트롤의 property를 표시할 것인가에 대한 고민
	if( (getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) == 0
		&& (getStyle() & I3UI_STYLE_SLAVE) == 0)
	{
		_UpdateTemplateListAndIndex();
		AddMFCProperty_Combo( List, m_TemplateList, (INT32*)&m_nTemplateIdx, "Template");
	}

	if( isStyle( I3UI_STYLE_LOCK))
		m_bLock = true;
	else
		m_bLock = false;

	AddMFCProperty_Bool( List, &m_bLock, "Locked Shape", "No Editing this shape by this flag.");

	if( isStyle( I3UI_STYLE_IGNOREINPUT))
		m_bIgnoreInput = true;
	else
		m_bIgnoreInput = false;

	AddMFCProperty_Bool( List, &m_bIgnoreInput, "InputDisable", "Mouse input event On/Off");

	{// Size 고정 방식
		if( isStyle( I3UI_STYLE_SIZE_RATIO))
			m_bFixedSize = false;
		else
			m_bFixedSize = true;

		AddMFCProperty_Bool( List, &m_bFixedSize, "Fixed Size", "Fixed Pixel Size");
	}

	{
		if( isStyle( I3UI_STYLE_AUTOALIGN))
			m_bAutoAlign = true;
		else
			m_bAutoAlign = false;

		AddMFCProperty_Bool( List, &m_bAutoAlign, "Enable Align", "Align to screen");
	}

	{
		if( isStyle( I3UI_STYLE_TOP_DRAW))
			m_bTopDraw = true;
		else
			m_bTopDraw = false;

		AddMFCProperty_Bool( List, &m_bTopDraw, "Top Draw", "Set to top layer");
	}

	{// 정렬 Style
		m_HorizontalAlignList.clear();
		m_HorizontalAlignList.push_back( "Left");
		m_HorizontalAlignList.push_back( "Center");
		m_HorizontalAlignList.push_back( "Right");
		m_HorizontalAlignList.push_back( "ForcedLeft");

		if( isStyle( I3UI_STYLE_POS_ALIGN_CENTER))
			m_HorizontalIndex = 1;
		else if( isStyle( I3UI_STYLE_POS_ALIGN_RIGHT))
			m_HorizontalIndex = 2;
		else
		if (isStyle( I3UI_STYLE_POS_ALIGN_FORCE_LEFT))
			m_HorizontalIndex = 3;
		else
			m_HorizontalIndex = 0;

		AddMFCProperty_Combo( List, m_HorizontalAlignList, &m_HorizontalIndex, "Horizontal Align", "Horizontal Align style");

		m_VerticalAlignList.clear();
		m_VerticalAlignList.push_back( "Top");
		m_VerticalAlignList.push_back( "Middle");
		m_VerticalAlignList.push_back( "Bottom");
		m_VerticalAlignList.push_back( "ForcedTop");

		if( isStyle( I3UI_STYLE_POS_ALIGN_MIDDLE))
			m_VerticalIndex = 1;
		else if( isStyle( I3UI_STYLE_POS_ALIGN_BOTTOM))
			m_VerticalIndex = 2;
		else
			if ( isStyle( I3UI_STYLE_POS_ALIGN_FORCE_TOP))
			m_VerticalIndex = 3;
		else
			m_VerticalIndex = 0;

		AddMFCProperty_Combo( List, m_VerticalAlignList, &m_VerticalIndex, "Vertical Align", "Vertical Align style");
	}

	AddMFCProperty_Real32( List, &m_Size.x, "Width");
	AddMFCProperty_Real32( List, &m_Size.y, "Height");
	AddMFCProperty_Real32( List, &m_Pos.x, "Pos X");
	AddMFCProperty_Real32( List, &m_Pos.y, "Pos Y");

	AddMFCProperty_Color( List, &m_Color, "Color");

	{
		if( isStyle( I3UI_STYLE_BLOCK_AUTO_TRASH_TEX))
			m_bBlockAutoTrashTex = true;
		else
			m_bBlockAutoTrashTex = false;

		AddMFCProperty_Bool( List, &m_bBlockAutoTrashTex, "Block Auto Trash Tex", "block trash texture automatically");
	}

	AddMFCProperty_Bool(List, &m_bAnimationState, "Animation", "Sprite Animation");
	AddMFCProperty_Bool(List, &m_bScissors, "Scissors", "Scissors");

	AddMFCProperty_EndGroup( List);
}

void i3UIControl::OnTDKPropertyChanged( const char* szName)
{
	if( i3::generic_is_iequal( szName, "Width")  ||
		i3::generic_is_iequal( szName, "Height") )
	{
		setSize( &m_Size);
	}
	else if( i3::generic_is_iequal( szName, "Pos X") ||
			i3::generic_is_iequal( szName, "Pos Y") )
	{
		setPos( &m_Pos);
	}
	else if( i3::generic_is_iequal( szName, "Template") )
	{
		if( m_nTemplateIdx > -1 )
		{
			const char* pszTemplateName = m_TemplateList[m_nTemplateIdx].c_str();
			SetTemplateByName( pszTemplateName);
		}
	}
	else if( i3::generic_is_iequal( szName, "Locked Shape") )
	{
		if( m_bLock)
			addStyle( I3UI_STYLE_LOCK);
		else
			removeStyle( I3UI_STYLE_LOCK);
	}
	else if( i3::generic_is_iequal( szName, "InputDisable") )
	{
		if( m_bIgnoreInput)
			addStyle( I3UI_STYLE_IGNOREINPUT);
		else
			removeStyle( I3UI_STYLE_IGNOREINPUT);
	}
	else if( i3::generic_is_iequal( szName, "Fixed Size") )
	{
		if( m_bFixedSize)
			_recModifyStyle( this, 0, I3UI_STYLE_SIZE_RATIO);
		else
			_recModifyStyle( this, I3UI_STYLE_SIZE_RATIO, 0);
	}
	else if( i3::generic_is_iequal( szName, "Enable Align") )
	{
		if( m_bAutoAlign)
			addStyle( I3UI_STYLE_AUTOALIGN);
		else
			removeStyle( I3UI_STYLE_AUTOALIGN);
	}
	else if( i3::generic_is_iequal( szName, "Top Draw") )
	{
		if( m_bTopDraw)
			addStyle( I3UI_STYLE_TOP_DRAW);
		else
			removeStyle( I3UI_STYLE_TOP_DRAW);
	}
	else if( i3::generic_is_iequal( szName, "Horizontal Align") )
	{
		removeStyle( I3UI_STYLE_POS_ALIGN_CENTER | I3UI_STYLE_POS_ALIGN_RIGHT | I3UI_STYLE_POS_ALIGN_FORCE_LEFT);

		if( m_HorizontalIndex == 1)
			addStyle( I3UI_STYLE_POS_ALIGN_CENTER);
		else if( m_HorizontalIndex == 2)
			addStyle( I3UI_STYLE_POS_ALIGN_RIGHT);
		else if (m_HorizontalIndex == 3)
			addStyle( I3UI_STYLE_POS_ALIGN_FORCE_LEFT);
	}
	else if( i3::generic_is_iequal( szName, "Vertical Align") )
	{
		removeStyle( I3UI_STYLE_POS_ALIGN_MIDDLE | I3UI_STYLE_POS_ALIGN_BOTTOM | I3UI_STYLE_POS_ALIGN_FORCE_TOP);

		if( m_VerticalIndex == 1)
			addStyle( I3UI_STYLE_POS_ALIGN_MIDDLE);
		else if( m_VerticalIndex == 2)
			addStyle( I3UI_STYLE_POS_ALIGN_BOTTOM);
		else if (m_VerticalIndex == 3)
			addStyle( I3UI_STYLE_POS_ALIGN_FORCE_TOP);

	}
	else if ( i3::generic_is_iequal( szName, "Block Auto Trash Tex") )
	{
		if( m_bBlockAutoTrashTex)
			addStyle( I3UI_STYLE_BLOCK_AUTO_TRASH_TEX);
		else
			removeStyle( I3UI_STYLE_BLOCK_AUTO_TRASH_TEX);
	}
	else if (i3::generic_is_iequal(szName, "Animation"))
	{
		ResetAnimation();
	}
	else if (i3::generic_is_iequal(szName, "Scissors"))
	{
		resetScissorRect(this);
	}
	else
	{
		i3GameNode::OnTDKPropertyChanged( szName);
	}
}
#endif	// I3_DEBUG

void i3UIControl::resetScissorRect(i3UIControl * pControl)
{
	i3UIControl * pChild = (i3UIControl*)pControl->getFirstChild();
	while (pChild != NULL)
	{
		pChild->InitScissorsRect();

		resetScissorRect(pChild);

		pChild = (i3UIControl*)pChild->getNext();
	}
}

void i3UIControl::_UpdateTemplateListAndIndex( void)
{
	m_TemplateList.clear();
	m_nTemplateIdx = -1;

	i3UILibrary* lib = i3UI::getUILibrary();

	char szTemplateName[MAX_PATH];

	if( m_pTemplate == nullptr)
	{
		m_TemplateList.push_back( "nullptr");
		m_nTemplateIdx = 0;
	}
	else
	{
		i3::string_ncopy_nullpad(szTemplateName, m_pTemplate->GetNameString(), MAX_PATH);

		if (lib == nullptr)
		{
			m_TemplateList.push_back(szTemplateName);
			m_nTemplateIdx = 0;
			return;
		}
	}
	
	
	//Search
	for( INT32 i=0; i< lib->getElementCount(); i++)
	{
		i3UITemplate* pTemplate = lib->getElement( i);
		
		if (m_pTemplate)
		{
			if (!pTemplate->kind_of(m_pTemplate->meta()))
				continue;
		}
		
		char pBuf[MAX_PATH];
		i3::string_ncopy_nullpad(pBuf, pTemplate->GetNameString(), MAX_PATH);

		m_TemplateList.push_back(pBuf);
	}

	std::set< std::string > setList;

	for each( i3::rc_string sz in m_TemplateList )
	{
		setList.insert( sz.c_str() );
	}

	m_TemplateList.clear();
	for each( std::string sz in setList )
	{
		m_TemplateList.push_back( sz.c_str() );
	}
 	
	INT32 nCount = (INT32)m_TemplateList.size();

	if (m_pTemplate)
	{
		//Set 
		for (INT32 i = 0; i < nCount; i++)
		{
			const char* pszSrc = m_TemplateList[i].c_str();

			if (i3::generic_is_iequal(pszSrc, szTemplateName))
			{
				m_nTemplateIdx = i;
			}
		}
	}

	/*I3ASSERT( m_TemplateList.GetCount() > 0);
	I3ASSERT( m_nTemplateIdx > -1);*/
}


// pRect : 전체 Rect
// pIntersect : 전체 Rect 중에서 실제로 보여질 영역
void i3UIControl::CalcCustomTexCoord( i3Rect* pIntersect, i3Rect* pRect)
{
	if( g_pUIRenderer == nullptr)
		return;

	if( isAllocedSprite() )
	{

		REAL32 r = 1.0f / I3VT_VTEX_SIZE;
		
		for( INT32 i=0; i< getShapeCount(); i++)
		{
			i3UIShape* pShape = getShape(i);
			
			REAL32 baseX = (REAL32) pShape->getTexCoordOffset()->x;
			REAL32 baseY = (REAL32) pShape->getTexCoordOffset()->y;

			REAL32 fOffsetU = pShape->getCoordW() * ((REAL32)(pIntersect->left - pRect->left) / (REAL32)pRect->getWidth());
			REAL32 fOffsetV = pShape->getCoordH() * ((REAL32)(pIntersect->top - pRect->top) / (REAL32)pRect->getHeight());

			REAL32 u1 = baseX + pShape->getCoordU() + fOffsetU;
			REAL32 v1 = baseY + pShape->getCoordV() + fOffsetV;

			REAL32 fWidth = pShape->getCoordW() * ((REAL32)pIntersect->getWidth() / (REAL32)pRect->getWidth());
			REAL32 fHeight = pShape->getCoordH() * ((REAL32)pIntersect->getHeight() / (REAL32)pRect->getHeight());
			
			REAL32 u2 = u1 + fWidth;
			REAL32 v2 = v1 + fHeight;

			INT32 hSprite = getSpriteHandle( i);
			I3ASSERT( hSprite != -1);
		
			g_pUIRenderer->setTexCoord( hSprite, u1*r, v1*r, u2*r, v2*r);
		}
	}

	m_bUseCustomTexCoord = true;
}

bool i3UIControl::calcTexCoordAndRange( i3UIShape * pShape, VEC4D * pCoord, VEC4D * pRange)
{
	if( pShape == nullptr || pShape->getEnable() == false)
		return false;

	// Texture의  UV값을 설정합니다.
	REAL32 offX2, offY2;
	REAL32 baseX, baseY, r;

	r = 1.0f / I3VT_VTEX_SIZE;
	baseX = (REAL32) pShape->getTexCoordOffset()->x;
	baseY = (REAL32) pShape->getTexCoordOffset()->y;

	REAL32 u1 = baseX + pShape->getCoordU();
	REAL32 v1 = baseY + pShape->getCoordV();
	REAL32 u2 = u1 + pShape->getCoordW();
	REAL32 v2 = v1 + pShape->getCoordH();

	i3Vector::Set( pCoord, u1 * r, v1 * r, u2 * r, v2 * r);

	offX2 = pShape->getTexCoordOffset()->x + pShape->getCoordW();
	offY2 = pShape->getTexCoordOffset()->y + pShape->getCoordH();
	
	i3Vector::Set( pRange, baseX * r, baseY * r, offX2 * r, offY2 * r);

	return true;
}

void i3UIControl::_LoseTimer( void)
{
	m_bHasTtTimer = false;
	
	if( isEvtEnable( I3UI_EVT_TOOLTIP ) ) Notify( I3UI_EVT_TOOLTIP, 0, 0);
	
	if( m_pTtMsg != nullptr)
	{
		if( m_pAncestor !=  nullptr)
		{
			i3UIToolTip* pToolTip = m_pAncestor->getDefToolTip();
			
			if( pToolTip != nullptr)
			{
				if( pToolTip->isEnable())
					pToolTip->EnableCtrl( false, false);
				else
					s_fTtTimer = 0.0f;
			}
		}

		if( s_fTtTimer > 0.0f)
			s_fTtTimer = max( 0.0f, s_fTtTimer - 0.1f);	//나가는 순간 timer를 조금 감소시켜둔다. 자연스러운 툴팁
	}
}

void i3UIControl::SetToolTipMsg( const WCHAR16* pszMsg)
{
	I3MEM_SAFE_FREE( m_pTtMsg);

	if( pszMsg != nullptr)
	{
		INT32 nLen = i3::generic_string_size( pszMsg);
		if( nLen > 0)
		{
			m_pTtMsg = (WCHAR16*) i3MemAlloc( sizeof(WCHAR16) * (nLen + 1));

			i3::string_ncopy_nullpad( m_pTtMsg, pszMsg, nLen);
			m_pTtMsg[nLen] = 0;
		}
	}
}

void i3UIControl::DrawShapes( i3UIRenderer * pRenderer)
{
	if( isAllocedSprite() )
	{
		INT32 i;

		for( i = 0; i < getShapeCount(); i++)
		{
			INT32 hSprite = getSpriteHandle( i);

			if( hSprite != -1)
				pRenderer->Draw_Kick( hSprite);
		}
	}
}


void i3UIControl::OnDraw( i3UIRenderer * pRenderer)
{
	m_bCalcDoneAccumPos = false;	// 초기화

	if( this->isEnable() == false)
		return;

	DrawShapes( pRenderer);

	i3UIControl * pChild = (i3UIControl *) getFirstChild();

	bool bTopDraw = false;

	while( pChild != nullptr)
	{
		bTopDraw = false;

		if ( pChild->isStyle(I3UI_STYLE_TOP_DRAW))
		{
			bTopDraw = true;
			pRenderer->Draw_SetTopMost( true);
		}

		pChild->OnDraw( pRenderer);

		pChild = (i3UIControl *) pChild->getNext();

		if ( bTopDraw )
		{
			pRenderer->Draw_SetTopMost( false);
		}
	}

}

void i3UIControl::GripDraw( HDC hdc)
{
	if( this->isEnable() == false)
		return;

	i3UIControl * pChild = (i3UIControl *) getFirstChild();

	while( pChild != nullptr)
	{
		pChild->GripDraw( hdc);
		pChild = (i3UIControl *) pChild->getNext();
	}
}

//***********////***********//
void i3UIControl::getCenPos(VEC3D *pVec3) const
{
	pVec3->x = m_Pos.x + (m_Size.x * 0.5f);
	pVec3->y = m_Pos.y + (m_Size.y * 0.5f);
	pVec3->z = m_Pos.z;
}

void i3UIControl::getAccumulatedCenPos(VEC3D *pVec3)
{
	CalcAccumulatedPos();

	pVec3->x = m_AccPos.x + (m_Size.x * 0.5f);
	pVec3->y = m_AccPos.y + (m_Size.y * 0.5f);
	pVec3->z = m_AccPos.z;
}

//***********////***********//
void i3UIControl::getRect(i3::pack::RECT *pRect) const
{
	pRect->left = m_Pos.x;
	pRect->top = m_Pos.y;
	pRect->right = m_Pos.x + m_Size.x;
	pRect->bottom = m_Pos.y + m_Size.y;
}

void i3UIControl::getAccumulatedRect(i3::pack::RECT *pRect)
{
	CalcAccumulatedPos();

	pRect->left = m_AccPos.x;
	pRect->top = m_AccPos.y;
	pRect->right = m_AccPos.x + m_Size.x;
	pRect->bottom = m_AccPos.y + m_Size.y;
}

void i3UIControl::setRect(i3::pack::RECT *pRect)
{
	setPos(pRect->left, pRect->top);
	setSize(pRect->Width(), pRect->Height() );
}

//***********////***********//
void i3UIControl::moveCenter(VEC3D *pCenPos)
{
	VEC3D center;
	getAccumulatedCenPos( &center );

	REAL32 offsetX = pCenPos->x - center.x;
	REAL32 offsetY = pCenPos->y - center.y;

	setPos( m_Pos.x + offsetX, m_Pos.y + offsetY );
}

void i3UIControl::moveCenter(i3UIControl *pi3UIControl)
{
	VEC3D cen;
	pi3UIControl->getAccumulatedCenPos( &cen );
	moveCenter( &cen );
}

void i3UIControl::_EnableSprite_IfSlave_Recursive(bool bEnable)
{
	if (bEnable)			// pChild->IsEnable검사는 할수없음(슬레이브 자체관리로..스프라잇은 별도 관리되어야함..)
	{					// 대신 m_bResetSprites가 기존에 존재하므로 이걸 활용하면 될것 같다..
		if (this->m_bResetSprites)				// 아직 여전이 결점이 있는데, 이것의 자식들은 재귀처리가 안되고 있음..
		{
			if (this->isEnable() )						// 최종적으로 해당콘트롤의 활성여부까지 따져서 생성한다..
			{
				if( i3UI::getToolDevMode() == false)
				{
					this->prepareSprites();
				}
			}
		}
	}
	else
	{
		if (!this->m_bResetSprites)
		{
			this->resetSprites(false);					// bEnabled 활성 유무와 상관없이 스프라잇을 무조건 내린다..
		}
	}
	
	i3UIControl * pChild = (i3UIControl*)getFirstChild();
	while( pChild != nullptr)
	{
		pChild->_EnableSprite_IfSlave_Recursive(bEnable);
		pChild = (i3UIControl*)pChild->getNext();
	}
}

void	i3UIControl::TA_GetPathID(char * pszPath, INT32 limit)
{
	i3::safe_string_copy(pszPath, GetName(), limit);
}

void	i3UIControl::TA_GetCtrlPath( char * pszPath, INT32 limit)
{
	i3GameNode * pParentNode = getParent();

	if (pParentNode != nullptr)
	{
		if (pParentNode->kind_of(i3UIControl::static_meta()))
		{
			i3UIControl * pParent = (i3UIControl *)pParentNode;

			pParent->TA_GetCtrlPath(pszPath, limit);
		}
		else if (pParentNode->kind_of(i3UIScene::static_meta()))
		{
			i3UIScene * pScene = (i3UIScene *)pParentNode;

			pScene->TA_GetCtrlPath(pszPath, limit);
		}
		else
		{
			I3TRACE(L"TA_GetCtrlPath() : Invalid Parent type (%s)\n", pParentNode->meta()->class_name());
		}
	}

	i3::safe_string_cat(pszPath, "/", limit);

	char id[256];

	TA_GetPathID(id, _countof(id) - 1);

	i3::safe_string_cat(pszPath, id, limit);
}

i3UIControl * i3UIControl::TA_FindCtrlByPath( const char * pszPath)
{
	// Path에서 구분자(/)가 발견될 때까지의 Path ID를 뽑아낸다.
	char id[128], myid[128];

	const char * pos = strchr(pszPath, '/');

	if (pos != nullptr)
	{
		int len = pos - pszPath;

		memcpy(id, pszPath, len);		// 구분자까지의 sub-string을 뽑아내기.

		id[len] = 0;		// Null-Termination
	}
	else
	{
		// nullptr이라면 마지막이란 뜻이므로...
		strcpy_s(id, pszPath);
	}

	// 나의 ID를 구해서..
	TA_GetPathID(myid, _countof(myid) - 1);

	if (i3::generic_icompare( myid, id) == 0)
	{
		// 이 Ctrl에 있다!
		if (pos == nullptr)
		{
			// 마지막 경로이므로 내가 곧 "찾는 놈"이란 뜻.
			if ( isActivated() && isVisible())
				return this;
		}

		i3GameNode * pChild = getFirstChild();

		while (pChild != nullptr)
		{
			if (pChild->kind_of(i3UIControl::static_meta()))
			{
				i3UIControl * pCtrl = (i3UIControl *)pChild;

				i3UIControl * pFind = pCtrl->TA_FindCtrlByPath(pos + 1);		// 구분자(/)가 발견된 다음부터 경로를 전달한다.

				if (pFind != nullptr)
					return pFind;			// 찾는 놈
			}

			pChild = pChild->getNext();
		}
	}

	// 이 Ctrl이 아니니 nullptr return
	return nullptr;
}


i3UIControl * i3UIControl::TA_ScanText(const wchar_t * pszText, bool bMustBeSame, bool bSubControl)
{
	if (bSubControl == false)
		return false;

	i3GameNode * pChild = getFirstChild();

	while (pChild != nullptr)
	{
		if (pChild->isEnable() && pChild->kind_of(i3UIControl::static_meta()))
		{
			i3UIControl * pCtrl = (i3UIControl *)pChild;

			if (pCtrl->isActivated() && pCtrl->isVisible())
			{
				i3UIControl * pFind = pCtrl->TA_ScanText(pszText, bMustBeSame, bSubControl);
				if (pFind != nullptr)
					return pFind;
			}
		}

		pChild = pChild->getNext();
	}

	return nullptr;
}

bool i3UIControl::TA_Select(const wchar_t * pszText, bool bMustBeSame)
{
	return false;
}

void i3UIControl::ResetAnimation()
{
	if (m_pSpriteAnimation)
	{
		for (INT32 i = 0; i < m_nShapeCount; i++)
		{
			I3_SAFE_RELEASE(m_pSpriteAnimation[i]._pSprite);
		}
		I3_SAFE_DELETE_ARRAY(m_pSpriteAnimation);

		m_pSpriteAnimation = nullptr;
	}
}

void i3UIControl::OnSpriteAnimation(REAL32 rDeltaSeconds)
{
	if (m_bAnimationState)
	{
		if (m_pSpriteAnimation == nullptr)
		{
			m_pSpriteAnimation = new SPRITE2D[m_nShapeCount];
			for (INT32 i = 0; i < m_nShapeCount; i++)
			{
				m_pSpriteAnimation[i]._pSprite = nullptr; 
			}
		}

		for (INT32 i = 0; i < m_nShapeCount; i++)
		{
			i3UIShape* pShape = getShape(i);

			i3Framework * pFramework = i3Framework::getCurrentFramework();
			if (pFramework)
			{
				i3UIAnimationMgr* pAnimation = pFramework->getUIAnimationMgr();
				I3ASSERT(pAnimation);

				i3UIAnimation2DSprite* p2DSprite = pAnimation->GetAnimation(pShape->GetAnimationName());

				if (p2DSprite && m_pSpriteAnimation)
				{
					if (m_pSpriteAnimation[i]._pSprite == nullptr)
					{
						m_pSpriteAnimation[i]._pSprite = i3UIAnimation2DSprite::new_object();
						m_pSpriteAnimation[i]._pSprite->Init(pShape, p2DSprite);
// 						m_pSpriteAnimation[i]._pSprite->SetRoof(pShape->IsAnimationRoof());
// 						p2DSprite->CopyTo(m_pSpriteAnimation[i]._pSprite, I3_COPY_INSTANCE);
// 
// 						m_pSpriteAnimation[i]._pSprite->SetTexCood(*pShape->getTextureCoord());
// 						m_pSpriteAnimation[i]._pSprite->SetVTexOffset(*pShape->getTexCoordOffset());
					}

					m_pSpriteAnimation[i]._pSprite->Drive(rDeltaSeconds);

					if (i3::generic_compare(pShape->GetAnimationName(), m_pSpriteAnimation[i]._pSprite->GetName()) != 0)
					{
						ResetAnimation();
						break;
					}
				}
			}
		}

		UpdateRenderObjects();
	}
}

void i3UIControl::_recCalcScissorsRect(i3UIControl * pControl)
{
	i3UIControl * pChild = (i3UIControl*)pControl->getFirstChild();
	while (pChild != NULL)
	{
		if (pControl->IsScissors())
		{
			i3::pack::RECT accrect;
			pControl->getAccumulatedRect(&accrect);

			i3Rect rt;
			rt.left = (INT32)accrect.left;
			rt.right = (INT32)accrect.right - 1;
			rt.top = (INT32)accrect.top;
			rt.bottom = (INT32)accrect.bottom - 1;

			if (pControl->GetScissorsRect()->getWidth() > 1 && pControl->GetScissorsRect()->getHeight() > 1)
			{
				rt.left = max(rt.left, pControl->GetScissorsRect()->left);
				rt.top = max(rt.top, pControl->GetScissorsRect()->top);
				rt.right = min(rt.right, pControl->GetScissorsRect()->right);
				rt.bottom = min(rt.bottom, pControl->GetScissorsRect()->bottom);
			}


			pChild->SetScissorsRect(rt);
		}
		else
		{
			pChild->SetScissorsRect(*pControl->GetScissorsRect());
		}

		_recCalcScissorsRect(pChild);

		pChild = (i3UIControl*)pChild->getNext();
	}
}
