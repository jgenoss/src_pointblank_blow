#include "i3GuiPCH.h"
#include "i3GuiControl.h"

#include "i3Base/string/compare/generic_is_niequal.h"

I3_CLASS_INSTANCE(i3GuiControl);

i3GuiControl* i3GuiControl::m_pClickedControl = nullptr;

INT32 i3GuiControl::m_nDefaultFontSize = 8;		//	폰트 크기 설정이 -1일 경우 기본 폰트 설정 komet

i3GuiControl::i3GuiControl(void)
{
	m_pData = i3GuiControlData::new_object();
}

i3GuiControl::~i3GuiControl(void)
{
	I3_SAFE_RELEASE(m_pCaption);

	if( m_pSprite)
	{
		I3_SAFE_RELEASE(m_pSprite);
	}

	I3_SAFE_RELEASE(m_pData);
}

void i3GuiControl::SetControlShapeTemplate(i3GuiTemplateElement* pShape)
{
	I3ASSERT(pShape != nullptr);

	m_pData->SetShape(pShape);

	CreateBaseSceneObjects();

	UpdateRenderObjects(); 
}

bool i3GuiControl::CreateBaseSceneObjects()
{
	INT32 i = 0;

	INT32 nShapeCount = m_pData->m_pShape->getShapeCount();

	I3_SAFE_RELEASE(m_pSprite);
	m_pSprite = i3Sprite2D::new_object();

#if USE_VERTEX_BANK
	m_pSprite->Create( nShapeCount, true, true, true);
#else
	m_pSprite->Create( nShapeCount, true, true, false);
#endif

	GetNode()->AddChild((i3Node*)m_pSprite);

	for(i=0; i<nShapeCount; i++)
	{
		m_pSprite->SetEnable( i, true);
	}

	return true;
}

void i3GuiControl::SetEnable(bool bTrue)
{
	i3GuiObjBase::SetEnable(bTrue);
}

void i3GuiControl::OnUpdate( REAL32 rDeltaSeconds )
{
	// 
	{
		GUI_CONTROL_STATE state = getControlState();

		if( m_UserDefineStateIndex[ (INT32)state] != -1)
		{
			SetAllShapeEnable(false);
			
			INT32 nShapeCount = GetControlData()->m_pShape->getShapeCount();
			if( m_UserDefineStateIndex[ (INT32)state]< nShapeCount)
			{
				SetShapeEnable( m_UserDefineStateIndex[ (INT32)state], true);
			}
			else if( nShapeCount > 0)
			{
				SetShapeEnable( 0, true);
			}
		}

		if( m_bClickAction)
		{
			if( state == I3GUI_CONTROL_STATE_CLICKED)
			{
				setCtrlOffset( 1, 1);
			}
			else
			{
				setCtrlOffset( 0, 0);
			}
		}

		if( m_bFocusAction)
		{
			if( state == I3GUI_CONTROL_STATE_FOCUS)
			{
				setCtrlOffset( 2, 2);
			}
			else
			{
				setCtrlOffset( 0, 0);
			}
		}
	}

	if( !m_bOnceUpdate)
	{
		m_bOnceUpdate = true;
		_ResizeControl();
	}

	if(m_bRenderUpdate)
	{
		UpdateRenderObjects();
		m_bRenderUpdate = false;
	} 

	i3GuiObjBase::OnUpdate(rDeltaSeconds);

	// GUI가 Render보다 Update가 한프레임 늦게 업데이트 되는 현상이 있으므로, Update를 하고난후 Render되도록 예외처리합니다 - 송명일
	if( m_SkipRender != RENDER_GUI_TYPE_DONE)
	{
		if( m_SkipRender == RENDER_GUI_TYPE_SKIP)
		{
			m_nRenderCount++;
			if( m_nRenderCount > 1)
			{
				m_SkipRender = RENDER_GUI_TYPE_ACTIVE;
				m_nRenderCount = 0;
			}
		}

		switch( m_SkipRender)
		{
		case RENDER_GUI_TYPE_SKIP:
			GetNode()->AddFlag( I3_NODEFLAG_INVISIBLE);
			break;
		case RENDER_GUI_TYPE_ACTIVE:
			GetNode()->RemoveFlag( I3_NODEFLAG_INVISIBLE);
			m_SkipRender = RENDER_GUI_TYPE_DONE;
			break;
		case RENDER_GUI_TYPE_DONE:
			break;
		}	
	}
}

void i3GuiControl::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{
	i3GuiObjBase::UserInput(fX, fY, state);
}

void i3GuiControl::OnKillFocus(void)
{
	i3GuiObjBase::OnKillFocus();
}

bool i3GuiControl::OnRevive( i3RenderContext * pCtx)
{
	if( i3GameNode::OnRevive( pCtx) == false)
		return false;

	m_SkipRender = RENDER_GUI_TYPE_SKIP;
	return true;
}

