#include "i3GfxType.h"

#ifdef I3G_PSP
#include <psptypes.h>
#include <libgu.h>
#include <libgum.h>
#include <stdio.h>
#include <stdlib.h>
#include <displaysvc.h>
#include <ctrlsvc.h>
#include <moduleexport.h>

#include "i3RenderContext.h"
#include "i3GfxDefinePSP.h"
#include "i3GfxUtilPSP.h"
#include "i3GfxGlobalVariable.h"
#include "i3TexturePSP.h"

#include "i3GfxPSPCommandBuilder.h"
#include "i3GPUCommandParserPSP.h"

I3_CLASS_INSTANCE( i3RenderContext, i3ElementBase);


// ============================================================================
// Defines and Functions for Display List
// ============================================================================

// GE_PROFILE
#define I3_PSP_GE_PROFILE

// Display List를 활성화
#define I3_PSP_DISPLAY_LIST

#ifdef I3_PSP_DISPLAY_LIST
	#define					DISPBUFSZ		0x400
	#define					PACKETSZ		0x10000 //0x100000
#else
	#define					DISPBUFSZ		0x10000
	#define					PACKETSZ		0x1
#endif

#ifdef I3_DEBUG
	// Display List 디버깅 기능 활성화 여부
	DispListTestFunc		s_pDispListTestFunc = NULL;
#endif

//#define	DEBUG_DISP

#ifdef DEBUG_DISP
	#define	__TRACE I3PRINTF
#else
	#if defined( I3_COMPILER_VC)
		#define __TRACE	__noop
	#else
		#define __TRACE(...)	(void)0
	#endif
#endif

// Clear Vertex
typedef struct _tagi3PSPClearVertex
{
	// ist
	//UINT16	color;
	//INT16	x, y;
	//UINT16	z;

	// from doc
	//UINT16	U, V;
	//UINT16	Color;
	//INT16	Xd, Yd, Xd;

	// real ??
	UINT32	Color;
	INT16	x, y;
	INT16	z, Pad;
} i3PSPClearVertex __attribute__((packed));

static i3PSPClearVertex			s_ClearRect[2] =
{
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
};
 

// Display List 관련 변수
static char				s_DispList[DISPBUFSZ] __attribute__((aligned(64)));
static UINT32			s_Packet[2][PACKETSZ] __attribute__((aligned(64)));
static UINT32 *			s_pCurPacketPos = s_Packet[0];
static INT32			s_id = 0;
//static SceGuContext		s_Context;
ScePspFMatrix4			s_MatrixStack[1 + 1 + 1 + 1];


// Display List 명령어 하나 추가
inline	void _setCmd( UINT32 cmd)
{
	*s_pCurPacketPos = cmd;
	s_pCurPacketPos++;
//	I3ASSERT( s_pCurPacketPos < &s_Packet[s_id][PACKETSZ] );
}

inline void _setBlendParam( I3G_BLEND src, I3G_BLEND dest)
{
	if( src == I3G_BLEND_SRCALPHA)
	{
		if( dest == I3G_BLEND_INVSRCALPHA)
			sceGuBlendFunc( SCEGU_ADD, SCEGU_SRC_ALPHA, SCEGU_ONE_MINUS_SRC_ALPHA, 0xFFFFFFFF, 0xFFFFFFFF);
		else
			sceGuBlendFunc( SCEGU_ADD, SCEGU_SRC_ALPHA, SCEGU_FIX_VALUE, 0xFFFFFFFF, 0xFFFFFFFF);
	}
}

#if defined( I3_PSP_GE_PROFILE)
static BOOL		s_bGEProfileStart = FALSE;
static INT64	s_timeGE=0, s_timeGEProfile = 0;
static INT64	signal_id = 0;

void _GEProfileSignalProc( int id)
{
	signal_id = id;
	s_bGEProfileStart = TRUE;

	SceRtcTick		Tick;
	sceRtcGetCurrentTick( &Tick );
	s_timeGE = INT64( Tick.tick );
/*
	if( id == 100)
	{
		s_bGEProfileStart = TRUE;

		SceRtcTick		Tick;
		sceRtcGetCurrentTick( &Tick );
		s_timeGE = INT64( Tick.tick );
	}
	//s_timeGE = i3Timer::GetProfileTime();
*/
}

void _GEProfileFinishProc( int id )
{
//	if((id == 100) && (s_bGEProfileStart))
	if( s_bGEProfileStart)
	{
		SceRtcTick		Tick;
		sceRtcGetCurrentTick( &Tick );

		s_timeGEProfile = INT64( Tick.tick ) - s_timeGE;

		s_bGEProfileStart = FALSE;
	}
	else
	{
		//printf( "s_bGEProfileStart == FALSE, id = %d\n", id );
	}
}
#endif

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
	i3Color::Set( &m_BackColor, 0.0f, 0.0f, 0.0f, 1.0f);
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

	m_TextureCoordSource = (I3G_TEXTURE_COORD_SOURCE) SCEGU_UV_MAP;
	m_bProjectiveTexture = FALSE;

	g_pRenderContext = this;

	m_SrcBlendMask = 0xFF;
	m_DestBlendMask = 0xFF;

	//
	m_TexFunc = 0;
	m_OldTexFunc = -1;

	i3Matrix::Identity( &m_LastViewMatrix);
	i3Matrix::Identity( &m_LastProjectMatrix);
	i3Matrix::Identity( &m_LastWorldMatrix);
	i3Matrix::Identity( &m_LastTextureMatrix);

	m_pTimer = NULL;

	m_pFPSTimer = NULL;

	m_bFPS30 = FALSE;

#ifdef I3_PSP
	m_bDumpDispList = FALSE;
	m_bDumpDispListOnce = TRUE;
	m_bFullFrameTimeRendering = TRUE;
#endif
}

i3RenderContext::~i3RenderContext(void)
{
	I3_SAFE_RELEASE( m_pFPSTimer);
	I3_SAFE_RELEASE( m_pTimer);
	I3_SAFE_RELEASE( m_pCaps);
	I3_SAFE_RELEASE( m_pPerfReport);
	I3_SAFE_RELEASE( m_pRenderTarget);

	{
		INT32 i;
		i3RenderTarget * pRT;

		for( i = 0; i < m_RenderTargetList.GetCount(); i++)
		{
			pRT = (i3RenderTarget *) m_RenderTargetList.Items[ i];

			pRT->Release();
		}
	}

	sceGuTerm();
}

static ScePspIMatrix4 dither =
{
    { -4, 4, -3, 1 },
    { 2, -2, 3, -1 },
    { -3, 1, -4, 4 },
    { 3, -1, 2, -2 }
};

