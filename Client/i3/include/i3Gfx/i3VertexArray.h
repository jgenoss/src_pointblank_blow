#if !defined( __I3_VERTEX_ARRAY_H)
#define __I3_VERTEX_ARRAY_H

#include "i3Base.h"
#include "i3GfxType.h"
#include "i3GfxResource.h"
#include "i3VertexFormat.h"

namespace i3
{
	namespace pack
	{
		struct VERTEX_OFFSET
		{
			i3VertexFormat	m_Format;
			UINT32	m_Stride = 0;
			UINT32	m_OffsetPos = 0;
			UINT32	m_SizePos = 0;
			UINT32	m_OffsetWeight = 0;
			UINT32	m_SizeWeight = 0;
			UINT32	m_OffsetNormal = 0;
			UINT32	m_SizeNormal = 0;
			UINT32	m_OffsetColor = 0;
			UINT32	m_SizeColor = 0;
			UINT32	m_OffsetTexCoord[8] = { 0 };
			UINT32	m_SizeTexCoord = 0;
			UINT32	m_OffsetBlendIndex = 0;
			UINT32	m_SizeBlendIndex = 0;
			UINT32	m_OffsetTangent = 0;
			UINT32	m_SizeTangent = 0;
			UINT32	m_OffsetBinormal = 0;
			UINT32	m_SizeBinormal = 0;
		};
	}
}

//#define		VTX_STREAM

#if defined( I3G_OGLES) || defined( VTX_STREAM)
#define VIRTUAL			virtual
#else
#define VIRTUAL
#endif

class I3_EXPORT_GFX i3VertexArray : public i3GfxResource
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3VertexArray, i3GfxResource);