void i3GuiControl::UpdateRenderObjects() 
{
	INT32 i = 0;
	INT32 nShapeCount;

	REAL32 fScreenWidthRate =  i3GuiRoot::getGuiResolutionWidth();
	REAL32 fScreenHeightRate =  i3GuiRoot::getGuiResolutionHeight();

	if(m_pData->m_pShape == nullptr)
	{
		return;
	}
	else
	{
		REAL32 X, Y;//, X2, Y2;

		REAL32 fParentPosX = getTotalPosX();
		REAL32 fParentPosY = getTotalPosY();

		nShapeCount = m_pData->m_pShape->getShapeCount();	

		for(i=0; i<nShapeCount; i++)
		{
			// 이미지 설정
			i3GuiImage *pImage = m_pData->m_pShape->getImage((I3GUI_CONTROL_SHAPE) i);

			// 가장 큰 크기의 Shape를 찾아서 그값을 멤버변수에 저장합니다.
			if( i == 0)
			{
				m_nWidth = (INT32) (pImage->m_nX + pImage->getWidth());
				m_nHeight = (INT32) (pImage->m_nY + pImage->getHeight());
			}
			else
			{
				// Width
				if( m_nWidth < (pImage->m_nX + (pImage->getWidth())) )
				{
					m_nWidth = (INT32) (pImage->m_nX + pImage->getWidth());
				}
		        
				// Height
				if( m_nHeight < (pImage->m_nY + (pImage->getHeight())) )
				{
					m_nHeight = (INT32) (pImage->m_nY + pImage->getHeight());
				}
			}
		}

		for(i=0; i<nShapeCount; i++)
		{
			// 이미지 설정
			i3GuiImage *pImage = m_pData->m_pShape->getImage((I3GUI_CONTROL_SHAPE) i);

			I3ASSERT( pImage != nullptr);
			
			// Texture의  UV값을 설정합니다.
			{
				I3ASSERT(pImage->m_pTexture != nullptr);

				m_pSprite->SetTexture( pImage->m_pTexture );

				REAL32 u1 = (REAL32)( pImage->m_u );
				REAL32 v1 = (REAL32)( pImage->m_v );
				REAL32 u2 = (REAL32)( pImage->m_u + pImage->m_w);
				REAL32 v2 = (REAL32)( pImage->m_v + pImage->m_h);

				bool bFlipX = pImage->m_bFlipX ^ m_pData->m_bFlipX;
				bool bFlipY = pImage->m_bFlipY ^ m_pData->m_bFlipY;

				m_pSprite->SetTextureCoord( i, u1, v1, u2, v2 );					
				m_pSprite->SetFlip( i, bFlipX, bFlipY );
			}

			// Align을 고려한 좌표 계산 (Offset은 Parent Window의 Position값)
			switch( m_pData->m_nAlignX )
			{
			case I3_GUI_ALIGN_LEFT:		X = fParentPosX + (REAL32)(pImage->m_nX); break;
			case I3_GUI_ALIGN_RIGHT:	X = fParentPosX - (REAL32)(getWidth()) + (REAL32)(pImage->m_nX); break;
			case I3_GUI_ALIGN_CENTER:	X = fParentPosX - i3Math::Mul( (REAL32)(getWidth()), 0.5f) + (REAL32)(pImage->m_nX); break;
			default:
				{
					I3ASSERT_0;
					X = fParentPosX + (REAL32)(pImage->m_nX);
				}
				break;
			}
			switch( m_pData->m_nAlignY )
			{
			case I3_GUI_ALIGN_TOP:		Y = fParentPosY + (REAL32)(pImage->m_nY); break;
			case I3_GUI_ALIGN_BOTTOM:	Y = fParentPosY - (REAL32)( getHeight()) + (REAL32)(pImage->m_nY); break;
			case I3_GUI_ALIGN_MIDDLE:	Y = fParentPosY - i3Math::Mul( (REAL32)( getHeight()), 0.5f + (REAL32)(pImage->m_nY)); break;
			default:
				{
					I3ASSERT_0;
					Y =fParentPosY + (REAL32)(pImage->m_nY);
				}
				break;
			}

//			X2 = X + (REAL32)(pImage->getWidth());
//			Y2 = Y + (REAL32)(pImage->getHeight());

			REAL32 HalfWidth = i3Math::Mul( (REAL32)(pImage->getWidth()), 0.5f);
			REAL32 HalfHeight = i3Math::Mul( (REAL32)(pImage->getHeight()), 0.5f);

			REAL32 CenterX = X + HalfWidth + (m_fCtrlOffsetX*fScreenWidthRate);
			REAL32 CenterY = Y + HalfHeight + (m_fCtrlOffsetY*fScreenHeightRate);

			m_pSprite->SetCenter( i, CenterX, CenterY,  0.0f);
			m_pSprite->SetColor( i, &m_pData->m_Color[i] );

			bool bFlipX = pImage->m_bFlipX ^ m_pData->m_bFlipX;
			bool bFlipY = pImage->m_bFlipY ^ m_pData->m_bFlipY;

			m_pSprite->SetFlip( i, bFlipX, bFlipY );

			REAL32 icx, icy;

			icx = (REAL32) pImage->getWidth();
			icy = (REAL32) pImage->getHeight();

			// Scale Event
			switch(getEventScaleDir())
			{
			case I3GUI_SCALE_DIR_NONE:
				m_pSprite->SetSize( i, icx*getEventScaleX(), icy*getEventScaleY());
				break;
			case I3GUI_SCALE_DIR_RIGHT:
				m_pSprite->SetSize( i,	 (REAL32)((icx) *getEventScaleX()),				icy);
				m_pSprite->SetCenter( i,	CenterX + icx - icx*getEventScaleX()*0.5f,	CenterY,  0.0f);
				break;
			case I3GUI_SCALE_DIR_LEFT:
				m_pSprite->SetSize( i, (REAL32)(icx * getEventScaleX()), icy);
				m_pSprite->SetCenter( i, CenterX - icx - icx*getEventScaleX()*0.5f, CenterY,  0.0f);
				break;
			case I3GUI_SCALE_DIR_UP:
				m_pSprite->SetSize( i, (REAL32)( (icx)) , (REAL32)( (icy)*getEventScaleY()));
				m_pSprite->SetCenter( i, CenterX , CenterY - icy - icy*getEventScaleY()*0.5f,  0.0f);
				break;
			case I3GUI_SCALE_DIR_DOWN:
				m_pSprite->SetSize( i, (REAL32)( (icx)) , (REAL32)( (icy)*getEventScaleY()));
				m_pSprite->SetCenter( i, CenterX , CenterY + icy - icy*getEventScaleY()*0.5f,  0.0f);
				break;
			default:
				m_pSprite->SetSize( i, (REAL32)( icx*getEventScaleX()) , (REAL32)( icy*getEventScaleY()));
				break;
			}

// Scale
//			{
//				m_pSprite->SetSize( i, (REAL32)( (pImage->getWidth()) * m_pData->m_fScaleX) , (REAL32)( (pImage->getHeight()) * m_pData->m_fScaleY));
//				m_pSprite->SetCenter( i, CenterX - ((pImage->getWidth()) - (pImage->getWidth()) * m_pData->m_fScaleX)*0.5f, CenterY - ((pImage->getHeight()) - (pImage->getHeight())* m_pData->m_fScaleY)*0.5f,  0.0f);
//			}
			

			// Event중에서 Roatation이 활성화 되어있다면, Sprite를 Rotate합니다.
			if(getEvent() & I3GUI_EVENT_ROTATE)
			{
				m_pSprite->SetRotation(i, getRotate());
			}

			if( m_pSprite->GetEnable(i) != pImage->getEnable())
			{
				m_pSprite->SetEnable(i, pImage->getEnable());
			}
		}
	}


	// 캡션
	_ReCalculateCaptionSize();
}

