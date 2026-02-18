#include "i3GfxType.h"
#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3Debug.h"
#include "i3GfxType.h"
#include "i3GfxDefine.h"
#include "i3VertexArray.h"
#include "i3GfxGlobalVariable.h"

#if defined( I3_DEBUG)
#include "i3GfxPSPCommandBuilder.h"
#include "i3GPUCommandParserPSP.h"

typedef void	(*I3PSPGPUCOMMANDPROC)( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pContext);

#define			I3PSP_GCP_REAL32				0x00000001
#define			I3PSP_GCP_BOOL					0x00000002
#define			I3PSP_GCP_UPPER_ADDR			0x00000003
#define			I3PSP_GCP_LOWER_ADDR			0x00000004
#define			I3PSP_GCP_COMPLEX				0x0000000F


struct I3PSP_GPU_COMMAND_INFO
{
	char					m_szName[32];
	I3PSPGPUCOMMANDPROC		m_pProc;
	UINT32					m_ParamType;
} ;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// UPPER 24bits FLOAT
static void _Upper_24bits_Float_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 iVal = (*pBuf & 0x00FFFFFF) << 8;
	REAL32 rVal = *((REAL32*)( &iVal ));
	I3TRACE( "[%p] %0.8X : %s = %f\n", pBuf, *pBuf, szName, rVal );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// VADR
static void _VADR_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	I3TRACE( "[%p] %0.8X : VADR - %08X\n", pBuf, *pBuf, *pBuf & 0x00FFFFFF);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// VTYPE
static void _VTYPE_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 TT, CT, NT, VT, WT, IT, WC, MC, TRU;
	static char s_FormatStr[][32] =
	{
		"None",
		"F8:7",
		"F16:15",
		"R32"
	};
	static char s_ColorStr[][32] =
	{
		"None",
		"Reserved",
		"Reserved",
		"Reserved",
		"16_565",
		"16_5551",
		"16_4444",
		"32_8888"
	};
	static char s_IndexStr[][32] =
	{
		"None",
		"8bits",
		"16bits",
		"Reserved",
	};

	TT = (*pBuf) & 0x03;
	CT = (*pBuf >> 2) & 0x07;
	NT = (*pBuf >> 5) & 0x03;
	VT = (*pBuf >> 7) & 0x03;
	WT = (*pBuf >> 9) & 0x03;
	IT = (*pBuf >> 11) & 0x03;
	WC = (*pBuf >> 14) & 0x07;
	MC = (*pBuf >> 18) & 0x07;
	TRU = (*pBuf >> 23) & 0x01;

	I3TRACE( "[%p] %0.8X : VTYPE - TT:%s  CT:%s  NT:%s  VT:%s  WT:%s  IT:%s  WC:%d  MC:%d  TRU:%d\n", pBuf, *pBuf, 
		s_FormatStr[TT],
		s_ColorStr[CT],
		s_FormatStr[NT],
		s_FormatStr[VT],
		s_FormatStr[WT],
		s_IndexStr[IT],
		WC, MC, TRU	);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIM
static void _PRIM_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 Prim, Count;
	static char s_PrimStr[][32] =
	{
		"POINTS",
		"LINES",
		"LINE_STRIP",
		"TRI_LIST",
		"TRI_STRIP",
		"TRI_FAN",
		"RECTS",
		"Reserved"
	};

	Prim = (*pBuf >> 16) & 0x07;
	Count = (*pBuf & 0xFFFF);

	I3TRACE( "[%p] %0.8X : PRIM - Prim:%s  Count:%d\n", pBuf, *pBuf, s_PrimStr[Prim], Count);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// JUMP
static void _JUMP_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 addr;

	addr = ((*pBuf & 0x00FFFFFF) | pCtx->GetBASE()) + pCtx->GetOFFSET();

	I3TRACE( "[%p] %0.8X : JUMP -> %08X (%08X)\n", pBuf, *pBuf, addr, addr + ((UINT32) pCtx->GetBuffer()));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// WORLDD
static void _WORLDD_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 val;

	val = (*pBuf & 0x00FFFFFF) << 8;

	I3TRACE( "[%p] %0.8X : WORLDD - (%d) %f\n", pBuf, *pBuf, pCtx->GetWORLDN(), *((REAL32 *)&val));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TMODE
static void _TMODE_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 hsm, mc, mxl;

	hsm = (*pBuf & 0x01);
	mc = (*pBuf >> 8) & 0x01;
	mxl = (*pBuf >> 16) & 0x07;

	I3TRACE( "[%p] %0.8X : TMODE - HSM:%d  MC:%d  MXL:%d\n", pBuf, *pBuf, hsm, mc, mxl);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TPF