protected:
	/// Persistent Members
	i3VertexFormat			m_Format;
	i3VertexFormat			m_PersistFormat;
	UINT32					m_Count = 0;
	VEC3D					m_Center;
	REAL32					m_Rad = -1.0f;
	UINT32					m_State = 0;

	/// Volatile Members
	UINT32					m_ColorOrder = I3VF_COLORORDER_ARGB; // 각 Platform에 해당하는 i3VertexArray들은 자신이 바라는 Color Order를 설정해야한다.
	UINT32					m_Stride = 0;

	UINT32					m_OffsetPos = 0;
	UINT32					m_OffsetRHW = 0;
	UINT32					m_OffsetWeight = 0;
	UINT32					m_OffsetNormal = 0;
	UINT32					m_OffsetColor = 0;
	UINT32					m_OffsetTexCoord[8] = { 0 };
	UINT32					m_OffsetBlendIndex = 0;
	UINT32					m_OffsetTangent = 0;
	UINT32					m_OffsetBinormal = 0;
	
	UINT32					m_LockStartIdx = 0;
	UINT32					m_LockCount = 0;
	bool					m_bLocked = 0;

	INT32					m_iLocked = 0;

	bool					m_bModified = false;

	UINT32					m_VertexFormatCode = 0;
	void *					m_pBuffer = nullptr;
	

	char *					m_pPersistData = nullptr;

	static INT32			m_ComponentSize[16];

	inline REAL32	_REAL_F2R( I3VF_TYPES type, INT8 * pSrc)
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

		return *(REAL32 *)pSrc;
	}

	inline void	_REAL_R2F( I3VF_TYPES type, INT8 * pDest, REAL32 val)
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
	INT8	*				m_pLockedBuf = nullptr;

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
	void				ForceCount( UINT32 cnt)			{ m_Count = cnt; }
	UINT32				GetStride(void)					{ return m_Stride; }

	void				SetModifiedFlag( bool bFlag)	{ m_bModified = bFlag; }

	virtual UINT32		CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count) = 0;
	UINT32				CalcPersistantBufferSizePSP( i3::pack::VERTEX_OFFSET * pInfo, UINT32 count);
	UINT32				CalcPersistantBufferSize20( i3::pack::VERTEX_OFFSET * pInfo, UINT32 count);
	UINT32				CalcPersistantBufferSize25( i3::pack::VERTEX_OFFSET * pInfo, UINT32 count);	

	INT8 *				GetLockedBuffer(void)			{ return m_pLockedBuf; }
	bool				IsLocked(void)					{ return m_bLocked; }


	virtual bool	Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE UsageFlags);
	virtual bool	Lock( I3G_LOCKMETHOD lock = I3G_LOCK_NONE, UINT32 StartIdx = 0, UINT32 Count = 0) = 0;
	virtual void	Unlock(void) = 0;

	virtual bool	SafeLock( I3G_LOCKMETHOD lock = I3G_LOCK_NONE, UINT32 StartIdx = 0, UINT32 Count = 0) = 0;
	virtual bool	SafeUnlock(void) = 0;

	// Position
	bool	HasPosition(void)					{ return m_Format.GetHasPosition(); }
	VIRTUAL	void	SetPosition( UINT32	Idx, VEC3D * pVec);
	VIRTUAL	void	GetPosition( UINT32 Idx, VEC3D * pVec);
	VIRTUAL	void	GetPositions( UINT32 Idx, INT32 count, VEC3D * pVec);
	VIRTUAL	void	SetPositions( UINT32 StartIdx, UINT32 Count, VEC3D * pPos, UINT32 pitch = sizeof(VEC3D));
	VIRTUAL bool	IsSamePosition( UINT32 Idx, VEC3D * pVec);

	// Vertex Color
	bool	HasColor(void)						{ return m_Format.GetHasColor(); }
	VIRTUAL	void	SetColor( UINT32 Idx, COLORREAL * pColor);
	VIRTUAL	void	GetColor( UINT32 Idx, COLORREAL * pColor);
	VIRTUAL	void	SetColors( UINT32 StartIdx, UINT32 Count, COLOR * pColor, UINT32 pitch = sizeof(COLOR));
	VIRTUAL	void	SetColor( UINT32 Idx, UINT32 Count, COLOR * pColor);
	VIRTUAL void	SetColor( UINT32 idx, UINT32 cnt, UINT32 col);
	VIRTUAL bool	IsSameColor( UINT32 Idx, COLORREAL * pColor);

	// Normal
	bool	HasNormal(void)						{ return m_Format.GetHasNormal(); }
	VIRTUAL	void	SetNormal( UINT32 Idx, VEC3D * pVec);
	VIRTUAL	void	GetNormal( UINT32 Idx, VEC3D * pVec);
	VIRTUAL	void	SetNormals( UINT32 StartIdx, UINT32 Count, VEC3D * pNormal, UINT32 pitch = sizeof(VEC3D));
	VIRTUAL bool	IsSameNormal( UINT32 Idx, VEC3D * pVec);

	// Tangent
	bool	HasTangent(void)						{ return m_Format.GetHasTangent(); }
	VIRTUAL	void	SetTangent( UINT32 Idx, VEC3D * pVec);
	VIRTUAL	void	GetTangent( UINT32 Idx, VEC3D * pVec);
	VIRTUAL	void	SetTangents( UINT32 StartIdx, UINT32 Count, VEC3D * pTangent, UINT32 pitch = sizeof(VEC3D));
	VIRTUAL bool	IsSameTangent( UINT32 Idx, VEC3D * pVec);

	// Binormal
	bool	HasBinormal(void)						{ return m_Format.GetHasBinormal(); }
	VIRTUAL	void	SetBinormal( UINT32 Idx, VEC3D * pVec);
	VIRTUAL	void	GetBinormal( UINT32 Idx, VEC3D * pVec);
	VIRTUAL	void	SetBinormals( UINT32 StartIdx, UINT32 Count, VEC3D * pBinormals, UINT32 pitch = sizeof(VEC3D));
	VIRTUAL bool	IsSameBinormal( UINT32 Idx, VEC3D * pVec);

	// Texture Coords.
	INT32	GetTextureCoordCount(void)			{ return m_Format.GetTextureCoordCount(); }
	VIRTUAL	void	SetTextureCoord( UINT32 which, UINT32 Idx, VEC2D * pVec);
	VIRTUAL	void	GetTextureCoord( UINT32 which, UINT32 Idx, VEC2D * pVec);
	VIRTUAL	void	SetTextureCoords( UINT32 which, UINT32 StartIdx, UINT32 Count, VEC2D * pUV, UINT32 pitch = sizeof(VEC2D));
	VIRTUAL bool	IsSameTextureCoord( UINT32 which, UINT32 Idx, VEC2D * pVec);

	void *			GetTextureCoordPointer( UINT32 which, UINT32 idx);

	// Blend Index
	INT32	GetBlendIndexCount(void)			{ return m_Format.GetBlendIndexCount(); }
	VIRTUAL	void	SetBlendIndex( UINT32 which, UINT32 Idx, UINT32 BlendIndex);
	VIRTUAL	UINT32	GetBlendIndex( UINT32 which, UINT32 Idx);
	VIRTUAL	void	SetBlendIndices( UINT32 which, UINT32 StartIdx, UINT32 Count, UINT8 * pIndices, UINT32 pitch = 1);
	VIRTUAL bool	IsSameBlendIndex( UINT32 which, UINT32 Idx, UINT32 BlendIndex);

	// Blend Weight
	INT32	GetBlendWeightCount(void)			{ return m_Format.GetBlendWeightCount(); }
	VIRTUAL	void	SetBlendWeight( UINT32 which, UINT32 Idx, REAL32 weight);
	VIRTUAL	REAL32	GetBlendWeight( UINT32 which, UINT32 Idx);
	VIRTUAL	void	SetBlendWeights( UINT32 which, UINT32 StartIdx, UINT32 Count, REAL32 * pWeights, UINT32 pitch = sizeof(REAL32));
	VIRTUAL bool	IsSameBlendWeight( UINT32 which, UINT32 Idx, REAL32 weight);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	
	virtual void	OnPreSave( INT8 * pBuffer);
	virtual void	OnPostSave( INT8 * pBuffer);

	virtual void	OnPreLoad( INT8 * pBuffer, i3::pack::VERTEX_OFFSET * pInfo);
	virtual void	OnPostLoad( INT8 * pBuffer, i3::pack::VERTEX_OFFSET * pInfo);

	virtual void	MakeRuntimeFormat( i3::pack::VERTEX_OFFSET * pInfo);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	void	Transform( MATRIX * pMatrix, i3VertexArray * pDest);

	static i3VertexArray * CreateGrid( VEC3D * pCenter, REAL32 xSize, REAL32 zSize, REAL32 xGrid, REAL32 zGrid);
	static i3VertexArray *	CreatePointSprite( UINT32 pointCount, UINT32 SizeCount, bool bColor, bool bUseUV);
	static i3VertexArray * CreateRect( REAL32 w, REAL32 h, bool bUV = true, bool bColor = false,
							COLORREAL * pColor1 = nullptr,
							COLORREAL * pColor2 = nullptr,
							COLORREAL * pColor3 = nullptr,
							COLORREAL * pColor4 = nullptr);
	static i3VertexArray *	CreateBox( VEC3D * pMin, VEC3D * pMax, bool bColor, bool bUV, COLORREAL * pColor = nullptr);

	bool			IsSameVertex( INT32 idxThis, i3VertexArray * pSrcVA, INT32 idxSrc);
	void			CopyVertexFrom( INT32 idxDest, i3VertexArray * pSrc, INT32 idxSrc);
	void			InterpolateVertex( INT32 idxDest, i3VertexArray * pSrc, INT32 idxStart, INT32 idxEnd, REAL32 t);
	void			ExchangeVertex( INT32 idx0, INT32 idx1);

#ifdef I3_DEBUG
	void Dump( INT32 nCount = -1 );
#endif

	
private:
	bool		m_bInThread = false;

public:
	void		setThreadMode( bool bEnable)					{ m_bInThread = bEnable; }
	bool		getThreadMode( void)							{ return m_bInThread; }

protected:
	i3Mutex2		m_Mutex;
};

#endif