void i3GuiControl::SetAlign( INT32 AlignX, INT32 AlignY )
{
	m_pData->m_nAlignX = AlignX;
	m_pData->m_nAlignY = AlignY;
	m_bRenderUpdate = true;
}

void i3GuiControl::SetControlResizeWidth( REAL32 fWidth)
{
	m_nWidth = (INT32)(m_nWidth * fWidth);
}

void i3GuiControl::SetControlResizeHeight( REAL32 fHeight)
{
	m_nHeight = (INT32)(m_nHeight * fHeight);
}

void i3GuiControl::GetControlShapeUV( INT32 index, INT32* pu, INT32* pv)
{
	I3ASSERT(  GetControlData()->m_pShape->getShapeCount() > index);

	i3GuiImage * pImage = GetControlData()->m_pShape->getImage(index);

	*pu = pImage->m_u;
	*pv = pImage->m_v;
}

void i3GuiControl::SetControlShapeUV( INT32 index, INT32 u, INT32 v)
{
	I3ASSERT(  GetControlData()->m_pShape->getShapeCount() > index);

	i3GuiImage * pImage = GetControlData()->m_pShape->getImage(index);

	pImage->m_u = u;
	pImage->m_v = v;
}

void i3GuiControl::SetResizingControlSize( INT32 nWidth, INT32 nHeight)
{
	REAL32 rX = 0.0f;
	REAL32 rY = 0.0f;

	if(m_nOriginalWidth != nWidth || m_nOriginalWidth != nHeight)
	{
		INT32 nShapeCount = m_pData->m_pShape->getShapeCount();
		for(INT32 i=0; i<nShapeCount; i++)
		{
			// 이미지 설정
			i3GuiImage *pImage = m_pData->m_pShape->getImage((I3GUI_CONTROL_SHAPE) i);

			pImage->Reset();

			switch( i)
			{
			case 0:
				break;
			case 1:
				rX = (REAL32)(nWidth - m_nOriginalWidth);
				pImage->setWidth( (pImage->getWidth() + rX));
				break;
			case 2:
				pImage->m_nX = pImage->m_nX + rX;
				break;
			case 3:
				rY = (REAL32)(nHeight - m_nOriginalWidth);
				pImage->setHeight(pImage->getHeight() + rY);
				break;
			case 4:		
				pImage->m_nX = (pImage->m_nX + rX);
				pImage->setHeight(pImage->getHeight() + rY);
				break;
			case 5:
				pImage->m_nY = (pImage->m_nY + rY);
				break;
			case 6:
				pImage->m_nY = (pImage->m_nY + rY);
				pImage->setWidth( pImage->getWidth() + rX);
				break;
			case 7:
				pImage->m_nX = (pImage->m_nX + rX);
				pImage->m_nY = (pImage->m_nY + rY);
				break;
			case 8:
				pImage->setWidth( pImage->getWidth() + rX);
				pImage->setHeight(pImage->getHeight() + rY);
				break;
			default:
				pImage->setWidth( pImage->getWidth() + rX);
				pImage->setHeight(pImage->getHeight() + rY);
				break;
			}
		}

		UpdateRenderObjects();
	}
}

