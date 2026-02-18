#include "i3FrameworkPCH.h"
#include "i3UIProgressBarImage.h"
#include "i3UIManager.h"	
#include "i3UITemplate_ProgressBarImage.h"
#include "../i3Framework.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/compare/generic_is_iequal.h"

I3_CLASS_INSTANCE( i3UIProgressBarImage);//, i3UICaptionControl);

void i3UIProgressBarImage::_UpdateShapeState( void)
{
	for( INT32 i=0; i< I3UI_PROGRESSBAR_SHAPE_COUNT; i++)
		setShapeVisible( i, true);

	if( m_pTemplate == nullptr)
		return;

	REAL32 size = getWidth() - m_pRects[0].right - m_pRects[2].right;

	m_rTextUV = m_rProgress;
	m_pRects[I3UI_PROGRESSBAR_SHAPE_BAR].right = size * m_rProgress;

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIProgressBarImage::UpdateRenderObjects()
{
	OnUpdateTextPos();

	INT32 i = 0;
	if( m_pTemplate == nullptr || g_pUIRenderer == nullptr)
		return;

	if( isEnable() == false || isVisible() == false)
		return;

	if( m_hSprites == nullptr)
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


			if( g_pUIRenderer->getEnableSprite( m_hSprites[i]))
			{
				// Texture의  UV값을 설정합니다.
				REAL32 offX2, offY2;
				REAL32 baseX, baseY, r;

				r = 1.0f / I3VT_VTEX_SIZE;
				baseX = (REAL32) pShape->getTexCoordOffset()->x;
				baseY = (REAL32) pShape->getTexCoordOffset()->y;

				if( !m_bUseCustomTexCoord)
				{
					REAL32 u1 = baseX + pShape->getCoordU();
					REAL32 v1 = baseY + pShape->getCoordV();
					REAL32 u2 = 0.0f;
					REAL32 v2 = 0.0f;

					if (i == (INT32)I3UI_PROGRESSBAR_SHAPE_BAR)
					{
						u2 = u1 + pShape->getCoordW()*m_rTextUV;
						v2 = v1 + pShape->getCoordH()*m_rTextUV;
					}
					else
					{
						u2 = u1 + pShape->getCoordW();
						v2 = v1 + pShape->getCoordH();
					}


					// 어서트 대신 메세지 박스를 띄움
					// Cancel시에 값 0으로 강제 재설정...
					//I3ASSERT( pShape->getCoordU() > -1);
					//I3ASSERT( pShape->getCoordV() > -1);
					if (pShape->getCoordU() < 0 || pShape->getCoordV() < 0)
					{
						char szTemp[1024];
						char szPath[ MAX_PATH] = {0,};
						if( pShape->hasTexture())
							i3::safe_string_copy( szPath, pShape->getTextureResPath(), MAX_PATH);
						sprintf( szTemp, "## %s: U:%.4f V:%.4f \n## Tex UVCoord Setting Error \n## if you are Click Cancel Button, Set Adjust Zero", szPath,pShape->getCoordU(),pShape->getCoordV());
						int nFunc = ::MessageBox( NULL,szTemp, "Error", MB_RETRYCANCEL );

						if (nFunc == IDCANCEL)
						{
							if (pShape->getCoordU() < 0)
								pShape->setCoordU(0);
							if (pShape->getCoordV() < 0 )
								pShape->setCoordV(0);
						}
					}

					g_pUIRenderer->setTexCoord( m_hSprites[i], u1 * r, v1 * r, u2 * r, v2 * r);
				}

				offX2 = pShape->getTexCoordOffset()->x + pShape->getCoordW();
				offY2 = pShape->getTexCoordOffset()->y + pShape->getCoordH();

				g_pUIRenderer->setTexRange( m_hSprites[i], baseX * r, baseY * r, offX2 * r, offY2 * r);
				//g_pUIRenderer->setTexRange( m_hSprites[i], baseX, baseY, offX2, offY2);

				g_pUIRenderer->setColor( m_hSprites[i], &m_Color);

				CalcAlignedPos( pRect, &Position);

				g_pUIRenderer->setPos( m_hSprites[i], &Position);
				g_pUIRenderer->setSizeAndRotate( m_hSprites[i], pRect->right, pRect->bottom, pShape->getRotation());
			}
		}

		removeState( I3UI_STATE_INVALIDSPRITE);
	}
}

void i3UIProgressBarImage::setProgress( REAL32 progress)
{
	if( i3Math::isAlike( m_rProgress, progress, I3_EPS) == false )
	{
		//temporary..
		Notify( I3UI_EVT_CHANGED, (INT32)(m_rProgress * 1000), (INT32)(progress * 1000) );	//param: 이전값, 바뀐값

		m_rProgress = progress; 

		_UpdateShapeState();
	}
}

