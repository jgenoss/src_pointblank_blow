#include "i3GfxType.h"

#ifdef I3G_OGLES
#include "i3GfxOGLES.h"
#include "i3RenderContext.h"
#include "i3GfxDefineOGLES.h"
#include "i3GfxUtilOGLES.h"
#include "i3GfxGlobalVariable.h"
#include "i3TextureOGLES.h"
#include "i3VertexArrayOGLES.h"

#ifdef I3_WIPI_KTF
	DEFINE_GP_GLES_INTERFACE;
	#ifdef I3_WIPI_GP
		DEFINE_GP_GRAPHIC_INTERFACE;
	#endif
#endif

#ifdef I3_DEBUG
	#if defined( I3_WIPI_GIGA) || defined( I3_WIPI_GP)
		#ifndef I3G_WIPI2D
			#define I3G_WIPI2D
			#include "i3GfxWIPI2D.h"
		#endif
	#endif	
#endif

I3_CLASS_INSTANCE( i3RenderContext, i3ElementBase);

/*
#ifdef DEBUG_DISP
	#define	__TRACE I3TRACE
#else
	#if defined( I3_COMPILER_VC)
		#define __TRACE	__noop
	#else
		#define __TRACE(...)	(void)0
	#endif
#endif
*/

#define SCR_WIDTH			800
#define	SCR_HEIGHT			480

// ============================================================================
//
// i3RenderContext
//
// ============================================================================
i3RenderContext::i3RenderContext(void)
{
	m_pSetupParam = NULL;
	m_pRenderTarget = NULL;

	m_bWindowed = FALSE;
	m_bReady = FALSE;

	m_pCurrentVA = NULL;
	m_pCurrentIA = NULL;

	m_ClearFlag = I3G_CLEAR_COLOR;
	i3Color::Set( &m_BackColor, 0, 0, 0, 255);
	m_ClearZValue = 1.0f;
	m_ClearStencilValue = 0;

	m_VertexBlendWeightCount = 0;

	m_pCaps = NULL;

	m_pPerfReport = NULL;
	
	m_DrawCallCount = 0;
	m_TriangleCount = 0;

	m_bConstantPointSpriteSizeEnable	=	FALSE;
	m_fConstantPointSpriteSize	=	1.0f;
	m_PointSpriteSizeSpace		=	I3G_POINT_SPRITE_SIZE_SPACE_CAMERA;

	m_bProjectiveTexture = FALSE;

	g_pRenderContext = this;

	m_SrcBlendMask = 0xFF;
	m_DestBlendMask = 0xFF;
	
	//
	m_TexFunc = 0;
	m_OldTexFunc = -1;

	m_AlphaFunc = I3G_COMP_GREATER;
	m_AlphaRef = 0;

	INT32 i, n;
	i3Matrix::Identity( &m_LastViewMatrix);
	i3Matrix::Identity( &m_LastProjectMatrix);
	i3Matrix::Identity( &m_LastWorldMatrix);

	n = sizeof(m_BoneMatrixArray) / sizeof(m_BoneMatrixArray[0]);
	for( i = 0; i < n; i++ )
	{
		i3Matrix::Identity( &m_BoneMatrixArray[i] );
	}

	n = sizeof(m_LastTextureMatrix) / sizeof(m_LastTextureMatrix[0]);
	for( i = 0; i < n; i++ )
	{
		i3Matrix::Identity( &m_LastTextureMatrix[i] );
	}

	m_pTimer = NULL;
}

i3RenderContext::~i3RenderContext(void)
{
	I3TRACE( "~i3RenderContext 1\n");
	I3_SAFE_RELEASE( m_pTimer);
	I3TRACE( "~i3RenderContext 2\n");
	I3_SAFE_RELEASE( m_pCaps);
	I3TRACE( "~i3RenderContext 3\n");
	I3_SAFE_RELEASE( m_pPerfReport);
	I3TRACE( "~i3RenderContext 4\n");
	I3_SAFE_RELEASE( m_pRenderTarget);
	I3TRACE( "~i3RenderContext 5\n");

	{
		INT32 i;
		i3RenderTarget * pRT;

		for( i = 0; i < m_RenderTargetList.GetCount(); i++)
		{
			pRT = (i3RenderTarget *) m_RenderTargetList.Items[ i];

			pRT->Release();
		} 
	}
	I3TRACE( "~i3RenderContext 6\n");

#if defined( I3_WIPI_GIGA)
	OEMC_g3D_eglxDelete();
	OEMC_gCOP_Terminate();
#elif defined( I3_WIPI_KTF)
	I3OGL::eglDestroyContext( I3OGL::glGetDisplay(), I3OGL::glGetContext());
	I3OGL::eglTerminate(I3OGL::glGetDisplay());	
	#if defined( I3_WIPI_GP)
		MC_gp3DSetDisplayMode(GP3DSDM_END);
	#endif
#endif
	I3TRACE("i3RenderContext::Destructor called\n");
} 
 
bool i3RenderContext::Create( i3VideoInfo * pVFormat, void * pSetupParam)
{
	I3TRACE( "ENTER > i3RenderContext::Create()\n" );
	m_pSetupParam = pSetupParam;

	I3TRACE( "I3OGL::glInit();\n" );
	
#if defined( I3_WIPI_KTF)
	#if defined( I3_WIPI_GP)
	// G1000 디스플레이어 생성
		if(!LOAD_GP_GLES_INTERFACE || !LOAD_GP_GRAPHIC_INTERFACE)
	#else
	// KV3600 디스플레이어 생성
		if(!LOAD_GP_GLES_INTERFACE)
	#endif
		{
			i3Log( "i3RenderContext::Create()", "Initialize Interface failed\n");
			return false;
		}
#elif defined( I3_WIPI_GIGA)
	OEMC_gCOP_Initialize();
#endif

	I3OGL::eglCreate( pVFormat->m_Width, pVFormat->m_Height, 0);

//	SetViewport( 0, 0, 800, 480 );

	m_LCD = I3OGL::glGetDisplay();
	if( m_LCD == EGL_NO_DISPLAY)
	{
		i3Log( "i3RenderContext::Create()", "eglGetDisplay() failed\n");
		return false;
	}

	// I3OGL::eglxCreate() 함수에서 내부적으로 처리하기 때문에
	// 호출할 필요가 없다.
	//
	// I3OGL::eglInitialize( m_LCD, &major, &minor);

#if defined( I3_DEBUG) && defined( I3G_WIPI2D)
	I3WIPI2D::InitContext( &m_grpContext);
#endif

	#if 0
	{
		const char * pszStr;
		#if defined( I3_DEBUG)
			pszStr = I3OGL::eglQueryString( m_LCD, EGL_VENDOR);
			I3TRACE1( "* EGL Vendor : %s\n", pszStr);

			pszStr = I3OGL::eglQueryString( m_LCD, EGL_VERSION);
			I3TRACE1( "* EGL Version : %s\n", pszStr);

			pszStr = I3OGL::eglQueryString( m_LCD, EGL_EXTENSIONS);
			I3TRACE1( "* EGL Extensions : %s\n", pszStr);				
		#endif
	}
	#endif

	// Device Capacity Information
	{
		m_pCaps = i3GfxCaps::NewObject();
		I3ASSERT( m_pCaps != NULL);
	}

	// Render Target
	{
		i3RenderTarget * pNewRT;

		pNewRT = i3RenderTarget::NewObject();

		pNewRT->Create( pVFormat->m_Width, pVFormat->m_Height, pVFormat->m_RenderTargetFormat);

		m_RenderTargetList.Add( pNewRT);
	}

	// Timer
	{
		m_pTimer = i3Timer::NewObject();
		I3ASSERT( m_pTimer != NULL);
	}

	// Performance Report
	{
		m_pPerfReport = i3GfxPerformanceReport::NewObject();
		I3ASSERT( m_pPerfReport != NULL);
	}

	I3TRACE( "I3OGL::glViewport( 2048, 2048, GL_SCR_WIDTH, GL_SCR_HEIGHT);\n" );
	//OGL_CHECK( I3OGL::glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT));

	m_ClearFlag = I3G_CLEAR_COLOR | I3G_CLEAR_DEPTH;
	
	SetClearColor( &m_BackColor);
	SetClearZValue( m_ClearZValue);

	OGL_CHECK( I3OGL::glScissor(0, 0, pVFormat->m_Width, pVFormat->m_Height));
	OGL_CHECK( I3OGL::glEnable( GL_SCISSOR_TEST));

	OGL_CHECK( I3OGL::glDisable( GL_DITHER));
	OGL_CHECK( I3OGL::glDisable( GL_MULTISAMPLE));

	#if defined( I3_DEBUG)
		ShowFPS(1);
	#endif

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// 여기서 부터는 DIsplay List를 사용한다.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Set Default Render States
	{
		Begin();
		Clear();	// 버퍼 내용을 한번 지워준다
		SetDefaultRenderState();
		End();
	}

    // The app is ready to go
    m_bReady = TRUE;

	I3TRACE( "EXIT > i3RenderContext::Create\n" );
	return true;
}

