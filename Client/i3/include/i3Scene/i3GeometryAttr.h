#if !defined( __I3_GEOMETRY_ATTR_H)
#define __I3_GEOMETRY_ATTR_H

#include "i3Base.h"
#include "i3Gfx.h"
#include "i3RenderAttr.h"
#include "i3BoneMatrixListAttr.h"
#include "i3SkinBuilder.h"

namespace i3 { struct mat4a;  }

class i3SkinMeshAABBProcessor;
//#define	OCC_CULL

typedef UINT8	I3_GEOMETRY_STYLE;
#define		I3_GEOMETRY_STYLE_PREBAKED			0x01			// Avatar에 적용됨. 이미 가공된 데이터(실시간으로 데이터를 가공할 필요가 없다)
#define		I3_GEOMETRY_STYLE_SHARED_VA			0x02			// VA를 공유

namespace i3 { class aabb; }
namespace i3 { class occ_mesh_set;  }

class I3_EXPORT_SCENE i3GeometryAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3GeometryAttr, i3RenderAttr);

protected:
	I3_GEOMETRY_STYLE		m_GeoStyle = 0;
	i3VertexArray *			m_pVA = nullptr;
	i3IndexArray *			m_pIA = nullptr;
	I3G_PRIMITIVE			m_PrimType = I3G_PRIM_NONE;
	UINT32					m_PrimCount = 0;
	UINT32					m_StartIdx = 0;

	i3VertexArray *			m_pPrimVA = nullptr;		// 옵티마이즈된 VA
	i3IndexArray *			m_pPrimIA = nullptr;
	I3_DRAW_GROUP_LIST_PTR	m_pPrimGroup;	// 스키닝 그룹
	UINT32 *				m_pPrimLength = nullptr;
	UINT32					m_VertexMargine = 0;
	I3G_VERTEXBLEND_METHOD	m_VertexBlendMethod = I3G_VERTEXBLEND_NONE;
	i3BoneMatrixListAttr *	m_pBlendMatrixList = nullptr;

	#if defined( I3G_OGLES)
	UINT32					m_hMacro = INVALID_HMACRO;
	#endif

	#if defined( I3G_DX)
	// progressive mesh
	LPD3DXMESH				m_pD3DXMesh = nullptr;
	LPD3DXPMESH				m_pD3DPMesh = nullptr;

	bool					m_bResetVertexCount = false;
	INT32					m_nMinCount = 0;
	INT32					m_nMaxCount = 0;
	INT32					m_nRate = 100;
	#endif

	i3Shader *				m_pDrawShader = nullptr;

	VEC3D					m_BoundMin;
	VEC3D					m_BoundMax;
	bool					m_bValidBoundBox = false;


	i3::shared_ptr<i3::mat4a>	m_matOBBPtr;

	i3::shared_ptr<i3SkinMeshAABBProcessor>		m_AABBProcessorPtr;

protected:
	void			_UpdateLights( i3RenderContext * pCtx);

	void			_DestroyPrimGroup(void);

	bool			_MakeOBBSet();
public:
	i3GeometryAttr(void);
	virtual ~i3GeometryAttr(void);

	void			addGeoStyle( I3_GEOMETRY_STYLE style)		{ m_GeoStyle |= style; }
	void			removeGeoStyle( I3_GEOMETRY_STYLE style)	{ m_GeoStyle &= ~style; }
	I3_GEOMETRY_STYLE	getGeoStyle( void)						{ return m_GeoStyle; }

#if defined( I3G_DX)
	void			SetProgressiveRate( INT32 rate);
