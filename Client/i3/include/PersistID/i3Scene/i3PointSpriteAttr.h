#if !defined( I3_SG_POINTSPRITE_ATTR_H)
#define I3_SG_POINTSPRITE_ATTR_H

#include "i3RenderAttr.h"

enum I3SG_POINTSPRITE_UPDATE_MODE
{
	I3SG_POINTSPRITE_UPDATE_ONCE	=	0,
	I3SG_POINTSPRITE_UPDATE_ALWAYS,
};

class I3_EXPORT_SCENE i3PointSpriteAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3PointSpriteAttr);

protected:
	i3VertexArray *					m_pVA;
	RT_VEC3D *						m_pPosBuf;
//	INT32							m_PosBufIdx; 

	COLOR *							m_pColorBuf;
//	INT32							m_ColorBufIdx; 

	RT_VEC2D *						m_pSizeBuf;
//	INT32							m_SizeBufIdx; 

	UINT32							m_PrimCount;
	UINT32							m_StartIdx;
	I3SG_POINTSPRITE_UPDATE_MODE	m_UpdateMode;
	BOOL							m_bEditted;

	BOOL							m_bConstantSize;
	RT_VEC2D						m_ConstantSize;

protected:
	void							UpdateVertexArray(void);

public:
	i3PointSpriteAttr(void);
	virtual ~i3PointSpriteAttr(void);

	BOOL			Create( UINT32 PrimCount, I3SG_POINTSPRITE_UPDATE_MODE UpdateMode = I3SG_POINTSPRITE_UPDATE_ALWAYS, BOOL bColor = TRUE, BOOL bSize = TRUE, BOOL bUV = FALSE);

	void			SetVertexArray( i3VertexArray * pVA, I3SG_POINTSPRITE_UPDATE_MODE  UpdateMode, UINT32 primcount, UINT32 startIdx = 0);
	i3VertexArray *	GetVertexArray(void)						{ return m_pVA; }

	UINT32			GetPrimitiveCount(void)						{ return m_PrimCount; }

	void			SetPosition( INT32 idx, RT_VEC3D * pVec)
	{	
		i3Vector::Copy( m_pPosBuf + idx, pVec);
		m_bEditted = TRUE;
	}

	RT_VEC3D *		GetPosition( INT32 idx)						{	return m_pPosBuf + idx; }

	void			SetColor( INT32 idx, COLOR * pCol)
	{
		i3Color::Set( m_pColorBuf + idx, pCol);
		m_bEditted = TRUE;
	}

	COLOR *			GetColor( INT32 idx)						{	return m_pColorBuf + idx; }

	void			SetSize( INT32 idx, RT_VEC2D * pVec)
	{
		m_bEditted = TRUE;
		i3Vector::Copy( m_pSizeBuf + idx, pVec);
	}

	RT_VEC2D *		GetSize( INT32 idx)							{	return m_pSizeBuf + idx; }

	void			SetConstantSizeEnable( BOOL bFlag)			{	m_bConstantSize = bFlag; }
	BOOL			GetConstantSizeEnable(void)					{	return m_bConstantSize; }

	void			SetConstantSize( RT_REAL32 cx, RT_REAL32 cy){	i3Vector::Set( &m_ConstantSize, cx, cy); }
	RT_REAL32		GetConstantSizeX(void)						{	return i3Vector::GetU( &m_ConstantSize); }
	RT_REAL32		GetConstantSizeY(void)						{	return i3Vector::GetV( &m_ConstantSize); }

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
