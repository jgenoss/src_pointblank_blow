#include "i3GuiPCH.h"
#include "i3GuiStatic.h"

// ============================================================================
//
// i3GuiStatic : 이미지 혹은 글자
//
// ============================================================================

I3_CLASS_INSTANCE( i3GuiStatic);

i3GuiStatic::i3GuiStatic()
{
	setControlState(I3GUI_CONTROL_STATE_NORMAL);
}

// 갱신
void i3GuiStatic::OnUpdate( REAL32 rDeltaSeconds )
{
	UpdateRenderObjects();

	if( getControlState() == I3GUI_CONTROL_STATE_CLICKED)
	{
		setControlState( I3GUI_CONTROL_STATE_NORMAL);
	}

	i3GuiControl::OnUpdate( rDeltaSeconds ); 
}

void i3GuiStatic::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{
	if( getControlDisable()) 
	{		
		i3GuiObjBase::UserInput(fX, fY, state);
		return;
	}

	// Mouse의 왼쪽 버튼이 클릭되었습니다.	
	if( IS_MOUSE_STATE(state, I3GUI_MOUSE_STATE_LBUTTON_DCLICK))
	{
		if (this == m_pClickedControl)
		{
			setControlState(I3GUI_CONTROL_STATE_RESERVED2);
			PushControlNotify(I3GUI_STATIC_NOTIFY_DOUBLECLICK, (INT32)fX, (INT32)fY);
		}
		else
		{
			setControlState(I3GUI_CONTROL_STATE_CLICKED);
			PushControlNotify(I3GUI_STATIC_NOTIFY_CLICKED, (INT32)fX, (INT32)fY);
			m_pClickedControl = this;
		}
	}
	else if( IS_MOUSE_STATE(state, I3GUI_MOUSE_STATE_LBUTTON_DOWN))
	{
		setControlState(I3GUI_CONTROL_STATE_CLICKED);
		PushControlNotify(I3GUI_STATIC_NOTIFY_CLICKED, (INT32)fX, (INT32)fY);
		m_pClickedControl = this;
	}
	// Mouse의 오른쪽 버튼이 클릭되었습니다.
	else if( IS_MOUSE_STATE(state, I3GUI_MOUSE_STATE_RBUTTON_DOWN))
	{
		setControlState( I3GUI_CONTROL_STATE_R_CLICKED);
		PushControlNotify(I3GUI_STATIC_NOTIFY_RCLICK, (INT32)fX, (INT32)fY);		
	}
	// Mouse가 버튼위에 올려져 있습니다.
	else
	{
		setControlState(I3GUI_CONTROL_STATE_ONMOUSE);	
		PushControlNotify(I3GUI_STATIC_NOTIFY_ONMOUSE, 0, 0);
	}

//	i3GuiObjBase::UserInput(fX, fY, state);
}

void i3GuiStatic::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GuiControl::CopyTo( pDest, method );

	if( !i3::kind_of<i3GuiStatic*>(pDest))
		return ;

	i3GuiStatic * pStatic = (i3GuiStatic*)pDest;

	pStatic->setControlState( m_ControlState );
}

void i3GuiStatic::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	i3GuiControl::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUISTATIC
	{
		UINT32	m_State = 0;
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3GuiStatic::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUISTATIC data;

	Result = i3GuiControl::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	data.m_State = m_ControlState;

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR )
	{
		i3Log("i3GuiStatic::OnSave()", "Could not Save data");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3GuiStatic::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUISTATIC data;

	Result = i3GuiControl::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiStatic::OnLoad()","Could not load data");
		return STREAM_ERR;
	}
	Result += Rc;

	m_ControlState = (GUI_CONTROL_STATE)data.m_State;

	return Result;
}

// Shape를 변경시킬 수 있는 인터페이스
bool i3GuiStatic::ChangeFirstShape(i3Texture* pTexture,INT32 u,INT32 v,INT32 w,INT32 h)
{
	if( !pTexture) return false;
	if( 0 > u || 0 > v || 0 > w || 0 > h) return false;

	UINT32 idxFirstShape = 0;

	if (9 <= m_pData->m_pShape->getShapeCount())
	{
		idxFirstShape = 8;
	}

	m_pData->m_pShape->getImage(idxFirstShape)->SetTexture( pTexture);

	m_pData->m_pShape->getImage(idxFirstShape)->m_u = u;
	m_pData->m_pShape->getImage(idxFirstShape)->m_v = v;
	m_pData->m_pShape->getImage(idxFirstShape)->m_w = w;
	m_pData->m_pShape->getImage(idxFirstShape)->m_h = h;


	SetAllShapeEnable(false);
	SetShapeEnable(idxFirstShape, true);

	return true;
}