bool i3RenderContext::Create( i3VideoInfo * pVFormat, void * pSetupParam)
{
	__TRACE( "ENTER > i3RenderContext::Create()\n" );
	m_pSetupParam = pSetupParam;

	s_pCurPacketPos = s_Packet[0];
	s_id = 0;

	__TRACE( "sceGuInit();\n" );
	sceGuInit();

	#if defined( I3_PSP_GE_PROFILE)
	{
		// GE의 Processing Time을 측정하기 위한 Interrupt Handler 설정
		
		sceGuSetCallback( SCEGU_INT_SIGNAL, _GEProfileSignalProc);
		sceGuSetCallback( SCEGU_INT_FINISH, _GEProfileFinishProc);
	}
	#endif

	//pVFormat->m_RenderTargetFormat = I3G_IMAGE_FORMAT_ARGB_1555;
	//pVFormat->m_RenderTargetFormat = I3G_IMAGE_FORMAT_RGBA_8888;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Create 단계에서는 sceGu 함수만을 사용한다.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Begin
	//Begin();
	i3System::DCacheWritebackAll();
	sceGuStart( SCEGU_IMMEDIATE, s_DispList, DISPBUFSZ);

	// Render Target
	{
		i3RenderTarget * pNewRT;

		pNewRT = i3RenderTarget::NewObject();

		pNewRT->Create( SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT, pVFormat->m_RenderTargetFormat);

		m_RenderTargetList.Add( pNewRT);
	}

	// Timer
	{
		m_pTimer = i3Timer::NewObject();
		I3ASSERT( m_pTimer != NULL);

		m_pFPSTimer = i3Timer::NewObject();
	}

	// Device Capacity Information
	{
		m_pCaps = i3GfxCaps::NewObject();
		I3ASSERT( m_pCaps != NULL);
	}

	// Performance Report
	{
		m_pPerfReport = i3GfxPerformanceReport::NewObject();
		I3ASSERT( m_pPerfReport != NULL);
	}

	//__TRACE( "SetDefaultRenderState();\n" );
	//SetDefaultRenderState();

	__TRACE( "sceGuViewport( 2048, 2048, SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT);\n" );
	sceGuViewport( 2048, 2048, SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT);

	s_ClearRect[0].x = 0;
	s_ClearRect[0].y = 0;
	s_ClearRect[1].x = SCEGU_SCR_WIDTH;
	s_ClearRect[1].y = SCEGU_SCR_HEIGHT;

	m_ClearFlag = I3G_CLEAR_COLOR | I3G_CLEAR_DEPTH;

	SetClearColor( &m_BackColor);
	SetClearZValue( m_ClearZValue);
	SetClearStencilValue( m_ClearStencilValue);

	sceGuScissor(0, 0, SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT);
	//sceGuScissor(0, 0, 0, 0);
	sceGuEnable(SCEGU_SCISSOR_TEST);

	sceGuEnable( SCEGU_DITHER);
	sceGuSetDither( (const ScePspIMatrix4 *) &dither);

	sceGuEnable( SCEGU_CLIP_PLANE);
	sceGuDisable( SCEGU_COLOR_DOUBLE);
	sceGuDisable( SCEGU_NORMAL_REVERSE_PRIM);
	sceGuDisable( SCEGU_NORMAL_REVERSE_PATCH);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// 이 함수를 호출함으로써 AA, MAA 를 0xFF 값으로 설정한다!!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	sceGuAmbient( 0xFF000000 );			// 글로벌 환경광
	sceGuAmbientColor( 0xFF000000 );	// 모델 환경광

	{
		//i3System::DCacheWritebackAll();

		sceGuFinish();
		sceGuSync( SCEGU_SYNC_FINISH, SCEGU_SYNC_WAIT);
		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// 여기서 부터는 DIsplay List를 사용한다.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Set Default Render States	
	Begin();
	I3TRACE( "SetDefaultRenderState();\n" );
	SetDefaultRenderState();
	I3TRACE( "112\n");
	End();

	sceGuDisplay( 1 );

    // The app is ready to go
    m_bReady = TRUE;

	__TRACE( "EXIT > i3RenderContext::Create\n" );
	return true;
}

#ifdef I3_DEBUG
// 디스플레이 리스트 버퍼 얻기
void i3RenderContext::DumpDispList( BOOL bOnce )
{
	m_bDumpDispList		= TRUE;
	m_bDumpDispListOnce	= bOnce;
}

void i3RenderContext::SetDispListTestFunc( DispListTestFunc pFunc )
{
	s_pDispListTestFunc = pFunc;
}
#endif

typedef void (*PSP_UTIL_CB)( void *p );

static void* s_pi3PSPUtilCBArg = NULL;
static PSP_UTIL_CB s_pPSPUtilCBFunc = NULL;

void i3SetPSPUtilCBFunc( PSP_UTIL_CB pFunc, void *pArg )
{
	s_pPSPUtilCBFunc = pFunc;
	s_pi3PSPUtilCBArg = pArg;
}

void i3RenderContext::EndDispListRender()
{
	// GPU 종료 대기
	sceGuSync( SCEGU_SYNC_FINISH, SCEGU_SYNC_WAIT);

	if( s_pPSPUtilCBFunc != NULL )
	{
		s_pPSPUtilCBFunc( s_pi3PSPUtilCBArg );
	}

	// 화면 갱신 대기
	sceDisplayWaitVblankStart();

	if( m_bFPS30)
	{
		REAL32 diff = m_pFPSTimer->GetDeltaSec();
		if( diff < 0.03f)
		{
			// 한번 Frame Skip한다.
			sceDisplayWaitVblankStart();

			m_pFPSTimer->GetDeltaSec();
		}
	}

	// (프레임) 버퍼 스왑
	sceGuSwapBuffers();
}

static INT32 s_DitherIdx = 0;
static ScePspIMatrix4 dither_matrices[] =
{
    { { 1, -1, 1, -1 }, { -1, 1, -1, 1 }, { 1, -1, 1, -1 }, { -1, 1, -1, 1 } },
    { { 1, -1, 0, 0 }, { -1, 1, 0, 0 }, { 0, 0, 1, -1 }, { 0, 0, -1, 1 } },
	{ { -1, 1, -1, 1 }, { 1, -1, 1, -1 }, { -1, 1, -1, 1 }, { 1, -1, 1, -1 } },
    { { 0, 0, -1, 1 }, { 0, 0, 1, -1 }, { -1, 1, 0, 0 }, { 1, -1, 0, 0 } },
    { { 1, -1, 1, -1 }, { -1, 1, -1, 1 }, { 1, -1, 1, -1 }, { -1, 1, -1, 1 } },
    { { -1, 1, 0, 0 }, { 1, -1, 0, 0 }, { 0, 0, -1, 1 }, { 0, 0, 1, 1 } },
    { { -1, 1, -1, 1 }, { 1, -1, 1, -1 }, { -1, 1, -1, 1 }, { 1, -1, 1, -1 } },
    { { 0, 0, 1, -1 }, { 0, 0, -1, 1 }, { 1, -1, 0, 0 }, { -1, 1, 0, 0 } },
};

bool i3RenderContext::Begin(void)
{
#ifdef I3_PSP_DISPLAY_LIST
	// =======================================================================
	// 원래 End 부분
	// =======================================================================
#ifdef I3_PSP_DISPLAY_LIST
	if( m_bFullFrameTimeRendering )
	{
		EndDispListRender();
	}
#endif

	// =======================================================================
	// 원래 Begin
	// =======================================================================

	// 저장된 디스플레이 리스트를 보내고 버퍼를 스왑

	// 캐시 저장
	i3System::DCacheWritebackAll();

	// sceGuStart
	sceGuStart( SCEGU_IMMEDIATE, s_DispList, DISPBUFSZ );

#if 1
	sceGuSetDither( &dither_matrices[ s_DitherIdx]);
	s_DitherIdx++;
	if( s_DitherIdx > 7)
		s_DitherIdx = 0;
#endif

	// 디스플레이 리스트 패킷 종료 설정
	__TRACE( "TRACE : _setCmd( i3PSPGfx::RET() );\n" );
	_setCmd( i3PSPGfx::RET() );

//#ifdef DEBUG_DISP
//	i3GPUContextPSP gpu;
//	// Debug. Parse Packet.
//	{
//		I3TRACE( "TRACE : Parsing Packet\n" );		
//		gpu.Parse( s_Packet[s_id] );
//	}
//#endif

	// 캐시 저장
	i3System::DCacheWritebackAll();

	#if defined( I3_PSP_GE_PROFILE)
		// 디스플레이 리스트의 시작을 알림.
		sceGuSignal( SCEGU_SIGNAL_NOWAIT, signal_id );
		sceGuSignalSync(signal_id);
	#endif

	// 디스플레이 리스트 패킷 호출( 이전 프레임에서 생성된 패킷 )
	sceGuCallList( s_Packet[s_id] );

#ifdef I3_DEBUG
	if( s_pDispListTestFunc != NULL )
	{
		s_pDispListTestFunc();
	}
#endif

	// sceGuFinish
	sceGuFinish();

#ifdef I3_DEBUG
	// Trace Display List
	if( m_bDumpDispList )
	{
		if( m_bDumpDispListOnce )
		{
			m_bDumpDispList = FALSE;
		}
		
		i3GPUContextPSP gpu;

		// Main Display List
		I3TRACE( "TRACE : Parsing Main DispList\n" );
		gpu.Parse( (UINT32*)s_DispList );

		// User Display Call List
		I3TRACE( "TRACE : Parsing User DispList\n" );
		gpu.Parse( (UINT32*)s_Packet[s_id] );
	}
#endif

	// sceGuSync 를 하지 않음.
	// CPU가 디스플레이 리스트 패킷을 생성을 하는 동안,
	// GPU는 이전 프레임에서 생성된 패킷을 처리 중.

	// 디스플레이 리스트 패킷 버퍼 스왑
	s_id ^= 1; // 0 -> 1, 1 -> 0 으로 바꿈.
	s_pCurPacketPos = s_Packet[s_id];

#else
	i3System::DCacheWritebackAll();
	sceGuStart( SCEGU_IMMEDIATE, s_DispList, DISPBUFSZ);
#endif

	return true;
}

void i3RenderContext::End(void)
{
	//I3TRACE( "i3RenderContext::End() -> ENTER\n");

	//I3TRACE( "i3RenderContext::End() -> 1\n");
#if defined( I3_DEBUG)
	if( m_bShowFPS )
	{
		char conv[64];
		sprintf( conv, "FPS:%.1f", m_pPerfReport->m_FPS);
		sceGuDebugPrint( 400, 3, 0xFFFFFFFF, conv);
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

		#if defined( I3_PSP_GE_PROFILE)
		m_pPerfReport->m_GPUProcessingTime = s_timeGEProfile;
		#endif
	}

#ifdef I3_PSP_DISPLAY_LIST

	if( ! m_bFullFrameTimeRendering )
	{
		EndDispListRender();
	}

#else

	I3TRACE( "i3RenderContext::End() -> 10\n");

	i3System::DCacheWritebackAll();

	sceGuFinish();
	sceGuSync( SCEGU_SYNC_FINISH, SCEGU_SYNC_WAIT);
	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
#endif

	//I3TRACE( "i3RenderContext::End() -> EXIT\n");
}

bool i3RenderContext::BeginRender(void)
{
	return true;
}

void i3RenderContext::EndRender(void)
{
    // End the scene.
}

void i3RenderContext::Clear(I3G_CLEAR AddMask, I3G_CLEAR RemoveMask)
{
	I3G_CLEAR flag;

	flag = (m_ClearFlag | AddMask);
	flag = flag & ~(RemoveMask);

#ifdef I3_PSP_DISPLAY_LIST
	{
		INT32 nNumCmd = sizeof(s_ClearRect) / sizeof(UINT32);
		__TRACE( "i3RenderContext::Clear() => nNumData = %d\n", nNumCmd );

		UINT32 *pJumpTo = s_pCurPacketPos + nNumCmd + 2;
		_setCmd( i3PSPGfx::BASE( pJumpTo )); // 1
		_setCmd( i3PSPGfx::JUMP( pJumpTo )); // 2

		UINT32 *pCmd = (UINT32*)s_ClearRect;
		UINT32 *pVaddr = s_pCurPacketPos;
		for( INT32 i = 0; i < nNumCmd; i++ )
		{
			_setCmd( *pCmd );
			pCmd++;
		};

		_setCmd( i3PSPGfx::CMODE(	i3PSPGfx::CMODE_CLEAR, 
									i3PSPGfx::CMODE_UPDATE,		// Color
									i3PSPGfx::CMODE_KEEP,		// Alpha
									i3PSPGfx::CMODE_UPDATE));	// Depth

		_setCmd( i3PSPGfx::BASE( pVaddr ));
		_setCmd( i3PSPGfx::VADR( pVaddr ));
		_setCmd( i3PSPGfx::OFFSET( NULL));


		_setCmd( i3PSPGfx::VTYPE(	i3PSPGfx::VTYPE_NONE,		// Texture UV Format
									//i3PSPGfx::VTYPE_CT5551,		// Color Format
									i3PSPGfx::VTYPE_CT8888,		// Color Format
									i3PSPGfx::VTYPE_NONE,		// Normal Format
									i3PSPGfx::VTYPE_USHORT,		// Position Format
									i3PSPGfx::VTYPE_NONE,		// Weight Format
									i3PSPGfx::VTYPE_NONE,		// Index Format
									(i3PSPGfx::VTYPE_WEIGHT)0,							// Weight Count
									i3PSPGfx::VTYPE_BLEND0,		// Blend Factor Count
									i3PSPGfx::VTYPE_THROUGH));

		_setCmd( i3PSPGfx::PRIM( (i3PSPGfx::PRIM_TYPE) I3G_PRIM_POINTSPRITE, 2));

		_setCmd( i3PSPGfx::CMODE(	i3PSPGfx::CMODE_NORMAL, 
									i3PSPGfx::CMODE_KEEP,		// Color
									i3PSPGfx::CMODE_KEEP,		// Alpha
									i3PSPGfx::CMODE_KEEP));		// Depth
	}

#else
	sceGuClear( flag);
#endif
}

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

#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::TFUNC( i3PSPGfx::TFUNC_TXF( m_TexFunc ), i3PSPGfx::TFUNC_RGBA, FALSE ) );
#else
	sceGuTexFunc( m_TexFunc, SCEGU_RGB );
#endif

	m_OldTexFunc = m_TexFunc;
}

