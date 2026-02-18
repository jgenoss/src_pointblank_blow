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
	I3_EXPORT_CLASS_DEFINE( i3PointSpriteAttr, i3RenderAttr);

protected:
	i3VertexArray *					m_pVA = nullptr;
	VEC3D *						m_pPosBuf = nullptr;
	COLOR *							m_pColorBuf = nullptr;
	VEC2D *						m_pSizeBuf = nullptr;

	UINT32							m_PrimCount = 0;
	UINT32							m_StartIdx = 0;
	I3SG_POINTSPRITE_UPDATE_MODE	m_UpdateMode = I3SG_POINTSPRITE_UPDATE_ALWAYS;
	bool							m_bEditted = false;

	bool							m_bConstantSize = false;
	VEC2D						m_ConstantSize = { 1.0f, 1.0f };

protected:
	void							UpdateVertexArray(void);

public:
	virtual ~i3PointSpriteAttr(void);

	bool			Create( UINT32 PrimCount, I3SG_POINTSPRITE_UPDATE_MODE UpdateMode = I3SG_POINTSPRITE_UPDATE_ALWAYS, bool bColor = true, bool bSize = true, bool bUV = false);

	void			SetVertexArray( i3VertexArray * pVA, I3SG_POINTSPRITE_UPDATE_MODE  UpdateMode, UINT32 primcount, UINT32 startIdx = 0);
	i3VertexArray *	GetVertexArray(void)						{ return m_pVA; }

	UINT32			GetPrimitiveCount(void)						{ return m_PrimCount; }

	void			SetPosition( INT32 idx, VEC3D * pVec)
	{	
		i3Vector::Copy( m_pPosBuf + idx, pVec);
		m_bEditted = true;
	}

	VEC3D *		GetPosition( INT32 idx)						{	return m_pPosBuf + idx; }

	void			SetColor( INT32 idx, COLOR * pCol)
	{
		i3Color::Set( m_pColorBuf + idx, pCol);
		m_bEditted = true;
	}

	COLOR *			GetColor( INT32 idx)						{	return m_pColorBuf + idx; }

	void			SetSize( INT32 idx, VEC2D * pVec)
	{
		m_bEditted = true;
		i3Vector::Copy( m_pSizeBuf + idx, pVec);
	}

	VEC2D *		GetSize( INT32 idx)							{	return m_pSizeBuf + idx; }

	void			SetConstantSizeEnable( bool bFlag)			{	m_bConstantSize = bFlag; }
	bool			GetConstantSizeEnable(void)					{	return m_bConstantSize; }

	void			SetConstantSize( REAL32 cx, REAL32 cy){	i3Vector::Set( &m_ConstantSize, cx, cy); }
	REAL32		GetConstantSizeX(void)						{	return i3Vector::GetU( &m_ConstantSize); }
	REAL32		GetConstantSizeY(void)						{	return i3Vector::GetV( &m_ConstantSize); }

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