void i3GuiStatic::SetShapeFlip(UINT32 idxShape,bool bFlipX,bool bFlipY)
{
	m_pData->m_pShape->getImage(idxShape)->m_bFlipX = bFlipX;
	m_pData->m_pShape->getImage(idxShape)->m_bFlipY = bFlipY;
}

void i3GuiStatic::SetShapeUV(UINT32 idxShape,INT32 u,INT32 v,INT32 w,INT32 h)
{
	m_pData->m_pShape->getImage(idxShape)->m_u = u;
	m_pData->m_pShape->getImage(idxShape)->m_v = v;
	m_pData->m_pShape->getImage(idxShape)->m_w = w;
	m_pData->m_pShape->getImage(idxShape)->m_h = h;
}

void i3GuiStatic::FillColorInTexture(COLORREAL* pCol)
{
	//char* pBit = nullptr;		//C4189
	INT32 nShapeCount = 0;
	i3GuiImage* pGuiImage = nullptr;
	INT32 i, j;

	nShapeCount = m_pData->m_pShape->getShapeCount();	

	pGuiImage = m_pData->m_pShape->getImage(I3GUI_CTRL_SHAPE_CENTER);
	
	pGuiImage->GetTexture()->Lock(0);

	for(i=pGuiImage->getPixelU(); i<(pGuiImage->getPixelU()+pGuiImage->getPixelWidth()); i++)
	{
		for(j=pGuiImage->getPixelV(); j<(pGuiImage->getPixelV()+pGuiImage->getPixelHeight()); j++)
		{
			pGuiImage->GetTexture()->SetPixel(i, j, pCol);
		}
	}

	pGuiImage->GetTexture()->Unlock( 0);
}

void i3GuiStatic::FillBrightColorInTexture(COLORREAL* pCol)
{
	//char* pBit = nullptr;	//C4189
	INT32 nShapeCount = 0;
	i3GuiImage* pGuiImage = nullptr;
	INT32 i, j;

	nShapeCount = m_pData->m_pShape->getShapeCount();	

	pGuiImage = m_pData->m_pShape->getImage(I3GUI_CTRL_SHAPE_CENTER);

	INT32 iLevel = 0;
	INT32 iStartV = pGuiImage->getPixelV();
	COLORREAL brightColor, darkColor;
	COLORREAL nowColor;

	brightColor.r = (1.f - pCol->r) / 16;
	brightColor.g = (1.f - pCol->g) / 16;
	brightColor.b = (1.f - pCol->b) / 16;
	

	darkColor.r = pCol->r / 15;
	darkColor.g = pCol->g / 15;
	darkColor.b = pCol->b / 15;
	

	nowColor.a = nowColor.r = nowColor.g = nowColor.b = 1.f;
	
	pGuiImage->GetTexture()->Lock(0);

	

	if(pGuiImage->getPixelHeight() != 31*6)
	{
		I3ASSERT(!"명도 조절 바는 항상 31단계, 1단계는 6픽셀로 이뤄져야 함");
	}

	for(j=pGuiImage->getPixelV(); j<(pGuiImage->getPixelV()+pGuiImage->getPixelHeight()); j++)
	{
		if(j - iStartV >=6)
		{
			iLevel++;
			iStartV = j;
		}

		if(iLevel > 15)
		{
			nowColor.r = pCol->r - (darkColor.r*(iLevel - 16));
			if(nowColor.r > 1.f) nowColor.r = 1.f;
			if(nowColor.r < 0) nowColor.r = 0.f;
			nowColor.g = pCol->g - (darkColor.g*(iLevel - 16));
			if(nowColor.g > 1.f) nowColor.g = 1.f;
			if(nowColor.g < 0) nowColor.g = 0.f;
			nowColor.b = pCol->b - (darkColor.b*(iLevel - 16));
			if(nowColor.b > 1.f) nowColor.b = 1.f;
			if(nowColor.b < 0) nowColor.b = 0.f;

			if(iLevel >= 30)
			{
				nowColor.r = nowColor.g = nowColor.b = 0.f;
			}

		}
		else if(iLevel <= 15)
		{
			nowColor.r = 1.f - brightColor.r*iLevel;
			if(nowColor.r > 1.f) nowColor.r = 1.f;
			if(nowColor.r < 0) nowColor.r = 0.f;
			nowColor.g = 1.f - brightColor.g*iLevel;
			if(nowColor.g > 1.f) nowColor.g = 1.f;
			if(nowColor.g < 0) nowColor.g = 0.f;
			nowColor.b = 1.f - brightColor.b*iLevel;
			if(nowColor.b > 1.f) nowColor.b = 1.f;
			if(nowColor.b < 0) nowColor.b = 0.f;

		}
		else
		{
			I3ASSERT_0;
		}

		for(i=pGuiImage->getPixelU(); i<(pGuiImage->getPixelU()+pGuiImage->getPixelWidth()); i++)
		{
			
			pGuiImage->GetTexture()->SetPixel(i, j, &nowColor);
		}
	}

	pGuiImage->GetTexture()->Unlock( 0);
}

