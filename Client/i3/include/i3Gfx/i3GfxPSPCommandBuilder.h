#if !defined( __I3G_PSP_COMMAND_BUILDER_H)
#define __I3G_PSP_COMMAND_BUILDER_H

#include "i3Base.h"

#ifdef I3_COMPILER_VC
#pragma warning( push )
#pragma warning( disable : 4311 )
#endif

#ifdef I3_DEBUG
	#define DEBUG_PSP_COMMAND_BUFFER
#endif

#ifdef DEBUG_PSP_COMMAND_BUFFER
	#define __ASSERT I3ASSERT
#else
	#if defined( I3_COMPILER_VC)
		#define __ASSERT	__noop
	#else
		#define __ASSERT(...)	(void)0
	#endif
#endif

#define CHECK_BITS_1(_bits)		__ASSERT((_bits) == ((_bits) & 0x01))
#define CHECK_BITS_2(_bits)		__ASSERT((_bits) == ((_bits) & 0x03))
#define CHECK_BITS_3(_bits)		__ASSERT((_bits) == ((_bits) & 0x07))
#define CHECK_BITS_4(_bits)		__ASSERT((_bits) == ((_bits) & 0x0F))
#define CHECK_BITS_5(_bits)		__ASSERT((_bits) == ((_bits) & 0x1F))
#define CHECK_BITS_6(_bits)		__ASSERT((_bits) == ((_bits) & 0x3F))
#define CHECK_BITS_7(_bits)		__ASSERT((_bits) == ((_bits) & 0x7F))
#define CHECK_BITS_8(_bits)		__ASSERT((_bits) == ((_bits) & 0xFF))
#define CHECK_BITS_9(_bits)		__ASSERT((_bits) == ((_bits) & 0x1FF))
#define CHECK_BITS_10(_bits)	__ASSERT((_bits) == ((_bits) & 0x3FF))