void i3RenderContext::SetVertexArray( i3VertexArray * pVA)
{
	UpdateTextureStageState( pVA->GetFormat()->GetHasColor() );

	m_pCurrentVA = pVA;
}

void i3RenderContext::SetIndexArray( i3IndexArray * pIA)
{
	m_pCurrentIA = pIA;
}

void i3RenderContext::SetMatrix( I3G_MATRIX_STACK Index, MATRIX * pMatrix)
{
	switch( Index)
	{
		case I3G_MATRIX_VIEW :			
			i3Matrix::Copy( &m_LastViewMatrix, pMatrix);

			pMatrix = & I3_IDENTITYMATRIX;

			#ifdef I3_PSP_DISPLAY_LIST
				// Density 문제때문에 View Matrix는 사용하지 않고 Identity Matrix로 유지한다.
				// 예외의 경우가 있는데, GPU command List를 Rendering하는 경우에는
				// 적절한 원래의 View Matrix를 사용해, Transform을 한다.

				_setCmd( i3PSPGfx::VIEWN( 0));

				_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get11( pMatrix)));
				_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get12( pMatrix)));
				_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get13( pMatrix)));

				_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get21( pMatrix)));
				_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get22( pMatrix)));
				_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get23( pMatrix)));

				_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get31( pMatrix)));
				_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get32( pMatrix)));
				_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get33( pMatrix)));

				_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get41( pMatrix)));
				_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get42( pMatrix)));
				_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get43( pMatrix)));
			#endif
			break;

		case I3G_MATRIX_PROJECTION :	
			i3Matrix::Copy( &m_LastProjectMatrix, pMatrix);	

			#ifdef I3_PSP_DISPLAY_LIST
				_setCmd( i3PSPGfx::PROJN( 0));

				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get11( pMatrix)));
				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get12( pMatrix)));
				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get13( pMatrix)));
				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get14( pMatrix)));

				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get21( pMatrix)));
				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get22( pMatrix)));
				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get23( pMatrix)));
				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get24( pMatrix)));

				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get31( pMatrix)));
				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get32( pMatrix)));
				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get33( pMatrix)));
				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get34( pMatrix)));

				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get41( pMatrix)));
				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get42( pMatrix)));
				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get43( pMatrix)));
				_setCmd( i3PSPGfx::PROJD( i3Matrix::Get44( pMatrix)));
			#endif
			break;

		case I3G_MATRIX_TEXTURE0 :		
			i3Matrix::Copy( &m_LastTextureMatrix, pMatrix);	

			#ifdef I3_PSP_DISPLAY_LIST
				_setCmd( i3PSPGfx::TGENN( 0));

				_setCmd( i3PSPGfx::TGEND( i3Matrix::Get11( pMatrix)));
				_setCmd( i3PSPGfx::TGEND( i3Matrix::Get12( pMatrix)));
				_setCmd( i3PSPGfx::TGEND( i3Matrix::Get13( pMatrix)));

				_setCmd( i3PSPGfx::TGEND( i3Matrix::Get21( pMatrix)));
				_setCmd( i3PSPGfx::TGEND( i3Matrix::Get22( pMatrix)));
				_setCmd( i3PSPGfx::TGEND( i3Matrix::Get23( pMatrix)));

				_setCmd( i3PSPGfx::TGEND( i3Matrix::Get31( pMatrix)));
				_setCmd( i3PSPGfx::TGEND( i3Matrix::Get32( pMatrix)));
				_setCmd( i3PSPGfx::TGEND( i3Matrix::Get33( pMatrix)));

				_setCmd( i3PSPGfx::TGEND( i3Matrix::Get41( pMatrix)));
				_setCmd( i3PSPGfx::TGEND( i3Matrix::Get42( pMatrix)));
				_setCmd( i3PSPGfx::TGEND( i3Matrix::Get43( pMatrix)));
			#endif
			break;

		case I3G_MATRIX_WORLD :			
			i3Matrix::Copy( &m_LastWorldMatrix, pMatrix);

			#ifdef I3_PSP_DISPLAY_LIST
				_setCmd( i3PSPGfx::WORLDN( 0));

				_setCmd( i3PSPGfx::WORLDD( i3Matrix::Get11( pMatrix)));
				_setCmd( i3PSPGfx::WORLDD( i3Matrix::Get12( pMatrix)));
				_setCmd( i3PSPGfx::WORLDD( i3Matrix::Get13( pMatrix)));

				_setCmd( i3PSPGfx::WORLDD( i3Matrix::Get21( pMatrix)));
				_setCmd( i3PSPGfx::WORLDD( i3Matrix::Get22( pMatrix)));
				_setCmd( i3PSPGfx::WORLDD( i3Matrix::Get23( pMatrix)));

				_setCmd( i3PSPGfx::WORLDD( i3Matrix::Get31( pMatrix)));
				_setCmd( i3PSPGfx::WORLDD( i3Matrix::Get32( pMatrix)));
				_setCmd( i3PSPGfx::WORLDD( i3Matrix::Get33( pMatrix)));

				_setCmd( i3PSPGfx::WORLDD( i3Matrix::Get41( pMatrix)));
				_setCmd( i3PSPGfx::WORLDD( i3Matrix::Get42( pMatrix)));
				_setCmd( i3PSPGfx::WORLDD( i3Matrix::Get43( pMatrix)));
			#endif
			break;
	}

#ifndef I3_PSP_DISPLAY_LIST
	sceGuSetMatrix( Index, (const ScePspFMatrix4 *) pMatrix);
#endif
}

void i3RenderContext::GetMatrix( I3G_MATRIX_STACK Index, MATRIX * pMatrix)
{
	switch( Index)
	{
		case I3G_MATRIX_VIEW :			i3Matrix::Copy( pMatrix, &m_LastViewMatrix);	break;
		case I3G_MATRIX_PROJECTION :	i3Matrix::Copy( pMatrix, &m_LastProjectMatrix);	break;
		case I3G_MATRIX_TEXTURE0 :		i3Matrix::Copy( pMatrix, &m_LastTextureMatrix);	break;
		case I3G_MATRIX_WORLD :			i3Matrix::Copy( pMatrix, &m_LastWorldMatrix);	break;
	}
}

