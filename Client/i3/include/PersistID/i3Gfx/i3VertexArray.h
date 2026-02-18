#if !defined( __I3_VERTEX_ARRAY_H)
#define __I3_VERTEX_ARRAY_H

#include "i3Base.h"
#include "i3GfxType.h"
#include "i3GfxResource.h"

//////////////////////////////////////////////////////////////////////////////////////
// Vertex Format
//
// Default Values :
//		Position					true
//		Normal						false
//		Tangent						false
//		Color						false
//		Transformed					false
//		Bi-Normal					false
//		Texture Coord. Count		2
//		Texture Coord. Set Count	0
//		Blend Index Count			0
//		Blend Weight Count			0
//		Point Sprite Size Count		0
//
#define I3VF_POSITION						0x00000001
#define I3VF_NORMAL							0x00000002
#define I3VF_TANGENT						0x00000004
#define I3VF_COLOR							0x00000008
#define I3VF_TRANSFORMED					0x00000010
#define	I3VF_BINORMAL						0x00000020

#define I3VF_MASK_TEXCOORDCOUNT				0x0000000F
#define I3VF_SHIFT_TEXCOORDCOUNT			6

#define	I3VF_MASK_TEXCOORDSETCOUNT			0x0000000F
#define I3VF_SHIFT_TEXCOORDSETCOUNT			10

#define I3VF_MASK_BLENDINDEXCOUNT			0x0000000F
#define I3VF_SHIFT_BLENDINDEXCOUNT			14

#define I3VF_MASK_BLENDWEIGHTCOUNT			0x0000000F
#define I3VF_SHIFT_BLENDWEIGHTCOUNT			18

#define I3VF_MASK_POINTSPRITESIZECOUNT		0x0000000F
#define I3VF_SHIFT_POINTSPRITESIZECOUNT		22

#define	I3VF_MASK_PLATFORMCOUNT				0x0000000F
#define	I3VF_SHIFT_PLATFORMCOUNT			26

#define I3VF_PLATFORM_NA					0
#define I3VF_PLATFORM_PSP					1

// Vertex Types
//
typedef UINT32								I3VF_TYPES;

#define I3VF_TYPE_REAL32					1
#define I3VF_TYPE_FIXED32_16				2
#define I3VF_TYPE_FIXED16_8					3
#define I3VF_TYPE_FIXED16_12				4
#define I3VF_TYPE_FIXED16_15				5
#define I3VF_TYPE_FIXED8_4					6
#define I3VF_TYPE_FIXED8_7					7
#define I3VF_TYPE_COLOR32_RGBA_8888			8
#define I3VF_TYPE_COLOR16_RGBA_1555			9
#define I3VF_TYPE_COLOR16_RGBX_565			10
#define I3VF_TYPE_COLOR16_RGBA_4444			11
#define I3VF_TYPE_FIXED16_0					12
#define I3VF_TYPE_FIXED8_0					13
#define	I3VF_TYPE_FIXED16_15_U				14
#define	I3VF_TYPE_FIXED8_7_U				15

#define I3VF_COLORORDER_ARGB				0
#define I3VF_COLORORDER_RGBA				1

///
/// Vertex Component Types
///
/// 0 ~ 3	: Position Type
/// 4 ~ 7	: Normal Type
/// 8 ~ 11	: Tangent Type
/// 12 ~ 15	: Color Type
/// 16 ~ 19	: Texture Coordinates Type
/// 20 ~ 23 : Blend Weight Type
/// 24 ~ 27 : Point-Sprite Size Type
///
#define I3VF_MASK_TYPE_POSITION				0x0000000F
#define I3VF_SHIFT_TYPE_POSITION			0

#define I3VF_MASK_TYPE_NORMAL				0x000000F0
#define I3VF_SHIFT_TYPE_NORMAL				4

#define I3VF_MASK_TYPE_TANGENT				0x00000F00
#define I3VF_SHIFT_TYPE_TANGENT				8

#define I3VF_MASK_TYPE_COLOR				0x0000F000
#define I3VF_SHIFT_TYPE_COLOR				12

#define I3VF_MASK_TYPE_TEXCOORD				0x000F0000
#define I3VF_SHIFT_TYPE_TEXCOORD			16

#define I3VF_MASK_TYPE_WEIGHT				0x00F00000
#define I3VF_SHIFT_TYPE_WEIGHT				20

#define I3VF_MASK_TYPE_BINORMAL				0x0F000000
#define I3VF_SHIFT_TYPE_BINORMAL			24

