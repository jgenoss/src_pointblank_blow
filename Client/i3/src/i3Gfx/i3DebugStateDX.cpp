#include "i3GFXType.h"

#ifdef I3_DEBUG
#ifdef I3G_DX

//#include <windows.h>
#include <stdio.h>
//#include <d3d8.h>
//#include <dxerr8.h>

// ============================================================================================================
// Render States
// ============================================================================================================
#define MAX_ARG_NAME 20
typedef struct
{
	D3DRENDERSTATETYPE Type;	// 렌더 스테이트 타입
	const char *Name;			// 렌더 스테이트 이름
	int NumArgName;				// 인자 인름의 개수
	const char *ArgName[MAX_ARG_NAME];	// 인자 이름 리스트
} RENDER_STATE_INFO;

static DWORD *s_RenderStateArg[2];

static RENDER_STATE_INFO s_RenderStateInfo[] =
{
	// D3DCMPFUNC
	{
		D3DRS_ZFUNC,
		"D3DRS_ZFUNC",
		9,
		{
			"<<error>>",			// 0
			"D3DCMP_NEVER",			// 1
			"D3DCMP_LESS",			// 2
			"D3DCMP_EQUAL",			// 3
			"D3DCMP_LESSEQUAL",		// 4
			"D3DCMP_GREATER",		// 5
			"D3DCMP_NOTEQUAL",		// 6
			"D3DCMP_GREATEREQUAL",	// 7
			"D3DCMP_ALWAYS",		// 8
		}
	}, 

	// D3DCMPFUNC 
	{ 
		D3DRS_ALPHAFUNC,
		"D3DRS_ALPHAFUNC",
		9,
		{
			"<<error>>",			// 0
			"D3DCMP_NEVER",			// 1
			"D3DCMP_LESS",			// 2
			"D3DCMP_EQUAL",			// 3
			"D3DCMP_LESSEQUAL",		// 4
			"D3DCMP_GREATER",		// 5
			"D3DCMP_NOTEQUAL",		// 6
			"D3DCMP_GREATEREQUAL",	// 7
			"D3DCMP_ALWAYS",		// 8
		}
	},

	// TRUE to enable alpha blending 
	{ 
		D3DRS_ALPHABLENDENABLE,
		"D3DRS_ALPHABLENDENABLE",
		2,
		{
			"FALSE",
			"TRUE",
		}
	},

	// TRUE to enable alpha tests 
	{ 
		D3DRS_ALPHATESTENABLE,
		"D3DRS_ALPHATESTENABLE",
		2,
		{
			"FALSE",
			"TRUE",
		}
	},

	// BYTE
	{ 
		D3DRS_ALPHAREF,	
		"D3DRS_ALPHAREF",
		0,
	}, 

	// D3DBLEND 
	{ 
		D3DRS_SRCBLEND,	
		"D3DRS_SRCBLEND",
		14,
		{
			"<<error>>",				// 0
			"D3DBLEND_ZERO",			//  1,
			"D3DBLEND_ONE",				//  2,
			"D3DBLEND_SRCCOLOR",		//  3,
			"D3DBLEND_INVSRCCOLOR",		//  4,
			"D3DBLEND_SRCALPHA",		//  5,
			"D3DBLEND_INVSRCALPHA",		//  6,
			"D3DBLEND_DESTALPHA",		//  7,
			"D3DBLEND_INVDESTALPHA",	//  8,
			"D3DBLEND_DESTCOLOR",		//  9,
			"D3DBLEND_INVDESTCOLOR",	// 10,
			"D3DBLEND_SRCALPHASAT",		// 11,
			"D3DBLEND_BOTHSRCALPHA",	// 12,
			"D3DBLEND_BOTHINVSRCALPHA",	// 13,
		}
	}, 

	// D3DBLEND 
	{ 
		D3DRS_DESTBLEND,	
		"D3DRS_DESTBLEND",
		14,
		{
			"<<error>>",				// 0
			"D3DBLEND_ZERO",			//  1,
			"D3DBLEND_ONE",				//  2,
			"D3DBLEND_SRCCOLOR",		//  3,
			"D3DBLEND_INVSRCCOLOR",		//  4,
			"D3DBLEND_SRCALPHA",		//  5,
			"D3DBLEND_INVSRCALPHA",		//  6,
			"D3DBLEND_DESTALPHA",		//  7,
			"D3DBLEND_INVDESTALPHA",	//  8,
			"D3DBLEND_DESTCOLOR",		//  9,
			"D3DBLEND_INVDESTCOLOR",	// 10,
			"D3DBLEND_SRCALPHASAT",		// 11,
			"D3DBLEND_BOTHSRCALPHA",	// 12,
			"D3DBLEND_BOTHINVSRCALPHA",	// 13,
		}
	}, 

	// TRUE to enable Z writes 
	{ 
		D3DRS_ZWRITEENABLE,	
		"D3DRS_ZWRITEENABLE",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// TRUE to enable dithering 
	{ 
		D3DRS_DITHERENABLE,	
		"D3DRS_DITHERENABLE",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// D3DSHADEMODE 
	{ 
		D3DRS_SHADEMODE,	
		"D3DRS_SHADEMODE",
		4,
		{
			"<<error>>",			// 0
			"D3DSHADE_FLAT",
			"D3DSHADE_GOURAUD",
			"D3DSHADE_PHONG",
		}
	}, 

	// D3DCOLORWRITEENABLE_ALPHA, etc. per-channel write enable
	{ 
		D3DRS_COLORWRITEENABLE,	
		"D3DRS_COLORWRITEENABLE",
		0,
	}, 

	// D3DSTENCILOP to do if stencil test passes and Z test fails 
	{ 
		D3DRS_STENCILZFAIL,	
		"D3DRS_STENCILZFAIL",
		9,
		{
			"<<error>>",			// 0
		    "D3DSTENCILOP_KEEP",	// 1, default
			"D3DSTENCILOP_ZERO",	// 2,
			"D3DSTENCILOP_REPLACE",	// 3,
			"D3DSTENCILOP_INCRSAT",	// 4,
			"D3DSTENCILOP_DECRSAT",	// 5,
			"D3DSTENCILOP_INVERT",	// 6,
			"D3DSTENCILOP_INCR",	// 7,
			"D3DSTENCILOP_DECR",	// 8,
		}
	}, 

	// D3DSTENCILOP to do if both stencil and Z tests pass 
	{ 
		D3DRS_STENCILPASS,	
		"D3DRS_STENCILPASS",
		9,
		{
			"<<error>>",			// 0
		    "D3DSTENCILOP_KEEP",	// 1, default
			"D3DSTENCILOP_ZERO",	// 2,
			"D3DSTENCILOP_REPLACE",	// 3,
			"D3DSTENCILOP_INCRSAT",	// 4,
			"D3DSTENCILOP_DECRSAT",	// 5,
			"D3DSTENCILOP_INVERT",	// 6,
			"D3DSTENCILOP_INCR",	// 7,
			"D3DSTENCILOP_DECR",	// 8,
		}
	}, 

	// D3DCMPFUNC
	{ 
		D3DRS_STENCILFUNC,	
		"D3DRS_STENCILFUNC",
		9,
		{
			"<<error>>",			// 0
			"D3DCMP_NEVER",			// 1
			"D3DCMP_LESS",			// 2
			"D3DCMP_EQUAL",			// 3
			"D3DCMP_LESSEQUAL",		// 4
			"D3DCMP_GREATER",		// 5
			"D3DCMP_NOTEQUAL",		// 6
			"D3DCMP_GREATEREQUAL",	// 7
			"D3DCMP_ALWAYS",		// 8
		}
	}, 

	// BYTE reference value used in stencil test 
	{ 
		D3DRS_STENCILREF,	
		"D3DRS_STENCILREF",
		0,
	}, 

	// BYTE mask value used in stencil test 
	{ 
		D3DRS_STENCILMASK,	
		"D3DRS_STENCILMASK",
		0,
	}, 

	// BYTE write mask applied to values written to stencil buffer 
	{ 
		D3DRS_STENCILWRITEMASK,	
		"D3DRS_STENCILWRITEMASK",
		0,
	}, 

	// D3DBLENDOP setting
	{ 
		D3DRS_BLENDOP,	
		"D3DRS_BLENDOP",
		6,
		{
			"<<error>>",				// 0
			"D3DBLENDOP_ADD",			// 1
			"D3DBLENDOP_SUBTRACT",		// 2
			"D3DBLENDOP_REVSUBTRACT",   // 3
			"D3DBLENDOP_MIN",			// 4
			"D3DBLENDOP_MAX",			// 5
		}
	}, 

	// TRUE to enable fog blending 
	{ 
		D3DRS_FOGENABLE,	
		"D3DRS_FOGENABLE",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// D3DFOGMODE 
	{ 
		D3DRS_FOGTABLEMODE,	
		"D3DRS_FOGTABLEMODE",
		4,
		{
			"D3DFOG_NONE",		// 0
			"D3DFOG_EXP",		// 1
			"D3DFOG_EXP2",		// 2
			"D3DFOG_LINEAR",	// 3
		}
	}, 

	// float fog start (for both vertex and pixel fog) 
	{ 
		D3DRS_FOGSTART,	
		"D3DRS_FOGSTART",
		-1, // float
	}, 

	// float fog end      
	{ 
		D3DRS_FOGEND,	
		"D3DRS_FOGEND",
		-1, // float
	}, 

	// float fog density  
	{ 
		D3DRS_FOGDENSITY,	
		"D3DRS_FOGDENSITY",
		-1, // float
	}, 

	// TRUE to enable range-based fog 
	{ 
		D3DRS_RANGEFOGENABLE,	
		"D3DRS_RANGEFOGENABLE",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 1st texture coord.
	{ 
		D3DRS_WRAP0,	
		"D3DRS_WRAP0",
		0,
	}, 

	// D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 2nd texture coord. 
	{ 
		D3DRS_WRAP1,	
		"D3DRS_WRAP1",
		0,
	}, 

	// D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 3rd texture coord. 
	{ 
		D3DRS_WRAP2,	
		"D3DRS_WRAP2",
		0,
	}, 

	// D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 4th texture coord. 
	{ 
		D3DRS_WRAP3,	
		"D3DRS_WRAP3",
		0,
	}, 

	// TRUE to enable lighting
	{ 
		D3DRS_LIGHTING,	
		"D3DRS_LIGHTING",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// TRUE to enable specular 
	{ 
		D3DRS_SPECULARENABLE,	
		"D3DRS_SPECULARENABLE",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// TRUE to enable camera-relative specular highlights
	{ 
		D3DRS_LOCALVIEWER,	
		"D3DRS_LOCALVIEWER",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// TRUE to enable per-vertex color
	{ 
		D3DRS_COLORVERTEX,	
		"D3DRS_COLORVERTEX",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// D3DMATERIALCOLORSOURCE 
	{ 
		D3DRS_SPECULARMATERIALSOURCE,	
		"D3DRS_SPECULARMATERIALSOURCE",
		3,
		{
			"D3DMCS_MATERIAL",	// 0
			"D3DMCS_COLOR1",	// 1
			"D3DMCS_COLOR2",	// 2
		}
	}, 

	// D3DMATERIALCOLORSOURCE 
	{ 
		D3DRS_DIFFUSEMATERIALSOURCE,	
		"D3DRS_DIFFUSEMATERIALSOURCE",
		3,
		{
			"D3DMCS_MATERIAL",	// 0
			"D3DMCS_COLOR1",	// 1
			"D3DMCS_COLOR2",	// 2
		}
	}, 

	// D3DMATERIALCOLORSOURCE 
	{ 
		D3DRS_AMBIENTMATERIALSOURCE,	
		"D3DRS_AMBIENTMATERIALSOURCE",
		3,
		{
			"D3DMCS_MATERIAL",	// 0
			"D3DMCS_COLOR1",	// 1
			"D3DMCS_COLOR2",	// 2
		}
	}, 

	// D3DMATERIALCOLORSOURCE 
	{ 
		D3DRS_EMISSIVEMATERIALSOURCE,	
		"D3DRS_EMISSIVEMATERIALSOURCE",
		3,
		{
			"D3DMCS_MATERIAL",	// 0
			"D3DMCS_COLOR1",	// 1
			"D3DMCS_COLOR2",	// 2
		}
	}, 

	// D3DCOLOR 
	{ 
		D3DRS_AMBIENT,	
		"D3DRS_AMBIENT",
		0,
	}, 

	// float point size 
	{ 
		D3DRS_POINTSIZE,	
		"D3DRS_POINTSIZE",
		-1, // float
	}, 

	// float point size min threshold 
	{ 
		D3DRS_POINTSIZE_MIN,	
		"D3DRS_POINTSIZE_MIN",
		-1, // float
	}, 

	// TRUE to enable point sprites
	{ 
		D3DRS_POINTSPRITEENABLE,	
		"D3DRS_POINTSPRITEENABLE",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// TRUE to enable point size scaling
	{ 
		D3DRS_POINTSCALEENABLE,	
		"D3DRS_POINTSCALEENABLE",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// float point attenuation A value 
	{ 
		D3DRS_POINTSCALE_A,	
		"D3DRS_POINTSCALE_A",
		-1, // float
	}, 

	// float point attenuation B value 
	{ 
		D3DRS_POINTSCALE_B,	
		"D3DRS_POINTSCALE_B",
		-1, // float
	}, 

	// float point attenuation C value 
	{ 
		D3DRS_POINTSCALE_C,	
		"D3DRS_POINTSCALE_C",
		-1, // float
	}, 

	// D3DPATCHEDGESTYLE
	{ 
		D3DRS_PATCHEDGESTYLE,	
		"D3DRS_PATCHEDGESTYLE",
		2,
		{
			"D3DPATCHEDGE_DISCRETE",	// 0
			"D3DPATCHEDGE_CONTINUOUS",  // 1
		}
	}, 

	// DWORD number of segments per edge when drawing patches
	{ 
		D3DRS_PATCHSEGMENTS,	
		"D3DRS_PATCHSEGMENTS",
		0,
	}, 

	// D3DVERTEXBLENDFLAGS
	{ 
		D3DRS_VERTEXBLEND,	
		"D3DRS_VERTEXBLEND",
		0,
	}, 

	// D3DCOLOR 
	{ 
		D3DRS_FOGCOLOR,	
		"D3DRS_FOGCOLOR",
		0,
	}, 

	// D3DFILLMODE        
	{ 
		D3DRS_FILLMODE,	
		"D3DRS_FILLMODE",
		4,
		{
			"<<error>>",			// 0
			"D3DFILL_POINT",		// 1
			"D3DFILL_WIREFRAME",	// 2
			"D3DFILL_SOLID",		// 3
		}
	}, 

	// TRUE to enable automatic normalization
	{ 
		D3DRS_NORMALIZENORMALS,	
		"D3DRS_NORMALIZENORMALS",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// D3DZBUFFERTYPE (or TRUE/FALSE for legacy) 
	{ 
		D3DRS_ZENABLE,	
		"D3DRS_ZENABLE",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// TRUE to enable stenciling
	{ 
		D3DRS_STENCILENABLE,	
		"D3DRS_STENCILENABLE",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// D3DSTENCILOP to do if stencil test fails 
	{ 
		D3DRS_STENCILFAIL,	
		"D3DRS_STENCILFAIL",
		9,
		{
			"<<error>>",			// 0
		    "D3DSTENCILOP_KEEP",	// 1, default
			"D3DSTENCILOP_ZERO",	// 2,
			"D3DSTENCILOP_REPLACE",	// 3,
			"D3DSTENCILOP_INCRSAT",	// 4,
			"D3DSTENCILOP_DECRSAT",	// 5,
			"D3DSTENCILOP_INVERT",	// 6,
			"D3DSTENCILOP_INCR",	// 7,
			"D3DSTENCILOP_DECR",	// 8,
		}
	}, 

	// D3DCULL 
	{ 
		D3DRS_CULLMODE,	
		"D3DRS_CULLMODE",
		4,
		{
			"<<error>>",	// 0
			"D3DCULL_NONE",	// 1
			"D3DCULL_CW",	// 2
			"D3DCULL_CCW",	// 3
		}
	}, 

	// D3DCOLOR used for multi-texture blend 
	{ 
		D3DRS_TEXTUREFACTOR,	
		"D3DRS_TEXTUREFACTOR",
		0,
	}, 

	// LONG Z bias 
	{ 
		D3DRS_ZBIAS,	
		"D3DRS_ZBIAS",
		0,
	}, 

	// TRUE to enable edge antialiasing (Xbox extension)
	{ 
		D3DRS_EDGEANTIALIAS,	
		"D3DRS_EDGEANTIALIAS",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// TRUE to enable multisample antialiasing
	{ 
		D3DRS_MULTISAMPLEANTIALIAS,	
		"D3DRS_MULTISAMPLEANTIALIAS",
		2,
		{
			"FALSE",
			"TRUE",
		}
	}, 

	// DWORD per-pixel and per-sample enable/disable
	{ 
		D3DRS_MULTISAMPLEMASK,	
		"D3DRS_MULTISAMPLEMASK",
		0,
	}, 
	#ifdef D3DRS_CLIPPING
	// DWORD per-pixel and per-sample enable/disable
	{ 
		D3DRS_CLIPPING,	
		"D3DRS_CLIPPING",
		0,
	}, 
	#endif

	#ifdef D3DRS_CLIPPLANEENABLE
	{ 
		D3DRS_CLIPPLANEENABLE,	
		"D3DRS_CLIPPLANEENABLE",
		0,
	},
	#endif

	#ifdef D3DRS_FOGVERTEXMODE
	// D3DFOGMODE
	{
		D3DRS_FOGVERTEXMODE,
		"D3DRS_FOGVERTEXMODE",
		4,
		{
			"D3DFOG_NONE",		// 0
			"D3DFOG_EXP",		// 1
			"D3DFOG_EXP2",		// 2
			"D3DFOG_LINEAR",	// 3
		}
	},
	#endif

	#ifdef D3DRS_SOFTWAREVERTEXPROCESSING
	{
		D3DRS_SOFTWAREVERTEXPROCESSING,
		"D3DRS_SOFTWAREVERTEXPROCESSING",
		2,
		{
			"FALSE",
			"TRUE",
		}
	},
	#endif

	#ifdef D3DRS_DEBUGMONITORTOKEN
	{
		D3DRS_DEBUGMONITORTOKEN,
		"D3DRS_DEBUGMONITORTOKEN",
		0,
	},
	#endif

	#ifdef D3DRS_POINTSIZE_MAX
	{
		D3DRS_POINTSIZE_MAX,
		"D3DRS_POINTSIZE_MAX" ,
		0,
	},
	#endif

	#ifdef D3DRS_INDEXEDVERTEXBLENDENABLE
	{
		D3DRS_INDEXEDVERTEXBLENDENABLE,
		"D3DRS_INDEXEDVERTEXBLENDENABLE",
		2,
		{
			"FALSE",
			"TRUE",
		}
	},
	#endif

	#ifdef D3DRS_TWEENFACTOR
	{
		D3DRS_TWEENFACTOR,
		"D3DRS_TWEENFACTOR",
		0,
	},
	#endif

	#ifdef D3DRS_POSITIONORDER
	{
		D3DRS_POSITIONORDER,
		"D3DRS_POSITIONORDER",
		0,
	},
	#endif

	#ifdef D3DRS_NORMALORDER
	{
		D3DRS_NORMALORDER,
		"D3DRS_NORMALORDER",
		0,
	},
	#endif

	#ifdef D3DRS_LINEPATTERN
	{
		D3DRS_LINEPATTERN,
		"D3DRS_LINEPATTERN",
		0,
	},
	#endif

	#ifdef D3DRS_LASTPIXEL
	{
		D3DRS_LASTPIXEL,
		"D3DRS_LASTPIXEL",
		0,
	},
	#endif

	#ifdef D3DRS_ZVISIBLE
	{
		D3DRS_ZVISIBLE,
		"D3DRS_ZVISIBLE",
		0,
	},
	#endif
};

static int g_NumRenderState = sizeof(s_RenderStateInfo) / sizeof(RENDER_STATE_INFO);

void i3DeleteRenderStateArgs()
{
	if(s_RenderStateArg[0])
	{
		delete[] s_RenderStateArg[0]; s_RenderStateArg[0] = NULL; 
	}
	if(s_RenderStateArg[1])
	{
		delete[] s_RenderStateArg[1]; s_RenderStateArg[1] = NULL; 
	}
}
void i3BackupRenderStates(LPDIRECT3DDEVICE8 d3ddevice, int n)
{
	if(!s_RenderStateArg[0]) s_RenderStateArg[0] = new DWORD[g_NumRenderState];
	if(!s_RenderStateArg[1]) s_RenderStateArg[1] = new DWORD[g_NumRenderState];

	HRESULT hr;
	for (int i=0; i<g_NumRenderState; i++)
	{
		hr = d3ddevice->GetRenderState(s_RenderStateInfo[i].Type, &s_RenderStateArg[n][i]);
		if( hr )
		{
			DXTRACE_ERR( "BackupRenderStates", hr );
		}
		I3ASSERT(!hr);
	}
}
void i3RestoreRenderStates(LPDIRECT3DDEVICE8 d3ddevice, int n)
{
	HRESULT hr;
	for (int i=0; i<g_NumRenderState; i++)
	{
		hr = d3ddevice->SetRenderState(s_RenderStateInfo[i].Type, s_RenderStateArg[n][i]);
		I3ASSERT(!hr);
	}
}

static void SprintRenderState( char* szBuf, int nState, DWORD nValue )
{
	if( s_RenderStateInfo[nState].NumArgName > 0 )
	{
		// Name
		if( 0 <= nValue && (INT32)nValue < s_RenderStateInfo[nState].NumArgName )
		{
			sprintf( szBuf, "%s", s_RenderStateInfo[nState].ArgName[nValue] );
		}
		else
		{
			sprintf( szBuf, "<<error>>(0x%X)", nValue );
		}
	}
	else if( s_RenderStateInfo[nState].NumArgName == -1 )
	{
		// FLOAT
		float fValue = *((float*)(&nValue));
		sprintf( szBuf, "%f", fValue );
	}
	else if( s_RenderStateInfo[nState].NumArgName == 0 )
	{
		sprintf( szBuf, "%d(0x%X)", nValue, nValue );
	}
	else
	{
		I3ASSERT_0;
	}
}

void i3CompareRenderStates()
{
	int c = 0;
	I3TRACE("<< Compare RenderStates : START >>----------------------------------------------\n");
	for (int i=0; i<g_NumRenderState; i++)
	{
		if(s_RenderStateArg[0][i] != s_RenderStateArg[1][i])
		{
			c++;
			char szArgName0[64], szArgName1[64];
			SprintRenderState( szArgName0, i, s_RenderStateArg[0][i] );
			SprintRenderState( szArgName1, i, s_RenderStateArg[1][i] );
			I3TRACE( "%-32s is differ : ( %s, %s )\n", s_RenderStateInfo[i].Name, szArgName0, szArgName1 );
		}
	}
	if(c)
	{
		I3TRACE( "%d difference found.\n", c );
	}
	else
	{
		I3TRACE( "No difference found.\n" );
	}
	I3TRACE( "<< Compare RenderStates : END >>---------------------------------------------------\n" );
}

void i3PrintRenderStates(int n)
{
	int i;
	I3TRACE( "<Print RenderState>\n" );
	for (i=0; i<g_NumRenderState; i++)
	{
		char szArgName[64];
		SprintRenderState( szArgName, i, s_RenderStateArg[n][i] );
		I3TRACE( "%-32s : %s\n", s_RenderStateInfo[i].Name, szArgName );
	}	
}

// ============================================================================================================
// Texture Statge States
// ============================================================================================================
#define MAX_TEXSTATE 27
#define MAX_TEX_ARG_NAME 30
typedef struct
{
	D3DTEXTURESTAGESTATETYPE Type;
	char* Name;
	int NumArgName;
	const char *ArgName[MAX_TEX_ARG_NAME];
} TEX_STATE_INFO;

static DWORD s_TexStateArg[2][MAX_TEXSTATE];

static TEX_STATE_INFO s_TexStateInfo[MAX_TEXSTATE] =
{
	// D3DTEXTUREADDRESS for U coordinate 
	{ 
		D3DTSS_ADDRESSU, 
		"D3DTSS_ADDRESSU",
		6,
		{
			"<<error>>",				// 0
			"D3DTADDRESS_WRAP",			// 1
			"D3DTADDRESS_MIRROR",		// 2
			"D3DTADDRESS_CLAMP",		// 3
			"D3DTADDRESS_BORDER",		// 4
			"D3DTADDRESS_MIRRORONCE",	// 5
		}
	}, 

	// D3DTEXTUREADDRESS for V coordinate 
	{ 
		D3DTSS_ADDRESSV, 
		"D3DTSS_ADDRESSV",
		6,
		{
			"<<error>>",				// 0
			"D3DTADDRESS_WRAP",			// 1
			"D3DTADDRESS_MIRROR",		// 2
			"D3DTADDRESS_CLAMP",		// 3
			"D3DTADDRESS_BORDER",		// 4
			"D3DTADDRESS_MIRRORONCE",	// 5
		}
	}, 

	// D3DTEXTUREADDRESS for W coordinate 
	{ 
		D3DTSS_ADDRESSW, 
		"D3DTSS_ADDRESSW",
		6,
		{
			"<<error>>",				// 0
			"D3DTADDRESS_WRAP",			// 1
			"D3DTADDRESS_MIRROR",		// 2
			"D3DTADDRESS_CLAMP",		// 3
			"D3DTADDRESS_BORDER",		// 4
			"D3DTADDRESS_MIRRORONCE",	// 5
		}
	}, 

	// D3DTEXF_* (D3DTEXF_LINEAR etc.) filter to use for magnification 
	{ 
		D3DTSS_MAGFILTER, 
		"D3DTSS_MAGFILTER",
		6,
		{
			"D3DTEXF_NONE",			// 0,
			"D3DTEXF_POINT",		// 1,
			"D3DTEXF_LINEAR",		// 2,
			"D3DTEXF_ANISOTROPIC",	// 3,
			"D3DTEXF_FLATCUBIC",	// 4,
			"D3DTEXF_GAUSSIANCUBIC",// 5,
		}
	}, 

	// D3DTEXF_* (D3DTEXF_LINEAR etc.) filter to use for minification 
	{ 
		D3DTSS_MINFILTER, 
		"D3DTSS_MINFILTER",
		6,
		{
			"D3DTEXF_NONE",			// 0,
			"D3DTEXF_POINT",		// 1,
			"D3DTEXF_LINEAR",		// 2,
			"D3DTEXF_ANISOTROPIC",	// 3,
			"D3DTEXF_FLATCUBIC",	// 4,
			"D3DTEXF_GAUSSIANCUBIC",// 5,
		}
	}, 

	// D3DTEXF_* (D3DTEXF_LINEAR etc.) filter to use between mipmaps during minification 
	{ 
		D3DTSS_MIPFILTER, 
		"D3DTSS_MIPFILTER",
		6,
		{
			"D3DTEXF_NONE",			// 0,
			"D3DTEXF_POINT",		// 1,
			"D3DTEXF_LINEAR",		// 2,
			"D3DTEXF_ANISOTROPIC",	// 3,
			"D3DTEXF_FLATCUBIC",	// 4,
			"D3DTEXF_GAUSSIANCUBIC",// 5,
		}
	}, 

	// float mipmap LOD bias 
	{ 
		D3DTSS_MIPMAPLODBIAS, 
		"D3DTSS_MIPMAPLODBIAS",
		0
	}, 

	// DWORD 0..(n-1) LOD index of largest map to use (0 == largest) 
	{ 
		D3DTSS_MAXMIPLEVEL, 
		"D3DTSS_MAXMIPLEVEL",
		0
	}, 

	// DWORD maximum anisotropy 
	{ 
		D3DTSS_MAXANISOTROPY, 
		"D3DTSS_MAXANISOTROPY",
		0
	}, 

	// D3DTEXTUREOP - per-stage blending controls for color channels 
	{ 
		D3DTSS_COLOROP, 
		"D3DTSS_COLOROP",
		27,
		{
			"<<error>>",						// 0
			"D3DTOP_DISABLE",					// 1
			"D3DTOP_SELECTARG1",				// 2
			"D3DTOP_SELECTARG2",				// 3
			"D3DTOP_MODULATE",					// 4
			"D3DTOP_MODULATE2X",				// 5
			"D3DTOP_MODULATE4X",				// 6
			"D3DTOP_ADD",						// 7
			"D3DTOP_ADDSIGNED",					// 8
			"D3DTOP_ADDSIGNED2X",				// 9
			"D3DTOP_SUBTRACT",					// 10
			"D3DTOP_ADDSMOOTH",					// 11
			"D3DTOP_BLENDDIFFUSEALPHA",			// 12
			"D3DTOP_BLENDTEXTUREALPHA",			// 13
			"D3DTOP_BLENDFACTORALPHA",			// 14
			"D3DTOP_BLENDTEXTUREALPHAPM",		// 15
			"D3DTOP_BLENDCURRENTALPHA",			// 16
			"D3DTOP_PREMODULATE",				// 17
			"D3DTOP_MODULATEALPHA_ADDCOLOR",	// 18
			"D3DTOP_MODULATECOLOR_ADDALPHA",	// 19
			"D3DTOP_MODULATEINVALPHA_ADDCOLOR",	// 20
			"D3DTOP_MODULATEINVCOLOR_ADDALPHA",	// 21
			"D3DTOP_BUMPENVMAP",				// 22
			"D3DTOP_BUMPENVMAPLUMINANCE",		// 23
			"D3DTOP_DOTPRODUCT3",				// 24
			"D3DTOP_MULTIPLYADD",				// 25
			"D3DTOP_LERP",						// 26
		}
	}, 

	// D3DTA_* (D3DTA_TEXTURE etc.) third arg for triadic ops 
	{ 
		D3DTSS_COLORARG0, 
		"D3DTSS_COLORARG0",
		-2 // Custom
	}, 

	// D3DTA_* (D3DTA_TEXTURE etc.) texture arg
	{ 
		D3DTSS_COLORARG1, 
		"D3DTSS_COLORARG1",
		-2 // Custom
	}, 

	// D3DTA_* (D3DTA_TEXTURE etc.) texture arg 
	{ 
		D3DTSS_COLORARG2, 
		"D3DTSS_COLORARG2",
		-2 // Custom
	}, 

	// D3DTEXTUREOP - per-stage blending controls for alpha channel 
	{ 
		D3DTSS_ALPHAOP, 
		"D3DTSS_ALPHAOP",
		27,
		{
			"<<error>>",						// 0
			"D3DTOP_DISABLE",					// 1
			"D3DTOP_SELECTARG1",				// 2
			"D3DTOP_SELECTARG2",				// 3
			"D3DTOP_MODULATE",					// 4
			"D3DTOP_MODULATE2X",				// 5
			"D3DTOP_MODULATE4X",				// 6
			"D3DTOP_ADD",						// 7
			"D3DTOP_ADDSIGNED",					// 8
			"D3DTOP_ADDSIGNED2X",				// 9
			"D3DTOP_SUBTRACT",					// 10
			"D3DTOP_ADDSMOOTH",					// 11
			"D3DTOP_BLENDDIFFUSEALPHA",			// 12
			"D3DTOP_BLENDTEXTUREALPHA",			// 13
			"D3DTOP_BLENDFACTORALPHA",			// 14
			"D3DTOP_BLENDTEXTUREALPHAPM",		// 15
			"D3DTOP_BLENDCURRENTALPHA",			// 16
			"D3DTOP_PREMODULATE",				// 17
			"D3DTOP_MODULATEALPHA_ADDCOLOR",	// 18
			"D3DTOP_MODULATECOLOR_ADDALPHA",	// 19
			"D3DTOP_MODULATEINVALPHA_ADDCOLOR",	// 20
			"D3DTOP_MODULATEINVCOLOR_ADDALPHA",	// 21
			"D3DTOP_BUMPENVMAP",				// 22
			"D3DTOP_BUMPENVMAPLUMINANCE",		// 23
			"D3DTOP_DOTPRODUCT3",				// 24
			"D3DTOP_MULTIPLYADD",				// 25
			"D3DTOP_LERP",						// 26
		}
	}, 

	// D3DTA_* (D3DTA_TEXTURE etc.) third arg for triadic ops 
	{ 
		D3DTSS_ALPHAARG0, 
		"D3DTSS_ALPHAARG0",
		-2 // Custom
	}, 

	// D3DTA_* (D3DTA_TEXTURE etc.) texture arg
	{ 
		D3DTSS_ALPHAARG1, 
		"D3DTSS_ALPHAARG1",
		-2 // Custom
	}, 

	// D3DTA_* (D3DTA_TEXTURE etc.) texture arg) 
	{ 
		D3DTSS_ALPHAARG2, 
		"D3DTSS_ALPHAARG2",
		-2 // Custom
	}, 

	// D3DTA_CURRENT or D3DTA_TEMP - arg for result 
	{ 
		D3DTSS_RESULTARG, 
		"D3DTSS_RESULTARG",
		-2 // Custom
	}, 

	// D3DTEXTURETRANSFORMFLAGS controls texture transform 
	{ 
		D3DTSS_TEXTURETRANSFORMFLAGS,
		"D3DTSS_TEXTURETRANSFORMFLAGS",
		0,
	}, 

	// float (bump mapping matrix) 
	{ 
		D3DTSS_BUMPENVMAT00, 
		"D3DTSS_BUMPENVMAT00",
		-1 // float
	}, 

	// float (bump mapping matrix) 
	{ 
		D3DTSS_BUMPENVMAT01, 
		"D3DTSS_BUMPENVMAT01",
		-1 // float
	}, 

	// float (bump mapping matrix) 
	{ 
		D3DTSS_BUMPENVMAT11, 
		"D3DTSS_BUMPENVMAT11",
		-1 // float
	}, 

	// float (bump mapping matrix) 
	{ 
		D3DTSS_BUMPENVMAT10, 
		"D3DTSS_BUMPENVMAT10",
		-1 // float
	}, 

	// float scale for bump map luminance 
	{ 
		D3DTSS_BUMPENVLSCALE, 
		"D3DTSS_BUMPENVLSCALE",
		-1 // float
	}, 

	// float offset for bump map luminance 
	{ 
		D3DTSS_BUMPENVLOFFSET, 
		"D3DTSS_BUMPENVLOFFSET",
		-1 // float
	}, 

	// DWORD identifies which set of texture coordinates index this texture 
	{ 
		D3DTSS_TEXCOORDINDEX, 
		"D3DTSS_TEXCOORDINDEX",
		-2 // Custom
	}, 

	// D3DCOLOR 
	{ 
		D3DTSS_BORDERCOLOR, 
		"D3DTSS_BORDERCOLOR",
		0
	},
};

void i3BackupTextureStates(LPDIRECT3DDEVICE8 d3ddevice, int n, int texture)
{
	HRESULT hr;
	for (int i=0; i<MAX_TEXSTATE; i++)
	{
		hr = d3ddevice->GetTextureStageState(texture, s_TexStateInfo[i].Type, &s_TexStateArg[n][i]);
		I3ASSERT(!hr);
	}
}
void i3RestoreTextureStates(LPDIRECT3DDEVICE8 d3ddevice, int n, int texture)
{
	HRESULT hr;	
	for (int i=0; i<MAX_TEXSTATE; i++)
	{
		hr = d3ddevice->SetTextureStageState(texture, s_TexStateInfo[i].Type, s_TexStateArg[n][i]);
		I3ASSERT(!hr);
	}
}

static void SprintTextureState( char* szBuf, int nState, DWORD nValue )
{
	if( s_TexStateInfo[nState].NumArgName > 0 )
	{
		// Name
		if( 0 <= nValue && (INT32) nValue < s_TexStateInfo[nState].NumArgName )
		{
			sprintf( szBuf, "%s", s_TexStateInfo[nState].ArgName[nValue] );
		}
		else
		{
			sprintf( szBuf, "<<error>>(0x%X)", nValue );
		}
	}
	else if( s_TexStateInfo[nState].NumArgName == -1 )
	{
		// FLOAT
		float fValue = *((float*)(&nValue));
		sprintf( szBuf, "%f", fValue );
	}
	else if( s_TexStateInfo[nState].NumArgName == 0 )
	{
		sprintf( szBuf, "%d(0x%X)", nValue, nValue );
	}
	else if( s_TexStateInfo[nState].NumArgName == -2 )
	{
		if( s_TexStateInfo[nState].Type == D3DTSS_COLORARG0 ||
			s_TexStateInfo[nState].Type == D3DTSS_COLORARG1 ||
			s_TexStateInfo[nState].Type == D3DTSS_COLORARG2 ||
			s_TexStateInfo[nState].Type == D3DTSS_ALPHAARG0 ||
			s_TexStateInfo[nState].Type == D3DTSS_ALPHAARG1 ||
			s_TexStateInfo[nState].Type == D3DTSS_ALPHAARG2 ||
			s_TexStateInfo[nState].Type == D3DTSS_RESULTARG
			)
		{
			switch( nValue )
			{
			case D3DTA_DIFFUSE:
				strcpy( szBuf, "D3DTA_DIFFUSE" );
				break;

			case D3DTA_CURRENT:
				strcpy( szBuf, "D3DTA_CURRENT" );
				break;

			case D3DTA_TEXTURE:
				strcpy( szBuf, "D3DTA_TEXTURE" );
				break;

			case D3DTA_TFACTOR:
				strcpy( szBuf, "D3DTA_TFACTOR" );
				break;

			case D3DTA_SPECULAR:
				strcpy( szBuf, "D3DTA_SPECULAR" );
				break;

			case D3DTA_TEMP:
				strcpy( szBuf, "D3DTA_TEMP" );
				break;

			case D3DTA_COMPLEMENT:
				strcpy( szBuf, "D3DTA_COMPLEMENT" );
				break;

			case D3DTA_ALPHAREPLICATE:
				strcpy( szBuf, "D3DTA_ALPHAREPLICATE" );
				break;

			default :
				strcpy( szBuf, "<<error>>" );
				break;
			}
		}
		else if ( s_TexStateInfo[nState].Type == D3DTSS_TEXCOORDINDEX )
		{
			switch( nValue )
			{
			case D3DTSS_TCI_PASSTHRU :
				strcpy( szBuf, "D3DTSS_TCI_PASSTHRU" );
				break;

			case D3DTSS_TCI_CAMERASPACENORMAL :
				strcpy( szBuf, "D3DTSS_TCI_CAMERASPACENORMAL" );
				break;

			case D3DTSS_TCI_CAMERASPACEPOSITION :
				strcpy( szBuf, "D3DTSS_TCI_CAMERASPACEPOSITION" );
				break;

			case D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR :
				strcpy( szBuf, "D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR" );
				break;

			default :
				strcpy( szBuf, "<<error>>" );
				break;
			}
		}
		else
		{
			I3ASSERT_0;
		}
	}
	else
	{
		I3ASSERT_0;
	}
}

void i3CompareTextureStates()
{	
	I3TRACE( "<< Compare TextureStageStates : START >>----------------------------------------\n" );
	int c = 0;
	for (int i=0; i<MAX_TEXSTATE; i++)
	{
		if(s_TexStateArg[0][i] != s_TexStateArg[1][i])
		{
			c++;
			char szArgName0[64];
			char szArgName1[64];
			SprintTextureState( szArgName0, i, s_TexStateArg[0][i] );
			SprintTextureState( szArgName1, i, s_TexStateArg[1][i] );

			I3TRACE( "%s is Differ: (%s, %s)\n", s_TexStateInfo[i].Name, szArgName0, szArgName1 );
		}
	}
	if(c)
	{
		I3TRACE( "%d difference found.\n", c );
	}
	else
	{
		I3TRACE( "No difference found.\n" );
	}
	I3TRACE( "<< Compare TextureStageStates : END >>------------------------------------------\n" );
}

void i3PrintTextureStates(int n)
{
	int i;
	I3TRACE( "<PrintTextureStates:Start>-----------------------\n" );
	for (i=0; i<MAX_TEXSTATE; i++)
	{
		char szArgName[64];
		SprintTextureState( szArgName, i, s_TexStateArg[n][i] );
		I3TRACE( "%-32s : %s\n", s_TexStateInfo[i].Name, szArgName );
	}	
}

#endif //I3G_DX
#endif //I3_DEBUG