void i3UIProgressBarImage::OnUpdate( REAL32 rDeltaSeconds)
{
	i3UICaptionControl::OnUpdate( rDeltaSeconds);

	if( m_bAnimating && isEnable() && isReadyToShow())
	{
		REAL32 de = m_rValue - m_rOldValue;

		m_rAnimateTime += rDeltaSeconds * m_rAnimateSlop;
		if( m_rAnimateTime > 1.f)
			m_rAnimateTime = 1.f;

		setProgress( m_rOldValue + (de * m_rAnimateTime));
	}
}

void i3UIProgressBarImage::OnEnabled( bool bEnable)
{
	i3UICaptionControl::OnEnabled( bEnable);
}

void i3UIProgressBarImage::OnLButtonDown( UINT32 nFlag, POINT point)
{
	if( !m_bReadOnly)
	{
		m_bDragging = true;

		if( point.x <= m_AccPos.x + m_pRects[0].right)
			m_rProgress = 0.0f;
		else if( point.x >= m_AccPos.x + m_pRects[0].right + m_pRects[1].right)
			m_rProgress = 1.0f;
		else
			m_rProgress = (point.x - m_AccPos.x - m_pRects[0].right) / m_pRects[1].right;

		setProgress( m_rProgress);
	}

	i3UICaptionControl::OnLButtonDown( nFlag, point);
}

void i3UIProgressBarImage::OnLButtonUp( UINT32 nFlag, POINT point)
{
	if( !m_bReadOnly)
	{
		m_bDragging = false;
	}

	i3UICaptionControl::OnLButtonUp( nFlag, point);
}

void i3UIProgressBarImage::OnMouseMove( UINT32 nFlag, POINT point)
{
	if( !m_bReadOnly)
	{
		if( m_bDragging)
		{
			if( point.x <= m_AccPos.x + m_pRects[0].right)
				m_rProgress = 0.0f;
			else if( point.x >= m_AccPos.x + m_pRects[0].right + m_pRects[1].right)
				m_rProgress = 1.0f;
			else
				m_rProgress = (point.x - m_AccPos.x - m_pRects[0].right) / m_pRects[1].right;

			setProgress( m_rProgress);
		}
	}

	i3UICaptionControl::OnMouseMove( nFlag, point);
}

INT32 i3UIProgressBarImage::OnCommand(LuaState * pState)
{
	char szCommand[ 260];

	if( i3Lua::GetStringArg( pState, 2, szCommand, sizeof( szCommand)) == nullptr)
		return 0;

	if( i3::generic_is_niequal( szCommand, "Increase", 8) )
	{
		setProgress( MINMAX( 0.0f, m_rProgress + 0.1f, 1.0f));
		setModifyState(true);
	}
	else if( i3::generic_is_niequal( szCommand, "Decrease", 8) )
	{
		setProgress( MINMAX( 0.0f, m_rProgress - 0.1f, 1.0f));
		setModifyState(true);
	}
	else if( i3::generic_is_niequal( szCommand, "SetValue", 8) )
	{
		REAL32 value = (REAL32)i3Lua::GetIntegerArg( pState, 3, 0);

		setProgress( MINMAX(0.0f, value, 1.0f));
		setModifyState(true);
	}

	return 1;
}

void i3UIProgressBarImage::OnSize( REAL32 cx, REAL32 cy)
{
	i3UICaptionControl::OnSize( cx, cy);

	addState( I3UI_STATE_NEED_UPDATE);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

#define I3UI_PROGRESS_STYLE_READONLY	0x0001
#define I3UI_PROGRESS_STYLE_ANIMATE		0x0002

namespace pack
{
	struct ALIGN4 UIPROGRESSBAR
	{
		UINT8		m_ID[4] = { 'U', 'P', 'B', '1' };
		UINT32		m_Style = 0;
		REAL32		m_AnimateSlop = 0.0f;
		UINT32		pad[30] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3UIProgressBarImage::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UIPROGRESSBAR data;

	Result = i3UICaptionControl::OnSave( pResFile);
	I3_CHKIO( Result);
	
	if( getReadOnly())
		data.m_Style |= I3UI_PROGRESS_STYLE_READONLY;

	if( isAnimating())
	{
		data.m_Style |= I3UI_PROGRESS_STYLE_ANIMATE;
		data.m_AnimateSlop = getAnimateSlop();
	}

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);

	Result += Rc;

	return Result;
}

UINT32 i3UIProgressBarImage::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UIPROGRESSBAR data;

	Result = i3UICaptionControl::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	setReadOnly( (data.m_Style & I3UI_PROGRESS_STYLE_READONLY)?true:false);
	setAnimate( (data.m_Style & I3UI_PROGRESS_STYLE_ANIMATE)?true:false);
	setAnimateSlop( data.m_AnimateSlop);

	return Result;
}