#define I3VF_MASK_TYPE_PLATFORM				0xF0000000
#define I3VF_SHIFT_TYPE_PLATFORM			26

class I3_EXPORT_GFX i3VertexFormat : public i3ElementBase
{
	I3_CLASS_DEFINE( i3VertexFormat);

protected:
	UINT32			m_Value;
	UINT32			m_Types;

public:
	i3VertexFormat( bool bHasPosition = true, INT8 nTextureCoordCount = 2 );
	virtual ~i3VertexFormat(void);

	UINT32			GetFormat(void)				{ return m_Value; }
	void			SetFormat( UINT32 val)		{ m_Value = val; }

	UINT32			GetTypes(void)				{ return m_Types; }
	void			SetTypes( UINT32 type)		{ m_Types = type; }

	//////////////////////////////////////////
	// Position
	void	SetHasPosition( bool flag = true )
	{
		if(	flag)
			SETBIT( m_Value, I3VF_POSITION);
		else
			UNSETBIT( m_Value, I3VF_POSITION);
	}

	bool	GetHasPosition(void)
	{
		return TESTBIT( m_Value, I3VF_POSITION) != 0;
	}

	I3VF_TYPES	GetPositionType(void)			{ return (m_Types & I3VF_MASK_TYPE_POSITION) >> I3VF_SHIFT_TYPE_POSITION; }
	void		SetPositionType( I3VF_TYPES type)
	{
		m_Types &= ~I3VF_MASK_TYPE_POSITION;
		m_Types |= (type << I3VF_SHIFT_TYPE_POSITION);
	}

	//////////////////////////////////////////
	// Normal
	void	SetHasNormal( bool flag = true )
	{
		if( flag)
			SETBIT( m_Value, I3VF_NORMAL);
		else
			UNSETBIT( m_Value, I3VF_NORMAL);
	}

	bool	GetHasNormal(void)
	{
		return TESTBIT( m_Value, I3VF_NORMAL) != 0;
	}

	I3VF_TYPES	GetNormalType(void)			{ return (m_Types & I3VF_MASK_TYPE_NORMAL) >> I3VF_SHIFT_TYPE_NORMAL; }
	void		SetNormalType( I3VF_TYPES type)
	{
		m_Types &= ~I3VF_MASK_TYPE_NORMAL;
		m_Types |= (type << I3VF_SHIFT_TYPE_NORMAL);
	}

	//////////////////////////////////////////
	// Vertex Color
	void	SetHasColor( bool flag = true )
	{
		if( flag)
			SETBIT( m_Value, I3VF_COLOR);
		else
			UNSETBIT( m_Value, I3VF_COLOR);
	}

	bool	GetHasColor(void)
	{
		return TESTBIT( m_Value, I3VF_COLOR) != 0;
	}

	I3VF_TYPES	GetColorType(void)			{ return (m_Types & I3VF_MASK_TYPE_COLOR) >> I3VF_SHIFT_TYPE_COLOR; }
	void		SetColorType( I3VF_TYPES type)
	{
		m_Types &= ~I3VF_MASK_TYPE_COLOR;
		m_Types |= (type << I3VF_SHIFT_TYPE_COLOR);
	}

	//////////////////////////////////////////
	// Tangent
	void	SetHasTangent( bool flag = true )
	{
		if( flag)
			SETBIT( m_Value, I3VF_TANGENT);
		else
			UNSETBIT( m_Value, I3VF_TANGENT);
	}

	bool	GetHasTangent(void)
	{
		return TESTBIT( m_Value, I3VF_TANGENT) != 0;
	}

	I3VF_TYPES	GetTangentType(void)			{ return (m_Types & I3VF_MASK_TYPE_TANGENT) >> I3VF_SHIFT_TYPE_TANGENT; }
	void		SetTangentType( I3VF_TYPES type)
	{
		m_Types &= ~I3VF_MASK_TYPE_TANGENT;
		m_Types |= (type << I3VF_SHIFT_TYPE_TANGENT);
	}

	//////////////////////////////////////////
	// Binormal
	void	SetHasBinormal( bool flag = true )
	{
		if( flag)
			SETBIT( m_Value, I3VF_BINORMAL);
		else
			UNSETBIT( m_Value, I3VF_BINORMAL);
	}

	bool	GetHasBinormal(void)
	{
		return TESTBIT( m_Value, I3VF_BINORMAL) != 0;
	}