void i3GuiControl::SetControlSize( INT32 nWidth, INT32 nHeight)
{
	INT32 i = 0;

	
	I3ASSERT( m_pData->m_pShape != nullptr);

	if(m_nWidth != nWidth || m_nHeight != nHeight)
	{
		INT32 nShapeCount = m_pData->m_pShape->getShapeCount();

		REAL32 sX	= ((REAL32)nWidth / m_nWidth);
		REAL32 sY	= ((REAL32)nHeight / m_nHeight);

		m_nWidth	= nWidth;
		m_nHeight	= nHeight;

		for(i = 0; i < nShapeCount; ++i)
		{
			i3GuiImage *pImage = m_pData->m_pShape->getImage((I3GUI_CONTROL_SHAPE) i);

			//	1픽셀 오차 때문에 int 변환시 무조건 올림으로 komet
			INT32 x = (INT32) ((pImage->m_nX * sX) + 0.999f);
			INT32 y = (INT32) ((pImage->m_nY * sY) + 0.999f);
			INT32 w = (INT32) ((pImage->getWidth() * sX) + 0.999f);
			INT32 h = (INT32) ((pImage->getHeight() * sY) + 0.999f);

			pImage->m_nX = (REAL32) x;
			pImage->m_nY = (REAL32) y;
			pImage->m_nWidth = (REAL32) w;
			pImage->m_nHeight = (REAL32) h;

			pImage->m_nOriginalX = (REAL32) x;
			pImage->m_nOriginalY = (REAL32) y;
			pImage->m_nOriginalWidth = (REAL32) w;
			pImage->m_nOriginalHeight = (REAL32) h;
		}

		UpdateRenderObjects();
	}
}


void i3GuiControl::SetControlScale(REAL32 sx, REAL32 sy)
{
	INT32 i = 0;

	if(m_pData->m_pShape == nullptr)
	{
		I3ASSERT(0);
	}

	INT32 nShapeCount = m_pData->m_pShape->getShapeCount();

	m_pData->m_fScaleX = sx;
	m_pData->m_fScaleY = sy;

	setControlResize( sx, sy);

	REAL32 rX = 0.0f;
	REAL32 rY = 0.0f;

	if( nShapeCount < 9)
	{
		if( nShapeCount == 1)
		{
			i3GuiImage *pImage = m_pData->m_pShape->getImage((I3GUI_CONTROL_SHAPE) 0);
			pImage->setWidth( (pImage->getWidth() * sx));
			pImage->setHeight((pImage->getHeight() * sy));

			m_bRenderUpdate = true;
			return;
		}

		I3PRINTLOG(I3LOG_WARN,  "Shape count is %d!! Must be 9 over!!!", nShapeCount);
	}

	for(i=0; i<nShapeCount; i++)
	{
		// 이미지 설정
		i3GuiImage *pImage = m_pData->m_pShape->getImage((I3GUI_CONTROL_SHAPE) i);

		pImage->Reset();

		switch( i)
		{
		case 0:
			break;
		case 1:
			rX = pImage->getWidth() * sx - pImage->getWidth();
			pImage->setWidth( (pImage->getWidth() * sx));
			break;
		case 2:
			pImage->m_nX = (pImage->m_nX + rX);
			break;
		case 3:
			rY = pImage->getHeight() * sy - pImage->getHeight();
			pImage->setHeight((pImage->getHeight() * sy));
			break;
		case 4:
			pImage->m_nX = (pImage->m_nX + rX);
			pImage->setHeight((pImage->getHeight() * sy));
			break;
		case 5:		
			pImage->m_nY = (pImage->m_nY + rY);
			break;
		case 6:
			pImage->m_nY = (pImage->m_nY + rY);
			pImage->setWidth((pImage->getWidth() * sx));
			break;
		case 7:
			pImage->m_nX = (pImage->m_nX + rX);
			pImage->m_nY = (pImage->m_nY + rY);
			break;
		case 8:
			pImage->setWidth((pImage->getWidth() * sx));
			pImage->setHeight((pImage->getHeight() * sy));
			break;
		default:
			pImage->setWidth((pImage->getWidth() * sx));
			pImage->setHeight((pImage->getHeight() * sy));
			break;
		}
	}

	m_bRenderUpdate = true;
}