bool i3UIProgressBarImage::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;

	INT32 temp = m_bReadOnly?1:0;
	pXML->addAttr( "ReadOnly", temp);

	temp = m_bAnimating?1:0;
	pXML->addAttr( "Animate", temp);
	pXML->addAttr( "AnimateSlop", m_rAnimateSlop);

	return bResult;
}

bool i3UIProgressBarImage::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);
	if( bResult == false)
		return false;

	INT32 temp;
	pXML->getAttr( "ReadOnly", &temp);
	m_bReadOnly = temp?true:false;

	pXML->getAttr( "Animate", &temp);
	m_bAnimating = temp?true:false;

	pXML->getAttr( "AnimateSlop", &m_rAnimateSlop);

	return bResult;
}

void i3UIProgressBarImage::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UICaptionControl::CopyTo( pDest, method );

	I3ASSERT( i3::same_of<i3UIProgressBarImage*>(pDest));

	i3UIProgressBarImage* pProgBar = (i3UIProgressBarImage*)pDest;

	pProgBar->setProgress( getProgress());
	pProgBar->setReadOnly( getReadOnly());
	pProgBar->setAnimate( isAnimating());
	pProgBar->setAnimateSlop( getAnimateSlop());

}

#if defined( I3_DEBUG)
void i3UIProgressBarImage::GetTDKGridProperties(i3::vector<PROPERTYINFO*>& List)
{
	i3UICaptionControl::GetTDKGridProperties( List);	

	AddMFCProperty_BeginGroup( List, "i3UIProgressBarImage");
	AddMFCProperty_Real32( List, &m_rProgress, "Initial Value", nullptr, 0.0f, 1.0f);
	AddMFCProperty_Bool( List, &m_bReadOnly, "ReadOnly");
	AddMFCProperty_Bool( List, &m_bAnimating, "Animate");
	AddMFCProperty_Real32( List, &m_rAnimateSlop, "Animate Slop", nullptr);
	AddMFCProperty_EndGroup( List);
}

void i3UIProgressBarImage::OnTDKPropertyChanged( const char* szName)
{
	if( i3::generic_is_iequal( szName, "Initial Value") )
	{
		setProgress( m_rProgress);
	}
	else
	{
		i3UICaptionControl::OnTDKPropertyChanged( szName);
	}
}
#endif	// I3_DEBUG

//////////////////////////////////////////////////////////////////////////////////
extern "C" int i3UIPImageBar_setValue(lua_State *L)
{
	i3UIProgressBarImage * pCtrl = (i3UIProgressBarImage*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		REAL32 fValue = (REAL32)i3Lua::GetNumberArg( L, 2, 0);
		if( pCtrl->isAnimating())
			pCtrl->setValue( fValue);
		else
			pCtrl->setProgress( fValue);
	}

	return 0;
}

extern "C" int i3UIPImageBar_getValue(lua_State *L)
{
	i3UIProgressBarImage * pCtrl = (i3UIProgressBarImage*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushNumber( L, pCtrl->getProgress());
	}

	return 1;
}

extern "C" int i3UIPImageBar_setReadOnly( lua_State* L)
{
	i3UIProgressBarImage * pCtrl = (i3UIProgressBarImage*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 nEnable = i3Lua::GetIntegerArg( L, 2, 0);
		pCtrl->setReadOnly( (nEnable == 0) ? false : true );
	}

	return 0;
}

extern "C" int i3UIPImageBar_getReadOnly( lua_State* L)
{
	i3UIProgressBarImage* pCtrl = (i3UIProgressBarImage*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		bool bReadOnly = pCtrl->getReadOnly();
		i3Lua::PushInteger( L, (bReadOnly == false) ? 0 : 1 );
	}

	return 1;
}

LuaFuncReg i3UIProgressImageGlue[] = 
{
	{"uiPImageBar_setValue",			i3UIPImageBar_setValue},
	{"uiPImageBar_getValue",			i3UIPImageBar_getValue},	

	{"uiPImageBar_setReadOnly",		i3UIPImageBar_setReadOnly},
	{"uiPImageBar_getReadOnly",		i3UIPImageBar_getReadOnly},

	{nullptr, nullptr}
};

void i3UIProgressBarImage::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIProgressImageGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIProgressImageGlue[i].name, i3UIProgressImageGlue[i].func);
	}
}