	I3VF_TYPES	GetBinormalType(void)			{ return (m_Types & I3VF_MASK_TYPE_BINORMAL) >> I3VF_SHIFT_TYPE_BINORMAL; }
	void		SetBinormalType( I3VF_TYPES type)
	{
		m_Types &= ~I3VF_MASK_TYPE_BINORMAL;
		m_Types |= (type << I3VF_SHIFT_TYPE_BINORMAL);
	}

	//////////////////////////////////////////
	// Transformed
	void	SetIsTransformed( bool flag = true )
	{
		if( flag)
			SETBIT( m_Value, I3VF_TRANSFORMED);
		else
			UNSETBIT( m_Value, I3VF_TRANSFORMED);
	}

	bool	IsTransformed(void)
	{
		return TESTBIT( m_Value, I3VF_TRANSFORMED) != 0;
	}


	//////////////////////////////////////////
	// Texture Coordinates Count
	void	SetTextureCoordCount( INT8 count)
	{
		UNSETBIT( m_Value, (I3VF_MASK_TEXCOORDCOUNT << I3VF_SHIFT_TEXCOORDCOUNT));
		
		SETBIT( m_Value, ((count & I3VF_MASK_TEXCOORDCOUNT) << I3VF_SHIFT_TEXCOORDCOUNT));
	}

	INT8	GetTextureCoordCount(void)
	{
		return (INT8)((m_Value >> I3VF_SHIFT_TEXCOORDCOUNT) & I3VF_MASK_TEXCOORDCOUNT);
	}

	I3VF_TYPES	GetTextureCoordType(void)			{ return (m_Types & I3VF_MASK_TYPE_TEXCOORD) >> I3VF_SHIFT_TYPE_TEXCOORD; }
	void		SetTextureCoordType( I3VF_TYPES type)
	{
		m_Types &= ~I3VF_MASK_TYPE_TEXCOORD;
		m_Types |= (type << I3VF_SHIFT_TYPE_TEXCOORD);
	}

	//////////////////////////////////////////
	// Texture Coordinates Set Count
	void	SetTextureCoordSetCount( INT8 count)
	{
		UNSETBIT( m_Value, (I3VF_MASK_TEXCOORDSETCOUNT << I3VF_SHIFT_TEXCOORDSETCOUNT));

		SETBIT( m_Value, ((count & I3VF_MASK_TEXCOORDSETCOUNT) << I3VF_SHIFT_TEXCOORDSETCOUNT));
	}

	INT8	GetTextureCoordSetCount(void)
	{
		return (INT8)((m_Value >> I3VF_SHIFT_TEXCOORDSETCOUNT) & I3VF_MASK_TEXCOORDSETCOUNT);
	}

	//////////////////////////////////////////
	// Blend Index Count
	void	SetBlendIndexCount( INT8 count)
	{
		UNSETBIT( m_Value, (I3VF_MASK_BLENDINDEXCOUNT << I3VF_SHIFT_BLENDINDEXCOUNT));
		
		SETBIT( m_Value, ((count & I3VF_MASK_BLENDINDEXCOUNT) << I3VF_SHIFT_BLENDINDEXCOUNT));
	}

	INT8	GetBlendIndexCount(void)
	{
		return (INT8)((m_Value >> I3VF_SHIFT_BLENDINDEXCOUNT) & I3VF_MASK_BLENDINDEXCOUNT);
	}

	//////////////////////////////////////////
	// Blend Weight Count
	void	SetBlendWeightCount( INT8 count)
	{
		UNSETBIT( m_Value, (I3VF_MASK_BLENDWEIGHTCOUNT << I3VF_SHIFT_BLENDWEIGHTCOUNT));
		
		SETBIT( m_Value, ((count & I3VF_MASK_BLENDWEIGHTCOUNT) << I3VF_SHIFT_BLENDWEIGHTCOUNT));
	}

	INT8	GetBlendWeightCount(void)
	{
		return (INT8)((m_Value >> I3VF_SHIFT_BLENDWEIGHTCOUNT) & I3VF_MASK_BLENDWEIGHTCOUNT);
	}

	I3VF_TYPES	GetBlendWeightType(void)			{ return (m_Types & I3VF_MASK_TYPE_WEIGHT) >> I3VF_SHIFT_TYPE_WEIGHT; }
	void		SetBlendWeightType( I3VF_TYPES type)
	{
		m_Types &= ~I3VF_MASK_TYPE_WEIGHT;
		m_Types |= (type << I3VF_SHIFT_TYPE_WEIGHT);
	}