#define		I3G_PSP_CMD_NOP				0x00000000			// No Operation
#define		I3G_PSP_CMD_VADR			0x01000000			// Set Vertex Data
#define		I3G_PSP_CMD_IADR			0x02000000			// Set Index Data
#define		I3G_PSP_CMD_PRIM			0x04000000			// Draw Primitive
#define		I3G_PSP_CMD_BEZIER			0x05000000			// Draw Bezier Surface
#define		I3G_PSP_CMD_SPLINE			0x06000000			// Draw Spline Surface
#define		I3G_PSP_CMD_BBOX			0x07000000			// Set Bounding Box
#define		I3G_PSP_CMD_JUMP			0x08000000			// Jump
#define		I3G_PSP_CMD_BJUMP			0x09000000			// Conditional Jump
#define		I3G_PSP_CMD_CALL			0x0A000000			// List Call
#define		I3G_PSP_CMD_RET				0x0B000000			// List Return
#define		I3G_PSP_CMD_END				0x0C000000			// End Reading
#define		I3G_PSP_CMD_SIGNAL			0x0E000000			// Signal Interrupt
#define		I3G_PSP_CMD_FINISH			0x0F000000			// Finish Drawing
#define		I3G_PSP_CMD_BASE			0x10000000			// Set Address Base
#define		I3G_PSP_CMD_VTYPE			0x12000000			// Set Vertex Type
#define		I3G_PSP_CMD_OFFSET			0x13000000			// Set Offset Address
#define		I3G_PSP_CMD_ORIGIN			0x14000000			// Set Origin Address
#define		I3G_PSP_CMD_REGION1			0x15000000			// Set Drawing Region 1
#define		I3G_PSP_CMD_REGION2			0x16000000			// Set Drawing Region 2
#define		I3G_PSP_CMD_LTE				0x17000000			// Lighting Enable
#define		I3G_PSP_CMD_LE0				0x18000000			// Light Enable 0
#define		I3G_PSP_CMD_LE1				0x19000000			// Light Enable 1
#define		I3G_PSP_CMD_LE2				0x1A000000			// Light Enable 2
#define		I3G_PSP_CMD_LE3				0x1B000000			// Light Enable 3
#define		I3G_PSP_CMD_CLE				0x1C000000			// Clipping Enable
#define		I3G_PSP_CMD_BCE				0x1D000000			// Culling Enable
#define		I3G_PSP_CMD_TME				0x1E000000			// Texture Enable
#define		I3G_PSP_CMD_FGE				0x1F000000			// Fog Enable
#define		I3G_PSP_CMD_DTE				0x20000000			// Dither Enable
#define		I3G_PSP_CMD_ABE				0x21000000			// Alpha Blending Enable
#define		I3G_PSP_CMD_ATE				0x22000000			// Alpha Test Enable
#define		I3G_PSP_CMD_ZTE				0x23000000			// Depth Test Enable
#define		I3G_PSP_CMD_STE				0x24000000			// Stencil Test Enable
#define		I3G_PSP_CMD_AAE				0x25000000			// Antialiasing Enable
#define		I3G_PSP_CMD_PCE				0x26000000			// Patch Culling Enable
#define		I3G_PSP_CMD_CTE				0x27000000			// Color Test Enable
#define		I3G_PSP_CMD_LOE				0x28000000			// Logical Operation Enable
#define		I3G_PSP_CMD_BONEN			0x2A000000			// Set Bone Matrix Number
#define		I3G_PSP_CMD_BONED			0x2B000000			// Set Bone Matrix Data
#define		I3G_PSP_CMD_WEIGHT0			0x2C000000			// Set Vertex Weight
#define		I3G_PSP_CMD_WEIGHT1			0x2D000000			// Set Vertex Weight
#define		I3G_PSP_CMD_WEIGHT2			0x2E000000			// Set Vertex Weight
#define		I3G_PSP_CMD_WEIGHT3			0x2F000000			// Set Vertex Weight
#define		I3G_PSP_CMD_WEIGHT4			0x30000000			// Set Vertex Weight
#define		I3G_PSP_CMD_WEIGHT5			0x31000000			// Set Vertex Weight
#define		I3G_PSP_CMD_WEIGHT6			0x32000000			// Set Vertex Weight
#define		I3G_PSP_CMD_WEIGHT7			0x33000000			// Set Vertex Weight
#define		I3G_PSP_CMD_DIVIDE			0x36000000			// Set Patch Division Count
#define		I3G_PSP_CMD_PPM				0x37000000			// Set Patch Primitive
#define		I3G_PSP_CMD_PFACE			0x38000000			// Set patch face
#define		I3G_PSP_CMD_WORLDN			0x3A000000			// Set World Matrix Number
#define		I3G_PSP_CMD_WORLDD			0x3B000000			// Set World Matrix Data
#define		I3G_PSP_CMD_VIEWN			0x3C000000			// Set View Matrix Number
#define		I3G_PSP_CMD_VIEWD			0x3D000000			// Set View Matrix Data
#define		I3G_PSP_CMD_PROJN			0x3E000000			// Set Projection Matrix Number
#define		I3G_PSP_CMD_PROJD			0x3F000000			// Set Projection Matrix Data
#define		I3G_PSP_CMD_TGENN			0x40000000			// Set Texture Generation Matrix Number
#define		I3G_PSP_CMD_TGEND			0x41000000			// Set Texture Generation Matrix Data
#define		I3G_PSP_CMD_SX				0x42000000			// Set Viewport ( SX )
#define		I3G_PSP_CMD_SY				0x43000000			// Set Viewport ( SY )
#define		I3G_PSP_CMD_SZ				0x44000000			// Set Viewport ( SZ )
#define		I3G_PSP_CMD_TX				0x45000000			// Set Viewport ( TX )
#define		I3G_PSP_CMD_TY				0x46000000			// Set Viewport	( TY )
#define		I3G_PSP_CMD_TZ				0x47000000			// Set Viewport ( TZ )
#define		I3G_PSP_CMD_SU				0x48000000			// Set Texture Scale ( U )
#define		I3G_PSP_CMD_SV				0x49000000			// Set Texture Scale ( V )
#define		I3G_PSP_CMD_TU				0x4A000000			// Set Texture Offste ( U )
#define		I3G_PSP_CMD_TV				0x4B000000			// Set Texture Offset ( V )
#define		I3G_PSP_CMD_OFFSETX			0x4C000000			// Set Screen Offset
#define		I3G_PSP_CMD_OFFSETY			0x4D000000			// Set Screen Offset
#define		I3G_PSP_CMD_SHADE			0x50000000			// Set Shading Mode
#define		I3G_PSP_CMD_NREV			0x51000000			// Normal Reverse
#define		I3G_PSP_CMD_MATERIAL		0x53000000			// Material
#define		I3G_PSP_CMD_MEC				0x54000000			// Set Model Color ( Emissive Color )
#define		I3G_PSP_CMD_MAC				0x55000000			// Set Model Color ( Ambient Color )
#define		I3G_PSP_CMD_MDC				0x56000000			// Set Model Color ( Diffuse Color )
#define		I3G_PSP_CMD_MSC				0x57000000			// Set Model Color ( Specular Color )
#define		I3G_PSP_CMD_MAA				0x58000000			// Set Model Color ( Alpha )
#define		I3G_PSP_CMD_MK				0x5B000000			// Set Model Specular Coefficiency
#define		I3G_PSP_CMD_AC				0x5C000000			// Set Ambient Light Color
#define		I3G_PSP_CMD_AA				0x5D000000			// Set Ambient Light Color ( Alpha )
#define		I3G_PSP_CMD_LMODE			0x5E000000			// Set Light Mode
#define		I3G_PSP_CMD_LTYPE0			0x5F000000			// Set Light Type 0
#define		I3G_PSP_CMD_LTYPE1			0x60000000			// Set Light Type 1
#define		I3G_PSP_CMD_LTYPE2			0x61000000			// Set Light Type 2
#define		I3G_PSP_CMD_LTYPE3			0x62000000			// Set Light Type 3
#define		I3G_PSP_CMD_LX0				0x63000000			// Set Light Vector 0 ( X )
#define		I3G_PSP_CMD_LY0				0x64000000			// Set Light Vector 0 ( Y )
#define		I3G_PSP_CMD_LZ0				0x65000000			// Set Light Vector 0 ( Z )
#define		I3G_PSP_CMD_LX1				0x66000000			// Set Light Vector 1 ( X )
#define		I3G_PSP_CMD_LY1				0x67000000			// Set Light Vector 1 ( Y )
#define		I3G_PSP_CMD_LZ1				0x68000000			// Set Light Vector 1 ( Z )
#define		I3G_PSP_CMD_LX2				0x69000000			// Set Light Vector 2 ( X )
#define		I3G_PSP_CMD_LY2				0x6A000000			// Set Light Vector 2 ( Y )
#define		I3G_PSP_CMD_LZ2				0x6B000000			// Set Light Vector 2 ( Z )
#define		I3G_PSP_CMD_LX3				0x6C000000			// Set Light Vector 3 ( X )
#define		I3G_PSP_CMD_LY3				0x6D000000			// Set Light Vector 3 ( Y )
#define		I3G_PSP_CMD_LZ3				0x6E000000			// Set Light Vector 3 ( Z )
#define		I3G_PSP_CMD_LDX0			0x6F000000			// Set Light Direction 0 ( X )
#define		I3G_PSP_CMD_LDY0			0x70000000			// Set Light Direction 0 ( Y )
#define		I3G_PSP_CMD_LDZ0			0x71000000			// Set Light Direction 0 ( Z )
#define		I3G_PSP_CMD_LDX1			0x72000000			// Set Light Direction 1 ( X )
#define		I3G_PSP_CMD_LDY1			0x73000000			// Set Light Direction 1 ( Y )
#define		I3G_PSP_CMD_LDZ1			0x74000000			// Set Light Direction 1 ( Z )
#define		I3G_PSP_CMD_LDX2			0x75000000			// Set Light Direction 2 ( X )
#define		I3G_PSP_CMD_LDY2			0x76000000			// Set Light Direction 2 ( Y )
#define		I3G_PSP_CMD_LDZ2			0x77000000			// Set Light Direction 2 ( Z )
#define		I3G_PSP_CMD_LDX3			0x78000000			// Set Light Direction 3 ( X )
#define		I3G_PSP_CMD_LDY3			0x79000000			// Set Light Direction 3 ( Y )
#define		I3G_PSP_CMD_LDZ3			0x7A000000			// Set Light Direction 3 ( Z )
#define		I3G_PSP_CMD_LKA0			0x7B000000			// Set Light Distance Attenuation Factor 0 ( A )
#define		I3G_PSP_CMD_LKB0			0x7C000000			// Set Light Distance Attenuation Factor 0 ( B )
#define		I3G_PSP_CMD_LKC0			0x7D000000			// Set Light Distance Attenuation Factor 0 ( C )
#define		I3G_PSP_CMD_LKA1			0x7E000000			// Set Light Distance Attenuation Factor 1 ( A )
#define		I3G_PSP_CMD_LKB1			0x7F000000			// Set Light Distance Attenuation Factor 1 ( B )
#define		I3G_PSP_CMD_LKC1			0x80000000			// Set Light Distance attenuation Factor 1 ( C )
#define		I3G_PSP_CMD_LKA2			0x81000000			// Set Light Distance Attenuation Factor 2 ( A )
#define		I3G_PSP_CMD_LKB2			0x82000000			// Set Light Distance Attenuation Factor 2 ( B )
#define		I3G_PSP_CMD_LKC2			0x83000000			// Set Light Distance Attenuation Factor 2 ( C )
#define		I3G_PSP_CMD_LKA3			0x84000000			// Set Light Distance Attenuation Factor 3 ( A )
#define		I3G_PSP_CMD_LKB3			0x85000000			// Set Light Distance Attenuation Factor 3 ( B )
#define		I3G_PSP_CMD_LKC3			0x86000000			// Set Light Distance Attenuation Factor 3 ( C )
#define		I3G_PSP_CMD_LKS0			0x87000000			// Set Light Convergence Factor 0
#define		I3G_PSP_CMD_LKS1			0x88000000			// Set Light Convergence Factor 1
#define		I3G_PSP_CMD_LKS2			0x89000000			// Set Light Convergence Factor 2
#define		I3G_PSP_CMD_LKS3			0x8A000000			// Set Light Convergence Factor 3
#define		I3G_PSP_CMD_LKO0			0x8B000000			// Set Light Cut-off Dot Product Coefficient 0
#define		I3G_PSP_CMD_LKO1			0x8C000000			// Set Light Cut-off Dot Product Coefficient 1
#define		I3G_PSP_CMD_LKO2			0x8D000000			// Set Lihgt Cut-off Dot Product Coefficient 2
#define		I3G_PSP_CMD_LKO3			0x8E000000			// Set Light Cut-off Dot Product Coefficient 3
#define		I3G_PSP_CMD_LAC0			0x8F000000			// Set Light Color 0 ( Ambient )
#define		I3G_PSP_CMD_LDC0			0x90000000			// Set Light Color 0 ( Diffuse )
#define		I3G_PSP_CMD_LSC0			0x91000000			// Set Light Color 0 ( Specular )
#define		I3G_PSP_CMD_LAC1			0x92000000			// Set Light Color 1 ( Ambient )
#define		I3G_PSP_CMD_LDC1			0x93000000			// Set Light Color 1 ( Diffuse )
#define		I3G_PSP_CMD_LSC1			0x94000000			// Set Light Color 1 ( Specular )
#define		I3G_PSP_CMD_LAC2			0x95000000			// Set Light Color 2 ( Ambient )
#define		I3G_PSP_CMD_LDC2			0x96000000			// Set Light Color 2 ( Diffuse )
#define		I3G_PSP_CMD_LSC2			0x97000000			// Set Light Color 2 ( Specular )
#define		I3G_PSP_CMD_LAC3			0x98000000			// Set Light Color 3 ( Ambient )
#define		I3G_PSP_CMD_LDC3			0x99000000			// Set Light Color 3 ( Diffuse )
#define		I3G_PSP_CMD_LSC3			0x9A000000			// Set Light Color 3 ( Specular )
#define		I3G_PSP_CMD_CULL			0x9B000000			// Culling Surface
#define		I3G_PSP_CMD_FBP				0x9C000000			// Set Frame Buffer Base Point
#define		I3G_PSP_CMD_FBW				0x9D000000			// Set Frame Buffer Width
#define		I3G_PSP_CMD_ZBP				0x9E000000			// Set Depth Buffer Base Point
#define		I3G_PSP_CMD_ZBW				0x9F000000			// Set Depth Buffer Width
#define		I3G_PSP_CMD_TBP0			0xA0000000			// Set Texture Base Point 0
#define		I3G_PSP_CMD_TBP1			0xA1000000			// Set Texture Base Point 1
#define		I3G_PSP_CMD_TBP2			0xA2000000			// Set Texture Base Point 2
#define		I3G_PSP_CMD_TBP3			0xA3000000			// Set Texture Base Point 3
#define		I3G_PSP_CMD_TBP4			0xA4000000			// Set Texture Base Point 4
#define		I3G_PSP_CMD_TBP5			0xA5000000			// Set Texture Base Point 5
#define		I3G_PSP_CMD_TBP6			0xA6000000			// Set Texture Base Point 6
#define		I3G_PSP_CMD_TBP7			0xA7000000			// Set Texture Base Point 7
#define		I3G_PSP_CMD_TBW0			0xA8000000			// Set Texture Buffer Width 0
#define		I3G_PSP_CMD_TBW1			0xA9000000			// Set Texture Buffer Width 1
#define		I3G_PSP_CMD_TBW2			0xAA000000			// Set Texture Buffer Width 2
#define		I3G_PSP_CMD_TBW3			0xAB000000			// Set Texture Buffer Width 3
#define		I3G_PSP_CMD_TBW4			0xAC000000			// Set Texture Buffer Width 4
#define		I3G_PSP_CMD_TBW5			0xAD000000			// Set Texture Buffer Width 5
#define		I3G_PSP_CMD_TBW6			0xAE000000			// Set Texture Buffer Width 6
#define		I3G_PSP_CMD_TBW7			0xAF000000			// Set Texture Buffer Width 7
#define		I3G_PSP_CMD_CBP				0xB0000000			// Set CLUT Buffer Base Point
#define		I3G_PSP_CMD_CBW				0xB1000000			// Set CLUT Buffer Width
#define		I3G_PSP_CMD_XBP1			0xB2000000			// Set Transfer Buffer Base Point ( Source )
#define		I3G_PSP_CMD_XBW1			0xB3000000			// Set Transfer Buffer Width ( Source )
#define		I3G_PSP_CMD_XBP2			0xB4000000			// Set Transfer Buffer Base Point ( Destination )
#define		I3G_PSP_CMD_XBW2			0xB5000000			// Set Transfer Buffer Width ( Destination )
#define		I3G_PSP_CMD_TSIZE0			0xB8000000			// Set Texture Size 0
#define		I3G_PSP_CMD_TSIZE1			0xB9000000			// Set Texture Size 1
#define		I3G_PSP_CMD_TSIZE2			0xBA000000			// Set Texture Size 2
#define		I3G_PSP_CMD_TSIZE3			0xBB000000			// Set Texture Size 3
#define		I3G_PSP_CMD_TSIZE4			0xBC000000			// Set Texture Size 4
#define		I3G_PSP_CMD_TSIZE5			0xBD000000			// Set Texture Size 5
#define		I3G_PSP_CMD_TSIZE6			0xBE000000			// Set Texture Size 6
#define		I3G_PSP_CMD_TSIZE7			0xBF000000			// Set Texture Size 7
#define		I3G_PSP_CMD_TMAP			0xC0000000			// Set Texture Map Mode
#define		I3G_PSP_CMD_TSHADE			0xC1000000			// Set Shade Mapping
#define		I3G_PSP_CMD_TMODE			0xC2000000			// Set Texture Mode
#define		I3G_PSP_CMD_TPF				0xC3000000			// Set Texture Pixel Format
#define		I3G_PSP_CMD_CLOAD			0xC4000000			// CLUT Load
#define		I3G_PSP_CMD_CLUT			0xC5000000			// Set CLUT
#define		I3G_PSP_CMD_TFILTER			0xC6000000			// Set Texture Filter
#define		I3G_PSP_CMD_TWRAP			0xC7000000			// Set Texture Wrap Mode
#define		I3G_PSP_CMD_TLEVEL			0xC8000000			// Set Texture Level Mode
#define		I3G_PSP_CMD_TFUNC			0xC9000000			// Set Texture Function
#define		I3G_PSP_CMD_TEC				0xCA000000			// Set Texture Environment Color
#define		I3G_PSP_CMD_TFLUSH			0xCB000000			// Texture Flush
#define		I3G_PSP_CMD_TSYNC			0xCC000000			// Texture Synchronization
#define		I3G_PSP_CMD_FOG1			0xCD000000			// Set Fog Parameter 1
#define		I3G_PSP_CMD_FOG2			0xCE000000			// Set Fog Parameter 2
#define		I3G_PSP_CMD_FC				0xCF000000			// Set Fog Color
#define		I3G_PSP_CMD_TSLOPE			0xD0000000			// Set Texture Slope
#define		I3G_PSP_CMD_FPF				0xD2000000			// Set Frame Pixel Format
#define		I3G_PSP_CMD_CMODE			0xD3000000			// Set Clear Mode
#define		I3G_PSP_CMD_SCISSOR1		0xD4000000			// Set Scissoring Area 1
#define		I3G_PSP_CMD_SCISSOR2		0xD5000000			// Set Scissoring Area 2
#define		I3G_PSP_CMD_MINZ			0xD6000000			// Set Depth Range ( Minimum )
#define		I3G_PSP_CMD_MAXZ			0xD7000000			// Set Depth Range ( Maximum )
#define		I3G_PSP_CMD_CTEST			0xD8000000			// Set Color Test Parameter
#define		I3G_PSP_CMD_CREF			0xD9000000			// Set Color Test Reference
#define		I3G_PSP_CMD_CMSK			0xDA000000			// Set Color Test Mask
#define		I3G_PSP_CMD_ATEST			0xDB000000			// Set Alpha Test Parameters
#define		I3G_PSP_CMD_STEST			0xDC000000			// Set Stencil Test Parameters
#define		I3G_PSP_CMD_SOP				0xDD000000			// Set Stencil Operation
#define		I3G_PSP_CMD_ZTEST			0xDE000000			// Set Depth Test Parameters
#define		I3G_PSP_CMD_BLEND			0xDF000000			// Set Alpha Blending Parameters
#define		I3G_PSP_CMD_FIXA			0xE0000000			// Set Fixed Color A
#define		I3G_PSP_CMD_FIXB			0xE1000000			// Set Fixed Color B
#define		I3G_PSP_CMD_DITH1			0xE2000000			// Set Dither Coefficients
#define		I3G_PSP_CMD_DITH2			0xE3000000			// Set Dither Coefficients
#define		I3G_PSP_CMD_DITH3			0xE4000000			// Set Dither Coefficients
#define		I3G_PSP_CMD_DITH4			0xE5000000			// Set Dither Coefficients
#define		I3G_PSP_CMD_LOP				0xE6000000			// Set Logical Operation Parameters
#define		I3G_PSP_CMD_ZMSK			0xE7000000			// Depth Mask
#define		I3G_PSP_CMD_PMSK1			0xE8000000			// Set Plane Mask
#define		I3G_PSP_CMD_PMSK2			0xE9000000			// Set Plane Mask
#define		I3G_PSP_CMD_XSTART			0xEA000000			// Inter-Buffer Transfer Start
#define		I3G_PSP_CMD_XPOS1			0xEB000000			// Set Source Buffer Transfer Position
#define		I3G_PSP_CMD_XPOS2			0xEC000000			// Set Destination Buffer Transfer Position
#define		I3G_PSP_CMD_XSIZE			0xEE000000			// Set Transfer Size
#define		I3G_PSP_CMD_CMD_X2			0xF0000000			// Set Screen Coordinate X
#define		I3G_PSP_CMD_CMD_Y2			0xF1000000			// Set Screen Coordinate Y
#define		I3G_PSP_CMD_CMD_Z2			0xF2000000			// Set Screen Coordinate Z
#define		I3G_PSP_CMD_CMD_S2			0xF3000000			// Set Texture Coordinate S
#define		I3G_PSP_CMD_CMD_T2			0xF4000000			// Set Texture Coordinate T
#define		I3G_PSP_CMD_CMD_Q2			0xF5000000			// Set Texture Coordinate Q
#define		I3G_PSP_CMD_CMD_RGB2		0xF6000000			// Set Vertex Color 
#define		I3G_PSP_CMD_CMD_AP2			0xF7000000			// Set Vertex Alpha and Primitive
#define		I3G_PSP_CMD_CMD_F2			0xF8000000			// Set Fog
#define		I3G_PSP_CMD_CMD_I2			0xF9000000			// Set Secondary Color

