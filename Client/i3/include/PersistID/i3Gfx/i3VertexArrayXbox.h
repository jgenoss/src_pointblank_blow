#if !defined( __I3_VERTEX_ARRAY_XB_H)
#define __I3_VERTEX_ARRAY_XB_H

#include "i3VertexArray.h"

#if !defined(I3_XBOX) || !defined(I3G_XBOX)
#error "This header file is only for Xbox."
#endif

#define i3VertexArrayPlatform i3VertexArrayXbox
class i3VertexArrayXbox : public i3VertexArray
{
	I3_CLASS_DEFINE( i3VertexArrayXbox);

protected:
	IDirect3DVertexBuffer8 *		m_pVertexBuf;

	UINT8							m_OffsetPos;
	UINT8							m_OffsetRHW;
	UINT8							m_OffsetWeight;
	UINT8							m_OffsetNormal;
	UINT8							m_OffsetPointSize;
	UINT8							m_OffsetColor;
	UINT8							m_OffsetTexCoord[8];

	UINT32		CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count);
public:
	i3VertexArrayXbox(void);
	virtual ~i3VertexArrayXbox(void);

	virtual void *	GetInstance(void)
	{
		return		m_pVertexBuf;
	}

	virtual BOOL	Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE UsageFlags);
	virtual BOOL	Lock( I3G_LOCKMETHOD lock = I3G_LOCK_NONE, UINT32 StartIdx = 0, UINT32 Count = 0);
	virtual void	Unlock(void);

	// Position
	virtual void	SetPosition( UINT32	Idx, VEC3D * pVec);
	virtual void	GetPosition( UINT32 Idx, VEC3D * pVec);

	// Vertex Color
	virtual void	SetColor( UINT32 Idx, COLORREAL * pColor);
	virtual void	GetColor( UINT32 Idx, COLORREAL * pColor);

	// Normal
	virtual void	SetNormal( UINT32 Idx, VEC3D * pVec);
	virtual void	GetNormal( UINT32 Idx, VEC3D * pVec);

	// Texture Coords.
	virtual void	SetTextureCoord( UINT32 which, UINT32 Idx, VEC2D * pVec);
	virtual void	GetTextureCoord( UINT32 which, UINT32 Idx, VEC2D * pVec);

	// Blend Index
	virtual void	SetBlendIndex( UINT32 which, UINT32 Idx, UINT32 BlendIndex);
	virtual UINT32	GetBlendIndex( UINT32 which, UINT32 Idx);

	// Blend Weight
	virtual void	SetBlendWeight( UINT32 which, UINT32 Idx, REAL32 weight);
	virtual REAL32	GetBlendWeight( UINT32 wihch, UINT32 Idx);

	// Point-Sprite Size
	virtual void	SetPointSpriteSize( UINT32 Idx, REAL32 size);
	virtual void	SetPointSpriteSize( UINT32 Idx, VEC2D * pVec);
	virtual REAL32	GetPointSpriteSize( UINT32 Idx);
	virtual void	GetPointSpriteSize( UINT32 Idx, VEC3D * pVec);
};

#endif