	// Platform
	void	SetPlatform( UINT32 platform)
	{
		UNSETBIT( m_Value, (I3VF_MASK_PLATFORMCOUNT << I3VF_SHIFT_PLATFORMCOUNT));
		
		SETBIT( m_Value, ((platform & I3VF_MASK_PLATFORMCOUNT) << I3VF_SHIFT_PLATFORMCOUNT));
	}

	UINT32	GetPlatform(void)
	{
		return ((m_Value >> I3VF_SHIFT_PLATFORMCOUNT) & I3VF_MASK_PLATFORMCOUNT);
	}


	inline
	BOOL	operator == (const i3VertexFormat & vf) const
	{
		return (m_Value == vf.m_Value) && (m_Types == vf.m_Types);
	}

	inline
	BOOL	operator != (const i3VertexFormat & vf) const
	{
		return (m_Value != vf.m_Value) || (m_Types != vf.m_Types);
	}

	inline
	i3VertexFormat & operator = (const i3VertexFormat & vf)
	{
		m_Value = vf.m_Value;
		m_Types = vf.m_Types;

		return *this;
	}
};

typedef struct _tagi3PersistVertexOffsetInfo
{
	i3VertexFormat	m_Format;
	UINT32	m_Stride;
	UINT32	m_OffsetPos;
	UINT32	m_SizePos;
	UINT32	m_OffsetWeight;
	UINT32	m_SizeWeight;
	UINT32	m_OffsetNormal;
	UINT32	m_SizeNormal;
	UINT32	m_OffsetColor;
	UINT32	m_SizeColor;
	UINT32	m_OffsetTexCoord[8];
	UINT32	m_SizeTexCoord;
	UINT32	m_OffsetBlendIndex;
	UINT32	m_SizeBlendIndex;
	UINT32	m_OffsetTangent;
	UINT32	m_SizeTangent;
	UINT32	m_OffsetBinormal;
	UINT32	m_SizeBinormal;
} I3_PERSIST_VERTEX_OFFSET;

#if defined( I3G_OGLES)
#define VIRTUAL			virtual
#else
#define VIRTUAL
#endif

class I3_EXPORT_GFX i3VertexArray : public i3GfxResource
{
	I3_ABSTRACT_CLASS_DEFINE( i3VertexArray);

protected:
	/// Persistent Members
	i3VertexFormat			m_Format;
	i3VertexFormat			m_PersistFormat;
	UINT32					m_Count;
	VEC3D					m_Center;
	REAL32					m_Rad;

	/// Volatile Members
	UINT32					m_ColorOrder;			/// 각 Platform에 해당하는 i3VertexArray들은 자신이 바라는 Color Order를 설정해야한다.
	UINT32					m_Stride;

	UINT32					m_OffsetPos;
	UINT32					m_OffsetRHW;
	UINT32					m_OffsetWeight;
	UINT32					m_OffsetNormal;
	UINT32					m_OffsetColor;
	UINT32					m_OffsetTexCoord[8];
	UINT32					m_OffsetBlendIndex;
	UINT32					m_OffsetTangent;
	UINT32					m_OffsetBinormal;

	BOOL					m_bLocked;
	INT8	*				m_pLockedBuf;
	UINT32					m_LockStartIdx;

	BOOL					m_bModified;

	UINT32					m_VertexFormatCode;
	void *					m_pBuffer;
	

	char *					m_pPersistData;

	static INT32			m_ComponentSize[16];