void i3RenderContext::SetViewport( INT32 x, INT32 y, INT32 Width, INT32 Height)
{
#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::TX( (REAL32)x));
	_setCmd( i3PSPGfx::TY( (REAL32)y));
	_setCmd( i3PSPGfx::TZ( 0.0f));

	_setCmd( i3PSPGfx::SX( (REAL32) Width));
	_setCmd( i3PSPGfx::SY( (REAL32) Height));
	_setCmd( i3PSPGfx::SZ( 1.0f));
#else
	//I3TRACE( "sceGuViewport : %d, %d, %d, %d\n", x, y, Width, Height);
	sceGuViewport( x, y, Width, Height);
#endif
}

void i3RenderContext::DrawPrim( I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 PrimCount)
{
	int vert_count;
	char * pVertex;

	I3ASSERT( m_pCurrentVA);

	m_DrawCallCount++;
	m_TriangleCount += PrimCount;

	switch( Prim)
	{
		case I3G_PRIM_POINTLIST :		vert_count = PrimCount;						break;
		case I3G_PRIM_LINELIST :		vert_count = PrimCount << 1;				break;
		case I3G_PRIM_POINTSPRITE:
		case I3G_PRIM_LINESTRIP :		vert_count = PrimCount + 1;					break;
		case I3G_PRIM_TRILIST :			vert_count = PrimCount * 3;					break;
		case I3G_PRIM_TRISTRIP :
		case I3G_PRIM_TRIFAN :			vert_count = PrimCount + 2;					break;
		default :
			i3Log( "i3RenderContext::DrawPrim", "Invalid Primitive Type : %d\n", Prim);
			vert_count = 0;
			break;
	}

	pVertex = (char *) m_pCurrentVA->GetInstance();
	pVertex += (StartIdx * m_pCurrentVA->GetStride());
	
#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::OFFSET( NULL));

	_setCmd( i3PSPGfx::BASE( pVertex));
	_setCmd( i3PSPGfx::VADR( pVertex));

	_setCmd( i3PSPGfx::VTYPE( m_pCurrentVA->GetVertexFormatCode()));

	_setCmd( i3PSPGfx::PRIM( (i3PSPGfx::PRIM_TYPE) Prim, vert_count));
#else
	sceGuDrawArray( (int)Prim, m_pCurrentVA->GetVertexFormatCode(), vert_count, NULL, (void *) pVertex);
#endif
}

void i3RenderContext::DrawStripPrim( I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 * pPrimCount, UINT32 Count)
{
	UINT32 vert_count, i;
	char * pVertex;

	I3ASSERT( m_pCurrentVA);

	pVertex = (char *) m_pCurrentVA->GetInstance();
	pVertex += (StartIdx * m_pCurrentVA->GetStride());

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

		#ifdef I3_PSP_DISPLAY_LIST
			_setCmd( i3PSPGfx::OFFSET( NULL));

			_setCmd( i3PSPGfx::BASE( pVertex));
			_setCmd( i3PSPGfx::VADR( pVertex));

			_setCmd( i3PSPGfx::VTYPE( m_pCurrentVA->GetVertexFormatCode()));

			_setCmd( i3PSPGfx::PRIM( (i3PSPGfx::PRIM_TYPE) Prim, vert_count));
		#else
			sceGuDrawArray( (int)Prim, m_pCurrentVA->GetVertexFormatCode(), vert_count, NULL, (void *) pVertex);
		#endif

		pVertex += (vert_count * m_pCurrentVA->GetStride());
		pPrimCount++;
	}

	//sceGuFinish();
}

void i3RenderContext::DrawIndexedPrim( I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 PrimCount)
{
	i3VertexArray * pVA = m_pCurrentVA;
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

#ifdef  I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::OFFSET( NULL));

	_setCmd( i3PSPGfx::BASE( pVA->GetInstance()));
	_setCmd( i3PSPGfx::VADR( pVA->GetInstance()));

	_setCmd( i3PSPGfx::BASE( pIndex));
	_setCmd( i3PSPGfx::IADR( pIndex));

	_setCmd( i3PSPGfx::VTYPE( m_pCurrentVA->GetVertexFormatCode()  | SCEGU_INDEX_USHORT));

	_setCmd( i3PSPGfx::PRIM( (i3PSPGfx::PRIM_TYPE) Prim, vert_count));
#else
	sceGuDrawArray( (int)Prim, pVA->GetVertexFormatCode() | SCEGU_INDEX_USHORT, 
		vert_count, pIndex, pVA->GetInstance());
#endif
}

void i3RenderContext::DrawStripIndexedPrim( I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 * pPrimCount, UINT32 Count)
{
	i3VertexArray * pVA = m_pCurrentVA;
	UINT32 vert_count, i;
	UINT16 * pIndex;

	I3ASSERT( m_pCurrentVA != NULL);
	I3ASSERT( m_pCurrentIA != NULL);

	pIndex = (UINT16 *) m_pCurrentIA->GetInstance();
	pIndex += StartIdx;

	m_DrawCallCount += Count;

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

		#ifdef I3_PSP_DISPLAY_LIST
			_setCmd( i3PSPGfx::OFFSET( NULL));

			_setCmd( i3PSPGfx::BASE( pVA->GetInstance()));
			_setCmd( i3PSPGfx::VADR( pVA->GetInstance()));

			_setCmd( i3PSPGfx::BASE( pIndex));
			_setCmd( i3PSPGfx::IADR( pIndex));

			_setCmd( i3PSPGfx::VTYPE( m_pCurrentVA->GetVertexFormatCode()) | SCEGU_INDEX_USHORT);

			_setCmd( i3PSPGfx::PRIM( (i3PSPGfx::PRIM_TYPE) Prim, vert_count));
		#else
			sceGuDrawArray( (int)Prim, pVA->GetVertexFormatCode() | SCEGU_INDEX_USHORT, 
				vert_count, pIndex, pVA->GetInstance());
		#endif

		pIndex += vert_count;
		//pPrimCount++;
	}

	//sceGuFinish();
}

void i3RenderContext::DrawPointSprite( UINT32 StartIdx, UINT32 PrimCount)
{
	i3VertexArray * pVA = m_pCurrentVA;

	m_DrawCallCount++;
	m_TriangleCount += PrimCount;

#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::OFFSET( NULL));

	_setCmd( i3PSPGfx::BASE( pVA->GetInstance()));
	_setCmd( i3PSPGfx::VADR( pVA->GetInstance()));

	//_setCmd( i3PSPGfx::BASE( NULL));
	//_setCmd( i3PSPGfx::IADR( NULL));

	_setCmd( i3PSPGfx::VTYPE( m_pCurrentVA->GetVertexFormatCode()));

	_setCmd( i3PSPGfx::PRIM( (i3PSPGfx::PRIM_TYPE) SCEGU_PRIM_RECTANGLES, PrimCount << 1));
#else
	sceGuDrawArray( SCEGU_PRIM_RECTANGLES, pVA->GetVertexFormatCode(), 
		PrimCount << 1, NULL, pVA->GetInstance());
#endif
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

void i3RenderContext::SetClearColor( COLORREAL * pColor)
{
	i3Color::Set( &m_BackColor, pColor);

	{
		COLOR col;

		i3Color::Set( &col, pColor);

		#ifdef I3_PSP_DISPLAY_LIST
			s_ClearRect[0].Color = i3Color::ConvertRGBA32( &col);
			s_ClearRect[1].Color = i3Color::ConvertRGBA32( &col);
		#else
			sceGuClearColor( i3Color::ConvertRGBA32( &col));
		#endif
	}
}

void i3RenderContext::SetClearZValue( REAL32 val)
{
	REAL32 range;

	m_ClearZValue = val;

	// PSP는 Z Buffer의 값이 DirectX(0 ~ 1)와는 다르게..
	// 더 큰 값일 수록 카메라와 가까운 방식이기 때문에
	// 값을 뒤집는다.
	val = 1.0f - val;

	if( m_RenderTargetList.GetCount() > 0)
	{
		i3RenderTarget * pRT = (i3RenderTarget *) m_RenderTargetList.Items[0];

		range = (REAL32) i3Math::abs( (REAL32)pRT->GetDepthMax() - pRT->GetDepthMin());
	}
	else
	{
		range = 65535.0f;
	}

#ifdef I3_PSP_DISPLAY_LIST
	s_ClearRect[0].z = (UINT16) (range * val);
	s_ClearRect[1].z = s_ClearRect[0].z;
#else
	sceGuClearDepth( (INT32)(range * val));
#endif
}

void i3RenderContext::SetClearStencilValue( UINT8 val)
{
	m_ClearStencilValue = val;

#ifdef I3_PSP_DISPLAY_LIST
#else
	sceGuClearStencil( m_ClearStencilValue);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Render State


void i3RenderContext::SetAlphaTestEnable( BOOL flag)
{
	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::ATE( flag));
	#else
		if( flag)
			sceGuEnable( SCEGU_ALPHA_TEST);
		else
			sceGuDisable( SCEGU_ALPHA_TEST);
	#endif

	m_bAlphaTestEnable = flag;
}

BOOL i3RenderContext::GetAlphaTestEnable(void)
{
	return m_bAlphaTestEnable;
}

void i3RenderContext::SetBlendEnable( BOOL flag)
{
	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::ABE( flag));
	#else
		if( flag)
			sceGuEnable( SCEGU_BLEND);
		else
			sceGuDisable( SCEGU_BLEND);
	#endif

	m_bBlendEnable = flag;
}