bool i3RenderContext::Begin(void)
{
	return true;
}

void i3RenderContext::End(void)
{
#if defined( I3_DEBUG) && defined( I3G_WIPI2D)
	if( m_bShowFPS )
	{
		char conv[64];
		sprintf( (PSTR)conv, (PSTR) "FPS:%.1f", m_pPerfReport->m_FPS);
	//	I3OGL::glDebugPrint( 400, 3, 0xFFFFFFFF, conv);
		// 디버그용 디스플레이
		I3WIPI2D::DrawString( I3WIPI2D::GetScreenFrameBuffer(0), 5, 5, conv, strlen(conv), &m_grpContext );
	}
#endif

	{
		m_pPerfReport->m_DrawCallCountPerFrame = m_DrawCallCount;
		m_pPerfReport->m_TriangleCountPerFrame = m_TriangleCount;
		m_pPerfReport->m_FrameCount++;

		m_FrameCountPerSec++;

		m_secFrameTime += m_pTimer->GetDeltaSec();

		if( m_secFrameTime >= 1.0f)
		{
			m_pPerfReport->m_FPS = (m_FrameCountPerSec / m_secFrameTime);
			m_FrameCountPerSec = 0;
			m_secFrameTime = 0.0f;
		}

		m_DrawCallCount = 0;
		m_TriangleCount = 0;
	}

	I3OGL::glFinish();
	I3OGL::glFlush();
	
#if defined( I3_WIPI_KTF)	
	I3OGL::eglSwapBuffers( I3OGL::glGetDisplay(), I3OGL::glGetWindowSurface());
	#if defined( I3_WIPI_KV3600)
		MC_grpRepaint( 0, 0, 0, 320, 240);
	#endif
#elif defined( I3_WINCE )
	I3OGL::eglSwapBuffers( I3OGL::glGetDisplay(), I3OGL::glGetWindowSurface());
#endif


}

bool i3RenderContext::BeginRender(void)
{
	return true;
}

void i3RenderContext::EndRender(void)
{
    // End the scene.
}

#define Float2Fxd(X) ((GLfixed)(((X) * 65536.0f) + 0.5f))

void i3RenderContext::Clear(I3G_CLEAR AddMask, I3G_CLEAR RemoveMask)
{
	I3G_CLEAR flag;

	flag = (m_ClearFlag | AddMask);
	flag = flag & ~(RemoveMask);

#if defined( I3_WIPI_SKT_SV360) || defined( I3_WIPI_KTF)
	SetZWriteEnable( TRUE);		//	by KOMET (LG-SV360 문제 해결)
	SetZTestEnable( FALSE);
#endif

	//::glClearColorx(0, 0, Float2Fxd(1.0f), Float2Fxd(1.0f));
	OGL_CHECK( I3OGL::glClear( flag));

#if defined( I3_WIPI_SKT_SV360) || defined( I3_WIPI_KTF)
	SetZWriteEnable( FALSE);
	SetZTestEnable( TRUE);
#endif
}

/*
inline
void i3RenderContext::UpdateTextureStageState( BOOL bIsVertexColor )
{
	INT32 TexFunc[2][2][2] =
	{
		// Light Off
		{
			// Texture Off
			{
				// Vertex Color Off
				// TEC
				i3PSPGfx::TFUNC_BLEND,

				// Vertex Color On
                // Diffuse * TEC
				// No Available Method. Use Diffuse
				i3PSPGfx::TFUNC_MODULATE
			},
			// Texture On
			{
				// Vertex Color Off
				// Texture * TEC
				//i3PSPGfx::TFUNC_BLEND, // <= 이게 맞음. 그러나, 일부 월드가 이상하게 나오고 기대했던 효과와 차이가 많이 남. -_-;
				i3PSPGfx::TFUNC_MODULATE,

				// Vertex Color On
				// TEC * Diffuse
				i3PSPGfx::TFUNC_MODULATE
			},
		},
		// Light On
		{
			// Texture Off
			{
				// Vertex Color Off
				// Diffuse
				i3PSPGfx::TFUNC_MODULATE,

				// Vertex Color On
				// Diffuse
				i3PSPGfx::TFUNC_MODULATE
			},
			// Texture On
			{
				// Vertex Color Off
				// Texture * Diffuse
				i3PSPGfx::TFUNC_MODULATE,

				// Vertex Color On
				//Texture * Diffuse
				i3PSPGfx::TFUNC_MODULATE
			},
		}
	};

	m_TexFunc = TexFunc[ m_bLightingEnable ][ m_bTextureEnable ][ bIsVertexColor ];

	if( m_TexFunc == m_OldTexFunc )
		return;	

	I3OGL::glTexFunc( m_TexFunc, GL_RGB );

	m_OldTexFunc = m_TexFunc;
}
*/

void i3RenderContext::SetVertexArray( i3VertexArray * pVA)
{
	m_pCurrentVA = pVA;
}

void i3RenderContext::SetIndexArray( i3IndexArray * pIA)
{
	m_pCurrentIA = pIA;
}