	inline RT_REAL32	_REAL_F2R( I3VF_TYPES type, INT8 * pSrc)
	{
		#if defined( I3_FIXED_POINT)
		switch( type)
		{			
			case  I3VF_TYPE_REAL32		:	return i3FXD::FromReal( *((REAL32 *) pSrc));
			case  I3VF_TYPE_FIXED16_0   :	return *((INT16 *)pSrc) << 16;										//
			case  I3VF_TYPE_FIXED16_8	:	return *((INT16 *)pSrc) << 8;			// 1 / 255
			case  I3VF_TYPE_FIXED16_12	:	return *((INT16 *)pSrc) << 4;		// 1 / 4095
			case  I3VF_TYPE_FIXED16_15	:	return *((INT16 *)pSrc) << 1;		// 1 / 32767
			case  I3VF_TYPE_FIXED16_15_U :	return (*((UINT16 *)pSrc) & 0x7FFF) << 1;		// 1 / 65535
			case  I3VF_TYPE_FIXED8_0   :	return *pSrc << 16;
			case  I3VF_TYPE_FIXED8_4	:	return *((INT8 *)pSrc) << 12;			// 1 / 15
			case  I3VF_TYPE_FIXED8_7	:	return *((INT8 *)pSrc) << 9;			// 1 / 127
			case  I3VF_TYPE_FIXED8_7_U	:	return (*((INT8 *)pSrc) & 0x7F) << 9;			// 1 / 255
		}
		#else
		switch( type)
		{			
			case  I3VF_TYPE_FIXED16_0   :	return (REAL32) *((INT16 *)pSrc);										//
			case  I3VF_TYPE_FIXED32_16	:	return (*((FIXED32 *)pSrc) - 0.5f) * 0.0000152587890625f;							// 1 / 65536
			case  I3VF_TYPE_FIXED16_8	:	return *((INT16 *)pSrc) * 0.003921568627450980392156862745098f;			// 1 / 255
			case  I3VF_TYPE_FIXED16_12	:	return *((INT16 *)pSrc) * 2.4420024420024420024420024420024e-4f;		// 1 / 4095
			case  I3VF_TYPE_FIXED16_15	:	return *((INT16 *)pSrc) * 3.0518509475997192297128208258309e-5f;		// 1 / 32767
			case  I3VF_TYPE_FIXED16_15_U :	return *((UINT16 *)pSrc) * 3.0518509475997192297128208258309e-5f;		// 1 / 65535
			case  I3VF_TYPE_FIXED8_0   :	return (REAL32)*pSrc;													// 
			case  I3VF_TYPE_FIXED8_4	:	return *((INT8 *)pSrc) * 0.066666666666666666666666666666667f;			// 1 / 15
			case  I3VF_TYPE_FIXED8_7	:	return *((INT8 *)pSrc) * 0.007874015748031496062992125984252f;			// 1 / 127
			case  I3VF_TYPE_FIXED8_7_U	:	return *((INT8 *)pSrc) * 0.003921568627450980392156862745098f;			// 1 / 255
		}
		#endif

		return *(RT_REAL32 *)pSrc;
	}

	inline void	_REAL_R2F( I3VF_TYPES type, INT8 * pDest, RT_REAL32 val)
	{
		#if defined( I3_FIXED_POINT)
		switch( type)
		{			
			case  I3VF_TYPE_FIXED16_0	:		*((INT16 *)pDest) = (INT16)((val >> 16) & 0xFFFF);	break;
			case  I3VF_TYPE_REAL32	:			*((REAL32 *)pDest) = i3FXD::ToReal( val);			break;
			case  I3VF_TYPE_FIXED16_8	:		*((INT16 *)pDest) = (INT16)(val >> 8);				break;
			case  I3VF_TYPE_FIXED16_12	:		*((INT16 *)pDest) = (INT16)(val >> 4);				break;
			case  I3VF_TYPE_FIXED16_15	:		*((INT16 *)pDest) = (INT16)(val >> 1);			break;
			case  I3VF_TYPE_FIXED16_15_U :		*((UINT16 *)pDest) = (UINT16)(val >> 1);		break;
			case  I3VF_TYPE_FIXED8_0	:		*(pDest) = (INT8)(val >> 16);						break;		
			case  I3VF_TYPE_FIXED8_4	:		*pDest = (INT8)(val >> 12);					break;
			case  I3VF_TYPE_FIXED8_7	:		*pDest = (INT8)(val >> 9);					break;
			case  I3VF_TYPE_FIXED8_7_U	:		*pDest = (INT8)(val >> 9);					break;
			default :						*((FIXED32 *) pDest) = val;						break;
		}
		#else
		switch( type)
		{			
			case  I3VF_TYPE_FIXED16_0	:		*((INT16 *)pDest) = (INT16)val;					break;
			case  I3VF_TYPE_FIXED32_16	:		*((FIXED32 *)pDest) = (FIXED32)((val * 65536.0f) + 0.5f);	break;
			case  I3VF_TYPE_FIXED16_8	:		*((INT16 *)pDest) = (INT16)(val * 255.0f);		break;
			case  I3VF_TYPE_FIXED16_12	:		*((INT16 *)pDest) = (INT16)(val * 4095.0f);		break;
			case  I3VF_TYPE_FIXED16_15	:		*((INT16 *)pDest) = (INT16)(val * 32767.0f);	break;
			case  I3VF_TYPE_FIXED16_15_U :		*((UINT16 *)pDest) = (UINT16)(val * 32767.0f);	break;
			case  I3VF_TYPE_FIXED8_0	:		*(pDest) = (INT8)val;							break;		
			case  I3VF_TYPE_FIXED8_4	:		*pDest = (INT8)(val * 15.0f);					break;
			case  I3VF_TYPE_FIXED8_7	:		*pDest = (INT8)(val * 127.0f);					break;
			case  I3VF_TYPE_FIXED8_7_U	:		*pDest = (INT8)(val * 255.0f);					break;
			default :						*((REAL32 *) pDest) = val;						break;
		}
		#endif
	}