void i3GuiStatic::GetPixelStaticCtrl(INT32 iX, INT32 iY, COLORREAL* pCol)
{
	INT32 nShapeCount = 0;
	// i3Color::Set( pCol, 0.0f, 0.0f, 0.0f, 0.0f);
	
	nShapeCount = m_pData->m_pShape->getShapeCount();	

	if(nShapeCount != 1)
	{
		return;
	}

	iX = iX - (INT32)getTotalPosX();
	iY = iY - (INT32)getTotalPosY();

	REAL32 rWidthRatio = ((float)getOriginalWidth()) / ((float)getWidth());
	REAL32 rHeightRatio = ((float)getOriginalHeight()) / ((float)getHeight());

	iX = (INT32)(iX * rWidthRatio);		//C4244
	iY = (INT32)(iY * rHeightRatio);		//C4244
	iX += m_pData->m_pShape->getImage(I3GUI_CTRL_SHAPE_CENTER)->getPixelU();
	iY += m_pData->m_pShape->getImage(I3GUI_CTRL_SHAPE_CENTER)->getPixelV();

	m_pData->m_pShape->getImage(I3GUI_CTRL_SHAPE_CENTER)->GetTexture()->Lock(0, I3G_LOCK_READONLY);
	m_pData->m_pShape->getImage(I3GUI_CTRL_SHAPE_CENTER)->GetTexture()->GetPixel(iX, iY, pCol);
	m_pData->m_pShape->getImage(I3GUI_CTRL_SHAPE_CENTER)->GetTexture()->Unlock( 0);
}

void i3GuiStatic::ResizeCenterShape(REAL32 width,REAL32 height)
{
	/*
	if (nullptr == m_pData->m_pShape) return;

	INT32 count = m_pData->m_pShape->getShapeCount();
	if (8 > count) return;
	
	i3GuiImage* image;
	
	// 상하 면
	image = m_pData->m_pShape->getImage(1);
	image->setWidth(width);

	image = m_pData->m_pShape->getImage(6);
	image->setWidth(width);


	// 좌우 면
	image = m_pData->m_pShape->getImage(3);
	image->setHeight(height);
	
	image = m_pData->m_pShape->getImage(4);
	image->setHeight(height);


	// 중간
	for(INT32 i = 8; i < count; i++)
	{
		image = m_pData->m_pShape->getImage(i);

		image->setWidth(width);
		image->setHeight(height);
	}


	// 우측면
	image = m_pData->m_pShape->getImage(1);
	REAL32 x = image->m_nX + width;

	image = m_pData->m_pShape->getImage(2);
	image->m_nX = x;

	image = m_pData->m_pShape->getImage(4);
	image->m_nX = x;

	image = m_pData->m_pShape->getImage(7);
	image->m_nX = x;

	// 하단면
	image = m_pData->m_pShape->getImage(3);
	REAL32 y = image->m_nY + height;

	image = m_pData->m_pShape->getImage(5);
	image->m_nY = y;

	image = m_pData->m_pShape->getImage(6);
	image->m_nY = y;

	image = m_pData->m_pShape->getImage(7);
	image->m_nY = y;
	*/
}