void i3RenderContext::SetMatrix( I3G_MATRIX_STACK Index, RT_MATRIX * pMatrix)
{
	switch( Index)
	{
		case I3G_MATRIX_VIEW :
			i3Matrix::Copy( &m_LastViewMatrix, pMatrix);
			break;

		case I3G_MATRIX_PROJECTION :
			{
				i3Matrix::Copy( &m_LastProjectMatrix, pMatrix);	
				
				OGL_CHECK( I3OGL::glMatrixMode( GL_PROJECTION));
				
				OGL_CHECK( I3OGL::glLoadMatrix( pMatrix));				
			}
			
			break;

		case I3G_MATRIX_TEXTURE0 : 
		case I3G_MATRIX_TEXTURE1 :
			i3Matrix::Copy( &m_LastTextureMatrix[0], pMatrix);	

			OGL_CHECK( I3OGL::glMatrixMode( GL_TEXTURE));

			OGL_CHECK( I3OGL::glLoadMatrix(pMatrix));
			break;

		case I3G_MATRIX_WORLD :			
			i3Matrix::Copy( &m_LastWorldMatrix, pMatrix);

			OGL_CHECK( I3OGL::glMatrixMode( GL_MODELVIEW));
			
			#if defined( I3_FIXED_POINT)
				FIXED32_MATRIX mv;
				i3Matrix::Mul( &mv, pMatrix, &m_LastViewMatrix);
			#else
				MATRIX mv;
				MATRIX mv2;
				//i3Matrix::Mul( &mv2, &m_LastViewMatrix, &m_LastProjectMatrix);
				i3Matrix::Mul( &mv, pMatrix, &m_LastViewMatrix);
			//	i3Matrix::Mul( &mv, &mv2, &m_LastProjectMatrix);

				//i3Matrix::Mul( &mv, pMatrix, &m_LastViewMatrix);
			#endif
			OGL_CHECK( I3OGL::glLoadMatrix( &mv));
			break;

		default :
			I3TRACE1( "Invalid Matrix Index : %d\n", Index);
			break;
	}
}

void i3RenderContext::GetMatrix( I3G_MATRIX_STACK Index, RT_MATRIX * pMatrix)
{
	switch( Index)
	{
		case I3G_MATRIX_VIEW :			i3Matrix::Copy( pMatrix, &m_LastViewMatrix);	break;
		case I3G_MATRIX_PROJECTION :	i3Matrix::Copy( pMatrix, &m_LastProjectMatrix);	break;
		case I3G_MATRIX_TEXTURE0 :		i3Matrix::Copy( pMatrix, &m_LastTextureMatrix[0]);	break;
		case I3G_MATRIX_WORLD :			i3Matrix::Copy( pMatrix, &m_LastWorldMatrix);	break;
		default :
			I3TRACE1( "GetMatrix() : Invalid Matrix Index : %d\n", Index);
			break;
	}
}

void i3RenderContext::SetViewport( INT32 x, INT32 y, INT32 Width, INT32 Height)
{
	i3RenderTarget * pTarget = GetRenderTarget( 0);

	if((Width <= 0) || (Height <= 0))
	{
		Width = pTarget->GetWidth();
		Height = pTarget->GetHeight();
	}

	// OpenGL에서의 Viewport는 y값이 Viewport의 바닥값을 가리킵니다.
	OGL_CHECK( I3OGL::glViewport( x, (pTarget->GetHeight() - Height)-y, Width, Height));
}

inline void _UpdateVertexArray( i3VertexArrayOGLES * pVA, UINT32 offset)
{
	char * p;

	// Normal
	if( pVA->GetFormat()->GetHasNormal())
	{
		p = ((char *)pVA->GetNormalPointer() + (pVA->GetNormalStride() * offset));

		OGL_CHECK( I3OGL::glEnableClientState( GL_NORMAL_ARRAY));
		OGL_CHECK( I3OGL::glNormalPointer( GL_FIXED, 0, p));
	}
	else
	{
		OGL_CHECK( I3OGL::glDisableClientState( GL_NORMAL_ARRAY));
	}

	// Texture Coord.
	if( pVA->GetFormat()->GetTextureCoordSetCount() > 0)
	{
		p = ((char *)pVA->GetTexCoordPointer() + (pVA->GetTexCoordStride() * offset));
		OGL_CHECK( I3OGL::glEnableClientState( GL_TEXTURE_COORD_ARRAY));
		OGL_CHECK( I3OGL::glTexCoordPointer( 2, GL_FIXED, 0, p));
	}
	else
	{
		OGL_CHECK( I3OGL::glDisableClientState( GL_TEXTURE_COORD_ARRAY));
	}

	// Color
	if( pVA->GetFormat()->GetHasColor())
	{
		p = ((char *)pVA->GetColorPointer() + (pVA->GetColorStride() * offset));

		OGL_CHECK( I3OGL::glEnableClientState( GL_COLOR_ARRAY));
		OGL_CHECK( I3OGL::glColorPointer( 4, GL_UNSIGNED_BYTE, 0, p));
	}
	else
	{
		OGL_CHECK( I3OGL::glDisableClientState( GL_COLOR_ARRAY));
	}

	// Position
	if( pVA->GetFormat()->GetHasPosition())
	{
		p = ((char *)pVA->GetPositionPointer() + (pVA->GetPositionStride() * offset));

		OGL_CHECK( I3OGL::glEnableClientState( GL_VERTEX_ARRAY));

		if( pVA->GetFormat()->IsTransformed())
		{
			OGL_CHECK( I3OGL::glVertexPointer( 2, GL_FIXED, 0, p));
		}
		else
		{
			OGL_CHECK( I3OGL::glVertexPointer( 3, GL_FIXED, 0, p));
		}
	}
	else
	{
		OGL_CHECK( I3OGL::glDisableClientState( GL_VERTEX_ARRAY));
	}
}

void i3RenderContext::DrawPrim( I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 PrimCount)
{
	int vert_count;

	I3ASSERT( m_pCurrentVA);

	m_DrawCallCount++;
	m_TriangleCount += PrimCount;

	_UpdateVertexArray( (i3VertexArrayOGLES *) m_pCurrentVA, 0);

	switch( Prim)
	{
		case I3G_PRIM_POINTLIST :		vert_count = PrimCount;						break;
		case I3G_PRIM_LINELIST :		vert_count = PrimCount << 1;				break;
		case I3G_PRIM_LINESTRIP :		vert_count = PrimCount + 1;					break;
		case I3G_PRIM_TRILIST :			vert_count = PrimCount * 3;					break;
		case I3G_PRIM_TRISTRIP :
		case I3G_PRIM_TRIFAN :			vert_count = PrimCount + 2;					break;
		default :
			i3Log( "i3RenderContext::DrawPrim", "Invalid Primitive Type : %d\n", Prim);
			vert_count = 0;
			break;
	}

	OGL_CHECK( I3OGL::glDrawArrays( (int)Prim, StartIdx, vert_count));

}

void i3RenderContext::DrawStripPrim( I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 * pPrimCount, UINT32 Count)
{
	UINT32 vert_count, i, Accm = 0;

	I3ASSERT( m_pCurrentVA);

	for( i = 0; i < Count; i++)
	{
		m_DrawCallCount++;
		m_TriangleCount += *pPrimCount;

		switch( Prim)
		{
			case I3G_PRIM_POINTLIST :		vert_count = *pPrimCount;					break;
			case I3G_PRIM_LINELIST :		vert_count = (*pPrimCount) << 1;			break;
			case I3G_PRIM_LINESTRIP :		vert_count = (*pPrimCount) + 1;				break;
			case I3G_PRIM_TRILIST :			vert_count = (*pPrimCount) * 3;				break;
			case I3G_PRIM_TRISTRIP :
			case I3G_PRIM_TRIFAN :			vert_count = (*pPrimCount) + 2;				break;
			default :
				i3Log( "i3RenderContext::DrawPrim", "Invalid Primitive Type : %d\n", Prim);
				vert_count = 0;
				break;
		}

		_UpdateVertexArray( (i3VertexArrayOGLES *) m_pCurrentVA, Accm);

		OGL_CHECK( I3OGL::glDrawArrays( (int)Prim, StartIdx, vert_count));

		Accm += vert_count;

		pPrimCount++;
	}

}