BOOL i3RenderContext::GetBlendEnable(void)
{
	return m_bBlendEnable;
}

void i3RenderContext::SetAlphaRefValue( UINT8 val)
{
	m_AlphaRef = val;

	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::ATEST( (i3PSPGfx::ATEST_FUNC) m_AlphaFunc, m_AlphaRef, 0xFF));
	#else
		sceGuAlphaFunc( m_AlphaFunc, m_AlphaRef, 0xFF);
	#endif
}

UINT8 i3RenderContext::GetAlphaRefValue(void)
{
	return m_AlphaRef;
}

void i3RenderContext::SetAlphaFunction( I3G_COMP_FUNC func)
{
	m_AlphaFunc = func;

	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::ATEST( (i3PSPGfx::ATEST_FUNC) m_AlphaFunc, m_AlphaRef, 0xFF));
	#else
		sceGuAlphaFunc( m_AlphaFunc, m_AlphaRef, 0xFF);
	#endif
}

I3G_COMP_FUNC i3RenderContext::GetAlphaFunction(void)
{
	return m_AlphaFunc;
}

// Z-Buffer
void i3RenderContext::SetZTestEnable( BOOL flag)
{
	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::ZTE( flag));
	#else
		if( flag)
			sceGuEnable( SCEGU_DEPTH_TEST);
		else
			sceGuDisable( SCEGU_DEPTH_TEST);
	#endif

	m_bZTestEnable = flag;
}

BOOL i3RenderContext::GetZTestEnable(void)
{
	return m_bZTestEnable;
}

void i3RenderContext::SetZWriteEnable( BOOL flag)
{
	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::ZMSK( ! flag ) );
	#else
		sceGuDepthMask( !flag);
	#endif

	m_bZWriteEnable = flag;
}

BOOL i3RenderContext::GetZWriteEnable(void)
{
	return m_bZWriteEnable;
}

void i3RenderContext::SetZFunction( I3G_COMP_FUNC	func)
{
	m_ZFunc = func;

	// PSP에서의 Depth Buffer의 값은 가까운 쪽이 더 큰값을 가지는데 반해,
	// I3의 기본 기능에서는 가까운 쪽이 더 작은 값을 가지기 때문에
	// 그 비교 함수를 역으로 적용해야만 동일한 결과를 가져올 수 있다.
	switch( func)
	{
		case I3G_COMP_LESS		:	func = I3G_COMP_GREATER;	break;
		case I3G_COMP_LEQUAL	:	func = I3G_COMP_GEQUAL;		break;
		case I3G_COMP_GREATER	:	func = I3G_COMP_LESS;		break;
		case I3G_COMP_GEQUAL	:	func = I3G_COMP_LEQUAL;		break;
		default :					func = func;				break;
	}

	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::ZTEST( (i3PSPGfx::ZTEST_MODE) func));
	#else
		sceGuDepthFunc( func);
	#endif
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
	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::SHADE( (i3PSPGfx::SHADE_MODE) mode));
	#else
		sceGuShadeModel( mode);
	#endif

	m_ShadeMode = mode;
}

I3G_SHADE_MODE i3RenderContext::GetShadeMode(void)
{
	return m_ShadeMode;
}

void i3RenderContext::SetFaceCullMode( I3G_FACE_CULL	mode)
{
	m_FaceCull = mode;

	if( mode == I3G_FACE_CULL_NONE)
	{
		m_bBCE = FALSE;

		#ifdef I3_PSP_DISPLAY_LIST
			_setCmd( i3PSPGfx::BCE( FALSE));
		#else
			sceGuDisable( SCEGU_CULL_FACE);
		#endif
	}
	else
	{
		m_bBCE = TRUE;

		#ifdef I3_PSP_DISPLAY_LIST
			_setCmd( i3PSPGfx::BCE( TRUE));
			_setCmd( i3PSPGfx::CULL( (i3PSPGfx::CULL_MODE) ! mode ) );
		#else
			sceGuEnable( SCEGU_CULL_FACE);
			sceGuFrontFace( mode);
		#endif
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

#ifdef I3_PSP_DISPLAY_LIST
	__TRACE( "SetSrcBlend( %d )\n", blend );
	SetBlend( m_SrcBlend, m_DestBlend, m_BlendOp );
#else
	_setBlendParam( m_SrcBlend, m_DestBlend);
#endif
}

I3G_BLEND i3RenderContext::GetSrcBlend(void)
{
	return m_SrcBlend;
}

void i3RenderContext::SetDestBlend( I3G_BLEND blend)
{
	m_DestBlend = blend;

#ifdef I3_PSP_DISPLAY_LIST
	__TRACE( "SetDestBlend( %d )\n", blend );
	SetBlend( m_SrcBlend, m_DestBlend, m_BlendOp );
#else
	_setBlendParam( m_SrcBlend, m_DestBlend);
#endif
}

I3G_BLEND i3RenderContext::GetDestBlend(void)
{
	return m_DestBlend;
}

void i3RenderContext::SetBlendOperation( I3G_BLEND_OP op)
{
	m_BlendOp = op;

#ifdef I3_PSP_DISPLAY_LIST
	__TRACE( "SetBlendOperation( %d )\n", op );
	SetBlend( m_SrcBlend, m_DestBlend, m_BlendOp );
#else
	//sceGuBlendFunc( m_BlendOp, m_SrcBlend, m_DestBlend, m_SrcBlendMask, m_DestBlendMask);
#endif
}

I3G_BLEND_OP i3RenderContext::GetBlendOperation(void)
{
	return m_BlendOp;
}

// Fog
void i3RenderContext::SetFogEnable( BOOL bFlag)
{
//	I3TRACE("i3RenderContext::SetFogEnable Enable?: %d\n", bFlag);
	m_bFogEnable = bFlag;

#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::FGE( bFlag));
#else
	if( bFlag)
		sceGuEnable( SCEGU_FOG);
	else
		sceGuDisable( SCEGU_FOG);

	sceGuDisable( SCEGU_FOG);
#endif
}

BOOL i3RenderContext::GetFogEnable(void)
{
	return m_bFogEnable;
}

// FIXME: Optimization - Earring
void i3RenderContext::SetFogColor( COLOR * pColor)
{
	i3Color::Set( &m_FogColor, pColor);

#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::FC( i3Color::GetR( pColor), i3Color::GetG( pColor), i3Color::GetB( pColor)));
#else
	//sceGuFog( -m_FogFar, -(1.0f / (m_FogFar - m_FogNear)), i3Color::ConvertRGBA32( &m_FogColor));
#endif
}

void i3RenderContext::GetFogColor( COLOR * pColor)
{
	i3Color::Set( pColor, &m_FogColor);
}

void i3RenderContext::SetFogMode( I3G_FOG_MODE mode)
{
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

void i3RenderContext::SetFogNearFar( REAL32 fNear, REAL32 fFar)
{
	m_FogNear = fNear;
	m_FogFar = fFar;

#ifdef I3_PSP_DISPLAY_LIST
//	I3TRACE("m_FogNear:%f\n", m_FogNear);
//	I3TRACE("m_FogFar:%f\n", m_FogFar);
	_setCmd( i3PSPGfx::FOG1( m_FogFar));
	_setCmd( i3PSPGfx::FOG2( (1.0f / (m_FogFar - m_FogNear) ) ) );
#else
	//sceGuFog( -m_FogFar, -(1.0f / range), i3Color::ConvertRGBA32( &m_FogColor));
#endif
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
}

REAL32 i3RenderContext::GetFogDensity(void)
{
	return 0.0f;
}

// Anti-aliase
void i3RenderContext::SetEdgeAntiAliasEnable( BOOL bFlag)
{
#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::AAE( bFlag));
#else
	if( bFlag)
		sceGuEnable( SCEGU_AALINE);
	else
		sceGuDisable( SCEGU_AALINE);

#endif

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
#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::STE( bFlag));
#else
	if( bFlag)
		sceGuEnable( SCEGU_STENCIL_TEST);
	else
		sceGuDisable( SCEGU_STENCIL_TEST);
#endif

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

#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::SOP( (i3PSPGfx::SOP_MODE) m_StencilSFail, 
							(i3PSPGfx::SOP_MODE) m_StencilZFail,
							(i3PSPGfx::SOP_MODE) m_StencilSPass));
#else
	sceGuStencilOp( SFail, SPass, ZFail);
#endif
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

#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::STEST( (i3PSPGfx::STEST_FUNC) m_StencilFunc, m_StencilRef, m_StencilMask));
#else
	sceGuStencilFunc( func, m_StencilRef, m_StencilMask);
#endif
}