	inline UINT32	_COLOR_F2R( I3VF_TYPES type, INT8 * pSrc)
	{
		UINT32 r, g, b, a;

		switch( type)
		{
			case I3VF_TYPE_COLOR16_RGBA_1555	:
				{
					UINT16 * pCol = (UINT16 *) pSrc;

					r = ((*pCol >> 0) & 0x1F) << 3;
					g = ((*pCol >> 5) & 0x1F) << 3;
					b = ((*pCol >> 10) & 0x1F) << 3;
					if( *pCol & 0x8000)
						a = 255;
					else
						a = 0;
				}
				break;

			case I3VF_TYPE_COLOR16_RGBX_565		:
				{
					UINT16 * pCol = (UINT16 *) pSrc;

					r = ((*pCol >> 0) & 0x1F) << 3;
					g = ((*pCol >> 5) * 0x3F) << 2;
					b = ((*pCol >> 11) & 0x1F) << 3;
					a = 255;
				}
				break;

			case I3VF_TYPE_COLOR16_RGBA_4444	:
				{
					UINT16 * pCol = (UINT16 *) pSrc;

					r = ((*pCol >> 0) & 0x0F) << 4;
					g = ((*pCol >> 4) & 0x0F) << 4;
					b = ((*pCol >> 8) & 0x0F) << 4;
					a = ((*pCol >> 12) & 0x0F) << 4;
				}
				break;

			//case I3VF_TYPE_COLOR32_RGBA_8888	:
			default :
				r = *((UINT8 *) pSrc);
				g = *((UINT8 *) pSrc + 1);
				b = *((UINT8 *) pSrc + 2);
				a = *((UINT8 *) pSrc + 3);
				break;
		}

		return (a << 24) | (b << 16) | (g << 8) | r;
	}

	inline void		_COLOR_R2F( I3VF_TYPES type, INT8 * pDest, UINT32 c)
	{
		UINT32 r, g, b, a;

		a = (c >> 24) & 0xFF;
		b = (c >> 16) & 0xFF;
		g = (c >> 8) & 0xFF;
		r = c & 0xFF;

		switch( type)
		{
			case I3VF_TYPE_COLOR16_RGBA_1555	:
				{
					UINT16 * pCol = (UINT16 *) pDest;

					*pCol = (UINT16)((r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10) | ((a >> 7) << 15));
				}
				break;

			case I3VF_TYPE_COLOR16_RGBX_565		:
				{
					UINT16 * pCol = (UINT16 *) pDest;

					*pCol = (UINT16)((r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11));
				}
				break;

			case I3VF_TYPE_COLOR16_RGBA_4444	:
				{
					UINT16 * pCol = (UINT16 *) pDest;

					*pCol = (UINT16)((r >> 4) | ((g >> 4) << 4) | ((b >> 4) << 8) | (((a >> 4)) << 12));
				}
				break;

			//case I3VF_TYPE_COLOR32_RGBA_8888	:
			default :
				*((UINT32 *)pDest) = c;
				break;
		}
	}

	UINT32				OnLoad10( i3ResourceFile * pResFile);
	UINT32				OnLoad20( i3ResourceFile * pResFile);
	UINT32				OnLoad25( i3ResourceFile * pResFile);
	UINT32				OnLoad30( i3ResourceFile * pResFile);
public:
	i3VertexArray(void);
	virtual ~i3VertexArray(void);

	void *	GetInstance(void)							{ return m_pBuffer; }
	UINT32	GetVertexFormatCode(void)					{ return m_VertexFormatCode; }

	VEC3D *			getLocalCenter(void)		{ return & m_Center; }
	
	REAL32			getLocalRadius(void)		{ return m_Rad; }