void i3RenderContext::DrawIndexedPrim( I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 PrimCount)
{
	UINT32 vert_count;
	char * pIndex;

	I3ASSERT( m_pCurrentVA != NULL);
	I3ASSERT( m_pCurrentIA != NULL);

	pIndex = (char *) m_pCurrentIA->GetInstance();
	pIndex += (StartIdx * sizeof(UINT16));

	m_DrawCallCount++;
	m_TriangleCount += PrimCount;

	switch( Prim)
	{
		case I3G_PRIM_POINTLIST :		vert_count = PrimCount;						break;
		case I3G_PRIM_LINELIST :		vert_count = PrimCount << 1;				break;
		case I3G_PRIM_LINESTRIP :		vert_count = PrimCount + 1;					break;
		case I3G_PRIM_TRILIST :			vert_count = PrimCount * 3;					break;
		case I3G_PRIM_TRISTRIP :
		case I3G_PRIM_TRIFAN :			vert_count = PrimCount + 2;					break;
		default :
			i3Log( "i3RenderContext::DrawPrim", "Invalid Primitive Type : %d\n", Prim);
			vert_count = 0;
			break;
	}

	_UpdateVertexArray( (i3VertexArrayOGLES *) m_pCurrentVA, 0);

	OGL_CHECK( I3OGL::glDrawElements( (int)Prim, vert_count, GL_UNSIGNED_SHORT, pIndex));

}

void i3RenderContext::DrawStripIndexedPrim( I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 * pPrimCount, UINT32 Count)
{
	UINT32 vert_count, i;
	UINT16 * pIndex;

	I3ASSERT( m_pCurrentVA != NULL);
	I3ASSERT( m_pCurrentIA != NULL);

	pIndex = (UINT16 *) m_pCurrentIA->GetInstance();
	pIndex += StartIdx;

	m_DrawCallCount += Count;

	_UpdateVertexArray( (i3VertexArrayOGLES *) m_pCurrentVA, 0);

	for( i = 0; i < Count; i++)
	{
		m_TriangleCount += pPrimCount[i];

		switch( Prim)
		{
			case I3G_PRIM_POINTLIST :		vert_count = pPrimCount[i];						break;
			case I3G_PRIM_LINELIST :		vert_count = pPrimCount[i] << 1;				break;
			case I3G_PRIM_LINESTRIP :		vert_count = pPrimCount[i] + 1;					break;
			case I3G_PRIM_TRILIST :			vert_count = pPrimCount[i] * 3;					break;
			case I3G_PRIM_TRISTRIP :
			case I3G_PRIM_TRIFAN :			vert_count = pPrimCount[i] + 2;					break;
			default :
				i3Log( "i3RenderContext::DrawPrim", "Invalid Primitive Type : %d\n", Prim);
				vert_count = 0;
				break;
		}

		OGL_CHECK( I3OGL::glDrawElements( (int)Prim, vert_count, GL_UNSIGNED_SHORT, pIndex));

		pIndex += vert_count;
	}

}

void i3RenderContext::DrawCommandList( i3GPUCommandList * pCmdList)
{
}

void i3RenderContext::DrawPointSprite( UINT32 StartIdx, UINT32 PrimCount)
{
}

void i3RenderContext::SetCurrentRenderTarget( i3RenderTarget * /*pRT*/)
{
}

void i3RenderContext::SetCurrentRenderTarget( i3Texture * pTex)
{
}


BOOL i3RenderContext::SetRenderTargetSize( UINT32 cx, UINT32 cy)
{
	return TRUE;
}

void i3RenderContext::SetClearColor( COLOR * pColor)
{
	i3Color::Set( &m_BackColor, pColor);

	{
#ifdef I3_FIXED_POINT
		FIXED32_COLORREAL col;
		i3Color::Copy( &col, pColor);
#else
		COLORREAL col;
		i3Color::Set( &col, pColor); 
#endif
		OGL_CHECK( I3OGL::glClearColor( col.r, col.g, col.b, col.a));
	}
}

void i3RenderContext::SetClearZValue( REAL32 val)
{
	m_ClearZValue = val;

#ifdef I3_FIXED_POINT
	OGL_CHECK( I3OGL::glClearDepth( i3FXD::FromReal( val)));
#else
	OGL_CHECK( I3OGL::glClearDepth( val));
#endif
}