void i3GuiControl::SetColor(GUI_CONTROL_STATE state, I3COLOR *pColor)
{
	m_bRenderUpdate = true;

	INT32 i = 0;

	INT32 nShapeCount = m_pData->m_pShape->getShapeCount();	

	for( i=0; i<nShapeCount; i++)
	{
		i3Color::Set( &m_pData->m_Color[i], pColor);
	}
}

void i3GuiControl::SetColor(GUI_CONTROL_STATE state, UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{
	m_bRenderUpdate = true;

	INT32 i = 0;

	INT32 nShapeCount = m_pData->m_pShape->getShapeCount();	

	for( i=0; i<nShapeCount; i++)
	{
		i3Color::Set( &m_pData->m_Color[i], r, g, b, a);
	}
}

/////

i3GuiControlData *	i3GuiControl::GetControlData()
{
	return m_pData;
}

void i3GuiControl::SetControlData(i3GuiControlData * pData)
{
	I3_REF_CHANGE( m_pData, pData );

	if( m_pData->m_pShape != nullptr)
	{
		SetControlShapeTemplate( m_pData->m_pShape );
	}
}

void i3GuiControl::SetSprite( i3Sprite2D * pSprite)
{
	I3_REF_CHANGE( m_pSprite, pSprite );
}

void i3GuiControl::SetAllShapeEnable( bool bEnable)
{
	INT32 i = 0;

	if(  m_pData->m_pShape)
	{
		INT32 nShapeCount = m_pData->m_pShape->getShapeCount();

		for( i=0; i<nShapeCount; i++)
		{
			m_pData->m_pShape->getImage(i)->setEnable(bEnable);
		}
	}
}

bool i3GuiControl::GetShapeEnable( INT32 index)
{ 
	if(  m_pData->m_pShape)
	{
		if( index >= m_pData->m_pShape->getShapeCount())
		{
			I3ASSERT( index < m_pData->m_pShape->getShapeCount());
		}

		if( m_pData->m_pShape->getImage(index))
		{
			return m_pData->m_pShape->getImage(index)->getEnable();
		}
	}

	return false;
}

void i3GuiControl::SetShapeEnable( INT32 index, bool bEnable)
{
	if(  m_pData->m_pShape)
	{
		if( index >= m_pData->m_pShape->getShapeCount())
		{
			I3ASSERT( index < m_pData->m_pShape->getShapeCount());		
		}	

		if( m_pData->m_pShape->getImage(index))
		{
			m_pData->m_pShape->getImage(index)->setEnable(bEnable);
		}
	}
}


void	i3GuiControl::SetUVPositioner(const SIZE32 & Interval, const SIZE32 & TextureSize, const SIZE32 * pImgSize)
{
	if ( pImgSize )
		m_ImgSize = *pImgSize;
	else
		m_ImgSize = Interval;

	m_UVPositioner.Set(Interval, TextureSize);
}


bool	i3GuiControl::SetShapeUVIndex(UINT32 Index)
{
	POINT32 p;
	if (m_UVPositioner.CalcuPosition(Index, p) == false )
		return false;

	i3GuiImage * pImage = m_pData->m_pShape->getImage(0);
	pImage->SetTexture(pImage->m_pTexture, p.x, p.y, m_ImgSize.w, m_ImgSize.h);

	return true;
}


i3GuiControl *	i3GuiControl::FindControlByName( const char * pszName, INT32 length, bool bRecursive)
{
	i3GuiControl * pChild = (i3GuiControl *) getFirstChild();

	while( pChild != nullptr)
	{
		if( pChild->hasName())
		{
			// 이름 확인
			if( length == 0)
			{
				if( i3::generic_is_iequal( pChild->GetNameString(), pszName) )		// 찾았다면...
					return pChild;
			}
			else
			{
				if( i3::generic_is_niequal( pChild->GetNameString(), pszName, length))
					return pChild;
			}
		}

		if( bRecursive)
		{
			// Recursive Search
			i3GuiControl * pRv = pChild->FindControlByName( pszName, length, true);

			if( pRv != nullptr)
				return pRv;
		}

		pChild = (i3GuiControl *) pChild->getNext();
	}

	return nullptr;
}

i3GuiControl *	i3GuiControl::FindControlByPath( const char * pszPath)
{
	char * pszEnd = (char *) pszPath;
	i3GuiControl * pCtrl = nullptr;
	char szName[ 256];
	INT32 idx;

	if( pszPath[0] == 0)
		return this;

	idx = 0;
	while( (*pszEnd != 0) && (*pszEnd != '/'))
	{
		szName[idx] = *pszEnd;
		idx++;
		pszEnd++;
	}

	szName[idx] = 0;

	if( pszEnd > pszPath)
	{
		pCtrl = FindControlByName( szName);
		if( pCtrl == nullptr)
			return nullptr;

		if( *pszEnd == '/')
		{
			// 아직 덜 찾았다.
			return pCtrl->FindControlByPath( pszEnd + 1);
		}
	}

	return pCtrl;
}

void i3GuiControl::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GuiObjBase::CopyTo( pDest, method);

	if( !i3::kind_of<i3GuiControl*>(pDest))
		return ;

	i3GuiControl * pCtrl = (i3GuiControl*)pDest;

	pCtrl->setControlState( m_ControlState );

	//m_Data
	i3GuiControlData * pDstData = pCtrl->GetControlData();
	m_pData->CopyTo( pDstData, method);
	if( pDstData->m_pShape != nullptr )
	{
		pCtrl->SetControlShapeTemplate( pDstData->m_pShape );
	}
/*
	//m_pSprite
	if( m_pSprite != nullptr )
	{
		i3Sprite2D * pNewSprite = i3Sprite2D::new_object();
		m_pSprite->CopyTo( pNewSprite, method);
	}
	*/
}


