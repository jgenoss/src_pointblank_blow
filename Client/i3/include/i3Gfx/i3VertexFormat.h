#if !defined( __I3_VERTEX_FORMAT_H)
#define __I3_VERTEX_FORMAT_H


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
	I3_EXPORT_CLASS_DEFINE( i3VertexFormat, i3ElementBase);

protected:
	UINT32			m_Value = 0;
	UINT32			m_Types = 0;

public:
	i3VertexFormat( bool bHasPosition = true, INT8 nTextureCoordCount = 2 );
	virtual ~i3VertexFormat(void) {}

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
	bool	operator == (const i3VertexFormat & vf) const
	{
		return (m_Value == vf.m_Value) && (m_Types == vf.m_Types);
	}

	inline
	bool	operator != (const i3VertexFormat & vf) const
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


#endif