void i3RenderContext::SetClearStencilValue( UINT8 val)
{
	m_ClearStencilValue = val;

	// I3OGL::glClearDepthx( i3FXD::From
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Render State


void i3RenderContext::SetAlphaTestEnable( BOOL flag)
{
	if( flag)
	{
		OGL_CHECK( I3OGL::glEnable( GL_ALPHA_TEST));
	}
	else
	{
		OGL_CHECK( I3OGL::glDisable( GL_ALPHA_TEST));
	}

	m_bAlphaTestEnable = flag;
}

BOOL i3RenderContext::GetAlphaTestEnable(void)
{
	return m_bAlphaTestEnable;
}

void i3RenderContext::SetBlendEnable( BOOL flag)
{
	if( flag)
	{
		OGL_CHECK( I3OGL::glEnable( GL_BLEND));
	}
	else
	{
		OGL_CHECK( I3OGL::glDisable( GL_BLEND));
	}

	m_bBlendEnable = flag;
}

BOOL i3RenderContext::GetBlendEnable(void)
{
	return m_bBlendEnable;
}

void i3RenderContext::SetAlphaRefValue( UINT8 val)
{
	m_AlphaRef = val;

#ifdef I3_FIXED_POINT
	FIXED32 ref = i3FXD::FromReal( val * (1.0f / 255.0f));
#else
	REAL32 ref = (REAL32)( val * (1.0f / 255.0f));
#endif

	OGL_CHECK( I3OGL::glAlphaFunc( m_AlphaFunc, ref));
}

UINT8 i3RenderContext::GetAlphaRefValue(void)
{
	return m_AlphaRef;
}

void i3RenderContext::SetAlphaFunction( I3G_COMP_FUNC func)
{
	m_AlphaFunc = func;

#ifdef I3_FIXED_POINT
	FIXED32 ref = i3FXD::FromReal( m_AlphaRef * (1.0f / 255.0f));
#else
	REAL32 ref = (REAL32)( m_AlphaRef * ( 1.0f / 255.0f ));
#endif

	OGL_CHECK( I3OGL::glAlphaFunc( m_AlphaFunc, ref));
}

I3G_COMP_FUNC i3RenderContext::GetAlphaFunction(void)
{
	return m_AlphaFunc;
}

// Z-Buffer
void i3RenderContext::SetZTestEnable( BOOL flag)
{
	if( flag)
	{
		OGL_CHECK( I3OGL::glEnable( GL_DEPTH_TEST));
	}
	else
	{
		OGL_CHECK( I3OGL::glDisable( GL_DEPTH_TEST));
	}

	m_bZTestEnable = flag;
}

BOOL i3RenderContext::GetZTestEnable(void)
{
	return m_bZTestEnable;
}

void i3RenderContext::SetZWriteEnable( BOOL flag)
{
	OGL_CHECK( I3OGL::glDepthMask( flag));

	m_bZWriteEnable = flag;
}

BOOL i3RenderContext::GetZWriteEnable(void)
{
	return m_bZWriteEnable;
}

void i3RenderContext::SetZFunction( I3G_COMP_FUNC	func)
{
	m_ZFunc = func;

	OGL_CHECK( I3OGL::glDepthFunc( func));
}

I3G_COMP_FUNC i3RenderContext::GetZFunction(void)
{
	return m_ZFunc;
}

// Render Parameters
void i3RenderContext::SetRenderMode( I3G_RENDER_MODE	mode)
{
	// PSP는 Solid Ploygon으로 Rendering 하는 것만 지원한다.
}

I3G_RENDER_MODE i3RenderContext::GetRenderMode(void)
{
	return I3G_RENDER_POLYGON;
}

void i3RenderContext::SetShadeMode( I3G_SHADE_MODE	mode)
{
	OGL_CHECK( I3OGL::glShadeModel( mode));

	m_ShadeMode = mode;
}

I3G_SHADE_MODE i3RenderContext::GetShadeMode(void)
{
	return m_ShadeMode;
}

void i3RenderContext::SetFaceCullMode( I3G_FACE_CULL	mode)
{
	if( mode == I3G_FACE_CULL_NONE)
	{
		OGL_CHECK( I3OGL::glDisable( GL_CULL_FACE));
	}
	else
	{
		OGL_CHECK( I3OGL::glEnable( GL_CULL_FACE));
		OGL_CHECK( I3OGL::glFrontFace( mode));
	}
}

I3G_FACE_CULL i3RenderContext::GetFaceCullMode(void)
{
	return m_FaceCull;
}

#ifdef I3_PSP_DISPLAY_LIST
I3_INLINE void SetBlend( I3G_BLEND src, I3G_BLEND dst, I3G_BLEND_OP op )
{
	i3PSPGfx::BLEND_A a = (i3PSPGfx::BLEND_A)src;
	i3PSPGfx::BLEND_B b = (i3PSPGfx::BLEND_B)dst;
	i3PSPGfx::BLEND_EQU equ = (i3PSPGfx::BLEND_EQU)op;

	// 두가지 경우에 대해서만 제대로 동작하도록 임시로 설정.
	// src = I3G_BLEND_ONE, dst = I3G_BLEND_ZERO, op = I3G_BLEND_OP_ADD
	// src = I3G_BLEND_SRCALPHA, dst = I3G_BLEND_ONE_MINUS_SRCALPHA, op = I3G_BLEND_OP_ADD

	if( src == I3G_BLEND_ONE )
	{
		_setCmd( i3PSPGfx::FIXA( 255, 255, 255 ) );
        a = i3PSPGfx::BLEND_A_FIXA;
	}
	else if( src == I3G_BLEND_ZERO )
	{
		_setCmd( i3PSPGfx::FIXA( 0, 0, 0 ) );
		a = i3PSPGfx::BLEND_A_FIXA;
	}

	if( dst == I3G_BLEND_ONE )
	{
		_setCmd( i3PSPGfx::FIXB( 255, 255, 255 ) );
		b = i3PSPGfx::BLEND_B_FIXB;
	}
	else if( dst == I3G_BLEND_ZERO )
	{
		_setCmd( i3PSPGfx::FIXB( 0, 0, 0 ) );
		b = i3PSPGfx::BLEND_B_FIXB;
	}

	equ = (i3PSPGfx::BLEND_EQU)I3G_BLEND_OP_ADD;

//	{
////		I3TRACE( "SetBlend( %d, %d, %d )\n", a, b, equ );
//		a = (i3PSPGfx::BLEND_A)I3G_BLEND_SRCALPHA;
//		b = (i3PSPGfx::BLEND_B)I3G_BLEND_INVSRCALPHA;
//		equ = (i3PSPGfx::BLEND_EQU)I3G_BLEND_OP_ADD;
//		//I3ASSERT(0);
//	}
//
	_setCmd( i3PSPGfx::BLEND( a, b, equ ) );
}
#endif

// Blend State
void i3RenderContext::SetSrcBlend( I3G_BLEND	blend)
{
	m_SrcBlend = blend;

	OGL_CHECK( I3OGL::glBlendFunc( m_SrcBlend, m_DestBlend));
}

I3G_BLEND i3RenderContext::GetSrcBlend(void)
{
	return m_SrcBlend;
}

void i3RenderContext::SetDestBlend( I3G_BLEND blend)
{
	m_DestBlend = blend;

	OGL_CHECK( I3OGL::glBlendFunc( m_SrcBlend, m_DestBlend));
}

I3G_BLEND i3RenderContext::GetDestBlend(void)
{
	return m_DestBlend;
}

void i3RenderContext::SetBlendOperation( I3G_BLEND_OP op)
{
	m_BlendOp = op;

	//I3OGL::glBlendFunc( m_BlendOp, m_SrcBlend, m_DestBlend, m_SrcBlendMask, m_DestBlendMask);
}

I3G_BLEND_OP i3RenderContext::GetBlendOperation(void)
{
	return m_BlendOp;
}

// Fog
void i3RenderContext::SetFogEnable( BOOL bFlag)
{
	m_bFogEnable = bFlag;

	if( bFlag)
	{
		OGL_CHECK( I3OGL::glEnable( GL_FOG));
	}
	else
	{
		OGL_CHECK( I3OGL::glDisable( GL_FOG));
	}
}

BOOL i3RenderContext::GetFogEnable(void)
{
	return m_bFogEnable;
}

// FIXME: Optimization - Earring
void i3RenderContext::SetFogColor( COLOR * pColor)
{
#ifdef I3_FIXED_POINT
	FIXED32_COLORREAL col;
	i3Color::Set( &m_FogColor, pColor);
	i3Color::Copy( &col, pColor);
#else
	COLORREAL col;
	i3Color::Set( &m_FogColor, pColor);
	i3Color::Set( &col, pColor);

#endif


	OGL_CHECK( I3OGL::glFogv( GL_FOG_COLOR, &col));
}

void i3RenderContext::GetFogColor( COLOR * pColor)
{
	i3Color::Set( pColor, &m_FogColor);
}

void i3RenderContext::SetFogMode( I3G_FOG_MODE mode)
{
	switch( mode)
	{
		case I3G_FOG_EXP :
		case I3G_FOG_EXP2 :
			OGL_CHECK( I3OGL::glFog( GL_FOG_MODE, mode));
			break;

		default :
			I3TRACE0( "just only EXP and EXP2 mode of Fog are supported on EGL\n");
			break;
	}
	
}

I3G_FOG_MODE i3RenderContext::GetFogMode(void)
{
	return I3G_FOG_LINEAR;
}

void i3RenderContext::SetFogTableEnable( BOOL /*bFlag*/)
{
}

BOOL i3RenderContext::GetFogTableEnable(void)
{
	return FALSE;
}

#if defined( I3_FIXED_POINT)
	void i3RenderContext::SetFogNearFar( FIXED32 fNear, FIXED32 fFar)
	{
		m_FogNear = fNear;
		m_FogFar = fFar;

		OGL_CHECK( I3OGL::glFog( GL_FOG_START, fNear));
		OGL_CHECK( I3OGL::glFog( GL_FOG_END, fFar));
	}

	FIXED32 i3RenderContext::GetFogNear(void)
	{
		return m_FogNear;
	}

	FIXED32 i3RenderContext::GetFogFar(void)
	{
		return m_FogFar;
	}

	void i3RenderContext::SetFogDensity( FIXED32 fVal)
	{
		OGL_CHECK( I3OGL::glFog( GL_FOG_END, fVal));
	}

	FIXED32 i3RenderContext::GetFogDensity(void)
	{
		return 0;
	}
#else
	void i3RenderContext::SetFogNearFar( REAL32 fNear, REAL32 fFar)
	{
		m_FogNear = fNear;
		m_FogFar = fFar;

		OGL_CHECK( I3OGL::glFog( GL_FOG_START, fNear));
		OGL_CHECK( I3OGL::glFog( GL_FOG_END, fFar));
	}

	REAL32 i3RenderContext::GetFogNear(void)
	{
		return m_FogNear;
	}

	REAL32 i3RenderContext::GetFogFar(void)
	{
		return m_FogFar;
	}

	void i3RenderContext::SetFogDensity( REAL32 fVal)
	{
		OGL_CHECK( I3OGL::glFog( GL_FOG_END, fVal));
	}

	REAL32 i3RenderContext::GetFogDensity(void)
	{
		return 0.0f;
	}
#endif

// Anti-aliase
void i3RenderContext::SetEdgeAntiAliasEnable( BOOL bFlag)
{
	m_bAALine = bFlag;
}

BOOL i3RenderContext::GetEdgeAntiAliasEnable(void)
{
	return m_bAALine;
}

// Stencil Test
BOOL i3RenderContext::GetStencilAvailable(void)
{
	return (m_ClearFlag & I3G_CLEAR_STENCIL) != 0;
}

void i3RenderContext::SetStencilEnable( BOOL bFlag)
{
	if( bFlag)
	{
		OGL_CHECK( I3OGL::glEnable( GL_STENCIL_TEST));
	}
	else
	{
		OGL_CHECK( I3OGL::glDisable( GL_STENCIL_TEST));
	}

	m_bStencilEnable = bFlag;
}

BOOL i3RenderContext::GetStencilEnable(void)
{
	return m_bStencilEnable;
}

void i3RenderContext::SetStencilOperation( I3G_STENCIL_OP SFail, I3G_STENCIL_OP SPass, I3G_STENCIL_OP ZFail)
{
	m_StencilSFail = SFail;
	m_StencilSPass = SPass;
	m_StencilZFail = ZFail;

	OGL_CHECK( I3OGL::glStencilOp( SFail, SPass, ZFail));
}

I3G_STENCIL_OP i3RenderContext::GetStencilOperationSFail(void)
{
	return m_StencilSFail;
}

I3G_STENCIL_OP i3RenderContext::GetStencilOperationSPass(void)
{
	return m_StencilSPass;
}

I3G_STENCIL_OP i3RenderContext::GetStencilOperationZFail(void)
{
	return m_StencilZFail;
}

void i3RenderContext::SetStencilFunction( I3G_COMP_FUNC	func)			// default == I3G_STENCIL_FUNC_ALWAYS
{
	m_StencilFunc = func;

	OGL_CHECK( I3OGL::glStencilFunc( func, m_StencilRef, m_StencilMask));
}

I3G_COMP_FUNC i3RenderContext::GetStencilFunction( void)
{
	return m_StencilFunc;
}

void i3RenderContext::SetStencilRefValue( UINT8 val)							// default == 0
{
	m_StencilRef = val;

	OGL_CHECK( I3OGL::glStencilFunc( m_StencilFunc, m_StencilRef, m_StencilMask));
}

UINT8 i3RenderContext::GetStencilRefValue(void)
{
	return m_StencilRef;
}

void i3RenderContext::SetStencilReadMask( UINT8 MaskVal)						// default == 0xFF
{
	m_StencilMask = MaskVal;

	OGL_CHECK( I3OGL::glStencilFunc( m_StencilFunc, m_StencilRef, m_StencilMask));
}

UINT8 i3RenderContext::GetStencilReadMask(void)
{
	return m_StencilMask;
}

void i3RenderContext::SetStencilWriteMask( UINT8 MaskVal)						// default == 0xFF
{
	m_StencilMask = MaskVal;

	OGL_CHECK( I3OGL::glStencilFunc( m_StencilFunc, m_StencilRef, m_StencilMask));
}

UINT8 i3RenderContext::GetStencilWriteMask(void)
{
	return m_StencilMask;
}

// Clipping
void i3RenderContext::SetClippingEnable( BOOL bFlag)							// default == TRUE
{
	m_bClipping = bFlag;
}

BOOL i3RenderContext::GetClippingEnable(void)
{
	return m_bClipping;
}

// Shadow Map
void i3RenderContext::SetShadowMapEnable( BOOL bFlag)						// default == FALSE
{

}

BOOL i3RenderContext::GetShadowMapEnable(void)
{
	return FALSE;
}

// Lighting
void i3RenderContext::SetLightingEnable( BOOL bFlag)							// default == FALSE
{
	m_bLightingEnable = bFlag;

	if( bFlag)
	{
		OGL_CHECK( I3OGL::glEnable( GL_LIGHTING));
	}
	else
	{
		OGL_CHECK( I3OGL::glDisable( GL_LIGHTING));
	}
}

BOOL i3RenderContext::GetLightingEnable(void)
{
	return m_bLightingEnable;
}

// Vertex Color Enable
void i3RenderContext::SetVertexColorEnable( BOOL bFlag)						// default == TRUE
{
}

BOOL i3RenderContext::GetVertexColorEnable(void)
{
	return TRUE;
}

// Normalize Normal Enable
void i3RenderContext::SetNormalizeNormalEnable( BOOL bFlag)					// default == FALSE
{
	m_bNormalizeNormal = bFlag;

	if( bFlag)
	{
		OGL_CHECK( I3OGL::glEnable( GL_NORMALIZE));
	}
	else
	{
		OGL_CHECK( I3OGL::glDisable( GL_NORMALIZE));
	}
}

BOOL i3RenderContext::GetNormalizeNormalEnable(void)
{
	return m_bNormalizeNormal;
}

// Material
void i3RenderContext::SetMaterial(	COLORREAL * pAmbientColor,
									COLORREAL *	pDiffuseColor,
									COLORREAL * pSpecularColor,
									COLORREAL * pEmissiveColor,
									REAL32		Shininess)
{
	i3Color::Set( &m_AmbientColor, pAmbientColor);
	i3Color::Set( &m_DiffuseColor, pDiffuseColor);
	i3Color::Set( &m_SpecularColor, pSpecularColor);
	i3Color::Set( &m_EmissiveColor, pEmissiveColor);

#ifdef I3_FIXED_POINT
	FIXED32_COLORREAL col;

	i3Color::Copy( &col, &m_AmbientColor);
	OGL_CHECK( I3OGL::glMaterialv( GL_FRONT_AND_BACK, GL_AMBIENT, &col));

	i3Color::Copy( &col, &m_DiffuseColor);
	OGL_CHECK( I3OGL::glMaterialv( GL_FRONT_AND_BACK, GL_DIFFUSE, &col));

	i3Color::Copy( &col, &m_SpecularColor);
	OGL_CHECK( I3OGL::glMaterialv( GL_FRONT_AND_BACK, GL_SPECULAR, &col));
#else
	OGL_CHECK( I3OGL::glMaterialv( GL_FRONT_AND_BACK, GL_AMBIENT, &pAmbientColor));
	OGL_CHECK( I3OGL::glMaterialv( GL_FRONT_AND_BACK, GL_DIFFUSE, &pDiffuseColor));
	OGL_CHECK( I3OGL::glMaterialv( GL_FRONT_AND_BACK, GL_SPECULAR, &pSpecularColor));
#endif
}

void i3RenderContext::GetMaterialAmbient( COLORREAL * pColor)
{
	i3Color::Set( pColor, &m_AmbientColor);
}

void i3RenderContext::GetMaterialDiffuse( COLORREAL * pColor)
{
	i3Color::Set( pColor, &m_DiffuseColor);
}

void i3RenderContext::GetMaterialSpecular( COLORREAL * pColor)
{
	i3Color::Set( pColor, &m_SpecularColor);
}

void i3RenderContext::GetMaterialEmissive( COLORREAL * pColor)
{
	i3Color::Set( pColor, 0.0f, 0.0f, 0.0f, 1.0f);
}

void i3RenderContext::SetColor( COLOR * pColor)
{
	i3Color::Set( &m_Color, pColor);

#ifdef I3_FIXED_POINT
	FIXED32_COLORREAL col;
	i3Color::Copy( &col, pColor);
#else
	COLORREAL col;
	i3Color::Set( &col, pColor);
#endif	

	OGL_CHECK( I3OGL::glColor4( col.r, col.g, col.b, col.a));
}

void i3RenderContext::GetColor( COLOR * pColor)
{
	i3Color::Set( pColor, &m_Color);
}

// Vertex Blend Mode
void i3RenderContext::SetVertexBlendEnable( BOOL bFlag)						// default == TRUE
{
}

BOOL i3RenderContext::GetVertexBlendEnable(void)
{
	return TRUE;
}

void i3RenderContext::SetVertexBlendWeightCount( INT32 count)
{
}


void i3RenderContext::SetVertexBlendMatrix( INT32 Idx, RT_MATRIX * pMatrix)
{
	I3ASSERT((Idx >= 0) && (Idx < 8));
}

void i3RenderContext::SetSoftwareVertexBlendEnable( BOOL bFlag)				// default == FALSE
{
}


BOOL i3RenderContext::GetSoftwareVertexBlendEnable(void)
{
	return FALSE;
}

// Point-Sprite
void i3RenderContext::SetPointSpriteSize( REAL32 fWidth, REAL32 fHeight)			// default == 1.0f
{
}

REAL32 i3RenderContext::GetPointSpriteSize(void)
{
	return 1.0f;
}

void i3RenderContext::SetPointSpriteConstantSize( BOOL bFlag)
{
}

// Texture State
void i3RenderContext::SetTextureEnable( INT32 StageIdx, BOOL bState)
{
	m_bTextureEnable = bState;

	if( bState)
	{
		OGL_CHECK( I3OGL::glEnable( GL_TEXTURE_2D));
	}
	else
	{
		OGL_CHECK( I3OGL::glDisable( GL_TEXTURE_2D));
	}
}

inline UINT32 getLog2( UINT32 val)
{
	int rv = 0;

	for(; val > 1; val >>= 1)
	{
		rv ++;
	}

	return rv;
}


void i3RenderContext::SetTexture( INT32 StageIdx, i3Texture * pTexture)
{
	if( pTexture != NULL)
	{
		i3TextureOGLES * pTexOGL = (i3TextureOGLES *) pTexture;

		OGL_CHECK( I3OGL::glBindTexture( GL_TEXTURE_2D, pTexOGL->GetHandle()));
	}
	else
	{
		OGL_CHECK( I3OGL::glBindTexture( GL_TEXTURE_2D, 0));
	}

}

void i3RenderContext::SetTextureFilter( INT32 StageIdx, I3G_TEXTURE_FILTER MagFilter, I3G_TEXTURE_FILTER MinFilter)
{
	OGL_CHECK( I3OGL::glTexParameter( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter));
	OGL_CHECK( I3OGL::glTexParameter( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter));
}

void i3RenderContext::SetTextureWrap( INT32 StageIdx, I3G_TEXTURE_WRAP HorzWrap, I3G_TEXTURE_WRAP VertWrap)
{
	OGL_CHECK( I3OGL::glTexParameter( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, HorzWrap));
	OGL_CHECK( I3OGL::glTexParameter( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, VertWrap));
}

void i3RenderContext::SetTextureFunction( INT32 StageIdx, I3G_TEXTURE_FUNCTION func)
{
	OGL_CHECK( I3OGL::glTexEnv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, func));
}

