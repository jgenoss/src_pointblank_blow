#if !defined( __I3_VERTEX_ARRAY_OGLES_H)
#define __I3_VERTEX_ARRAY_OGLES_H

#if !defined(I3G_OGLES)
#error "This header file is needed only for WIPI OpenGL ES."
#endif

#include "i3VertexArray.h"

//#define		SINGLE_ARRAY

class i3VertexArrayOGLES : public i3VertexArray
{
	I3_CLASS_DEFINE( i3VertexArrayOGLES);

protected:
	INT8 *			m_pPos;
	INT8 *			m_pNormal;
	INT8 *			m_pColor;
	INT8 *			m_pTexCoord;

	UINT32			m_PosStride;
	UINT32			m_NormalStride;
	UINT32			m_ColorStride;
	UINT32			m_TexCoordStride;
	
protected:
	virtual UINT32		CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count);

public:
	i3VertexArrayOGLES(void);
	virtual ~i3VertexArrayOGLES(void);

	void *			GetPositionPointer(void)		{ return (void *) m_pPos; }
	void *			GetNormalPointer(void)			{ return (void *) m_pNormal; }
	void *			GetTexCoordPointer(void)		{ return (void *) m_pTexCoord; }
	void *			GetColorPointer(void)			{ return (void *) m_pColor; }

	UINT32			GetPositionStride(void)			{ return m_PosStride; }
	UINT32			GetNormalStride(void)			{ return m_NormalStride; }
	UINT32			GetColorStride(void)			{ return m_ColorStride; }
	UINT32			GetTexCoordStride(void)			{ return m_TexCoordStride; }

	#if !defined( SINGLE_ARRAY)
	VIRTUAL	void	SetPosition( UINT32	Idx, RT_VEC3D * pVec);
	VIRTUAL	void	GetPosition( UINT32 Idx, RT_VEC3D * pVec);
	VIRTUAL	void	SetPositions( UINT32 StartIdx, UINT32 Count, RT_VEC3D * pPos, UINT32 pitch = sizeof(RT_VEC3D));

	VIRTUAL	void	SetColor( UINT32 Idx, COLORREAL * pColor);
	VIRTUAL	void	GetColor( UINT32 Idx, COLORREAL * pColor);
	VIRTUAL	void	SetColors( UINT32 StartIdx, UINT32 Count, COLOR * pColor, UINT32 pitch = sizeof(COLOR));
	VIRTUAL	void	SetColor( UINT32 Idx, UINT32 Count, COLOR * pColor);

	VIRTUAL	void	SetNormal( UINT32 Idx, RT_VEC3D * pVec);
	VIRTUAL	void	GetNormal( UINT32 Idx, RT_VEC3D * pVec);
	VIRTUAL	void	SetNormals( UINT32 StartIdx, UINT32 Count, RT_VEC3D * pNormal, UINT32 pitch = sizeof(RT_VEC3D));

	VIRTUAL	void	SetTextureCoord( UINT32 which, UINT32 Idx, RT_VEC2D * pVec);
	VIRTUAL	void	GetTextureCoord( UINT32 which, UINT32 Idx, RT_VEC2D * pVec);
	VIRTUAL	void	SetTextureCoords( UINT32 which, UINT32 StartIdx, UINT32 Count, RT_VEC2D * pUV, UINT32 pitch = sizeof(RT_VEC2D));

	VIRTUAL	void	SetBlendIndex( UINT32 which, UINT32 Idx, UINT32 BlendIndex);
	VIRTUAL	UINT32	GetBlendIndex( UINT32 which, UINT32 Idx);
	VIRTUAL	void	SetBlendIndices( UINT32 which, UINT32 StartIdx, UINT32 Count, UINT8 * pIndices, UINT32 pitch = 1);

	VIRTUAL	void	SetPointSpriteSize( UINT32 Idx, RT_REAL32 size);
	VIRTUAL	void	SetPointSpriteSize( UINT32 Idx, RT_VEC2D * pVec);
	VIRTUAL	RT_REAL32	GetPointSpriteSize( UINT32 Idx);
	VIRTUAL	void	GetPointSpriteSize( UINT32 Idx, RT_VEC3D * pVec);
	VIRTUAL	void	SetPointSpriteSizes( UINT32 StartIdx, UINT32 Count, RT_VEC2D * pVec, UINT32 pitch = sizeof(RT_VEC2D));
	#endif

	virtual BOOL	Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE UsageFlags);
	virtual BOOL	Lock( I3G_LOCKMETHOD lock = I3G_LOCK_OVERWRITE, UINT32 StartIdx = 0, UINT32 EndIdx = 0);
	virtual void	Unlock(void);
	virtual void	MakeRuntimeFormat( I3_PERSIST_VERTEX_OFFSET * pInfo);
};

#endif