void i3GuiControl::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	m_pData->OnBuildObjectList( List );

//	if( m_pSprite != nullptr )
//		m_pSprite->OnBuildObjectList( pList );

	i3GuiObjBase::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUI_CONTROL
	{
		OBJREF	m_ControlData = 0;
		//	OBJREF	m_pSprite;
		INT32	m_State = 0;
		INT8	Reserved[24] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

bool	i3GuiControl::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3GuiObjBase::OnSaveXML( pFile, pXML );
	if( bRv == false)
		return false;

	pXML->addAttr( "Data",	(INT32) pFile->GetObjectPersistID( (i3PersistantElement *) m_pData));
	pXML->addAttr( "State", (INT32)m_ControlState);

	return true;
}

bool	i3GuiControl::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3GuiObjBase::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	INT32 iVal, State;

	pXML->getAttr( "Data", &iVal);
	if( iVal > 0)
	{
		i3GuiControlData * pData = (i3GuiControlData*) pFile->FindObjectByID((OBJREF)iVal);
		I3ASSERT( pData != nullptr);

		SetControlData( pData);
	}

	pXML->getAttr( "State", &State);
	m_ControlState = (GUI_CONTROL_STATE)State;

	return true;
}

UINT32	i3GuiControl::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	
	Result = i3GuiObjBase::OnSave( pResFile );
	if( Result == STREAM_ERR )
	{
		return STREAM_ERR;
	}

	pack::GUI_CONTROL data;
	data.m_ControlData = (OBJREF) pResFile->GetObjectPersistID( (i3PersistantElement *) m_pData);
	data.m_State	   = m_ControlState;

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3Guicontrl::OnSave()", "Could not save file");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32	i3GuiControl::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUI_CONTROL data;

	Result = i3GuiObjBase::OnLoad( pResFile );
	if( Result == STREAM_ERR )
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data ));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiControl::OnLoad()", "Could not load data");
		return STREAM_ERR;
	}
	Result += Rc;

	if( data.m_ControlData != 0)
	{
		i3GuiControlData * pData = (i3GuiControlData*)pResFile->FindObjectByID( data.m_ControlData);
		I3ASSERT( pData != nullptr );

		SetControlData( pData );
	}

	m_ControlState			=	(GUI_CONTROL_STATE)data.m_State;

	return Result;
}