void i3RenderContext::SetTextureMatrix( INT32 StageIdx, RT_MATRIX * pMatrix, INT32 CoordCount, BOOL bProjective)
{
	m_bProjectiveTexture = bProjective;

	i3Matrix::Copy( &m_LastTextureMatrix[0], pMatrix);	

	OGL_CHECK( I3OGL::glMatrixMode( GL_TEXTURE));

	OGL_CHECK( I3OGL::glLoadMatrix( pMatrix));
}

void i3RenderContext::SetTextureCoordScale( INT32 StageIdx, VEC3D * pScale, INT32 CoordCount)
{
}

void i3RenderContext::SetTextureCoordSource( INT32 StageIdx, I3G_TEXTURE_COORD_SOURCE source)
{
}

static int s_LightTbl[4] = 
{
	GL_LIGHT0,
	GL_LIGHT1,
	GL_LIGHT2,
	GL_LIGHT3
};

void i3RenderContext::SetLight( i3Light * pLight)
{
//	if( pLight->GetEnable())
//	{
		/*
		VEC3D * pAtt = pLight->GetAttenuation();
		COLOR col;

		// Enable
		I3OGL::glEnable( s_LightTbl[ pLight->GetIndex()]);
		I3OGL::glLightx( s_LightTbl[ pLight->GetIndex()], pLight->GetLightType(), GL_DIFFUSE, pLight->GetWorldDirection());

		// Light Colors
		{
			// Ambient
			i3Color::Set( &col, pLight->GetAmbient());

			I3OGL::glLightColor( pLight->GetIndex(), GL_AMBIENT, i3Color::ConvertRGBA32( &col));
			I3OGL::glAmbientColor( i3Color::ConvertRGBA32( &col));

			// Diffuse
			i3Color::Set( &col, pLight->GetDiffuse());

			I3OGL::glLightColor( pLight->GetIndex(), GL_DIFFUSE, i3Color::ConvertRGBA32( &col));

			// Specular
			i3Color::Set( &col, pLight->GetSpecular());

			I3OGL::glLightColor( pLight->GetIndex(), GL_SPECULAR, i3Color::ConvertRGBA32( &col));
		}

		// Spot & Point Light
		switch( pLight->GetType())
		{
			case I3G_LIGHT_DIRECTIONAL :
				{
					VEC3D dir;
					MATRIX invMtx;

					i3Matrix::Transpose3x3( &invMtx, &m_LastViewMatrix);
					i3Vector::TransformNormal( &dir, pLight->GetDirection(), &invMtx);

					// Direction
					_setCmd( i3PSPGfx::LX( pLight->GetIndex(), i3Vector::GetX( &dir)));
					_setCmd( i3PSPGfx::LY( pLight->GetIndex(), i3Vector::GetY( &dir)));
					_setCmd( i3PSPGfx::LZ( pLight->GetIndex(), i3Vector::GetZ( &dir)));

					_setCmd( i3PSPGfx::LDX( pLight->GetIndex(), i3Vector::GetX( &dir)));
					_setCmd( i3PSPGfx::LDY( pLight->GetIndex(), i3Vector::GetY( &dir)));
					_setCmd( i3PSPGfx::LDZ( pLight->GetIndex(), i3Vector::GetZ( &dir)));
				}
				break;

			case I3G_LIGHT_SPOT :
			case I3G_LIGHT_POINT :
				{
				#ifdef I3_PSP_DISPLAY_LIST
					// Direction
					_setCmd( i3PSPGfx::LDX( pLight->GetIndex(), i3Vector::GetX( pLight->GetWorldDirection())));
					_setCmd( i3PSPGfx::LDY( pLight->GetIndex(), i3Vector::GetY( pLight->GetWorldDirection())));
					_setCmd( i3PSPGfx::LDZ( pLight->GetIndex(), i3Vector::GetZ( pLight->GetWorldDirection())));

					// Attenuation
					_setCmd( i3PSPGfx::LKA( pLight->GetIndex(), i3Vector::GetX( pAtt)));
					_setCmd( i3PSPGfx::LKB( pLight->GetIndex(), i3Vector::GetY( pAtt)));
					_setCmd( i3PSPGfx::LKC( pLight->GetIndex(), i3Vector::GetZ( pAtt)));

					// Cut-off angle
					_setCmd( i3PSPGfx::LKO( pLight->GetIndex(), i3Math::cos( pLight->GetCutoffAngle())));

					// Fall-off rate
					_setCmd( i3PSPGfx::LKS( pLight->GetIndex(), pLight->GetFalloffRate()));

				#else
					I3OGL::glLightSpot( pLight->GetIndex(), pLight->GetFalloffRate(),
						i3Math::cos( pLight->GetCutoffAngle()),
						(const ScePspFVector3 *) pLight->GetWorldDirection());
				
					I3OGL::glLightAtt( pLight->GetIndex(), i3Vector::GetX( pAtt), i3Vector::GetY( pAtt), i3Vector::GetZ( pAtt));
				#endif
				}
				break;

			default :
				break;
		}
		*/
//	}
//	else
//	{
//		OGL_CHECK( I3OGL::glDisable( s_LightTbl[ pLight->GetIndex()]));
//	}
}