I3G_COMP_FUNC i3RenderContext::GetStencilFunction( void)
{
	return m_StencilFunc;
}

void i3RenderContext::SetStencilRefValue( UINT8 val)							// default == 0
{
	m_StencilRef = val;

#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::STEST( (i3PSPGfx::STEST_FUNC) m_StencilFunc, m_StencilRef, m_StencilMask));
#else
	sceGuStencilFunc( m_StencilFunc, m_StencilRef, m_StencilMask);
#endif
}

UINT8 i3RenderContext::GetStencilRefValue(void)
{
	return m_StencilRef;
}

void i3RenderContext::SetStencilReadMask( UINT8 MaskVal)						// default == 0xFF
{
	m_StencilMask = MaskVal;

#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::STEST( (i3PSPGfx::STEST_FUNC) m_StencilFunc, m_StencilRef, m_StencilMask));
#else
	sceGuStencilFunc( m_StencilFunc, m_StencilRef, m_StencilMask);
#endif
}

UINT8 i3RenderContext::GetStencilReadMask(void)
{
	return m_StencilMask;
}

void i3RenderContext::SetStencilWriteMask( UINT8 MaskVal)						// default == 0xFF
{
	m_StencilMask = MaskVal;

#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::STEST( (i3PSPGfx::STEST_FUNC) m_StencilFunc, m_StencilRef, m_StencilMask));
#else
	sceGuStencilFunc( m_StencilFunc, m_StencilRef, m_StencilMask);
#endif
}

UINT8 i3RenderContext::GetStencilWriteMask(void)
{
	return m_StencilMask;
}

// Clipping
void i3RenderContext::SetClippingEnable( BOOL bFlag)							// default == TRUE
{
	m_bClipping = bFlag;

#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::CLE( bFlag));
#else
	//s_SetProc[bFlag]( SCEGU_CLIP_PLANE);
#endif
}

BOOL i3RenderContext::GetClippingEnable(void)
{
	return m_bClipping;
}

// Lighting
void i3RenderContext::SetLightingEnable( BOOL bFlag)							// default == FALSE
{
	m_bLightingEnable = (bFlag != FALSE);

#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::LTE( bFlag));
#else
	if( bFlag)
		sceGuEnable( SCEGU_LIGHTING);
	else
		sceGuDisable( SCEGU_LIGHTING);
#endif
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
}

BOOL i3RenderContext::GetNormalizeNormalEnable(void)
{
	return TRUE;
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

#ifdef I3_PSP_DISPLAY_LIST
	// Ambient
	_setCmd( i3PSPGfx::MAC( i3Color::GetR( &m_AmbientColor),
							i3Color::GetG( &m_AmbientColor),
							i3Color::GetB( &m_AmbientColor)));

	// Ambient Alpha !!!
	_setCmd( i3PSPGfx::MAA( i3Color::GetA( &m_AmbientColor )));

	// Diffuse
	_setCmd( i3PSPGfx::MDC( i3Color::GetR( &m_DiffuseColor),
							i3Color::GetG( &m_DiffuseColor),
							i3Color::GetB( &m_DiffuseColor)));

	// Specular
	_setCmd( i3PSPGfx::MSC( i3Color::GetR( &m_SpecularColor),
							i3Color::GetG( &m_SpecularColor),
							i3Color::GetB( &m_SpecularColor)));

	// Emissive
	_setCmd( i3PSPGfx::MEC( i3Color::GetR( &m_EmissiveColor),
							i3Color::GetG( &m_EmissiveColor),
							i3Color::GetB( &m_EmissiveColor)));

	// Shininess
	_setCmd( i3PSPGfx::MK( Shininess));
#else
	COLORREAL clrAmbient;
	i3Color::Set( &clrAmbient, 0.0f, 0.0f, 0.0f, i3Color::GetA( &m_AmbientColor ) );
	sceAmbientColor( i3Color::ConvertRGBA32( &m_AmbientColor) );
	sceGuMaterial( SCEGU_AMBIENT, i3Color::ConvertRGBA32( &m_AmbientColor));
	sceGuMaterial( SCEGU_DIFFUSE, i3Color::ConvertRGBA32( &m_DiffuseColor));
	sceGuMaterial( SCEGU_SPECULAR, i3Color::ConvertRGBA32( &m_SpecularColor));
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

#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::TEC( i3Color::GetR( &m_Color), i3Color::GetG( &m_Color), i3Color::GetB( &m_Color) ) );
#else
	//sceGuColor( i3Color::ConvertRGBA32( &m_Color));
	sceGuSetexEnvColor( i3Color::ConvertRGBA32( &m_Color) );
#endif
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


void i3RenderContext::SetVertexBlendMatrix( INT32 Idx, MATRIX * pMatrix)
{
	I3ASSERT((Idx >= 0) && (Idx < 8));

#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::BONEN( Idx * 12));

	_setCmd( i3PSPGfx::BONED( i3Matrix::Get11( pMatrix)));
	_setCmd( i3PSPGfx::BONED( i3Matrix::Get12( pMatrix)));
	_setCmd( i3PSPGfx::BONED( i3Matrix::Get13( pMatrix)));

	_setCmd( i3PSPGfx::BONED( i3Matrix::Get21( pMatrix)));
	_setCmd( i3PSPGfx::BONED( i3Matrix::Get22( pMatrix)));
	_setCmd( i3PSPGfx::BONED( i3Matrix::Get23( pMatrix)));

	_setCmd( i3PSPGfx::BONED( i3Matrix::Get31( pMatrix)));
	_setCmd( i3PSPGfx::BONED( i3Matrix::Get32( pMatrix)));
	_setCmd( i3PSPGfx::BONED( i3Matrix::Get33( pMatrix)));

	_setCmd( i3PSPGfx::BONED( i3Matrix::Get41( pMatrix)));
	_setCmd( i3PSPGfx::BONED( i3Matrix::Get42( pMatrix)));
	_setCmd( i3PSPGfx::BONED( i3Matrix::Get43( pMatrix)));
#else
	sceGuBoneMatrix( Idx, (const ScePspFMatrix4 *) pMatrix);
#endif
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
//	bState = FALSE;
	m_bTextureEnable = (bState != FALSE);


#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::TME( bState));
#else
	if( bState)
		sceGuEnable( SCEGU_TEXTURE);
	else
		sceGuDisable( SCEGU_TEXTURE);
#endif
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
	#ifdef I3_PSP_DISPLAY_LIST
	_setCmd( i3PSPGfx::TFLUSH());
	#endif

	if( pTexture != NULL)
	{
		i3TexturePSP * pTexPSP = (i3TexturePSP *) pTexture;
		UINT32 i;

		UINT32 level = pTexPSP->GetLevelCount() - 1;

		#ifdef I3_PSP_DISPLAY_LIST
			i3PSPGfx::TMODE_HSM hsm = ( pTexPSP->GetFormat() & I3G_IMAGE_FORMAT_MASK_PSP ) ? (i3PSPGfx::TMODE_HIGHSPEED) : (i3PSPGfx::TMODE_NORMAL);
			i3PSPGfx::TMODE_MC mc = ( pTexPSP->GetLevelCount() > 1) ? (i3PSPGfx::TMODE_MULTI_CLUT) : (i3PSPGfx::TMODE_SINGLE_CLUT);

			// Texture Mode
			_setCmd( i3PSPGfx::TMODE( hsm, mc, level ) );
			//_setCmd( i3PSPGfx::TLEVEL( i3PSPGfx::TLEVEL_VARIABLE1, (-24 & 0xFF) ) );
			//_setCmd( i3PSPGfx::TSLOPE( 10.00002f ) );
			_setCmd( i3PSPGfx::TLEVEL( i3PSPGfx::TLEVEL_VARIABLE2, 0 ) );
			_setCmd( i3PSPGfx::TSLOPE( 0.017f ) );
			// Texture Pixel Format
			_setCmd( i3PSPGfx::TPF( (i3PSPGfx::TEX_FORMAT) pTexPSP->GetNativeFormat()));
		#else
			int hsm = ( pTexPSP->GetFormat() & I3G_IMAGE_FORMAT_MASK_PSP ) ? SCEGU_TEXBUF_FAST : SCEGU_TEXBUF_NORMAL;
			int mc = ( pTexPSP->GetLevelCount() > 1) ? SCEGU_MULTI_CLUT : SCEGU_SINGLE_CLUT;

			// Texture Mode & Pixel Format
			sceGuTexMode( pTexPSP->GetNativeFormat(), level, mc, hsm );
		#endif

		if( pTexPSP->IsIndexed())
		{	// 인덱스 컬러일 경우 CLUT 로드
			i3Clut * pClut;

			pClut = pTexPSP->GetClut(0);

		#ifdef I3_PSP_DISPLAY_LIST
			_setCmd( i3PSPGfx::CBP( (UINT32) pClut->GetData()));
			_setCmd( i3PSPGfx::CBW( (UINT32) pClut->GetData()));

			_setCmd( i3PSPGfx::CLUT( (i3PSPGfx::CLUT_FORMAT)SCEGU_PF8888, 0, 0xFF, 0) );
			_setCmd( i3PSPGfx::CLOAD( pClut->GetColorCount() >> 3));
		#else
			sceGuClutMode( SCEGU_PF8888, 0, 0xFF, 0);
			sceGuClutLoad( pClut->GetColorCount() >> 3, pClut->GetData());
		#endif
		}

		#ifdef I3_PSP_DISPLAY_LIST
			UINT32 w = getLog2( pTexPSP->GetWidth());
			UINT32 h = getLog2( pTexPSP->GetHeight());
		#endif

		for( i = 0; i < pTexPSP->GetLevelCount(); i++)
		{
			#ifdef I3_PSP_DISPLAY_LIST
				_setCmd( i3PSPGfx::TSIZE( i, w, h));

				_setCmd( i3PSPGfx::TBP( i, (UINT32) pTexPSP->GetImageData( i)));
				_setCmd( i3PSPGfx::TBW( i, (UINT32) pTexPSP->GetImageData( i), pTexPSP->GetBufferWidth( i)));

				w--;
				h--;
				
			#else
				sceGuTexImage( i, pTexPSP->GetWidth(), pTexPSP->GetHeight(),
						pTexPSP->GetBufferWidth(i),	pTexPSP->GetImageData(i));
			#endif
		}
	}
}

void i3RenderContext::SetTextureFilter( INT32 StageIdx, I3G_TEXTURE_FILTER MagFilter, I3G_TEXTURE_FILTER MinFilter)
{
	m_TexMinFilter = MinFilter;
	m_TexMagFilter = MagFilter;

	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::TFILTER( (i3PSPGfx::TFILTER_MODE) MinFilter, 
									(i3PSPGfx::TFILTER_MODE) MagFilter));
	#else
		sceGuTexFilter( MinFilter, MagFilter);
	#endif
}