namespace i3PSPGfx
{
	// 명령어 설정
	I3_INLINE
	UINT32	MakeCmd( UINT32 Cmd, UINT32 Data )
	{
		return Cmd | Data;
	}

	// 인덱스가 필요한 명령어 설정
	I3_INLINE
	UINT32	MakeIndexCmd( UINT32 Cmd, UINT8 Index, UINT32 Data )
	{
		return ( Cmd + ( Index << 24 ) ) | Data;
	}

	// 하위 24비트
	I3_INLINE
	UINT32	Lower24( UINT32 val )
	{
		return val & 0x00FFFFFF;
	}

	// 상위 8비트
	I3_INLINE
	UINT32	Upper8( UINT32 val )
	{
		return ( val >> 8 ) & 0x00FF0000;
	}

	// 32비트 부동 소수점의 상위 24비트
	I3_INLINE
	UINT32	Upper24Bits( REAL32 data )
	{
		return *( ( UINT32 * ) & data ) >> 8;
	}

	////////////////////////////////////////////////////////////////////////

	// 처리 없음.
	I3_INLINE
	UINT32	NOP( void )
	{
		return I3G_PSP_CMD_NOP;
	}

	// 정점 데이터의 시작 주소를 설정
	I3_INLINE
	UINT32	VADR( void * pAdr )
	{
		return MakeCmd( I3G_PSP_CMD_VADR, Lower24( ( UINT32 ) pAdr ) );
	}

	// 인덱스 데이터의 시작 주소를 설정
	I3_INLINE
	UINT32	IADR( void * pAdr )
	{
		return MakeCmd( I3G_PSP_CMD_IADR, Lower24( ( UINT32 ) pAdr ) );
	}

	enum PRIM_TYPE
	{
		PRIM_NONE			=	-1,
		PRIM_POINTLIST		=	0,	// SCEGU_PRIM_POINTS,
		PRIM_LINELIST		=	1,	// SCEGU_PRIM_LINES,
		PRIM_LINESTRIP		=	2,	// SCEGU_PRIM_LINE_STRIP,
		PRIM_TRILIST		=	3,	// SCEGU_PRIM_TRIANGLES,
		PRIM_TRISTRIP		=	4,	// SCEGU_PRIM_TRIANGLE_STRIP,
		PRIM_TRIFAN			=	5,	// SCEGU_PRIM_TRIANGLE_FAN,
		PRIM_POINTSPRITE	=	6,	// SCEGU_PRIM_RECTANGLES,
		PRIM_KEEP			=	7,	// AP2 명령어에서 쓰기 위한 것.
	};

	// 프리미티브 드로잉
	// count : 드로잉할 정점의 개수
	// prim : 프리미티브 타입
	I3_INLINE
	UINT32	PRIM( PRIM_TYPE prim, UINT16 count )
	{
		return MakeCmd( I3G_PSP_CMD_PRIM, ( prim << 16 ) | ( count & 0xFFFF ) );
	}

	// 베지어 드로임
	I3_INLINE
	UINT32	BEZIER( UINT8 VCount, UINT8 UCount )
	{
		return MakeCmd( I3G_PSP_CMD_BEZIER, ( VCount << 8 ) | UCount );
	}

	// VF, UF
	enum SPLINE_TYPE
	{
		SPLINE_CLOSE_CLOSE	= 0,
		SPLINE_OPEN_CLOSE	= 1,
		SPLINE_CLOSE_OPEN	= 2,
		SPLINE_OPEN_OPEN	= 3,
	};

	// 스플라인 드로잉
	I3_INLINE
	UINT32	SPLINE( SPLINE_TYPE VF, SPLINE_TYPE UF, UINT8 VCount, UINT8 UCount )
	{
		return MakeCmd( I3G_PSP_CMD_SPLINE, ( VF << 18 ) | ( UF << 16 ) | ( VCount << 8 ) | UCount );
	}

	// 바운딩 박스의 정점 개수를 설정
	I3_INLINE
	UINT32	BBOX( UINT8 Count )
	{
		return MakeCmd( I3G_PSP_CMD_BBOX, Count );
	}

	// 점프할 메모리 주소 설정. (하위 2비트는 무시)
	I3_INLINE
	UINT32	JUMP( void * pAdr )
	{
		return MakeCmd( I3G_PSP_CMD_JUMP, Lower24( ( UINT32 ) pAdr ) );
	}

	// 조건 점프
	// BBOX가 드로잉 영역 내에 있는 경우 점프하지 않고 다음 커맨드를 실행. (하위 2비트는 무시)
	I3_INLINE
	UINT32	BJUMP( void * pAdr )
	{
		return MakeCmd( I3G_PSP_CMD_BJUMP, Lower24( ( UINT32 ) pAdr ) );
	}

	// 리스트 호출
	// 호출할 메모리 주소를 설정. (하위 2비트는 무시)
	I3_INLINE
	UINT32	CALL( void * pAdr )
	{
		return MakeCmd( I3G_PSP_CMD_CALL, Lower24( ( UINT32 ) pAdr ) );
	}

	// 리스트 리턴
	// 이 커맨드를 읽으면 호출중인 리스트에서 원래 리스트로 복귀합니다.
	I3_INLINE
	UINT32	RET( void )
	{
		return I3G_PSP_CMD_RET;
	}

	// 읽기 종료
	// 이 커맨드를 읽으면 디스플레이 리스트의 읽기를 종료하고, 읽기 종료 인터럽트가 발생한다.
	I3_INLINE
	UINT32	END( void )
	{
		return I3G_PSP_CMD_END;
	}

	// 시그널 인터럽트
	// 시그널 인터럽트를 발생시킴.
	I3_INLINE
	UINT32	SIGNAL( void )
	{
		return I3G_PSP_CMD_SIGNAL;
	}

	// 드로잉 종료
	// 드로인 완료의 인터럽트를 요청합니다.
	// 이 커맨드를 읽고, 커맨드 이전의 모든 드로잉 처리가 완료된 시점에서
	// 인터럽트를 발생합니다. 따라서, 인터럽트를 받은 시점에서 이 커맨드 이전의
	// 모든 드로잉 처리를 완료한 것이 보증됩니다.
	I3_INLINE
	UINT32	FINISH( void )
	{
		return I3G_PSP_CMD_FINISH;
	}

	// 어드레스 베이스
	// 베이스 어드레스를 설정.
	// JUMP, BJUMP, CALL, VADR, IADR 어드레스와 조합하여 상위 8비트(bit31~bit24)의
	// 어드레스로서 사용합니다.
	I3_INLINE
	UINT32	BASE( void * pAdr )
	{
		return MakeCmd( I3G_PSP_CMD_BASE, Upper8( ( UINT32 ) pAdr ) );
	}

	// TT (Texture Type), NT (Normal Type), VT (Vertex Type), WT (Weight Type), IT (Index Type)
	enum VTYPE_TYPE
	{
		VTYPE_NONE		= 0,	// 없음
		VTYPE_UCHAR		= 1,	// 8비트 부호없는 고정 소수점
		VTYPE_USHORT	= 2,	// 16비트 부호없는 고정 소수점 / 16비트 부호없는 정수
		VTYPE_FLOAT		= 3,	// 32비트 부동 소수점
	};

	// CT (Color Type)
	enum VTYPE_COLOR
	{
		VTYPE_CTNONE	= 0,	// 없음.
									// 1~3 : Reserved
		VTYPE_CT5650	= 4,
		VTYPE_CT5551	= 5,
		VTYPE_CT4444	= 6,
		VTYPE_CT8888	= 7,
	};

	// WC (Weight Count)
	enum VTYPE_WEIGHT
	{
		VTYPE_WEIGHT1	= 0,
		VTYPE_WEIGHT2	= 1,
		VTYPE_WEIGHT3	= 2,
		VTYPE_WEIGHT4	= 3,
	};

	// MC (Vertex Blend Count)
	enum VTYPE_BLEND
	{
		VTYPE_BLEND0	= 0,
		VTYPE_BLEND2	= 1,
		VTYPE_BLEND3	= 2,
		VTYPE_BLEND4	= 3,
		VTYPE_BLEND5	= 4,
		VTYPE_BLEND6	= 5,
		VTYPE_BLEND7	= 6,
		VTYPE_BLEND8	= 7,
	};

	// THR (Through Mode)
	enum VTYPE_THR
	{
		VTYPE_NORMAL	= 0,
		VTYPE_THROUGH	= 1,
	};

	// Set vertex type
	// 버텍스 타입 설정
	I3_INLINE
	UINT32	VTYPE
	(
		VTYPE_TYPE		TT,	// 텍스쳐 좌표 타입
		VTYPE_COLOR		CT,	// 컬러 타입
		VTYPE_TYPE		NT,	// 법선 벡터 타입
		VTYPE_TYPE		VT,	// 모델 좌표 타입
		VTYPE_TYPE		WT,	// 웨이트 타입 설정
		VTYPE_TYPE		IT,	// 인덱스 타입
		VTYPE_WEIGHT	WC,	// 웨이트 개수
		VTYPE_BLEND		MC,	// 정점 블렌드의 개수
		VTYPE_THR		TRU	// 통과 모드의 설정
	)
	{
		return MakeCmd( I3G_PSP_CMD_VTYPE, 
			( TT ) |
			( CT << 2 ) |
			( NT << 5 ) |
			( VT << 7 ) |
			( WT << 9 ) |
			( IT << 11 ) |
			( WC << 14 ) |
			( MC << 18 ) |
			( TRU << 23 ) );
	}

	// Set vertex type
	// 버텍스 타입 설정
	I3_INLINE
	UINT32	VTYPE(	UINT32 type)
	{
		return MakeCmd( I3G_PSP_CMD_VTYPE, type);
	}

	// 오프셋 어드레스
	// 오프셋 어드레스 레지스터에 저장되는 오프셋 어드레스의
	// 상위 24비트를 설정합니다. 나머지 8비트는 0이 저장됩니다.
	I3_INLINE
	UINT32	OFFSET( void * pAdr )
	{
		return MakeCmd( I3G_PSP_CMD_OFFSET, Lower24( ( UINT32 ) pAdr ) );
	}

	// 베이스 포인트 어드레스
	// 이 커맨드를 읽으면 이 커맨드 위치의 어드레스를 오프셋 레지스터에 저장합니다.
	I3_INLINE
	UINT32	ORIGIN( void )
	{
		return I3G_PSP_CMD_ORIGIN;
	}