void i3RenderContext::SetPointSpriteEnable( BOOL bFlag)
{
}

void i3RenderContext::SetPointSpriteSizeSpace( I3G_POINT_SPRITE_SIZE_SPACE SizeSpace)
{
}

BOOL i3RenderContext::Reset(void)
{
	I3TRACE("Reset Complete!!\n");

	return TRUE;
}

void i3RenderContext::SetTextureCoordOffset( INT32 StageIdx, VEC3D * pOffset, INT32 CoordCount)
{
}

void i3RenderContext::_SetTextureWrap( INT32 StageIdx, I3G_TEXTURE_WRAP HorzWrap, I3G_TEXTURE_WRAP VertWrap)
{
}

void i3RenderContext::_SetTextureFilter( INT32 StageIdx, I3G_TEXTURE_FILTER MagFilter, I3G_TEXTURE_FILTER MinFilter)
{
}

void i3RenderContext::SetTextureFunction( I3G_TEXTURE_BIND bind, I3G_TEXTURE_FUNCTION func)
{
}

void i3RenderContext::SetTextureFilter( I3G_TEXTURE_BIND bind, I3G_TEXTURE_FILTER MagFilter, I3G_TEXTURE_FILTER MinFilter)
{
}

void i3RenderContext::SetTextureWrap( I3G_TEXTURE_BIND bind, I3G_TEXTURE_WRAP HorzWrap, I3G_TEXTURE_WRAP VertWrap)
{
}