void i3RenderContext::SetTextureWrap( INT32 StageIdx, I3G_TEXTURE_WRAP HorzWrap, I3G_TEXTURE_WRAP VertWrap)
{
	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::TWRAP(	(i3PSPGfx::TWRAP_MODE) HorzWrap,
									(i3PSPGfx::TWRAP_MODE) VertWrap));
	#else
		sceGuTexWrap( HorzWrap, VertWrap);
	#endif
}

void i3RenderContext::SetTextureFunction( INT32 StageIdx, I3G_TEXTURE_FUNCTION func)
{
	m_TexFunc = func;

	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::TFUNC( (i3PSPGfx::TFUNC_TXF) m_TexFunc, i3PSPGfx::TFUNC_RGBA, FALSE));
	#else
		sceGuTexFunc( func, SCEGU_RGBA);
	#endif
}

void i3RenderContext::SetTextureMatrix( INT32 StageIdx, MATRIX * pMatrix, INT32 CoordCount, BOOL bProjective)
{
	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::TGENN( 0));
		_setCmd( i3PSPGfx::TGEND( i3Matrix::Get11( pMatrix )));
		_setCmd( i3PSPGfx::TGEND( i3Matrix::Get12( pMatrix )));
		_setCmd( i3PSPGfx::TGEND( i3Matrix::Get13( pMatrix )));
		_setCmd( i3PSPGfx::TGEND( i3Matrix::Get21( pMatrix )));
		_setCmd( i3PSPGfx::TGEND( i3Matrix::Get22( pMatrix )));
		_setCmd( i3PSPGfx::TGEND( i3Matrix::Get23( pMatrix )));
		_setCmd( i3PSPGfx::TGEND( i3Matrix::Get31( pMatrix )));
		_setCmd( i3PSPGfx::TGEND( i3Matrix::Get32( pMatrix )));
		_setCmd( i3PSPGfx::TGEND( i3Matrix::Get33( pMatrix )));
		_setCmd( i3PSPGfx::TGEND( i3Matrix::Get41( pMatrix )));
		_setCmd( i3PSPGfx::TGEND( i3Matrix::Get42( pMatrix )));
		_setCmd( i3PSPGfx::TGEND( i3Matrix::Get43( pMatrix )));
	#else
		sceGuSetMatrix( SCEGU_MATRIX_TEXTURE, (const ScePspFMatrix4 *) pMatrix);
	#endif

	m_bProjectiveTexture = bProjective;

	if( bProjective)
	{
		#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::TMAP(	i3PSPGfx::TMAP_TMN_PROJECTION, 
									(i3PSPGfx::TMAP_TMI) m_TextureCoordSource));
		#else
			sceGuTexMapMode( SCEGU_PROJECTION_MAP, 0, 0);
		#endif
	}
	else
	{
		#ifdef I3_PSP_DISPLAY_LIST
			_setCmd( i3PSPGfx::TMAP(	i3PSPGfx::TMAP_TMN_UV, 
										(i3PSPGfx::TMAP_TMI) m_TextureCoordSource));
		#else
			sceGuTexMapMode( SCEGU_UV_MAP, 0, 0);
		#endif
	}
}

void i3RenderContext::SetTextureCoordScale( INT32 StageIdx, VEC3D * pScale, INT32 CoordCount)
{
	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::SU( i3Vector::GetX( pScale)));
		_setCmd( i3PSPGfx::SV( i3Vector::GetY( pScale)));
	#else
		sceGuSetTexScale( i3Vector::GetX( pScale), i3Vector::GetY( pScale));
	#endif
}

void i3RenderContext::SetTextureCoordOffset( INT32 StageIdx, VEC3D * pOffset, INT32 CoordCount)
{
	#ifdef I3_PSP_DISPLAY_LIST
		_setCmd( i3PSPGfx::TU( i3Vector::GetX( pOffset)));
		_setCmd( i3PSPGfx::TV( i3Vector::GetY( pOffset)));
	#else
		sceGuSetTexScale( i3Vector::GetX( pOffset), i3Vector::GetY( pOffset));
	#endif
}

void i3RenderContext::SetTextureCoordSource( INT32 StageIdx, I3G_TEXTURE_COORD_SOURCE source)
{
	m_TextureCoordSource = source;

	#ifdef I3_PSP_DISPLAY_LIST
		if( m_bProjectiveTexture)
		{
			_setCmd( i3PSPGfx::TMAP(	i3PSPGfx::TMAP_TMN_PROJECTION, 
								(i3PSPGfx::TMAP_TMI) m_TextureCoordSource));
		}
		else
		{
			_setCmd( i3PSPGfx::TMAP(	i3PSPGfx::TMAP_TMN_UV, 
								(i3PSPGfx::TMAP_TMI) m_TextureCoordSource));
		}
	#else
		sceGuTexProjMapMode( m_TextureCoordSource);
	#endif
}

#ifndef I3_PSP_DISPLAY_LIST
static int s_LightTbl[4] = 
{
	SCEGU_LIGHT0,
	SCEGU_LIGHT1,
	SCEGU_LIGHT2,
	SCEGU_LIGHT3
};
#endif