void i3GuiControl::CreateCaption(const i3TextFontSetInfo * pFontSetInfo,INT32 nTextMaxCount,INT32 nSize,UINT32 nWeight,bool bItalic,bool bUnderLine,INT32 nQuality)
{
	m_FontSetInfo	= *pFontSetInfo;
	m_nCaptionTextMaxCount	= nTextMaxCount;

	if( nSize == -1)
		m_nCaptionSize = m_nDefaultFontSize;
	else
		m_nCaptionSize = nSize;

	m_nCaptionWeight = nWeight;
	m_bCaptionItalic = bItalic;
	m_bCaptionUnderLine = bUnderLine;
	m_nCaptionQuality = nQuality;
	wchar_t wszTemp[1024];

	if( m_pCaption)
	{
		i3::safe_string_copy( wszTemp, m_pCaption->getTextString(), 1024);
		// 기존 캡션 제거
		GetNode()->RemoveChild(m_pCaption);
		I3_SAFE_RELEASE(m_pCaption);
	}
	else
	{
		wszTemp[0] = 0;
	}
	
	//REAL32 rx = i3GuiRoot::getViewer()->GetViewWidth() * 0.9765625e-3f;		// / I3GUI_RESOLUTION_WIDTH;	// C4189
	REAL32 ry = i3GuiRoot::getViewer()->GetViewHeight() * 0.13020833333333333333333333333333e-2f; // / I3GUI_RESOLUTION_HEIGHT;

	if( !i3GuiRoot::isChangeResolution())
	{	
		ry = 1.0f;
	}

	nSize = (INT32) ((REAL32) nSize * ry);

	//	한글판에서만 두꺼운(?) 글자가 나오게 하기 위해 예외처리합니다.
	switch( PRIMARYLANGID(GetSystemDefaultLangID()) )
	{
	case LANG_KOREAN:
		{
			nWeight = (UINT32)(nWeight * ry);

			UINT32 val = nWeight%100;

			if( val < 50) nWeight -= val;
			else nWeight += (100 - val);
		}
		break;
	case LANG_THAI:
		{
		}
		break;
	default:
		{
			nWeight = (UINT32)(nWeight * ry);

			UINT32 val = nWeight%100;

			if( val < 50) nWeight -= val;
			else nWeight += (100 - val);
		}
		break;
	}

	// 캡션 생성
	m_pCaption = i3TextNodeDX2::new_object();
#if USE_VERTEX_BANK
	m_pCaption->Create(&m_FontSetInfo, nTextMaxCount, nSize, nWeight, bItalic, bUnderLine, nQuality, false, FONT_SHADOW_QUALITY_LOW, 1.0f, true);
#else
	m_pCaption->Create(&m_FontSetInfo, nTextMaxCount, nSize, nWeight, bItalic, bUnderLine, nQuality, false, FONT_SHADOW_QUALITY_LOW, 1.0f, false);
#endif
	
	m_pCaption->SetAlign(m_CaptionAlignX, m_CaptionAlignY);
	m_pCaption->SetColor(&m_CaptionColor);
	_ReCalculateCaptionSize();
	
	m_pCaption->RemoveFlag( I3_NODEFLAG_DISABLE);
	m_pCaption->setAutoWrapEnable(true);
	m_pCaption->SetTextSpace( m_CaptionTextSpaceX, m_CaptionTextSpaceY);
	m_pCaption->SetOffsetPos( m_CaptionTextOffsetX, m_CaptionTextOffsetY);
	m_pCaption->SetText(wszTemp);
	
	GetNode()->AddChild(m_pCaption);
}

void i3GuiControl::CreateCaption(const char* pszFont, UINT32 CharSet,INT32 nTextMaxCount,INT32 nSize,UINT32 nWeight,bool bItalic,bool bUnderLine,INT32 nQuality)
{
	i3TextFontSetInfo	fontSetInfo(pszFont, CharSet);
	CreateCaption(&fontSetInfo, nTextMaxCount, nSize, nWeight, bItalic, bUnderLine, nQuality);
}

void i3GuiControl::SetCaption(const wchar_t* strText)
{
	if (nullptr == m_pCaption)	return;

	m_pCaption->SetText(strText);
}

void i3GuiControl::SetCaptionEllipsis(const wchar_t* pText)
{
	if (nullptr == m_pCaption)	return;

	m_pCaption->SetTextEllipsis(pText);
}

void i3GuiControl::SetCaptionAlign(UINT32 AlignX,UINT32 AlignY)
{
	if (nullptr == m_pCaption)	return;

	m_CaptionAlignX = AlignX;
	m_CaptionAlignY = AlignY;

	m_pCaption->SetAlign(AlignX, AlignY);
}

void i3GuiControl::SetCaptionColor(I3COLOR* pColor)
{
	if (nullptr == m_pCaption)	return;

	i3Color::Copy( &m_CaptionColor, pColor);

	m_pCaption->SetColor(pColor);
}

void i3GuiControl::SetCaptionEnable(bool bEnable)
{
	if (nullptr == m_pCaption)	return;

	if( bEnable)
		m_pCaption->RemoveFlag( I3_NODEFLAG_DISABLE);
	else
		m_pCaption->AddFlag( I3_NODEFLAG_DISABLE);
}