	bool				isValidBoundSphere(void)		{ return (m_Rad >= 0.0f); }
	VEC3D *				getBoundSphereCenter(void)		{ return &m_Center; }
	void				setBoundSphereCenter( VEC3D * pCenter)		{ i3Vector::Copy( &m_Center, pCenter); }
	REAL32				getBoundSphereRadius(void)		{ return m_Rad; }
	void				setBoundSphereRadius( REAL32 val)	{ m_Rad = val; }

	i3VertexFormat *	GetFormat(void)					{ return &m_Format; }

	i3VertexFormat *	GetPersistFormat(void)			{ return &m_PersistFormat; }
	void				SetPersistFormat( i3VertexFormat * pFmt)	{ m_PersistFormat = *pFmt; }

	char *				GetPersistData(void)			{ return m_pPersistData; }
	void				SetPersistData( char * pBuf ); 
	UINT32				GetCount(void)					{ return m_Count; }
	UINT32				GetStride(void)					{ return m_Stride; }

	void				SetModifiedFlag( BOOL bFlag)	{ m_bModified = bFlag; }

	virtual UINT32		CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count) = 0;
	UINT32				CalcPersistantBufferSizePSP( I3_PERSIST_VERTEX_OFFSET * pInfo, UINT32 count);
	UINT32				CalcPersistantBufferSize20( I3_PERSIST_VERTEX_OFFSET * pInfo, UINT32 count);
	UINT32				CalcPersistantBufferSize25( I3_PERSIST_VERTEX_OFFSET * pInfo, UINT32 count);	

	INT8 *				GetLockedBuffer(void)			{ return m_pLockedBuf; }
	BOOL	IsLocked(void)						{ return m_bLocked; }


	virtual BOOL	Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE UsageFlags);
	virtual BOOL	Lock( I3G_LOCKMETHOD lock = I3G_LOCK_NONE, UINT32 StartIdx = 0, UINT32 Count = 0) = 0;
	virtual void	Unlock(void) = 0;

	// Position
	bool	HasPosition(void)					{ return m_Format.GetHasPosition(); }
	VIRTUAL	void	SetPosition( UINT32	Idx, RT_VEC3D * pVec);
	VIRTUAL	void	GetPosition( UINT32 Idx, RT_VEC3D * pVec);
	VIRTUAL	void	GetPositions( UINT32 Idx, INT32 count, RT_VEC3D * pVec);
	VIRTUAL	void	SetPositions( UINT32 StartIdx, UINT32 Count, RT_VEC3D * pPos, UINT32 pitch = sizeof(RT_VEC3D));
	VIRTUAL BOOL	IsSamePosition( UINT32 Idx, RT_VEC3D * pVec);

	// Vertex Color
	bool	HasColor(void)						{ return m_Format.GetHasColor(); }
	VIRTUAL	void	SetColor( UINT32 Idx, COLORREAL * pColor);
	VIRTUAL	void	GetColor( UINT32 Idx, COLORREAL * pColor);
	VIRTUAL	void	SetColors( UINT32 StartIdx, UINT32 Count, COLOR * pColor, UINT32 pitch = sizeof(COLOR));
	VIRTUAL	void	SetColor( UINT32 Idx, UINT32 Count, COLOR * pColor);
	VIRTUAL void	SetColor( UINT32 idx, UINT32 cnt, UINT32 col);
	VIRTUAL BOOL	IsSameColor( UINT32 Idx, COLORREAL * pColor);

	// Normal
	bool	HasNormal(void)						{ return m_Format.GetHasNormal(); }
	VIRTUAL	void	SetNormal( UINT32 Idx, RT_VEC3D * pVec);
	VIRTUAL	void	GetNormal( UINT32 Idx, RT_VEC3D * pVec);
	VIRTUAL	void	SetNormals( UINT32 StartIdx, UINT32 Count, RT_VEC3D * pNormal, UINT32 pitch = sizeof(RT_VEC3D));
	VIRTUAL BOOL	IsSameNormal( UINT32 Idx, RT_VEC3D * pVec);

	// Tangent
	bool	HasTangent(void)						{ return m_Format.GetHasTangent(); }
	VIRTUAL	void	SetTangent( UINT32 Idx, RT_VEC3D * pVec);
	VIRTUAL	void	GetTangent( UINT32 Idx, RT_VEC3D * pVec);
	VIRTUAL	void	SetTangents( UINT32 StartIdx, UINT32 Count, RT_VEC3D * pTangent, UINT32 pitch = sizeof(RT_VEC3D));
	VIRTUAL BOOL	IsSameTangent( UINT32 Idx, RT_VEC3D * pVec);

	// Binormal
	bool	HasBinormal(void)						{ return m_Format.GetHasBinormal(); }
	VIRTUAL	void	SetBinormal( UINT32 Idx, RT_VEC3D * pVec);
	VIRTUAL	void	GetBinormal( UINT32 Idx, RT_VEC3D * pVec);
	VIRTUAL	void	SetBinormals( UINT32 StartIdx, UINT32 Count, RT_VEC3D * pBinormals, UINT32 pitch = sizeof(RT_VEC3D));
	VIRTUAL BOOL	IsSameBinormal( UINT32 Idx, RT_VEC3D * pVec);

	// Texture Coords.
	INT32	GetTextureCoordCount(void)			{ return m_Format.GetTextureCoordCount(); }
	VIRTUAL	void	SetTextureCoord( UINT32 which, UINT32 Idx, RT_VEC2D * pVec);
	VIRTUAL	void	GetTextureCoord( UINT32 which, UINT32 Idx, RT_VEC2D * pVec);
	VIRTUAL	void	SetTextureCoords( UINT32 which, UINT32 StartIdx, UINT32 Count, RT_VEC2D * pUV, UINT32 pitch = sizeof(RT_VEC2D));
	VIRTUAL BOOL	IsSameTextureCoord( UINT32 which, UINT32 Idx, RT_VEC2D * pVec);

	// Blend Index
	INT32	GetBlendIndexCount(void)			{ return m_Format.GetBlendIndexCount(); }
	VIRTUAL	void	SetBlendIndex( UINT32 which, UINT32 Idx, UINT32 BlendIndex);
	VIRTUAL	UINT32	GetBlendIndex( UINT32 which, UINT32 Idx);
	VIRTUAL	void	SetBlendIndices( UINT32 which, UINT32 StartIdx, UINT32 Count, UINT8 * pIndices, UINT32 pitch = 1);
	VIRTUAL BOOL	IsSameBlendIndex( UINT32 which, UINT32 Idx, UINT32 BlendIndex);

	// Blend Weight
	INT32	GetBlendWeightCount(void)			{ return m_Format.GetBlendWeightCount(); }
	VIRTUAL	void	SetBlendWeight( UINT32 which, UINT32 Idx, RT_REAL32 weight);
	VIRTUAL	RT_REAL32	GetBlendWeight( UINT32 which, UINT32 Idx);
	VIRTUAL	void	SetBlendWeights( UINT32 which, UINT32 StartIdx, UINT32 Count, RT_REAL32 * pWeights, UINT32 pitch = sizeof(RT_REAL32));
	VIRTUAL BOOL	IsSameBlendWeight( UINT32 which, UINT32 Idx, RT_REAL32 weight);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	
	virtual void	OnPreSave( INT8 * pBuffer);
	virtual void	OnPostSave( INT8 * pBuffer);

	virtual void	OnPreLoad( INT8 * pBuffer, I3_PERSIST_VERTEX_OFFSET * pInfo);
	virtual void	OnPostLoad( INT8 * pBuffer, I3_PERSIST_VERTEX_OFFSET * pInfo);

	virtual void	MakeRuntimeFormat( I3_PERSIST_VERTEX_OFFSET * pInfo);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	void	Transform( RT_MATRIX * pMatrix, i3VertexArray * pDest);

	static i3VertexArray * CreateGrid( VEC3D * pCenter, REAL32 xSize, REAL32 zSize, REAL32 xGrid, REAL32 zGrid);
	static i3VertexArray *	CreatePointSprite( UINT32 pointCount, UINT32 SizeCount, BOOL bColor, BOOL bUseUV);
	static i3VertexArray * CreateRect( REAL32 w, REAL32 h, BOOL bUV = TRUE, BOOL bColor = FALSE, 
							COLORREAL * pColor1 = NULL,
							COLORREAL * pColor2 = NULL,
							COLORREAL * pColor3 = NULL,
							COLORREAL * pColor4 = NULL);
	static i3VertexArray *	CreateBox( VEC3D * pMin, VEC3D * pMax, BOOL bColor, BOOL bUV, COLORREAL * pColor = NULL);

	void			CopyVertexFrom( INT32 idxDest, i3VertexArray * pSrc, INT32 idxSrc);
	void			InterpolateVertex( INT32 idxDest, i3VertexArray * pSrc, INT32 idxStart, INT32 idxEnd, REAL32 t);

#ifdef I3_DEBUG
	void Dump( INT32 nCount = -1 );
#endif
};

#endif
