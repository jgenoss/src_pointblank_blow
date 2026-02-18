#if !defined( __I3_GEOMETRY_ATTR_H)
#define __I3_GEOMETRY_ATTR_H

#include "i3Base.h"
#include "i3Gfx.h"
#include "i3RenderAttr.h"
#include "i3BoneMatrixListAttr.h"
#include "i3SkinOptimize.h"

class I3_EXPORT_SCENE i3GeometryAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3GeometryAttr);

protected:
	i3VertexArray *			m_pVA;
	i3IndexArray *			m_pIA;
	I3G_PRIMITIVE			m_PrimType;
	UINT32					m_PrimCount;
	UINT32					m_StartIdx;

	i3VertexArray *			m_pPrimVA;		// 옵티마이즈된 VA
	i3IndexArray *			m_pPrimIA;
	i3List *				m_pPrimGroup;	// 스키닝 그룹
	//INT32					m_PrimLengthIdx; 
	UINT32 *				m_pPrimLength;
	UINT32					m_VertexMargine;
	I3G_VERTEXBLEND_METHOD	m_VertexBlendMethod;
	i3BoneMatrixListAttr *	m_pBlendMatrixList;

	#if defined( I3G_OGLES)
	UINT32					m_hMacro;
	#endif

	i3Shader *				m_pDrawShader;

	//void				CalcBlendedVertex();

protected:
	void					_DetermineVertexBlendMethod(void);
	void			_UpdateLights( i3RenderContext * pCtx);

public:
	i3GeometryAttr(void);
	virtual ~i3GeometryAttr(void);

	void			SetVertexArray( i3VertexArray * pVA, I3G_PRIMITIVE prim, UINT32 primcount, UINT32 startIdx = 0, bool bBuildSkin = true);
	void			SetVertexArray( i3VertexArray * pVA, bool bBuildSkin = true);
	i3VertexArray *	GetVertexArray(void)						{ return m_pVA; }

	void			SetIndexArray( i3IndexArray * pIA);
	i3IndexArray *	GetIndexArray(void)							{ return m_pIA; }

	void			SetPrimitiveType( I3G_PRIMITIVE prim)		{ m_PrimType = prim; }
	I3G_PRIMITIVE	GetPrimitiveType(void)						{ return m_PrimType; }

	void			SetPrimitiveLength( UINT32 Idx, UINT32 Length);
	UINT32			GetPrimitiveLength( UINT32 Idx);
	BOOL			IsPrimitiveLength(void)						{ return m_pPrimLength != NULL; }
	UINT32	*		GetPrimitiveLengthArray(void)				{ return m_pPrimLength; }

	UINT32			GetPrimitiveCount(void)						{ return m_PrimCount; }
	void			SetPrimitiveCount( UINT32 nCount )			{ m_PrimCount = nCount; }

	UINT32			GetStartIndex(void)							{ return m_StartIdx; }

	i3BoneMatrixListAttr *	getBoneMatrixListAttr(void)					{ return m_pBlendMatrixList; }
	void					setBoneMatrixListAttr( i3BoneMatrixListAttr * pAttr)
	{
		I3_REF_CHANGE( m_pBlendMatrixList, pAttr);
	}

	I3G_VERTEXBLEND_METHOD	getVertexBlendMethod(void)			{ return m_VertexBlendMethod; }

	void			CalcBoundBox( RT_VEC3D * pMin, RT_VEC3D * pMax);

	void			Transform( RT_MATRIX * pMat);

	#if defined( I3G_OGLES)
	void			BuildMacro(void);
	#endif
	
	void			BuildSkin( INT32 nBlendIndexCount, INT32 nBoneMatrixCount );
	void			OptimizeForSkin(void);

	// for i3RenderAttr
	virtual void	Apply( i3RenderContext * pContext);
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL	IsSame( i3RenderAttr * pAttr);

	// for i3PersistantElement
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	void			Dump(void);
	#endif
};

#endif