void i3GuiControl::SetCaptionTextSpace(INT32 nX, INT32 nY)
{
	if (nullptr == m_pCaption)	return;

	m_CaptionTextSpaceX = nX;
	m_CaptionTextSpaceY = nY;

	m_pCaption->SetTextSpace( nX, nY);
}

void i3GuiControl::SetCaptionTextOffset(REAL32 fX, REAL32 fY)
{
	if (nullptr == m_pCaption)	return;

	m_CaptionTextOffsetX = fX;
	m_CaptionTextOffsetY = fY;

	m_pCaption->SetOffsetPos(fX, fY);
}

void i3GuiControl::_ReCalculateCaptionSize(void)
{
	if (nullptr == m_pCaption) return;

	m_pCaption->setPos((UINT32)getTotalPosX(), (UINT32)getTotalPosY());
	m_pCaption->setSize(getWidth(), getHeight());

	//m_pCaption->getTextAttrDX()->CalcTextWidth();		
}

bool i3GuiControl::_ResizeControl(void)
{
	if( !i3GuiRoot::isChangeResolution()) return false;

	INT32 i = 0;

	//
	if (m_pCaption)
	{
		CreateCaption(&m_FontSetInfo, m_nCaptionTextMaxCount, m_nCaptionSize, m_nCaptionWeight, m_bCaptionItalic, m_bCaptionUnderLine, m_nCaptionQuality);
	}

	REAL32 rx = i3GuiRoot::getGuiResolutionWidth(); //i3GuiRoot::getViewer()->GetViewWidth() * 0.9765625e-3f; // / I3GUI_RESOLUTION_WIDTH;
	REAL32 ry = i3GuiRoot::getGuiResolutionHeight(); //i3GuiRoot::getViewer()->GetViewHeight() * 0.13020833333333333333333333333333e-2f; // / I3GUI_RESOLUTION_HEIGHT;

	if( m_pData->m_pShape)
	{
		INT32 nShapeCount = m_pData->m_pShape->getShapeCount();		

		for(i=0; i<nShapeCount; i++)
		{
			// Shape의 크기를 화면 해상도에 맞게 변경합니다.

			i3GuiImage *pImage = m_pData->m_pShape->getImage((I3GUI_CONTROL_SHAPE) i);

			// 컨트롤의 초기위치와 본사이즈로 재설정합니다.
			pImage->Reset();

			pImage->setWidth( (pImage->getWidth() * rx));
			pImage->setHeight( (pImage->getHeight() * ry));			

			pImage->m_nX = (pImage->m_nX * rx);
			pImage->m_nY = (pImage->m_nY * ry);			
		}
	}

	//
	setSize( (INT32)(getOriginalWidth() * rx), (INT32)(getOriginalHeight() * ry));
	
	return true;
}

void i3GuiControl::SetTexture( i3Texture * pTex)
{
	if( m_pData->m_pShape)
	{
		INT32 nShapeCount = m_pData->m_pShape->getShapeCount();		
		INT32 i;

		for( i = 0; i < nShapeCount; i++)
		{
			i3GuiImage *pImage = m_pData->m_pShape->getImage((I3GUI_CONTROL_SHAPE) i);

			pImage->SetTexture( pTex);
		}
	}

	m_pSprite->SetTexture( pTex);
}

void i3GuiControl::SetShapeTextureCoord( INT32 idx, INT32 u, INT32 v, INT32 w, INT32 h)
{
	if( m_pData->m_pShape)
	{
		i3GuiImage *pImage = m_pData->m_pShape->getImage((I3GUI_CONTROL_SHAPE) idx);

		pImage->m_u = u;
		pImage->m_v = v;
		pImage->m_w = w;
		pImage->m_h = h;
	}

	m_bRenderUpdate = true;
}

void i3GuiControl::SetPosToCenter( INT32 cx, INT32 cy)
{
	i3Viewer * pViewer = i3GuiRoot::getViewer();
	INT32 scrCX, scrCY;

	scrCX = pViewer->GetViewWidth();
	scrCY = pViewer->GetViewHeight();

	INT32 x, y;

	if( cx < 0)
		cx = getWidth();

	if( cy < 0)
		cy = getHeight();

	x = (scrCX >> 1) - (cx >> 1);
	y = (scrCY >> 1) - (cy >> 1);

	REAL32 rx = getTotalPosX();
	REAL32 ry = getTotalPosY();

	rx = x - rx;
	ry = y - ry;

	m_fStartX += rx;
	m_fStartY += ry;
	//setPosition( (REAL32) x, (REAL32) y);
}

void i3GuiControl::SetCaptionDefaultFontSize(INT32 nSize)			
{ 
	m_nDefaultFontSize = nSize;				
}

INT32 i3GuiControl::GetCaptionDefaultFontSize(void)				
{ 
	return m_nDefaultFontSize;				
}