void i3RenderContext::SetLight( i3Light * pLight)
{
	if( pLight->GetEnable())
	{
		VEC3D * pAtt = pLight->GetAttenuation();
		COLOR col;

		// Enable
		#ifdef I3_PSP_DISPLAY_LIST
			switch( pLight->GetIndex())
			{
				case 1 :	_setCmd( i3PSPGfx::LE1( TRUE));	break;
				case 2 :	_setCmd( i3PSPGfx::LE2( TRUE));	break;
				case 3 :	_setCmd( i3PSPGfx::LE3( TRUE));	break;
				default :	_setCmd( i3PSPGfx::LE0( TRUE));	break;
			}

			_setCmd( i3PSPGfx::LMODE( i3PSPGfx::LMODE_SINGLE_COLOR));
			_setCmd( i3PSPGfx::LTYPE( pLight->GetIndex(), i3PSPGfx::LTYPE_DIFFUSE,
				(i3PSPGfx::LIGHT_TYPE) pLight->GetLightType()));
		#else
			sceGuEnable( s_LightTbl[ pLight->GetIndex()]);
			sceGuLight( pLight->GetIndex(), pLight->GetLightType(), SCEGU_DIFFUSE, (const ScePspFVector3 *) pLight->GetWorldDirection());
		#endif

		// Light Colors
		{
			// Ambient
			i3Color::Set( &col, pLight->GetAmbient());

			#ifdef I3_PSP_DISPLAY_LIST
				_setCmd( i3PSPGfx::LAC( pLight->GetIndex(), 
							i3Color::GetR( &col),
							i3Color::GetG( &col),
							i3Color::GetB( &col)));
			#else
				sceGuLightColor( pLight->GetIndex(), SCEGU_AMBIENT, i3Color::ConvertRGBA32( &col));
				sceGuAmbientColor( i3Color::ConvertRGBA32( &col));
			#endif

			// Diffuse
			i3Color::Set( &col, pLight->GetDiffuse());

			#ifdef I3_PSP_DISPLAY_LIST
				_setCmd( i3PSPGfx::LDC( pLight->GetIndex(), 
							i3Color::GetR( &col),
							i3Color::GetG( &col),
							i3Color::GetB( &col)));
			#else
				sceGuLightColor( pLight->GetIndex(), SCEGU_DIFFUSE, i3Color::ConvertRGBA32( &col));
			#endif

			// Specular
			i3Color::Set( &col, pLight->GetSpecular());

			#ifdef I3_PSP_DISPLAY_LIST
				_setCmd( i3PSPGfx::LSC( pLight->GetIndex(), 
							i3Color::GetR( &col),
							i3Color::GetG( &col),
							i3Color::GetB( &col)));
			#else
				sceGuLightColor( pLight->GetIndex(), SCEGU_SPECULAR, i3Color::ConvertRGBA32( &col));
			#endif
		}

		// Spot & Point Light
		switch( pLight->GetType())
		{
			case I3G_LIGHT_DIRECTIONAL :
				{
					VEC3D * pDir;
					//MATRIX invMtx;

					//i3Matrix::Transpose3x3( &invMtx, &m_LastViewMatrix);
					//i3Vector::TransformNormal( &dir, pLight->GetDirection(), &invMtx);
					pDir =  pLight->GetWorldDirection();

					// Direction
					_setCmd( i3PSPGfx::LX( pLight->GetIndex(), i3Vector::GetX( pDir)));
					_setCmd( i3PSPGfx::LY( pLight->GetIndex(), i3Vector::GetY( pDir)));
					_setCmd( i3PSPGfx::LZ( pLight->GetIndex(), i3Vector::GetZ( pDir)));

					_setCmd( i3PSPGfx::LDX( pLight->GetIndex(), i3Vector::GetX( pDir)));
					_setCmd( i3PSPGfx::LDY( pLight->GetIndex(), i3Vector::GetY( pDir)));
					_setCmd( i3PSPGfx::LDZ( pLight->GetIndex(), i3Vector::GetZ( pDir)));
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
					sceGuLightSpot( pLight->GetIndex(), pLight->GetFalloffRate(),
						i3Math::cos( pLight->GetCutoffAngle()),
						(const ScePspFVector3 *) pLight->GetWorldDirection());
				
					sceGuLightAtt( pLight->GetIndex(), i3Vector::GetX( pAtt), i3Vector::GetY( pAtt), i3Vector::GetZ( pAtt));
				#endif
				}
				break;

			default :
				break;
		}
	}
	else
	{
		#ifdef I3_PSP_DISPLAY_LIST
			switch( pLight->GetIndex())
			{
				case 1 :	_setCmd( i3PSPGfx::LE1( FALSE));	break;
				case 2 :	_setCmd( i3PSPGfx::LE2( FALSE));	break;
				case 3 :	_setCmd( i3PSPGfx::LE3( FALSE));	break;
				default :	_setCmd( i3PSPGfx::LE0( FALSE));	break;
			}
		#else
			sceGuDisable( s_LightTbl[ pLight->GetIndex()]);
		#endif
	}
}

void i3RenderContext::SetPointSpriteEnable( BOOL bFlag)
{
}

void i3RenderContext::SetPointSpriteSizeSpace( I3G_POINT_SPRITE_SIZE_SPACE SizeSpace)
{
}

void i3RenderContext::DrawCommandList( i3GPUCommandList * pCmdList )
{
	I3ASSERT( pCmdList->GetGPUType() == I3G_GPU_PSP );

	{
		MATRIX * pMatrix = & m_LastViewMatrix;

		_setCmd( i3PSPGfx::VIEWN( 0));

		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get11( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get12( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get13( pMatrix)));

		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get21( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get22( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get23( pMatrix)));

		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get31( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get32( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get33( pMatrix)));

		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get41( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get42( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get43( pMatrix)));
	}

	void *pCallAddr = pCmdList->GetBuffer();
	_setCmd( i3PSPGfx::BASE( pCallAddr ) );
	_setCmd( i3PSPGfx::CALL( pCallAddr ) );

	_setCmd( i3PSPGfx::TME( m_bTextureEnable));
	_setCmd( i3PSPGfx::LTE( m_bLightingEnable));
	_setCmd( i3PSPGfx::TFUNC( i3PSPGfx::TFUNC_REPLACE, i3PSPGfx::TFUNC_RGBA, FALSE));
	_setCmd( i3PSPGfx::SU( 1.0f));
	_setCmd( i3PSPGfx::SV( 1.0f));
	_setCmd( i3PSPGfx::TEC( i3Color::GetR( &m_Color), i3Color::GetG( &m_Color), i3Color::GetB( &m_Color) ) );
	_setCmd( i3PSPGfx::FGE( m_bFogEnable));
	_setCmd( i3PSPGfx::TFILTER( (i3PSPGfx::TFILTER_MODE) m_TexMinFilter, 
									(i3PSPGfx::TFILTER_MODE) m_TexMagFilter));
	_setCmd( i3PSPGfx::BCE( m_bBCE));
	_setCmd( i3PSPGfx::CULL( (i3PSPGfx::CULL_MODE) ! m_FaceCull) );
	_setCmd( i3PSPGfx::ZMSK( ! m_bZWriteEnable ) );
	_setCmd( i3PSPGfx::ABE( m_bBlendEnable));
	_setCmd( i3PSPGfx::ATE( m_bAlphaTestEnable));

	SetSrcBlend( m_SrcBlend);
	SetDestBlend( m_DestBlend);

	{
		MATRIX * pMatrix = & I3_IDENTITYMATRIX;

		// View Matrix 설정을 Identity Matrix로 되돌린다.
		// 정도(Density)문제를 피하기 위해, PSP에서는 View Matrix를 사용하지 않기 때문이다.
		// 단, 지금과 같이 GPU Command List를 Rendering하는 경우에는 View Matrix를
		// 사용하지 않을 수 없다.

		_setCmd( i3PSPGfx::VIEWN( 0));

		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get11( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get12( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get13( pMatrix)));

		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get21( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get22( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get23( pMatrix)));

		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get31( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get32( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get33( pMatrix)));

		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get41( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get42( pMatrix)));
		_setCmd( i3PSPGfx::VIEWD( i3Matrix::Get43( pMatrix)));
	}

	m_bTextureEnable = TRUE;
	m_bLightingEnable = FALSE;

	m_OldTexFunc = -1;
}

BOOL i3RenderContext::Reset(void)
{
	INT32 i;

	sceGuInit();

#if defined( I3_PSP_GE_PROFILE)
	{
		// GE의 Processing Time을 측정하기 위한 Interrupt Handler 설정

		sceGuSetCallback( SCEGU_INT_SIGNAL, _GEProfileSignalProc);
		sceGuSetCallback( SCEGU_INT_FINISH, _GEProfileFinishProc);
	}
#endif

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Create 단계에서는 sceGu 함수만을 사용한다.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Begin
	//Begin();
	i3System::DCacheWritebackAll();
	sceGuStart( SCEGU_IMMEDIATE, s_DispList, DISPBUFSZ );

	for( i = 0; i < GetRenderTargetCount(); i++)
	{
		i3RenderTarget * pRT = GetRenderTarget( i);
		
		pRT->Create( SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT, pRT->GetFormat());
	}

	SetDefaultRenderState();

	sceGuViewport( 2048, 2048, SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT);

	s_ClearRect[0].x = 0;
	s_ClearRect[0].y = 0;
	s_ClearRect[1].x = SCEGU_SCR_WIDTH;
	s_ClearRect[1].y = SCEGU_SCR_HEIGHT;

	m_ClearFlag = I3G_CLEAR_COLOR | I3G_CLEAR_DEPTH;

	SetClearColor( &m_BackColor);
	SetClearZValue( m_ClearZValue);
	SetClearStencilValue( m_ClearStencilValue);

	sceGuClearColor( 0x00800000 );
	sceGuScissor(0, 0, SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT);
	sceGuEnable(SCEGU_SCISSOR_TEST);

	sceGuEnable( SCEGU_DITHER);
	sceGuSetDither( (const ScePspIMatrix4 *) &dither);

	sceGuEnable( SCEGU_CLIP_PLANE);
	sceGuDisable( SCEGU_COLOR_DOUBLE);
	sceGuDisable( SCEGU_NORMAL_REVERSE_PRIM);
	sceGuDisable( SCEGU_NORMAL_REVERSE_PATCH);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// 이 함수를 호출함으로써 AA, MAA 를 0xFF 값으로 설정한다!!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	sceGuAmbient( 0xFF000000 );			// 글로벌 환경광
	sceGuAmbientColor( 0xFF000000 );	// 모델 환경광

	i3System::DCacheWritebackAll();

	sceGuFinish();
	sceGuSync( SCEGU_SYNC_FINISH, SCEGU_SYNC_WAIT);
	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();

	// Display On
	I3TRACE( "sceGuDisplay( 1 );\n");
	sceGuDisplay( 1 );

	I3TRACE("Reset Complet!!\n");

	return TRUE;
}

#endif //I3G_PSP