void i3RenderContext::SetTextureEnable( I3G_TEXTURE_BIND bind, BOOL bState)
{
}

void i3RenderContext::SetVertexBlendMatricesByRef( RT_MATRIX ** pMatrix, INT32 cnt)
{
}

void i3RenderContext::ResetLight(void)
{
	m_LightCount = 0;
}

void i3RenderContext::AddLight( i3Light * pLight)
{
}

void i3RenderContext::SetWorldMatrix( RT_MATRIX * pMatrix)
{
	SetMatrix( I3G_MATRIX_WORLD, pMatrix );
}

void i3RenderContext::SetProjectMatrix( RT_MATRIX * pMatrix)
{
	SetMatrix( I3G_MATRIX_PROJECTION, pMatrix );
}

void i3RenderContext::SetDepthRenderTarget( i3Texture * pTex)
{
}

void i3RenderContext::SetColorRenderTarget( i3Texture * pTex)
{
}

void i3RenderContext::SetTextureCoordScale( I3G_TEXTURE_BIND bind, VEC3D * pScale, INT32 CoordCount)
{
}

void i3RenderContext::SetTextureCoordOffset( I3G_TEXTURE_BIND bind, VEC3D * pOffset, INT32 CoordCount)
{
}

void i3RenderContext::SetTexture( I3G_TEXTURE_BIND bind, i3Texture * pTexture)
{
	SetTexture( 0, pTexture );

	//m_TexBindInfo[ bind].m_pTexture = pTexture;
	//m_TexBindInfo[ bind].m_ModifyFlag |= I3G_TEXTURE_MODIFY_TEXTURE;
}

void i3RenderContext::SetTextureCoordSource( I3G_TEXTURE_BIND bind, I3G_TEXTURE_COORD_SOURCE source)
{
}

void i3RenderContext::SetViewMatrix( RT_MATRIX * pMatrix)
{
	SetMatrix( I3G_MATRIX_VIEW, pMatrix );
}

void i3RenderContext::SetZBias( REAL32 fSlope, REAL32 fBias)
{
}

void i3RenderContext::SetVertexBlendMatrices( RT_MATRIX * pMatrix, INT32 cnt)
{
}

void i3RenderContext::SetTextureMatrix( I3G_TEXTURE_BIND bind, RT_MATRIX * pMatrix, INT32 CoordCount, BOOL bProjective)
{
	i3Matrix::Copy( &m_TexBindInfo[ bind].m_Matrix, pMatrix);
	m_TexBindInfo[bind].m_CoordCount = CoordCount;
	m_TexBindInfo[bind].m_bProjective = bProjective;
	m_TexBindInfo[bind].m_ModifyFlag |= I3G_TEXTURE_MODIFY_MATRIX;

	OGL_CHECK( I3OGL::glMatrixMode( GL_TEXTURE));
	OGL_CHECK( I3OGL::glLoadMatrix( pMatrix));
}

void i3RenderContext::UpdateDevice(void)
{ 
}
#endif 