static void _TPF_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	static char s_PFStr[][32]=
	{
		"16_565",
		"16_5551",
		"16_4444",
		"32_8888",
		"I4",
		"I8",
		"I16",
		"I32",
		"DXT1",
		"DXT3",
		"DXT5",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved"
	};

	I3TRACE( "[%p] %0.8X : TPF - %s\n", pBuf, *pBuf, s_PFStr[(*pBuf & 0x0F)]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TSIZE
static void _TSIZE_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 level, th, tw;

	level = ((*pBuf & 0xFF000000) - I3G_PSP_CMD_TSIZE0) >> 24;
	th = (*pBuf >> 8) & 0x0F;
	tw = (*pBuf & 0x0F);

	I3TRACE( "[%p] %0.8X : TSIZE - LEVEL:%d  TW:%d  TH:%d\n", pBuf, *pBuf, level, tw, th);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TBW
static void _TBW_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 level;

	level = ((*pBuf & 0xFF000000) - I3G_PSP_CMD_TBW0) >> 24;

	I3TRACE( "[%p] %0.8X : TBW - LEVEL:%d  TBW:%d\n", pBuf, *pBuf, level, (*pBuf & 0x7F));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CLUT
static void _CLUT_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 cpf, sft, msk, csa;
	static char s_CPFStr[][32] =
	{
		"16_565",
		"16_5551",
		"16_4444",
		"32_8888"
	};

	cpf = (*pBuf & 0x03);			// 2bits
	sft = (*pBuf >> 2) & 0x1F;		// 5bits
	msk = (*pBuf >> 8) & 0xFF;	// 8bits
	csa = (*pBuf >> 16) & 0x1F;		// 5bits

	I3TRACE( "[%p] %0.8X : CLUT - CPF:%s  SFT:%d  MSK:%d  CSA:%d\n", pBuf, *pBuf, s_CPFStr[cpf], sft, msk, csa);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CLOAD
static void _CLOAD_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 np;

	np = (*pBuf & 0x3F);			// 6bits

	I3TRACE( "[%p] %0.8X : CLOAD - NP:%d\n", pBuf, *pBuf, np);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TFILTER
static void _TFILTER_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 min, mag;
	static char s_FilterStr[][32] =
	{
		"POINT",
		"LINEAR",
		"POINT_MIPMAP_POINT",
		"LINEAR_MIPMAP_POINT",
		"POINT_MIPMAP_LINEAR",
		"LINEAR_MIPMAP_LINEAR"
	};

	min = (*pBuf & 0x07);
	mag = (*pBuf >> 8) & 0x01;

	I3TRACE( "[%p] %0.8X : TFILTER - MIN:%s  MAG:%s\n", pBuf, *pBuf, s_FilterStr[min], s_FilterStr[mag]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TME
static void _TME_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	I3TRACE( "[%p] %0.8X : TME - %d\n", pBuf, *pBuf, (*pBuf & 0x01));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// SX
static void _SX_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 val;

	val = (*pBuf << 8);
	I3TRACE( "[%p] %0.8X : SX - %f\n", pBuf, *pBuf, *((REAL32 *) &val));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// SY
static void _SY_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 val;

	val = (*pBuf << 8);
	I3TRACE( "[%p] %0.8X : SY - %f\n", pBuf, *pBuf, *((REAL32 *) &val));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// SZ
static void _SZ_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 val;

	val = (*pBuf << 8);
	I3TRACE( "[%p] %0.8X : SZ - %f\n", pBuf, *pBuf, *((REAL32 *) &val));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TX
static void _TX_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 val;

	val = (*pBuf << 8);
	I3TRACE( "[%p] %0.8X : TX - %f\n", pBuf, *pBuf, *((REAL32 *) &val));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TY
static void _TY_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 val;

	val = (*pBuf << 8);
	I3TRACE( "[%p] %0.8X : TY - %f\n", pBuf, *pBuf, *((REAL32 *) &val));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TZ
static void _TZ_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 val;

	val = (*pBuf << 8);
	I3TRACE( "[%p] %0.8X : TZ - %f\n", pBuf, *pBuf, *((REAL32 *) &val));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// SU
static void _SU_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 val;

	val = (*pBuf << 8);
	I3TRACE( "[%p] %0.8X : SU - %f\n", pBuf, *pBuf, *((REAL32 *) &val));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// SV
static void _SV_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 val;

	val = (*pBuf << 8);
	I3TRACE( "[%p] %0.8X : SV - %f\n", pBuf, *pBuf, *((REAL32 *) &val));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// REGION1
static void _REGION1_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 sx, sy;

	sx = (*pBuf & 0x3FF);
	sy = (*pBuf >> 10) & 0x3FF;

	I3TRACE( "[%p] %0.8X : REGION1 - SX:%u  SY:%u\n", pBuf, *pBuf, sx, sy);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// REGION2
static void _REGION2_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 sx, sy;

	sx = (*pBuf & 0x3FF);
	sy = (*pBuf >> 10) & 0x3FF;

	I3TRACE( "[%p] %0.8X : REGION2 - SX:%u  SY:%u\n", pBuf, *pBuf, sx, sy);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// OFFSETX
static void _OFFSETX_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 off;

	off = (*pBuf & 0xFFFF);

	I3TRACE( "[%p] %0.8X : OFFSETX - %f\n", pBuf, *pBuf, off / 16.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// OFFSETY
static void _OFFSETY_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 off;

	off = (*pBuf & 0xFFFF);

	I3TRACE( "[%p] %0.8X : OFFSETY - %f\n", pBuf, *pBuf, off / 16.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// SCISSOR1
static void _SCISSOR1_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 sx, sy;

	sx = (*pBuf & 0x3FF);
	sy = (*pBuf >> 10) & 0x3FF;

	I3TRACE( "[%p] %0.8X : SCISSOR1 - SX:%u  SY:%u\n", pBuf, *pBuf, sx, sy);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// SCISSOR2
static void _SCISSOR2_Proc( const char *szName, UINT32 * pBuf, i3GPUContextPSP * pCtx)
{
	UINT32 sx, sy;

	sx = (*pBuf & 0x3FF);
	sy = (*pBuf >> 10) & 0x3FF;

	I3TRACE( "[%p] %0.8X : SCISSOR2 - SX:%u  SY:%u\n", pBuf, *pBuf, sx, sy);
}

static	I3PSP_GPU_COMMAND_INFO s_GPUCmdInfo[] = 
{
	{ "NOP",		nullptr,		0},		// 0x00		 No Operation
	{ "VADR",		_VADR_Proc,	0},		// 0x01		 Set Vertex Data
	{ "IADR",		nullptr,		0},		// 0x02		 Set Index Data
	{ "?",			nullptr,		0},		// 0x03
	{ "PRIM",		_PRIM_Proc,	0},		// 0x04		 Draw Primitive
	{ "BEZIER",		nullptr,		0},		// 0x05		 Draw Bezier Surface
	{ "SPLINE",		nullptr,		0},		// 0x06		 Draw Spline Surface
	{ "BBOX",		nullptr,		0},		// 0x07		 Set Bounding Box
	{ "JUMP",		_JUMP_Proc,	0},		// 0x08		 Jump
	{ "BJUMP",		nullptr,		0},		// 0x09		 Conditional Jump
	{ "CALL",		nullptr,		0},		// 0x0A		 List Call
	{ "RET",		nullptr,		0},		// 0x0B		 List Return
	{ "END",		nullptr,		0},		// 0x0C		 End Reading
	{ "?",			nullptr,		0},		// 0x0D
	{ "SIGNAL",		nullptr,		0},		// 0x0E		 Signal Interrupt
	{ "FINISH",		nullptr,		0},		// 0x0F		 Finish Drawing
	{ "BASE",		nullptr,		0},		// 0x10		 Set Address Base
	{ "?",			nullptr,		0},		// 0x11
	{ "VTYPE",		_VTYPE_Proc,0},		// 0x12		 Set Vertex Type
	{ "OFFSET",		nullptr,		0},		// 0x13		 Set Offset Address
	{ "ORIGIN",		nullptr,		0},		// 0x14		 Set Origin Address
	{ "REGION1",	_REGION1_Proc,	0},	// 0x15		 Set Drawing Region 1
	{ "REGION2",	_REGION2_Proc,	0},	// 0x16		 Set Drawing Region 2
	{ "LTE",		nullptr,		0},		// 0x17		 Lighting Enable
	{ "LE0",		nullptr,		0},		// 0x18		 Light Enable 0
	{ "LE1",		nullptr,		0},		// 0x19		 Light Enable 1
	{ "LE2",		nullptr,		0},		// 0x1A		 Light Enable 2
	{ "LE3",		nullptr,		0},		// 0x1B		 Light Enable 3
	{ "CLE",		nullptr,		0},		// 0x1C		 Clipping Enable
	{ "BCE",		nullptr,		0},		// 0x1D		 Culling Enable
	{ "TME",		_TME_Proc,	0},		// 0x1E		 Texture Enable
	{ "FGE",		nullptr,		0},		// 0x1F		 Fog Enable
	{ "DTE",		nullptr,		0},		// 0x20		 Dither Enable
	{ "ABE",		nullptr,		0},		// 0x21		 Alpha Blending Enable
	{ "ATE",		nullptr,		0},		// 0x22		 Alpha Test Enable
	{ "ZTE",		nullptr,		0},		// 0x23		 Depth Test Enable
	{ "STE",		nullptr,		0},		// 0x24		 Stencil Test Enable
	{ "AAE",		nullptr,		0},		// 0x25		 Antialiasing Enable
	{ "PCE",		nullptr,		0},		// 0x26		 Patch Culling Enable
	{ "CTE",		nullptr,		0},		// 0x27		 Color Test Enable
	{ "LOE",		nullptr,		0},		// 0x28		 Logical Operation Enable
	{ "?",			nullptr,		0},		// 0x29
	{ "BONEN",		nullptr,		0},		// 0x2A		 Set Bone Matrix Number
	{ "BONED",		nullptr,		0},		// 0x2B		 Set Bone Matrix Data
	{ "WEIGHT0",	nullptr,		0},		// 0x2C		 Set Vertex Weight
	{ "WEIGHT1",	nullptr,		0},		// 0x2D		 Set Vertex Weight
	{ "WEIGHT2",	nullptr,		0},		// 0x2E		 Set Vertex Weight
	{ "WEIGHT3",	nullptr,		0},		// 0x2F		 Set Vertex Weight
	{ "WEIGHT4",	nullptr,		0},		// 0x30		 Set Vertex Weight
	{ "WEIGHT5",	nullptr,		0},		// 0x31		 Set Vertex Weight
	{ "WEIGHT6",	nullptr,		0},		// 0x32		 Set Vertex Weight
	{ "WEIGHT7",	nullptr,		0},		// 0x33		 Set Vertex Weight
	{ "?",			nullptr,		0},		// 0x34
	{ "?",			nullptr,		0},		// 0x35
	{ "DIVIDE",		nullptr,		0},		// 0x36		 Set Patch Division Count
	{ "PPM",		nullptr,		0},		// 0x37		 Set Patch Primitive
	{ "PFACE",		nullptr,		0},		// 0x38		 Set patch face
	{ "?",			nullptr,		0},		// 0x39
	{ "WORLDN",		nullptr,		0},		// 0x3A		 Set World Matrix Number
	{ "WORLDD",		_WORLDD_Proc,	0},	// 0x3B		 Set World Matrix Data
	{ "VIEWN",		nullptr,		0},		// 0x3C		 Set View Matrix Number
	{ "VIEWD",		nullptr,		0},		// 0x3D		 Set View Matrix Data
	{ "PROJN",		nullptr,		0},		// 0x3E		 Set Projection Matrix Number
	{ "PROJD",		nullptr,		0},		// 0x3F		 Set Projection Matrix Data
	{ "TGENN",		nullptr,		0},		// 0x40		 Set Texture Generation Matrix Number
	{ "TGEND",		nullptr,		0},		// 0x41		 Set Texture Generation Matrix Data
	{ "SX",			_SX_Proc,	0},		// 0x42		 Set Viewport ( SX )
	{ "SY",			_SY_Proc,	0},		// 0x43		 Set Viewport ( SY )
	{ "SZ",			_SZ_Proc,	0},		// 0x44		 Set Viewport ( SZ )
	{ "TX",			_TX_Proc,	0},		// 0x45		 Set Viewport ( TX )
	{ "TY",			_TY_Proc,	0},		// 0x46		 Set Viewport	( TY )
	{ "TZ",			_TZ_Proc,	0},		// 0x47		 Set Viewport ( TZ )
	{ "SU",			_SU_Proc,	0},		// 0x48		 Set Texture Scale ( U )
	{ "SV",			_SV_Proc,	0},		// 0x49		 Set Texture Scale ( V )
	{ "TU",			nullptr,		0},		// 0x4A		 Set Texture Offste ( U )
	{ "TV",			nullptr,		0},		// 0x4B		 Set Texture Offset ( V )
	{ "OFFSETX",	_OFFSETX_Proc,	0},	// 0x4C		 Set Screen Offset
	{ "OFFSETY",	_OFFSETY_Proc,	0},	// 0x4D		 Set Screen Offset
	{ "?",			nullptr,		0},		// 0x4E
	{ "?",			nullptr,		0},		// 0x4F
	{ "SHADE",		nullptr,		0},		// 0x50		 Set Shading Mode
	{ "NREV	",		nullptr,		0},		// 0x51		 Normal Reverse
	{ "?",			nullptr,		0},		// 0x52
	{ "MATERIA",	nullptr,		0},		// 0x53		 Material
	{ "MEC",		nullptr,		0},		// 0x54		 Set Model Color ( Emissive Color )
	{ "MAC",		nullptr,		0},		// 0x55		 Set Model Color ( Ambient Color )
	{ "MDC",		nullptr,		0},		// 0x56		 Set Model Color ( Diffuse Color )
	{ "MSC",		nullptr,		0},		// 0x57		 Set Model Color ( Specular Color )
	{ "MAA",		nullptr,		0},		// 0x58		 Set Model Color ( Alpha )
	{ "?",			nullptr,		0},		// 0x59
	{ "?",			nullptr,		0},		// 0x5A
	{ "MK",			nullptr,		0},		// 0x5B		 Set Model Specular Coefficiency
	{ "AC",			nullptr,		0},		// 0x5C		 Set Ambient Light Color
	{ "AA",			nullptr,		0},		// 0x5D		 Set Ambient Light Color ( Alpha )
	{ "LMODE",		nullptr,		0},		// 0x5E		 Set Light Mode
	{ "LTYPE0",		nullptr,		0},		// 0x5F		 Set Light Type 0
	{ "LTYPE1",		nullptr,		0},		// 0x60		 Set Light Type 1
	{ "LTYPE2",		nullptr,		0},		// 0x61		 Set Light Type 2
	{ "LTYPE3",		nullptr,		0},		// 0x62		 Set Light Type 3
	{ "LX0",		nullptr,		0},		// 0x63		 Set Light Vector 0 ( X )
	{ "LY0",		nullptr,		0},		// 0x64		 Set Light Vector 0 ( Y )
	{ "LZ0",		nullptr,		0},		// 0x65		 Set Light Vector 0 ( Z )
	{ "LX1",		nullptr,		0},		// 0x66		 Set Light Vector 1 ( X )
	{ "LY1",		nullptr,		0},		// 0x67		 Set Light Vector 1 ( Y )
	{ "LZ1",		nullptr,		0},		// 0x68		 Set Light Vector 1 ( Z )
	{ "LX2",		nullptr,		0},		// 0x69		 Set Light Vector 2 ( X )
	{ "LY2",		nullptr,		0},		// 0x6A		 Set Light Vector 2 ( Y )
	{ "LZ2",		nullptr,		0},		// 0x6B		 Set Light Vector 2 ( Z )
	{ "LX3",		nullptr,		0},		// 0x6C		 Set Light Vector 3 ( X )
	{ "LY3",		nullptr,		0},		// 0x6D		 Set Light Vector 3 ( Y )
	{ "LZ3",		nullptr,		0},		// 0x6E		 Set Light Vector 3 ( Z )
	{ "LDX0",		nullptr,		0},		// 0x6F		 Set Light Direction 0 ( X )
	{ "LDY0",		nullptr,		0},		// 0x70		 Set Light Direction 0 ( Y )
	{ "LDZ0",		nullptr,		0},		// 0x71		 Set Light Direction 0 ( Z )
	{ "LDX1",		nullptr,		0},		// 0x72		 Set Light Direction 1 ( X )
	{ "LDY1",		nullptr,		0},		// 0x73		 Set Light Direction 1 ( Y )
	{ "LDZ1",		nullptr,		0},		// 0x74		 Set Light Direction 1 ( Z )
	{ "LDX2",		nullptr,		0},		// 0x75		 Set Light Direction 2 ( X )
	{ "LDY2",		nullptr,		0},		// 0x76		 Set Light Direction 2 ( Y )
	{ "LDZ2",		nullptr,		0},		// 0x77		 Set Light Direction 2 ( Z )
	{ "LDX3",		nullptr,		0},		// 0x78		 Set Light Direction 3 ( X )
	{ "LDY3",		nullptr,		0},		// 0x79		 Set Light Direction 3 ( Y )
	{ "LDZ3",		nullptr,		0},		// 0x7A		 Set Light Direction 3 ( Z )
	{ "LKA0",		nullptr,		0},		// 0x7B		 Set Light Distance Attenuation Factor 0 ( A )
	{ "LKB0",		nullptr,		0},		// 0x7C		 Set Light Distance Attenuation Factor 0 ( B )
	{ "LKC0",		nullptr,		0},		// 0x7D		 Set Light Distance Attenuation Factor 0 ( C )
	{ "LKA1",		nullptr,		0},		// 0x7E		 Set Light Distance Attenuation Factor 1 ( A )
	{ "LKB1",		nullptr,		0},		// 0x7F		 Set Light Distance Attenuation Factor 1 ( B )
	{ "LKC1",		nullptr,		0},		// 0x80		 Set Light Distance attenuation Factor 1 ( C )
	{ "LKA2",		nullptr,		0},		// 0x81		 Set Light Distance Attenuation Factor 2 ( A )
	{ "LKB2",		nullptr,		0},		// 0x82		 Set Light Distance Attenuation Factor 2 ( B )
	{ "LKC2",		nullptr,		0},		// 0x83		 Set Light Distance Attenuation Factor 2 ( C )
	{ "LKA3",		nullptr,		0},		// 0x84		 Set Light Distance Attenuation Factor 3 ( A )
	{ "LKB3",		nullptr,		0},		// 0x85		 Set Light Distance Attenuation Factor 3 ( B )
	{ "LKC3",		nullptr,		0},		// 0x86		 Set Light Distance Attenuation Factor 3 ( C )
	{ "LKS0",		nullptr,		0},		// 0x87		 Set Light Convergence Factor 0
	{ "LKS1",		nullptr,		0},		// 0x88		 Set Light Convergence Factor 1
	{ "LKS2",		nullptr,		0},		// 0x89		 Set Light Convergence Factor 2
	{ "LKS3",		nullptr,		0},		// 0x8A		 Set Light Convergence Factor 3
	{ "LKO0",		nullptr,		0},		// 0x8B		 Set Light Cut-off Dot Product Coefficient 0
	{ "LKO1",		nullptr,		0},		// 0x8C		 Set Light Cut-off Dot Product Coefficient 1
	{ "LKO2",		nullptr,		0},		// 0x8D		 Set Lihgt Cut-off Dot Product Coefficient 2
	{ "LKO3",		nullptr,		0},		// 0x8E		 Set Light Cut-off Dot Product Coefficient 3
	{ "LAC0",		nullptr,		0},		// 0x8F		 Set Light Color 0 ( Ambient )
	{ "LDC0",		nullptr,		0},		// 0x90		 Set Light Color 0 ( Diffuse )
	{ "LSC0",		nullptr,		0},		// 0x91		 Set Light Color 0 ( Specular )
	{ "LAC1",		nullptr,		0},		// 0x92		 Set Light Color 1 ( Ambient )
	{ "LDC1",		nullptr,		0},		// 0x93		 Set Light Color 1 ( Diffuse )
	{ "LSC1",		nullptr,		0},		// 0x94		 Set Light Color 1 ( Specular )
	{ "LAC2",		nullptr,		0},		// 0x95		 Set Light Color 2 ( Ambient )
	{ "LDC2",		nullptr,		0},		// 0x96		 Set Light Color 2 ( Diffuse )
	{ "LSC2",		nullptr,		0},		// 0x97		 Set Light Color 2 ( Specular )
	{ "LAC3",		nullptr,		0},		// 0x98		 Set Light Color 3 ( Ambient )
	{ "LDC3",		nullptr,		0},		// 0x99		 Set Light Color 3 ( Diffuse )
	{ "LSC3",		nullptr,		0},		// 0x9A		 Set Light Color 3 ( Specular )
	{ "CULL",		nullptr,		0},		// 0x9B		 Culling Surface
	{ "FBP",		nullptr,		0},		// 0x9C		 Set Frame Buffer Base Point
	{ "FBW",		nullptr,		0},		// 0x9D		 Set Frame Buffer Width
	{ "ZBP",		nullptr,		0},		// 0x9E		 Set Depth Buffer Base Point
	{ "ZBW",		nullptr,		0},		// 0x9F		 Set Depth Buffer Width
	{ "TBP0",		nullptr,		0},		// 0xA0		 Set Texture Base Point 0
	{ "TBP1",		nullptr,		0},		// 0xA1		 Set Texture Base Point 1
	{ "TBP2",		nullptr,		0},		// 0xA2		 Set Texture Base Point 2
	{ "TBP3",		nullptr,		0},		// 0xA3		 Set Texture Base Point 3
	{ "TBP4",		nullptr,		0},		// 0xA4		 Set Texture Base Point 4
	{ "TBP5",		nullptr,		0},		// 0xA5		 Set Texture Base Point 5
	{ "TBP6",		nullptr,		0},		// 0xA6		 Set Texture Base Point 6
	{ "TBP7",		nullptr,		0},		// 0xA7		 Set Texture Base Point 7
	{ "TBW0",		_TBW_Proc,	0},		// 0xA8		 Set Texture Buffer Width 0
	{ "TBW1",		_TBW_Proc,	0},		// 0xA9		 Set Texture Buffer Width 1
	{ "TBW2",		_TBW_Proc,	0},		// 0xAA		 Set Texture Buffer Width 2
	{ "TBW3",		_TBW_Proc,	0},		// 0xAB		 Set Texture Buffer Width 3
	{ "TBW4",		_TBW_Proc,	0},		// 0xAC		 Set Texture Buffer Width 4
	{ "TBW5",		_TBW_Proc,	0},		// 0xAD		 Set Texture Buffer Width 5
	{ "TBW6",		_TBW_Proc,	0},		// 0xAE		 Set Texture Buffer Width 6
	{ "TBW7",		_TBW_Proc,	0},		// 0xAF		 Set Texture Buffer Width 7
	{ "CBP",		nullptr,		0},		// 0xB0		 Set CLUT Buffer Base Point
	{ "CBW",		nullptr,		0},		// 0xB1		 Set CLUT Buffer Width
	{ "XBP1",		nullptr,		0},		// 0xB2		 Set Transfer Buffer Base Point ( Source )
	{ "XBW1",		nullptr,		0},		// 0xB3		 Set Transfer Buffer Width ( Source )
	{ "XBP2",		nullptr,		0},		// 0xB4		 Set Transfer Buffer Base Point ( Destination )
	{ "XBW2",		nullptr,		0},		// 0xB5		 Set Transfer Buffer Width ( Destination )
	{ "?",			nullptr,		0},		// 0xB6
	{ "?",			nullptr,		0},		// 0xB7
	{ "TSIZE0",		_TSIZE_Proc,	0},	// 0xB8		 Set Texture Size 0
	{ "TSIZE1",		_TSIZE_Proc,	0},	// 0xB9		 Set Texture Size 1
	{ "TSIZE2",		_TSIZE_Proc,	0},	// 0xBA		 Set Texture Size 2
	{ "TSIZE3",		_TSIZE_Proc,	0},	// 0xBB		 Set Texture Size 3
	{ "TSIZE4",		_TSIZE_Proc,	0},	// 0xBC		 Set Texture Size 4
	{ "TSIZE5",		_TSIZE_Proc,	0},	// 0xBD		 Set Texture Size 5
	{ "TSIZE6",		_TSIZE_Proc,	0},	// 0xBE		 Set Texture Size 6
	{ "TSIZE7",		_TSIZE_Proc,	0},	// 0xBF		 Set Texture Size 7
	{ "TMAP",		nullptr,		0},		// 0xC0		 Set Texture Map Mode
	{ "TSHADE",		nullptr,		0},		// 0xC1		 Set Shade Mapping
	{ "TMODE",		_TMODE_Proc,	0},	// 0xC2		 Set Texture Mode
	{ "TPF",		_TPF_Proc,	0},		// 0xC3		 Set Texture Pixel Format
	{ "CLOAD",		_CLOAD_Proc,	0},	// 0xC4		 CLUT Load
	{ "CLUT",		_CLUT_Proc,	0},		// 0xC5		 Set CLUT
	{ "TFILTER",	_TFILTER_Proc,	0},	// 0xC6		 Set Texture Filter
	{ "TWRAP",		nullptr,		0},		// 0xC7		 Set Texture Wrap Mode
	{ "TLEVEL",		nullptr,		0},		// 0xC8		 Set Texture Level Mode
	{ "TFUNC",		nullptr,		0},		// 0xC9		 Set Texture Function
	{ "TEC",		nullptr,		0},		// 0xCA		 Set Texture Environment Color
	{ "TFLUSH",		nullptr,		0},		// 0xCB		 Texture Flush
	{ "TSYNC",		nullptr,		0},		// 0xCC		 Texture Synchronization
	{ "FOG1",		_Upper_24bits_Float_Proc,		0},		// 0xCD		 Set Fog Parameter 1
	{ "FOG2",		_Upper_24bits_Float_Proc,		0},		// 0xCE		 Set Fog Parameter 2
	{ "FC",			nullptr,		0},		// 0xCF		 Set Fog Color
	{ "TSLOPE",		nullptr,		0},		// 0xD0		 Set Texture Slope
	{ "?",			nullptr,		0},		// 0xD1
	{ "FPF",		nullptr,		0},		// 0xD2		 Set Frame Pixel Format
	{ "CMODE",		nullptr,		0},		// 0xD3		 Set Clear Mode
	{ "SCISSOR1",	_SCISSOR1_Proc,	0},	// 0xD4		 Set Scissoring Area 1
	{ "SCISSOR2",	_SCISSOR2_Proc,	0},	// 0xD5		 Set Scissoring Area 2
	{ "MINZ",		nullptr,		0},		// 0xD6		 Set Depth Range ( Minimum )
	{ "MAXZ",		nullptr,		0},		// 0xD7		 Set Depth Range ( Maximum )
	{ "CTEST",		nullptr,		0},		// 0xD8		 Set Color Test Parameter
	{ "CREF",		nullptr,		0},		// 0xD9		 Set Color Test Reference
	{ "CMSK",		nullptr,		0},		// 0xDA		 Set Color Test Mask
	{ "ATEST",		nullptr,		0},		// 0xDB		 Set Alpha Test Parameters
	{ "STEST",		nullptr,		0},		// 0xDC		 Set Stencil Test Parameters
	{ "SOP",		nullptr,		0},		// 0xDD		 Set Stencil Operation
	{ "ZTEST",		nullptr,		0},		// 0xDE		 Set Depth Test Parameters
	{ "BLEND",		nullptr,		0},		// 0xDF		 Set Alpha Blending Parameters
	{ "FIXA	",		nullptr,		0},		// 0xE0		 Set Fixed Color A
	{ "FIXB	",		nullptr,		0},		// 0xE1		 Set Fixed Color B
	{ "DITH1",		nullptr,		0},		// 0xE2		 Set Dither Coefficients
	{ "DITH2",		nullptr,		0},		// 0xE3		 Set Dither Coefficients
	{ "DITH3",		nullptr,		0},		// 0xE4		 Set Dither Coefficients
	{ "DITH4",		nullptr,		0},		// 0xE5		 Set Dither Coefficients
	{ "LOP",		nullptr,		0},		// 0xE6		 Set Logical Operation Parameters
	{ "ZMSK	",		nullptr,		0},		// 0xE7		 Depth Mask
	{ "PMSK1",		nullptr,		0},		// 0xE8		 Set Plane Mask
	{ "PMSK2",		nullptr,		0},		// 0xE9		 Set Plane Mask
	{ "XSTART",		nullptr,		0},		// 0xEA		 Inter-Buffer Transfer Start
	{ "XPOS1",		nullptr,		0},		// 0xEB		 Set Source Buffer Transfer Position
	{ "XPOS2",		nullptr,		0},		// 0xEC		 Set Destination Buffer Transfer Position
	{ "?",			nullptr,		0},		// 0xED
	{ "XSIZE",		nullptr,		0},		// 0xEE		 Set Transfer Size
	{ "?",			nullptr,		0},		// 0xEF
	{ "CMD_X2",		nullptr,		0},		// 0xF0		 Set Screen Coordinate X
	{ "CMD_Y2",		nullptr,		0},		// 0xF1		 Set Screen Coordinate Y
	{ "CMD_Z2",		nullptr,		0},		// 0xF2		 Set Screen Coordinate Z
	{ "CMD_S2",		nullptr,		0},		// 0xF3		 Set Texture Coordinate S
	{ "CMD_T2",		nullptr,		0},		// 0xF4		 Set Texture Coordinate T
	{ "CMD_Q2",		nullptr,		0},		// 0xF5		 Set Texture Coordinate Q
	{ "CMD_RGB2",	nullptr,		0},		// 0xF6		 Set Vertex Color 
	{ "CMD_AP2",	nullptr,		0},		// 0xF7		 Set Vertex Alpha and Primitive
	{ "CMD_F2",		nullptr,		0},		// 0xF8		 Set Fog
	{ "CMD_I2",		nullptr,		0},		// 0xF9		 Set Secondary Color
	{ "?",			nullptr,		0},		// 0xFA
	{ "?",			nullptr,		0},		// 0xFB
	{ "?",			nullptr,		0},		// 0xFC
	{ "?",			nullptr,		0},		// 0xFD
	{ "?",			nullptr,		0},		// 0xFE
	{ "?",			nullptr,		0},		// 0xFF
};

I3_CLASS_INSTANCE( i3GPUContextPSP);

void i3GPUContextPSP::Parse( UINT32 * pBuffer, INT32 BufferLength )
{
	I3PSPGPUCOMMANDPROC pProc;
	UINT32 idx;
	bool bContinue = true;
	INT32 nIndex = 0;

	m_pCommandBuffer = pBuffer;
	m_pCur = pBuffer;

	m_Offset = 0;
	m_Base = 0;
	m_WorldN = 0;

	if( m_bReport)
	{
		I3TRACE( "------------------ GPU Command Parsing ----------------------\n");
	}

	if( BufferLength == 0 )
	{
		bContinue = false;
	}

	while( bContinue )
	{
		idx = (*m_pCur >> 24) & 0xFF;

		if( m_bReport)
		{

			pProc = s_GPUCmdInfo[idx].m_pProc;
			if( pProc == nullptr)
			{
				I3TRACE( "[%p] %0.8X : %s\n", m_pCur, *m_pCur, s_GPUCmdInfo[idx].m_szName );
			}
			else
			{
				pProc( s_GPUCmdInfo[idx].m_szName, m_pCur, this);
			}
		}

		switch( *m_pCur & 0xFF000000)
		{
			case I3G_PSP_CMD_WORLDN :	m_WorldN = (*m_pCur & 0x0F);	break;
			case I3G_PSP_CMD_WORLDD :	m_WorldN ++;	break;

			case I3G_PSP_CMD_ORIGIN :	m_Offset = (UINT32) m_pCur;	break;
			case I3G_PSP_CMD_OFFSET :	m_Offset = (*m_pCur & 0x00FFFFFF) << 8;	break;
			case I3G_PSP_CMD_BASE :		m_Base = (*m_pCur & 0x00FF0000) << 8;	break;
			case I3G_PSP_CMD_JUMP :		
				m_pCur = (UINT32 *) ((((*m_pCur & 0x00FFFFFF) | m_Base) + m_Offset) + (UINT32)pBuffer);	
				m_pCur--;
				break;
		}

		if( BufferLength == -1 )
		{	// ÆÄ½Ì ±æÀÌ Á¦ÇÑÀÌ ¾øÀ¸¸é...

			// RET ¸í·É¾î¸¦ ¸¸³ª¸é ¸ØÃã
			switch( (*m_pCur & 0xFF000000))
			{
				// I3G_PSP_CMD_RET ¸í·É¾î°¡ ³ª¿À¸é ¸ØÃã
			case I3G_PSP_CMD_RET :
				bContinue = false;
				break;
				
				// I3G_PSP_CMD_FINISH¿Í I3G_PSP_CMD_END ¸í·É¾î°¡ ¿¬¼ÓÀ¸·Î ³ª¿À¸é ¸ØÃã
			default :
				if( pBuffer != m_pCur )
				{
					if( ( *(m_pCur - 1) == I3G_PSP_CMD_FINISH ) && ( *m_pCur == I3G_PSP_CMD_END ) )
					{
						bContinue = false;
					}
				}
			}
		}
		else
		{	// ÆÄ½Ì ±æÀÌ Á¦ÇÑÀÌ ÀÖ´Â °æ¿ì

			if( nIndex >= BufferLength - 1 )
			{
				bContinue = false;
			}
		}

		nIndex ++;
		m_pCur ++;
	}

	if( m_bReport)
	{
		I3TRACE( "Parsed Command Buffer Length : %d\n", nIndex );
		I3TRACE( "--------------- End of GPU Command Parsing ------------------\n");
	}
}

#endif // I3G_PSP