	// 드로잉 영역
	// 드로잉 영역의 좌측 상단 끝점의 X,Y 좌표 설정.
	I3_INLINE
	UINT32	REGION1( UINT32 sx, UINT32 sy )
	{
		return MakeCmd( I3G_PSP_CMD_REGION1, ( sy << 10 ) | sx );
	}

	// 드로잉 영역
	// 드로잉 영역의 우측 하단 끝점의 X,Y 좌표 설정.
	I3_INLINE
	UINT32 REGION2( UINT32 sx, UINT32 sy )
	{
		return MakeCmd( I3G_PSP_CMD_REGION2, ( sy << 10 ) | sx );
	}

	// 라이팅 활성화
	I3_INLINE
	UINT32 LTE( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_LTE, bEnable );
	}

	// (0번) 라이팅 활성화
	I3_INLINE
	UINT32 LE0( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_LE0, bEnable );
	}

	// (1번) 라이팅 활성화
	I3_INLINE
	UINT32 LE1( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_LE1, bEnable );
	}

	// (2번) 라이팅 활성화
	I3_INLINE
	UINT32 LE2( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_LE2, bEnable );
	}

	// (3번) 라이팅 활성화
	I3_INLINE
	UINT32 LE3( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_LE3, bEnable );
	}

	// 클리핑 활성화
	I3_INLINE
	UINT32 CLE( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_CLE, bEnable );
	}

	// 백페이스 컬링 활성화
	I3_INLINE
	UINT32 BCE( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_BCE, bEnable );
	}

	// 텍스쳐 매핑 활성화
	I3_INLINE
	UINT32 TME( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_TME, bEnable );
	}

	// 포그 활성화
	I3_INLINE
	UINT32 FGE( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_FGE, bEnable );
	}

	// 디더링 활성화
	I3_INLINE
	UINT32 DTE( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_DTE, bEnable );
	}

	// 알파 블렌드 활성화
	I3_INLINE
	UINT32 ABE( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_ABE, bEnable );
	}

	// 알파 테스트 활성화
	I3_INLINE
	UINT32 ATE( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_ATE, bEnable );
	}

	// 깊이 테스트 활성화
	I3_INLINE
	UINT32 ZTE( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_ZTE, bEnable );
	}

	// 스텐실 테스트 활성화
	I3_INLINE
	UINT32 STE( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_STE, bEnable );
	}

	// 안티 앨리어싱 활성화
	I3_INLINE
	UINT32 AAE( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_AAE, bEnable );
	}

	// 패치 컬링 활성화
	I3_INLINE
	UINT32 PCE( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_PCE, bEnable );
	}

	// 컬러 테스트 활성화
	I3_INLINE
	UINT32 CTE( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_CTE, bEnable );
	}

	// 논리 연산 활성화
	I3_INLINE
	UINT32 LOE( BOOL bEnable )
	{
		return MakeCmd( I3G_PSP_CMD_LOE, bEnable );
	}

	// Set bone matrix number
	// 본 행렬 번호 설정
	I3_INLINE
	UINT32 BONEN( UINT32 num )
	{
		CHECK_BITS_7( num );

		return MakeCmd( I3G_PSP_CMD_BONEN, num );
	}

	// Set bone matrix data
	// 본 행렬 번호가 나타내는 위치에 본 행렬을 설정
	// 설정후에는 본 행렬 번호를 +1 합니다.
	I3_INLINE
	UINT32 BONED( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_BONED, Upper24Bits( data ) );
	}

	// Set vertex weight
	// 모핑시의 웨이트를 설정
	I3_INLINE
	UINT32 WEIGHT( UINT8 Index, REAL32 weight )
	{
		__ASSERT( Index <= 7 );

		return MakeIndexCmd( I3G_PSP_CMD_WEIGHT0, Index, Upper24Bits( weight ) );
	}

	// Set patch division count
	// 패치 분할 수
	I3_INLINE
	UINT32 DIVIDE( UINT8 UDiv, UINT8 VDiv )
	{
		return MakeCmd( I3G_PSP_CMD_DIVIDE, ( VDiv << 8 ) | UDiv );
	}

	//I3INLINE
	//UINT32 PPM( )
	//{
	//}

	//I3INLINE
	//UINT32 PFACE()
	//{
	//}

	// 월드 행렬 번호
	// num : 0 ~ 11, 총 12개 (어파인 매트릭스)
	I3_INLINE
	UINT32 WORLDN( UINT8 num )
	{
		//CHECK_BITS_4( num );
		__ASSERT( num <= 11 );

		return MakeCmd( I3G_PSP_CMD_WORLDN, num );
	}

	// 월드 행렬 데이터
	// 월드 행렬 번호가 가르키는 위치에 월드 행렬 값을 설정.
	// 설정 후에는 월드 행렬 번호를 +1 합니다.
	I3_INLINE
	UINT32 WORLDD( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_WORLDD, Upper24Bits( data ) );
	}

	// 뷰 행렬 번호
	// num : 0 ~ 11, 총 12개 (어파인 매트릭스)
	I3_INLINE
	UINT32 VIEWN( UINT8 num )
	{
		__ASSERT( num <= 11 );

		return MakeCmd( I3G_PSP_CMD_VIEWN, num );
	}

	// 뷰 행렬 데이터
	// 뷰 행렬 번호가 가르키는 위치에 뷰 행렬 값을 설정.
	// 설정 후에는 뷰 행렬 번호를 +1 합니다.
	I3_INLINE
	UINT32 VIEWD( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_VIEWD, Upper24Bits( data ) );
	}

	// 투시 행렬 번호
	// num : 0 ~ 15, 총 16개, 4x4 매트릭스 전체 설정 가능.
	I3_INLINE
	UINT32 PROJN( UINT8 num )
	{
		__ASSERT( num <= 15 );

		return MakeCmd( I3G_PSP_CMD_PROJN, num );
	}

	// 투시 행렬 데이터
	// 투시 행렬 번호가 가르키는 위치에 투시 행렬 값을 설정.
	// 설정 후에는 투시 행렬 번호를 +1 합니다.
	I3_INLINE
	UINT32 PROJD( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_PROJD, Upper24Bits( data ) );
	}

	// 텍스쳐 생성 행렬 번호
	// num : 0 ~ 11, 총 12개 (어파인 매트릭스)
	I3_INLINE
	UINT32 TGENN( UINT8 num )
	{
		return MakeCmd( I3G_PSP_CMD_TGENN, num );
	}

	// 텍스쳐 생성 행렬 데이터
	// 텍스쳐 생성 행렬 번호가 가르키는 위치에 텍스쳐 생성 행렬 값을 설정.
	// 설정 후에는 텍스쳐 생성 행렬 번호를 +1 합니다.
	I3_INLINE
	UINT32 TGEND( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_TGEND, Upper24Bits( data ) );
	}

	// 뷰포트 설정
	I3_INLINE
	UINT32 SX( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_SX, Upper24Bits( data ) );
	}

	// 뷰포트 설정
	I3_INLINE
	UINT32 SY( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_SY, Upper24Bits( data ) );
	}

	// 뷰포트 설정
	I3_INLINE
	UINT32 SZ( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_SZ, Upper24Bits( data ) );
	}

	// 뷰포트 설정
	I3_INLINE
	UINT32 TX( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_TX, Upper24Bits( data ) );
	}

	// 뷰포트 설정
	I3_INLINE
	UINT32 TY( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_TY, Upper24Bits( data ) );
	}

	// 뷰포트 설정
	I3_INLINE
	UINT32 TZ( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_TZ, Upper24Bits( data ) );
	}

	// 텍스쳐 스케일
	I3_INLINE
	UINT32 SU( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_SU, Upper24Bits( data ) );
	}

	// 텍스쳐 스케일
	I3_INLINE
	UINT32 SV( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_SV, Upper24Bits( data ) );
	}

	// 텍스쳐의 오프셋
	I3_INLINE
	UINT32 TU( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_TU, Upper24Bits( data ) );
	}

	// 텍스쳐의 오프셋
	I3_INLINE
	UINT32 TV( REAL32 data )
	{
		return MakeCmd( I3G_PSP_CMD_TV, Upper24Bits( data ) );
	}

	// 스크린의 오프셋
	// Offset의 X좌표. 소수부 4비트의 16비트 고정 소수점수. ( 0.0 ~ 4095.9375 )
	I3_INLINE
	UINT32 OFFSETX( UINT16 fp4XOffset )
	{
		return MakeCmd( I3G_PSP_CMD_OFFSETX, fp4XOffset );
	}

	// 스크린의 오프셋
	// Offset의 Y좌표. 소수부 4비트의 16비트 고정 소수점수. ( 0.0 ~ 4095.9375 )
	I3_INLINE
	UINT32 OFFSETY( UINT16 fp4YOffset )
	{
		return MakeCmd( I3G_PSP_CMD_OFFSETY, fp4YOffset );
	}

	enum SHADE_MODE
	{
		SHADE_FLAT		= 0,
		SHADE_GOURAUD	= 1
	};

	// 쉐이딩 모드 설정.
	I3_INLINE
	UINT32 SHADE( SHADE_MODE Mode )
	{
		return MakeCmd( I3G_PSP_CMD_SHADE, ( UINT32 ) Mode );
	}

	enum NREV_MODE
	{
		NREV_NORMAL		= 0,
		NREV_REVERSE	= 1
	};

	// 조명처리를 할 경우 법선 반전을 선택합니다.
	I3_INLINE
	UINT32 NREV( NREV_MODE Mode )
	{
		return MakeCmd( I3G_PSP_CMD_SHADE, ( UINT32 ) Mode );
	}

	// 조명 컬러 업데이트 여부
	I3_INLINE
	UINT32 MATERIAL( BOOL bUpdateAmbient, BOOL bUpdateDiffuse, BOOL bUpdateSpecular )
	{
		return MakeCmd( I3G_PSP_CMD_MATERIAL, ( bUpdateSpecular << 2 ) | ( bUpdateDiffuse << 1 ) | ( bUpdateAmbient ) );
	}

	// 모델컬러 설정. Emissive(?)
	I3_INLINE
	UINT32 MEC( UINT8 r, UINT8 g, UINT8 b )
	{
		return MakeCmd( I3G_PSP_CMD_MEC, ( b << 16 ) | ( g << 8 ) | r );
	}

	// 모델컬러 설정. Ambient
	I3_INLINE
	UINT32 MAC( UINT8 r, UINT8 g, UINT8 b )
	{
		return MakeCmd( I3G_PSP_CMD_MAC, ( b << 16 ) | ( g << 8 ) | r );
	}

	// 모델컬러 설정. Diffuse
	I3_INLINE
	UINT32 MDC( UINT8 r, UINT8 g, UINT8 b )
	{
		return MakeCmd( I3G_PSP_CMD_MDC, ( b << 16 ) | ( g << 8 ) | r );
	}

	// 모델컬러 설정. Specular
	I3_INLINE
	UINT32 MSC( UINT8 r, UINT8 g, UINT8 b )
	{
		return MakeCmd( I3G_PSP_CMD_MSC, ( b << 16 ) | ( g << 8 ) | r );
	}

	// 모델의 알파값 설정.
	I3_INLINE
	UINT32 MAA( UINT8 a )
	{
		return MakeCmd( I3G_PSP_CMD_MAA, a );
	}

	// 모델의 경면광(Specular Light?) 계수를 설정합니다.
	I3_INLINE
	UINT32 MK( REAL32 coef )
	{
		return MakeCmd( I3G_PSP_CMD_MK, Upper24Bits( coef ) );
	}

	// 라이팅시에 사용할 환경광의 컬러값을 설정합니다.
	I3_INLINE
	UINT32 AC( UINT8 r, UINT8 g, UINT8 b )
	{
		return MakeCmd( I3G_PSP_CMD_AC, ( b << 16 ) | ( g << 8 ) | r );
	}

	// 라이팅시에 사용할 환경광의 알파값을 설정합니다.
	I3_INLINE
	UINT32 AA( UINT8 a )
	{
		return MakeCmd( I3G_PSP_CMD_AA, a );
	}

	enum LMODE_COLOR
	{
		LMODE_SINGLE_COLOR				= 0,
		LMODE_SEPARATE_SPECULAR_COLOR	= 1
	};

	// 모델컬러의 계산방법을 설정합니다.
	I3_INLINE
	UINT32 LMODE( LMODE_COLOR ColorMode )
	{
		return MakeCmd( I3G_PSP_CMD_LMODE, ( UINT32 ) ColorMode );
	}

	enum LTYPE_COMP
	{
		LTYPE_DIFFUSE					= 0,	// 00 확산광
		LTYPE_DIFFUSE_SPECULAR			= 1,	// 01 확산광 + 경면광
		LTYPE_POWERED_DIFFUSE_SPECULAR	= 2,	// 10 배율 확산광 + 경면광
	};

	///////////////////////////////////////////////////////////////////////////////////////
	// Light type
	enum	LIGHT_TYPE
	{
		LIGHT_NONE							=	-1,
		LIGHT_DIRECTIONAL					=	0,
		LIGHT_SPOT							=	2,
		LIGHT_POINT							=	1,
	};

	// 라이팅 계산시 확산광및 경면광의 유무와 광원의 유무를 설정합니다.
	I3_INLINE
	UINT32 LTYPE( UINT8 Index, LTYPE_COMP Comp, LIGHT_TYPE Type )
	{
		__ASSERT( Index <= 3 );
		CHECK_BITS_2( Comp );
		CHECK_BITS_2( Type );

		return MakeIndexCmd( I3G_PSP_CMD_LTYPE0, Index, ( Type << 8 ) | Comp );
	}

	// Set light vector
	// 광원의 월드좌표계를 설정한다. X 좌표 설정.
	// 평행광원은 벡터로 인식.
	// Index(광원 인덱스) : 0 ~ 3 (4개)
	I3_INLINE
	UINT32 LX( UINT8 Index, REAL32 x )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LX0, ( Index * 3 ), Upper24Bits( x ) );
	}

	// 광원의 월드좌표계를 설정한다. Y 좌표 설정.
	I3_INLINE
	UINT32 LY( UINT8 Index, REAL32 y )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LY0, ( Index * 3 ), Upper24Bits( y ) );
	}

	// 광원의 월드좌표계를 설정한다. Z 좌표 설정.
	I3_INLINE
	UINT32 LZ( UINT8 Index, REAL32 z )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LZ0, ( Index * 3 ), Upper24Bits( z ) );
	}

	// Set light direction
	// 광원의 X 방향을 설정한다.
	// Index(광원 인덱스) : 0 ~ 3 (4개)
	I3_INLINE
	UINT32 LDX( UINT8 Index, REAL32 x )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LDX0, ( Index * 3 ), Upper24Bits( x ) );
	}

	// 광원의 Y 방향을 설정한다.
	I3_INLINE
	UINT32 LDY( UINT8 Index, REAL32 y )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LDY0, ( Index * 3 ), Upper24Bits( y ) );
	}

	// 광원의 Z 방향을 설정한다.
	I3_INLINE
	UINT32 LDZ( UINT8 Index, REAL32 z )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LDZ0, ( Index * 3 ), Upper24Bits( z ) );
	}

	// Set light distance attenuation factor
	// 광원의 거리감쇄계수를 설정한다. A 값
	// Index(광원 인덱스) : 0 ~ 3 (4개)
	I3_INLINE
	UINT32 LKA( UINT8 Index, REAL32 a )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LKA0, ( Index * 3 ), Upper24Bits( a ) );
	}

	// 광원의 거리감쇄계수를 설정한다. B 값
	I3_INLINE
	UINT32 LKB( UINT8 Index, REAL32 b )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LKB0, ( Index * 3 ), Upper24Bits( b ) );
	}

	// 광원의 거리감쇄계수를 설정한다. C 값
	I3_INLINE
	UINT32 LKC( UINT8 Index, REAL32 c )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LKC0, ( Index * 3 ), Upper24Bits( c ) );
	}

	// Set light convergence factor
	// 광원의 집속계수를 설정한다.
	// Index(광원 인덱스) : 0 ~ 3 (4개)
	I3_INLINE
	UINT32 LKS( UINT8 Index, REAL32 s )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LKS0, Index, Upper24Bits( s ) );
	}

	// Set light distance attenuation factor
	// 광원의 차단내적값을 설정합니다.
	// Index(광원 인덱스) : 0 ~ 3 (4개)
	I3_INLINE
	UINT32 LKO( UINT8 Index, REAL32 cutoff_dot )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LKO0, Index, Upper24Bits( cutoff_dot ) );
	}

	// Set light color
	// 조명의 컬러값을 설정합니다. Ambient
	I3_INLINE
	UINT32 LAC( UINT8 Index, UINT8 r, UINT8 g, UINT8 b )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LAC0, ( Index * 3 ), ( b << 16 ) | ( g << 8 ) | r );
	}

	// 조명의 컬러값을 설정합니다. Diffuse
	I3_INLINE
	UINT32 LDC( UINT8 Index, UINT8 r, UINT8 g, UINT8 b )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LDC0, ( Index * 3 ), ( b << 16 ) | ( g << 8 ) | r );
	}

	// 조명의 컬러값을 설정합니다. Specular
	I3_INLINE
	UINT32 LSC( UINT8 Index, UINT8 r, UINT8 g, UINT8 b )
	{
		__ASSERT( Index <= 3 );

		return MakeIndexCmd( I3G_PSP_CMD_LSC0, ( Index * 3 ), ( b << 16 ) | ( g << 8 ) | r );
	}

	enum CULL_MODE
	{
		CULL_CCW	= 0,	// 반시계 방향
		CULL_CW		= 1		// 시계 방향
	};

	// 백페이스 컬링을 할 경우 삼각형의 정점배열 순서.
	I3_INLINE
	UINT32 CULL( CULL_MODE Mode )
	{
		return MakeCmd( I3G_PSP_CMD_CULL, Mode );
	}

	// Set frame buffer
	// 프레임 버퍼의 베이스 포인터를 8KB 단위로 설정(하위 13비트 무시)
	I3_INLINE
	UINT32 FBP( UINT32 vram_adr )
	{
		return MakeCmd( I3G_PSP_CMD_FBP, Lower24( vram_adr ) );
	}

	// Set frame buffer width
	// 프레임 버퍼의 폭을 64의 배수로 설정합니다.
	// fbw : 버퍼의 폭. 64~1024 범위의 설정이 가능하며, 하위 6비트는 무시됩니다.
	// vram_adr : 프레임 버퍼의 베이스 어드레스. (상위 8 비트를 설정합니다.)
	I3_INLINE
	UINT32 FBW( UINT32 vram_adr, UINT32 fbw )
	{
		return MakeCmd( I3G_PSP_CMD_FBW, Upper8( vram_adr ) | fbw );
	}

	// Set depth buffer
	// 깊이 버퍼의 베이스 포인터를 8KB 단위로 설정(하위 13비트 무시)
	I3_INLINE
	UINT32 ZBP( UINT32 vram_adr )
	{
		return MakeCmd( I3G_PSP_CMD_ZBP, Lower24( vram_adr ) );
	}

	// Set depth buffer width
	// 깊이 버퍼의 폭을 64의 배수로 설정합니다.
	// zbw : 버퍼의 폭. 64~1024 범위의 설정이 가능하며, 하위 6비트는 무시됩니다.
	// vram_adr : 깊이 버퍼의 베이스 어드레스. (상위 8 비트를 설정합니다.)
	I3_INLINE
	UINT32 ZBW( UINT32 vram_adr, UINT32 zbw )
	{
		return MakeCmd( I3G_PSP_CMD_ZBW, Upper8( vram_adr ) | zbw );
	}

	// Set texture buffer
	// 텍스쳐 버퍼 0~7의 베이스 포인터를 16바이트 단위로 설정합니다. (하위 4비트 무시)
	I3_INLINE
	UINT32 TBP( UINT8 Index, UINT32 vram_adr )
	{
		__ASSERT( Index <= 7 );

		return MakeIndexCmd( I3G_PSP_CMD_TBP0, Index, Lower24( vram_adr ) );
	}

	// Set texture buffer width
	// 텍스쳐 버퍼의 폭을 설정합니다.
	// 최대값 1024.
	// 픽셀 포맷별 크기 제한.
	// 32bit / 32bit Indexed : 4의 배수
	// 16bit / 16bit Indexed : 8의 배수
	// 8bit Indexed : 16의 배수
	// 4bit Indexed : 32의 배수
	// DXT1 : 8의 배수
	// DXT3,5 : 4의 배수
	// vram_adr : 텍스쳐 버퍼의 베이스 어드레스. (상위 8 비트를 설정합니다.)
	I3_INLINE
	UINT32 TBW( UINT8 Index, UINT32 vram_adr, UINT32 tbw )
	{
		__ASSERT( Index <= 7 );

		return MakeIndexCmd( I3G_PSP_CMD_TBW0, Index, Upper8( vram_adr ) | tbw );
	}

	// Set CLUT buffer
	// CLUT 버퍼의 베이스 포인터를 16바이트 단위로 설정. (하위 4비트 무시)
	I3_INLINE
	UINT32 CBP( UINT32 vram_adr )
	{
		return MakeCmd( I3G_PSP_CMD_CBP, Lower24( vram_adr ) );
	}

	// Set CLUT buffer width
	// CLUT 버퍼의 베이스 어드레스의 상위 8비트를 설정합니다.
	I3_INLINE
	UINT32 CBW( UINT32 vram_adr )
	{
		return MakeCmd( I3G_PSP_CMD_CBW, Upper8( vram_adr ) );
	}

	// Set transfer buffer (source)
	// 소스 전송 버퍼
	// 소스가 되는 전송 버퍼의 베이스 포인터를 16바이트 단위로 설정합니다.
	// 하위 4비트는 무시됩니다.
	I3_INLINE
	UINT32 XBP1( UINT32 vram_adr )
	{
		return MakeCmd( I3G_PSP_CMD_XBP1, Lower24( vram_adr ) );
	}

	// Set transfer buffer (source)
	// 소스 전송 버퍼
	// w : 소스가 되는 전송 버퍼의 폭을 8의 배수로 설정합니다.
    //     8~1024 의 범위로 설정이 가능하며, 하위 3비트는 무시됩니다.
	// vram_adr : 소스가 되는 전송 버퍼의 베이스 포인터. ( 상위 8비트를 설정합니다. )
	I3_INLINE
	UINT32 XBW1( UINT32 vram_adr, UINT32 w )
	{
		return MakeCmd( I3G_PSP_CMD_XBW1, Upper8( vram_adr ) | w );
	}

	// Set transfer buffer (destination)
	// 대상 전송 버퍼
	// 대상이 되는 전송 버퍼의 베이스 포인터를 16바이트 단위로 설정합니다. 하위 4비트는 무시됩니다.
	I3_INLINE
	UINT32 XBP2( UINT32 vram_adr )
	{
		return MakeCmd( I3G_PSP_CMD_XBP2, Lower24( vram_adr ) );
	}

	// Set transfer buffer (destination)
	// 대상 전송 버퍼
	// w : 소스가 되는 전송 버퍼의 폭을 8의 배수로 설정합니다.
	//     8~1024 의 범위로 설정이 가능하며, 하위 3비트는 무시됩니다.
	// vram_adr : 소스가 되는 전송 버퍼의 베이스 포인터. ( 상위 8비트를 설정합니다. )
	I3_INLINE
	UINT32 XBW2( UINT32 vram_adr, UINT32 w )
	{
		return MakeCmd( I3G_PSP_CMD_XBW2, Upper8( vram_adr ) | w );
	}

	// Set texture size
	// 텍스쳐의 크기
	// tw : 텍스쳐의 폭을 각 레벨마다 설정합니다. 2의 n승에서 n을 설정하며, n은 0 이상 9 이하이어야 합니다.
	// th : 텍스쳐의 높이를 각 레벨마다 설정합니다. 2의 n승에서 n을 설정하며, n은 0 이상 9 이하이어야 합니다.
	I3_INLINE
	UINT32 TSIZE( UINT8 Index, UINT8 tw, UINT8 th )
	{
		__ASSERT( Index <= 7 );
		__ASSERT( tw <= 9 );
		__ASSERT( th <= 9 );

		return MakeIndexCmd( I3G_PSP_CMD_TSIZE0, Index, ( th << 8 ) | tw );
	}

	// TMAP
	enum TMAP_TMN
	{
		TMAP_TMN_UV			= 0,	// UV 매핑
		TMAP_TMN_PROJECTION = 1,	// 프로젝션 매핑
		TMAP_TMN_SHADE		= 2,	// 쉐이드 매핑
	};

	enum TMAP_TMI
	{
		TMAP_TMI_XYZ		= 0,	// XYZ 좌표
		TMAP_TMI_UV			= 1,	// 텍스쳐 좌표
		TMAP_TMI_NORMAL		= 2,	// 정규화 법선 좌표
		TMAP_TMI_DENORMAL	= 3,	// 비정규화 법선 좌표
	};

	// Set texture map mode
	// 텍스쳐 매핑 모드
	// tmn : 텍스쳐의 매핑 모드
	// tmi : 프로젝션 매핑시 패러미터 설정
	I3_INLINE
	UINT32 TMAP( TMAP_TMN tmn, TMAP_TMI tmi )
	{
		return MakeCmd( I3G_PSP_CMD_TMAP, ( tmi << 8 ) | tmn );
	}

	// Set shade mapping
	// 쉐이드 매핑
	// 쉐이드 매핑시 사용할 광원의 번호를 U 방향과 V방향에 대해서 설정합니다.
	I3_INLINE
	UINT32 TSHADE( UINT32 u, UINT32 v )
	{
		CHECK_BITS_2( u );
		CHECK_BITS_2( v );

		return MakeCmd( I3G_PSP_CMD_TSHADE, ( v << 8 ) | u );
	}

	// TMODE, High Speed Mode
	enum TMODE_HSM
	{
		TMODE_NORMAL		= 0,
		TMODE_HIGHSPEED		= 1
	};

	// TMODE, Multi CLUT
	enum TMODE_MC
	{
		TMODE_SINGLE_CLUT	= 0,
		TMODE_MULTI_CLUT	= 1
	};

	// Set texture mode
	// 텍스쳐 모드
	// bHighSpeed ( hsm ) : 텍스쳐 데이터의 저장 형식 모드를 설정합니다. (hsm)
	// bMc : 멀티 CLUT의 사용여부를 설정합니다.
	// mxl (Max Texture Level): 텍스쳐의 최대레벨(0~7)을 설정합니다.
	I3_INLINE
	UINT32 TMODE( TMODE_HSM hsm, TMODE_MC mc, UINT32 mxl )
	{
		__ASSERT( mxl <= 7 );

		return MakeCmd( I3G_PSP_CMD_TMODE, ( mxl << 16 ) | ( mc << 8 ) | hsm );
	}

	enum TEX_FORMAT
	{
		TPF_5650		= 0,
		TPF_5551		= 1,
		TPF_4444		= 2,
		TPF_IDTEX4		= 4,
		TPF_IDTEX8		= 5,
		TPF_IDTEX6		= 6,
		TPF_IDTEX32		= 7,
		TPF_DXT1		= 8,
		TPF_DXT3		= 9,
		TPF_DXT5		= 10,
	};

	// Set texture pixel format
	// 텍스쳐 버퍼의 픽셀 포맷을 설정합니다.
	I3_INLINE
	UINT32 TPF( TEX_FORMAT tpf, UINT8 ext = 0 )
	{
		CHECK_BITS_4( tpf );
		CHECK_BITS_1( ext );

		return MakeCmd( I3G_PSP_CMD_TPF, ( ext << 8 ) | tpf );
	}

	// CLUT load
	// CLUT 버퍼에서 CLUT로 로드할 팔레트 개수를 설정합니다.
	// 16비트 컬러 포맷은 16팔레트 단위, 32비트 컬러 포맷은 팔레트 단위의 설정이 됩니다.
	// 16 비트 컬러
	// 로드할 팔레트 개수 = 16 * NP ( 1 <= NP <= 32 )
	// 32 비트 컬러
	// 로드할 팔레트 개수 = 8 * NP ( 1 <= NP <= 32 )
	I3_INLINE
	UINT32 CLOAD( UINT32 np )
	{
		CHECK_BITS_6( np );

		return MakeCmd( I3G_PSP_CMD_CLOAD, np );
	}

	enum CLUT_FORMAT
	{
		CLUT_CPF_5650	= 0,
		CLUT_CPF_5551	= 1,
		CLUT_CPF_4444	= 2,
		CLUT_CPF_8888	= 3,
	};

	// Set CLUT
	// CLUT의 픽셀 포맷을 설정합니다.
	// cpf : CLUT Pixel Format
	// sft : 인덱스 컬러 사용시의 인덱스 생성용 시프트량을 설정합니다.
	// msk : 인덱스 컬러 사용시의 인덱스 생성용 마스크를 설정합니다.
	// csa : CLUT의 읽기 선두 위치를 설정합니다. 선두 팔레트 번호 = CSA * 16
	I3_INLINE
	UINT32 CLUT( CLUT_FORMAT cpf, UINT32 sft, UINT32 msk, UINT32 csa )
	{
		CHECK_BITS_2( cpf );
		CHECK_BITS_5( sft );
		CHECK_BITS_8( msk );
		CHECK_BITS_5( csa );

		return MakeCmd( I3G_PSP_CMD_CLUT, ( csa << 16 ) | ( msk << 8 ) | ( sft << 2 ) | cpf );
	}

	enum TFILTER_MODE
	{
		TFILTER_NEAREST					= 0,
		TFILTER_LINEAR					= 1,
		TFILTER_NEAREST_MIPMAP_NEAREST	= 4,
		TFILTER_LINEAR_MIPMAP_NEAREST	= 5,
		TFILTER_NEAREST_MIPMAP_LINEAR	= 6,
		TFILTER_LINEAR_MIPMAP_LINEAR	= 7,
	};

	// Set texture filter
	// 텍스쳐 매핑시 필터 모드
	// min : 축소시 필터 모드 ( 모든 필터 사용 가능 )
	// mag : 확대시 필터 모드 ( TFILTER_NEAREST, TFILTER_LINEAR 만 사용 가능 )
	I3_INLINE
	UINT32 TFILTER( TFILTER_MODE min, TFILTER_MODE mag )
	{
		CHECK_BITS_3( min );
		CHECK_BITS_1( mag );

		return MakeCmd( I3G_PSP_CMD_TFILTER, ( mag << 8 ) | min );
	}

	enum TWRAP_MODE
	{
		TWRAP_REPEAT	= 0,
		TWRAP_CLAMP		= 1,
	};

	// Set texture wrap mode
	// 텍스쳐 좌표의 래핑 모드를 설정한다.
	// wms : S 좌표의 래핑 모드
	// wmt : T 좌표의 래핑 모드
	I3_INLINE
	UINT32 TWRAP( TWRAP_MODE wms, TWRAP_MODE wmt )
	{
		CHECK_BITS_1( wms );
		CHECK_BITS_1( wmt );

		return MakeCmd( I3G_PSP_CMD_TWRAP, ( wmt << 8 ) | wms );
	}

	enum TLEVEL_MODE
	{
		TLEVEL_VARIABLE1	= 0,	// 가변 모드 1
		TLEVEL_FIX			= 1,	// 고정 모드
		TLEVEL_VARIABLE2	= 2,	// 가변 모드 2
	};

	// Set texture level mode
	// 텍스쳐 레벨 모드
	// lcm : 텍스쳐 레벨 산출 모드
	// fp4Offl : 레벨의 오프셋을 설정. 하위 4비트는 소수부의 부호있는 고정 소수로 표현합니다.
	I3_INLINE
	UINT32 TLEVEL( TLEVEL_MODE lcm, UINT32 fp4Offl )
	{
		CHECK_BITS_2( lcm );
		CHECK_BITS_8( fp4Offl );

		return MakeCmd( I3G_PSP_CMD_TLEVEL, ( fp4Offl << 16 ) | lcm );
	}

	enum TFUNC_TXF
	{
		TFUNC_MODULATE	= 0,
		TFUNC_DECAL		= 1,
		TFUNC_BLEND		= 2,
		TFUNC_REPLACE	= 3,
		TFUNC_ADD		= 4,
	};

	enum TFUNC_TCC
	{
		TFUNC_RGB		= 0,
		TFUNC_RGBA		= 1,
	};

	// Set texture function
	// 텍스쳐의 함수를 설정합니다.
	// txf : 텍스쳐의 함수
	// tcc : 텍스쳐의 컬러 컴포넌트
	// cd : 컬러 더블링의 유무를 설정합니다.
	I3_INLINE
	UINT32 TFUNC( TFUNC_TXF txf, TFUNC_TCC tcc, BOOL cd )
	{
		CHECK_BITS_3( txf );
		CHECK_BITS_1( tcc );
		CHECK_BITS_1( cd );

		return MakeCmd( I3G_PSP_CMD_TFUNC, ( cd << 16 ) | ( tcc << 8 ) | txf );
	}

	// Set texture environment color
	// 텍스쳐 환경컬러 값 설정
	I3_INLINE
	UINT32 TEC( UINT8 r, UINT8 g, UINT8 b )
	{
		return MakeCmd( I3G_PSP_CMD_TEC, ( b << 16 ) | ( g << 8 ) | r );
	}

	// Texture flush
	// 텍스쳐 제거. 텍스쳐 캐시를 제거합니다.
	I3_INLINE
	UINT32 TFLUSH( void )
	{
		return I3G_PSP_CMD_TFLUSH;
	}

	// Texture synchronization
	// 텍스쳐 동기
	// 드로잉 완료까지 로컬 메모리의 버퍼에 대한 텍스쳐 억세스를 블록합니다.
	I3_INLINE
	UINT32 TSYNC( void )
	{
		return I3G_PSP_CMD_TSYNC;
	}

	// Set fog parameter
	// 포그 패러미터 설정 1
	I3_INLINE
	UINT32 FOG1( REAL32 fog )
	{
		return MakeCmd( I3G_PSP_CMD_FOG1, Upper24Bits( fog ) );
	}

	// Set fog parameter
	// 포그 패러미터 설정 2
	I3_INLINE
	UINT32 FOG2( REAL32 fog )
	{
		return MakeCmd( I3G_PSP_CMD_FOG2, Upper24Bits( fog ) );
	}

	// Set fog color
	// 포그 컬러값
	I3_INLINE
	UINT32 FC( UINT8 r, UINT8 g, UINT8 b )
	{
		return MakeCmd( I3G_PSP_CMD_FC, ( b << 16 ) | ( g << 8 ) | r );
	}

	// Set texture slope
	// 텍스쳐 기울기
	// LOD 산출에 사용할 텍셀 좌표의 기울기 설정.
	I3_INLINE
	UINT32 TSLOPE( REAL32 slope )
	{
		return MakeCmd( I3G_PSP_CMD_TSLOPE, Upper24Bits( slope ) );
	}

	enum FRAME_FORMAT
	{
		FPF_5650	= 0,
		FPF_5551	= 1,
		FPF_4444	= 2,
		FPF_8888	= 3
	};
	// Set frame pixel format
	// 프레임 버퍼의 픽셀 포맷 설정
	I3_INLINE
	UINT32 FPF( FRAME_FORMAT fpf )
	{
		CHECK_BITS_2( fpf );

		return MakeCmd( I3G_PSP_CMD_FPF, fpf );
	}

	enum CMODE_SET
	{
		CMODE_NORMAL		= 0,	// 일반 모드
		CMODE_CLEAR			= 1		// 클리어 모드
	};
	enum CMODE_EN
	{
		CMODE_KEEP			= 0,	// 업데이트 안 함.
		CMODE_UPDATE		= 1		// 업데이트 함.
	};
	// Set clear mode
	// 클리어 모드
	// set : 클리어 모드 설정
	// cen : 클리어 모드 설정시 드로잉한 후, 컬러값 업데이트 여부.
	// aen : 클리어 모드 설정시 드로잉한 후, 알파값 업데이트 여부.
	// zen : 클리어 모드 설정시 드로잉한 후, 깊이값 업데이트 여부.
	I3_INLINE
	UINT32 CMODE( CMODE_SET set, CMODE_EN cen, CMODE_EN aen, CMODE_EN zen )
	{
		CHECK_BITS_1( set );
		CHECK_BITS_1( cen );
		CHECK_BITS_1( aen );
		CHECK_BITS_1( zen );

		return MakeCmd( I3G_PSP_CMD_CMODE, ( zen << 10 ) | ( aen << 9 ) | ( cen << 8 ) | set );
	}

	// Set scissoring area
	// 자르기 영역 설정
	// sx1 : 좌측 상단 끝점의 X 좌표. (10비트 부호없는 정수)
	// sy1 : 좌측 상단 끝점의 Y 좌표. (10비트 부호없는 정수)
	I3_INLINE
	UINT32 SCISSOR1( UINT32 sx1, UINT32 sy1 )
	{
		CHECK_BITS_10( sx1 );
		CHECK_BITS_10( sy1 );

		return MakeCmd( I3G_PSP_CMD_SCISSOR1, ( sy1 << 10 ) | sx1 );
	}

	// Set scissoring area
	// 자르기 영역 설정
	// sx2 : 우측 하단 끝점의 X 좌표. (10비트 부호없는 정수)
	// sy2 : 우측 하단 끝점의 Y 좌표. (10비트 부호없는 정수)
	I3_INLINE
	UINT32 SCISSOR2( UINT32 sx2, UINT32 sy2 )
	{
		CHECK_BITS_10( sx2 );
		CHECK_BITS_10( sy2 );

		return MakeCmd( I3G_PSP_CMD_SCISSOR2, ( sy2 << 10 ) | sx2 );
	}

	// Set depth range
	// 깊이 영역 설정
	// minz : z의 최소값. 16비트 부호 없는 정수.
	I3_INLINE
	UINT32 MINZ( UINT16 minz )
	{
		return MakeCmd( I3G_PSP_CMD_MINZ, minz );
	}

	// Set depth range
	// 깊이 영역 설정
	// maxz : z의 최대값. 16비트 부호 없는 정수.
	I3_INLINE
	UINT32 MAXZ( UINT16 maxz )
	{
		return MakeCmd( I3G_PSP_CMD_MAXZ, maxz );
	}

	enum CTEST_FUNC
	{
		CTEST_NEVER		= 0,
		CTEST_ALWAYS	= 1,
		CTEST_EQUAL		= 2,
		CTEST_NOTEQUAL	= 3,
	};
	// Set color test parameter
	// 컬러 테스트 패러미터
	// ctf : 컬러 테스트 함수
	I3_INLINE
	UINT32 CTEST( UINT32 ctf )
	{
		CHECK_BITS_2( ctf );

		return MakeCmd( I3G_PSP_CMD_CTEST, ctf );
	}

	// Set color test reference
	// 컬러 테스트 레퍼런스
	I3_INLINE
	UINT32 CREF( UINT8 r, UINT8 g, UINT8 b )
	{
		return MakeCmd( I3G_PSP_CMD_CREF, ( b << 16 ) | ( g << 8 ) | r );
	}

	// Set color test mask
	// 컬러 테스트 마스크
	// 1로 설정된 비트만 컬러 테스트 레퍼런스와 비교합니다.
	I3_INLINE
	UINT32 CMSK( UINT8 r, UINT8 g, UINT8 b )
	{
		return MakeCmd( I3G_PSP_CMD_CMSK, ( b << 16 ) | ( g << 8 ) | r );
	}

	enum ATEST_FUNC
	{
		ATEST_NEVER		= 0,
		ATEST_ALWAYS	= 1,
		ATEST_EQUAL		= 2,
		ATEST_NOTEQUAL	= 3,
		ATEST_LESS		= 4,
		ATEST_LEQUAL	= 5,
		ATEST_GREATER	= 6,
		ATEST_GEQUAL	= 7,
	};

	// Set alpha test parameters
	// 알파 테스트 패러미터
	// atf : 알파테스트 함수.
	// aref : 알파 테스트의 레퍼런스. 부호없는 8비트 정수 지정.
	// amsk : 알파 테스트의 마스크. 1의 비트만 레퍼런스와 비교.
	I3_INLINE
	UINT32 ATEST( ATEST_FUNC atf, UINT8 aref, UINT8 amsk )
	{
		return MakeCmd( I3G_PSP_CMD_ATEST, ( amsk << 16 ) | ( aref << 8 ) | atf );
	}

	enum STEST_FUNC
	{
		STEST_NEVER		= 0,
		STEST_ALWAYS	= 1,
		STEST_EQUAL		= 2,
		STEST_NOTEQUAL	= 3,
		STEST_LESS		= 4,
		STEST_LEQUAL	= 5,
		STEST_GREATER	= 6,
		STEST_GEQUAL	= 7,
	};

	// Set stencil test parameters
	// 스텐실 테스트 패러미터
	// stf : 스텐실 테스트 함수 설정.
	// sref : 스텐실 테스트의 레퍼런스.
	// smsk : 스텐실 테스트의 마스크. 1의 비트만 레퍼런스와 비교.
	I3_INLINE
	UINT32 STEST( STEST_FUNC stf, UINT8 sref, UINT8 smsk )
	{
		return MakeCmd( I3G_PSP_CMD_STEST, ( smsk << 16 ) | ( sref << 8 ) | stf );
	}

	enum SOP_MODE
	{
		SOP_KEEP		= 0,
		SOP_ZERO		= 1,
		SOP_REPLACE		= 2,
		SOP_INVERT		= 3,
		SOP_INCR		= 4,
		SOP_DECR		= 5,
	};

	// Set stencil operation
	// 스텐실 오퍼레이션
	// SFail : 스텐실 테스트에서 실패했을 때 오퍼레이션 설정.
	// ZFail : 스텐실 테스트 성공하고 깊이 테스트에서 실패했을 때 오퍼레이션 설정.
	// ZPass : 스텐실, 깊이 테스트 모두 성공했을 때 오퍼레이션 설정
	I3_INLINE
	UINT32 SOP( SOP_MODE SFail, SOP_MODE ZFail, SOP_MODE ZPass )
	{
		return MakeCmd( I3G_PSP_CMD_SOP, ( ZPass << 16 ) | ( ZFail << 8 ) | SFail );
	}

	enum ZTEST_MODE
	{
		ZTEST_NEVER		= 0,
		ZTEST_ALWAYS	= 1,
		ZTEST_EQUAL		= 2,
		ZTEST_NOTEQUAL	= 3,
		ZTEST_LESS		= 4,
		ZTEST_LEQUAL	= 5,
		ZTEST_GREATER	= 6,
		ZTEST_GEQUAL	= 7,
	};

	// Set depth test parameter
	// 깊이 테스트 패러미터
	// 
	I3_INLINE
	UINT32 ZTEST( ZTEST_MODE ztf )
	{
		return MakeCmd( I3G_PSP_CMD_ZTEST, ztf );
	}

	enum BLEND_A
	{
		BLEND_A_CD		= 0,
		BLEND_A_255_CD	= 1,
		BLEND_A_AS		= 2,
		BLEND_A_255_AS	= 3,
		BLEND_A_AD		= 4,
		BLEND_A_255_AD	= 5,
		BLEND_A_2AS		= 6,
		BLEND_A_255_2AS	= 7,
		BLEND_A_2AD		= 8,
		BLEND_A_255_2AD	= 9,
		BLEND_A_FIXA	= 10,
	};

	//I3G_BLEND
	//I3G_BLEND_SRCCOLOR		=	SCEGU_SRC_COLOR,				// 0
	//I3G_BLEND_INVSRCCOLOR	=	SCEGU_ONE_MINUS_SRC_COLOR,		// 1
	//I3G_BLEND_SRCALPHA		=	SCEGU_SRC_ALPHA,				// 2
	//I3G_BLEND_INVSRCALPHA	=	SCEGU_ONE_MINUS_SRC_ALPHA,		// 3
	//I3G_BLEND_DESTALPHA		=	SCEGU_DST_ALPHA,				// 4
	//I3G_BLEND_INVDESTALPHA	=	SCEGU_ONE_MINUS_DST_ALPHA,		// 5
	//I3G_BLEND_DESTCOLOR		=	SCEGU_DST_COLOR,				// 0
	//I3G_BLEND_INVDESTCOLOR	=	SCEGU_ONE_MINUS_DST_COLOR,		// 1
	//I3G_BLEND_ZERO			=	20,								// 10
	//I3G_BLEND_ONE			=	21,								// 10
	//I3G_BLEND_SRCALPHASAT	=	SCEGU_SRC_ALPHA,				// 2

	enum BLEND_B
	{
		BLEND_B_CS		= 0,
		BLEND_B_255_CS	= 1,
		BLEND_B_AS		= 2,
		BLEND_B_255_AS	= 3,
		BLEND_B_AD		= 4,
		BLEND_B_255_AD	= 5,
		BLEND_B_2AS		= 6,
		BLEND_B_255_2AS	= 7,
		BLEND_B_2AD		= 8,
		BLEND_B_255_2AD	= 9,
		BLEND_B_FIXB	= 10,
	};

	enum BLEND_EQU
	{
		BLEND_CSA_PLUS_CDB	= 0,
		BLEND_CSA_MINUS_CDB	= 1,
		BLEND_CDB_MINUS_CDA	= 2,
		BLEND_MIN_CS_CD		= 3,
		BLEND_MAX_CS_CD		= 4,
		BLEND_ABS_CS_CD		= 5,
	};

	// Set alpha blending parameters
	// 알파 블렌드 패러미터
	// a : 알파 블렌딩 함수의 A 값
	// b : 알파 블렌딩 함수의 B 값
	// equ : 알파 블렌딩 함수의 연산을 설정합니다.
	I3_INLINE
	UINT32 BLEND( BLEND_A a, BLEND_B b, BLEND_EQU equ )
	{
		CHECK_BITS_4( a );
		CHECK_BITS_4( b );
		CHECK_BITS_3( equ );

		return MakeCmd( I3G_PSP_CMD_BLEND, ( equ << 8 ) | ( b << 4 ) | a );
	}

	// Set fixed color A
	// 고정 컬러 A 설정
	I3_INLINE
	UINT32 FIXA( UINT8 r, UINT8 g, UINT8 b )
	{
		return MakeCmd( I3G_PSP_CMD_FIXA, ( b << 16 ) | ( g << 8 ) | r );
	}

	// Set fixed color B
	// 고정 컬러 B 설정
	I3_INLINE
	UINT32 FIXB( UINT8 r, UINT8 g, UINT8 b )
	{
		return MakeCmd( I3G_PSP_CMD_FIXB, ( b << 16 ) | ( g << 8 ) | r );
	}

	// Set dither coefficients
	// 디더링 계수 설정
	// dm00~dm03 : 디더링 행렬의 00~03을 설정합니다. 디더링 계수를 -8 ~ +7 범위에서 설정합니다.
	I3_INLINE
	UINT32 DITH1( INT32 dm00, INT32 dm01, INT32 dm02, INT32 dm03 )
	{
		return MakeCmd( I3G_PSP_CMD_DITH1, 
				( ( dm03 & 0xF ) << 12 ) | 
				( ( dm02 & 0xF ) <<  8 ) | 
				( ( dm01 & 0xF ) <<  4 ) | 
				( ( dm00 & 0xF )	   ) );
	}

	// Set dither coefficients
	// 디더링 계수 설정
	// dm04~dm07 : 디더링 행렬의 04~07을 설정합니다. 디더링 계수를 -8 ~ +7 범위에서 설정합니다.
	I3_INLINE
	UINT32 DITH2( INT32 dm04, INT32 dm05, INT32 dm06, INT32 dm07 )
	{
		return MakeCmd( I3G_PSP_CMD_DITH2, 
				( ( dm07 & 0xF ) << 12 ) | 
				( ( dm06 & 0xF ) <<  8 ) | 
				( ( dm05 & 0xF ) <<  4 ) | 
				( ( dm04 & 0xF )       ) );
	}

	// Set dither coefficients
	// 디더링 계수 설정
	// dm08~dm11 : 디더링 행렬의 08~11을 설정합니다. 디더링 계수를 -8 ~ +7 범위에서 설정합니다.
	I3_INLINE
	UINT32 DITH3( INT32 dm08, INT32 dm09, INT32 dm10, INT32 dm11 )
	{
		return MakeCmd( I3G_PSP_CMD_DITH3, 
				( ( dm11 & 0xF ) << 12 ) | 
				( ( dm10 & 0xF ) <<  8 ) | 
				( ( dm09 & 0xF ) <<  4 ) | 
				( ( dm08 & 0xF )       ) );
	}

	// Set dither coefficients
	// 디더링 계수 설정
	// dm12~dm15 : 디더링 행렬의 12~15을 설정합니다. 디더링 계수를 -8 ~ +7 범위에서 설정합니다.
	I3_INLINE
	UINT32 DITH4( INT32 dm12, INT32 dm13, INT32 dm14, INT32 dm15 )
	{
		return MakeCmd( I3G_PSP_CMD_DITH4, 
				( ( dm15 & 0xF ) << 12 ) | 
				( ( dm14 & 0xF ) <<  8 ) | 
				( ( dm13 & 0xF ) <<  4 ) | 
				( ( dm12 & 0xF )       ) );
	}

	enum LOP_OP
	{
		LOP_CLEAR			= 0,
		LOP_AND				= 1,
		LOP_AND_REVERSE		= 2,
		LOP_COPY			= 3,
		LOP_AND_INVERTED	= 4,
		LOP_NOOP			= 5,
		LOP_XOR				= 6,
		LOP_OR				= 7,
		LOP_NOR				= 8,
		LOP_EQUIV			= 9,
		LOP_INVERT			= 10,
		LOP_OR_REVERSE		= 11,
		LOP_COPY_INVERTED	= 12,
		LOP_OR_INVERTED		= 13,
		LOP_NAND			= 14,
		LOP_SET				= 15,
	};
	// Set logical operation parameter
	// 논리 연산 패러미터
	I3_INLINE
	UINT32 LOP( LOP_OP lop )
	{
		CHECK_BITS_4( lop );
		return MakeCmd( I3G_PSP_CMD_LOP, lop );
	}

	// Depth mask
	// 깊이 마스크
	// Z 버퍼의 값을 업데이트를 막을지 여부를 설정합니다.
	// ZMSK : TRUE 이면 업데이트 안함. FALSE 이면 없데이트 함.
	I3_INLINE
	UINT32 ZMSK( BOOL ZMSK )
	{
		CHECK_BITS_1( ZMSK );
		return MakeCmd( I3G_PSP_CMD_ZMSK, ZMSK );
	}

	// Set plane mask
	// 플레인 마스크
	// rmsk, gmsk, bmsk : 드로잉시의 컬러값의 마스크를 설정합니다. 1의 비트는 업데이트하지 않습니다.
	I3_INLINE
	UINT32 PMSK1( UINT8 rmsk, UINT8 gmsk, UINT8 bmsk )
	{
		return MakeCmd( I3G_PSP_CMD_PMSK1, ( bmsk << 16 ) | ( gmsk << 8 ) | rmsk );
	}

	// Set plane mask
	// 플레인 마스크
	// amsk, gmsk, bmsk : 드로잉시의 알파값의 마스크를 설정합니다. 1의 비트는 업데이트하지 않습니다.
	I3_INLINE
	UINT32 PMSK2( UINT8 amsk )
	{
		return MakeCmd( I3G_PSP_CMD_PMSK2, amsk );
	}

	enum XSTART_XPF
	{
		XSTART_XPF_16BIT	= 0, // 16 비트 컬러 포맷 (5:6:5/5:5:5:1/4:4:4:4)
		XSTART_XPF_32BIT	= 1, // 32 비트 컬러 포맷 (8:8:8:8)
	};

	// Inter-buffer transfer start
	// 버퍼간 전송 시작
	// 버퍼간 전송시의 픽셀 포맷을 설정하고 전송을 시작합니다.
	I3_INLINE
	UINT32 XSTART( XSTART_XPF xpf )
	{
		CHECK_BITS_1( xpf );

		return MakeCmd( I3G_PSP_CMD_XSTART, xpf );
	}

	// 소스 버퍼의 전송 위치
	// sx1, sy1 : 소스 버퍼의 전송영역의 좌측 상단 끝점을 설정한다.
	I3_INLINE
	UINT32 XPOS1( UINT32 sx1, UINT32 sy1 )
	{
		CHECK_BITS_10( sx1 );
		CHECK_BITS_10( sy1 );

		return MakeCmd( I3G_PSP_CMD_XPOS1, ( sy1 << 10 ) | sx1 );
	}

	// Set destination buffer transfer position
	// 대상 버퍼의 전송 위치
	// sx2, sy2 : 대상 버퍼의 전송영역의 좌측 상단 끝점을 설정한다.
	I3_INLINE
	UINT32 XPOS2( UINT32 sx2, UINT32 sy2 )
	{
		CHECK_BITS_10( sx2 );
		CHECK_BITS_10( sy2 );

		return MakeCmd( I3G_PSP_CMD_XPOS2, ( sy2 << 10 ) | sx2 );
	}

	// Set transfer size
	// 전송 크기
	// width : 전송영역의 폭 - 1
	// height : 전송영역의 높이 - 1
	I3_INLINE
	UINT32 XSIZE( UINT32 width, UINT32 height )
	{
		CHECK_BITS_10( width );
		CHECK_BITS_10( height );

		return MakeCmd( I3G_PSP_CMD_XSIZE, ( height << 10 ) | width );
	}

	// Set Screen Coordinate X
	// 정점의 스크린 좌표의 X.
	// 소수부 4비트의 부호없는 고정 소수점 수.
	// 0.0 부터 4095.9375의 범위값을 설정할 수 있습니다.
	I3_INLINE
	UINT32 X2( UINT16 fp4_x )
	{
		return MakeCmd( I3G_PSP_CMD_CMD_X2, fp4_x );
	}

	// Set Screen Coordinate Y
	// 정점의 스크린 좌표의 Y.
	// 소수부 4비트의 부호없는 고정 소수점 수.
	// 0.0 부터 4095.9375의 범위값을 설정할 수 있습니다.
	I3_INLINE
	UINT32 Y2( UINT16 fp4_y )
	{
		return MakeCmd( I3G_PSP_CMD_CMD_Y2, fp4_y );
	}

	// Set Screen Coordinate Z
	// 정점의 스크린 좌표의 Z.
	// 소수부 4비트의 부호없는 고정 소수점 수.
	// 0.0 부터 4095.9375의 범위값을 설정할 수 있습니다.
	I3_INLINE
	UINT32 Z2( UINT16 fp4_z )
	{
		return MakeCmd( I3G_PSP_CMD_CMD_Z2, fp4_z );
	}

	// Set Texture Coordinate S
	// 정점의 텍스쳐 좌표의 S.
	I3_INLINE
	UINT32 S2( REAL32 s )
	{
		return MakeCmd( I3G_PSP_CMD_CMD_S2, Upper24Bits( s ) );
	}

	// Set Texture Coordinate T
	// 정점의 텍스쳐 좌표의 T.
	I3_INLINE
	UINT32 T2( REAL32 t )
	{
		return MakeCmd( I3G_PSP_CMD_CMD_T2, Upper24Bits( t ) );
	}

	// Set Texture Coordinate Q
	// 정점의 텍스쳐 좌표의 Q.
	I3_INLINE
	UINT32 Q2( REAL32 q )
	{
		return MakeCmd( I3G_PSP_CMD_CMD_Q2, Upper24Bits( q ) );
	}

	// Set Vertex Color
	// 정점의 컬러값.
	I3_INLINE
	UINT32 RGB2( UINT8 r, UINT8 g, UINT8 b )
	{
		return MakeCmd( I3G_PSP_CMD_CMD_RGB2, ( b << 16 ) | ( g << 8 ) | r );
	}

	// Set Vertex Alpha and Primitive
	// 정점 알파 및 프리미티브
	I3_INLINE
	UINT32 AP2(
		UINT8 alpha,		// alpha : 정점의 a 컬러값.
		PRIM_TYPE prim,	// prim : 드로잉 프리미티브 설정.
							//        KEEP을 지정하면 이전의 프리미티브및 속성을 유지합니다.
							//        IIP,BCE,CIS,TME,FGE,DTE도 업데이트하지 않습니다.
		BOOL AAE,			// AAE : 안티알리어스 활성화 설정. TRUE = 활성화.
		UINT8 CFLG,			// CFLG : CLIP 플래그의 설정. 6비트 부호없는 정수.
		SHADE_MODE IIP,		// IIP : 쉐이딩 모드 설정.
		BOOL BCE,			// BCE : 백페이스 컬링 활성화 설정. TRUE = 활성화.
		CULL_MODE CIS,		// CIS : 백페이스 컬링할 경우. 삼각형의 세정점의 순서 설정.
		BOOL TME,			// TME : 텍스쳐 매핍의 활성화 설정. TRUE = 활성화
		BOOL FGE,			// FGE : 포그의 활성화 설정.
		BOOL DTE			// DTE : 디더링의 활성화 설정.
	)
	{
		CHECK_BITS_1( AAE );
		CHECK_BITS_6( CFLG );
		CHECK_BITS_1( IIP );
		CHECK_BITS_1( BCE );
		CHECK_BITS_1( CIS );
		CHECK_BITS_1( TME );
		CHECK_BITS_1( FGE );
		CHECK_BITS_1( DTE );

		return MakeCmd( I3G_PSP_CMD_CMD_AP2, 
			( DTE << 23 ) | 
			( FGE << 22 ) | 
			( TME << 21 ) |
			( CIS << 20 ) |
			( BCE << 19 ) |
			( IIP << 18 ) |
			( CFLG << 12 ) |
			( AAE << 11 ) |
			( prim << 8 ) |
			alpha );
	}

	// Set Fog
	// f : 정점의 포그 계수
	I3_INLINE
	UINT32 F2( UINT8 f )
	{
		return MakeCmd( I3G_PSP_CMD_CMD_F2, f );
	}

	// Set Secondary Color
	// 등화색
	I3_INLINE
	UINT32 I2( UINT8 r, UINT8 g, UINT8 b )
	{
		return MakeCmd( I3G_PSP_CMD_CMD_I2, ( b << 16 ) | ( g << 8 ) | r );
	}
};

#ifdef __ASSERT
#undef __ASSERT
#endif

#ifdef I3_COMPILER_VC
#pragma warning( pop )
#endif

#endif