#endif

	void			SetVertexArray( i3VertexArray * pVA, I3G_PRIMITIVE prim, UINT32 primcount, UINT32 startIdx = 0, bool bBuildSkin = true);
	void			SetVertexArray( i3VertexArray * pVA, bool bBuildSkin = true);
	i3VertexArray *	GetVertexArray(void)						{ return m_pVA; }

	void			SetIndexArray( i3IndexArray * pIA);
	i3IndexArray *	GetIndexArray(void)							{ return m_pIA; }

	void			SetPrimitiveType( I3G_PRIMITIVE prim)		{ m_PrimType = prim; }
	I3G_PRIMITIVE	GetPrimitiveType(void)						{ return m_PrimType; }

	void			SetPrimitiveLength( UINT32 Idx, UINT32 Length);
	UINT32			GetPrimitiveLength( UINT32 Idx);
	bool			IsPrimitiveLength(void)						{ return m_pPrimLength != nullptr; }
	UINT32	*		GetPrimitiveLengthArray(void)				{ return m_pPrimLength; }

	UINT32			GetPrimitiveCount(void)						{ return m_PrimCount; }
	void			SetPrimitiveCount( UINT32 nCount )			{ m_PrimCount = nCount; }

	UINT32			GetStartIndex(void)							{ return m_StartIdx; }
	void			SetStartIndex( UINT32 idx)					{ m_StartIdx = idx; }

	i3VertexArray *	getPrimitiveVertexArray(void)				{ return m_pPrimVA; }
	void			setPrimitiveVertexArray( i3VertexArray * pVA)	{ I3_REF_CHANGE( m_pPrimVA, pVA); }
	i3IndexArray *	getPrimitiveIndexArray(void)				{ return m_pPrimIA; }
	void			setPrimitiveIndexArray( i3IndexArray * pIA)	{ I3_REF_CHANGE( m_pPrimIA, pIA); }
	I3_DRAW_GROUP_LIST_PTR&	getPrimitiveGroupList(void)			{ return m_pPrimGroup; }

	i3BoneMatrixListAttr *	getBoneMatrixListAttr(void)					{ return m_pBlendMatrixList; }
	void					setBoneMatrixListAttr( i3BoneMatrixListAttr * pAttr)
	{
		I3_REF_CHANGE( m_pBlendMatrixList, pAttr);
	}

	I3G_VERTEXBLEND_METHOD	getVertexBlendMethod(void)			{ return m_VertexBlendMethod; }
	void			setVertexBlendMethod( I3G_VERTEXBLEND_METHOD method)	{ m_VertexBlendMethod = method; }

	const VEC3D	*	getBoundMin(void)							{ return &m_BoundMin; }
	const VEC3D *	getBoundMax(void)							{ return &m_BoundMax; }
	bool			isValidBound(void);
	void			setBound( const VEC3D * pMin, const VEC3D * pMax);
	void			setBound(const i3::pack::VEC3D & Min, const i3::pack::VEC3D & Max);
	void			RefreshBound(void);
	void			CalcBoundBox( VEC3D * pMin, VEC3D * pMax);

	void			CalcUpdateAABB(i3::aabb& outAABB, MATRIX* inputMatWorld, i3BoneMatrixListAttr* inputMatAttr);
	void			CalcOccMeshSet(i3::occ_mesh_set* occMeshSet, const MATRIX* matStack, bool twoSideEnabled);

	void			Transform( MATRIX * pMat);

	#if defined( I3G_OGLES)
	void			BuildMacro(void);
	#endif
	
	virtual void	BuildSkin( INT32 nBlendIndexCount, INT32 nBoneMatrixCount );
	virtual void	OptimizeForSkin(void);

	void			setGroupList( I3_DRAW_GROUP_LIST_PTR * group);

	void			CreateSkinMeshAABBProcessor();
	void			CreateSkinMeshAABBProcessor(i3VertexArray* pExternVA);

	// for i3RenderAttr
	virtual void	Apply( i3RenderContext * pContext) override;
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool	IsSame( i3RenderAttr * pAttr) override;

	virtual void	OnLostDevice( bool bLostDevice) override;
	virtual void	OnRevive( i3RenderContext * pCtx) override;

	// for i3PersistantElement
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;


#if defined( I3_DEBUG)
	virtual void	Dump(void) override;
#endif	// I3_DEBUG
};

#endif
