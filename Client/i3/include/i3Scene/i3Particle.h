#if !defined( __I3_PARTICLE_H)
#define __I3_PARTICLE_H

#include "i3Node.h"
#include "i3AttrSet.h"

#include "i3GeometryAttr.h"
#include "i3LightingEnableAttr.h"
#include "i3TextureBindAttr.h"
#include "i3BlendEnableAttr.h"
#include "i3BlendModeAttr.h"
#include "i3TextureEnableAttr.h"
#include "i3ZWriteEnableAttr.h"
#include "i3NormalMapEnableAttr.h"
#include "i3NormalMapBindAttr.h"
#include "i3ZBiasAttr.h"
#include "i3SceneGraphInfo.h"
#include "i3ParticleInfo.h"

#include "i3Base/pool/class_object_pool.h"

class i3ParticleRender;
class i3ParticleGroup;
class i3Decal;
class i3DecalDrawInfo;

typedef UINT16 I3_PARTICLE_STATE;
#define		I3_PARTICLE_STATE_ENABLE			0x0001
#define		I3_PARTICLE_STATE_INITIALIZED		0x0002
#define		I3_PARTICLE_STATE_FIRED				0x0004
#define		I3_PARTICLE_STATE_FIRSTTIME			0x0008
#define		I3_PARTICLE_STATE_FIRE_DECAL		0x0010

typedef UINT16 I3_PARTICLE_STYLE;
#define I3_PARTICLE_STYLE_TEX_ENABLE		0x0001
#define I3_PARTICLE_STYLE_BLEND_ENABLE		0x0002
#define I3_PARTICLE_STYLE_ZWRITE_ENABLE		0x0004
#define I3_PARTICLE_STYLE_ZTEST_ENABLE		0x0008
#define I3_PARTICLE_STYLE_NMMAP_ENABLE		0x0010
#define I3_PARTICLE_STYLE_LIGHTING_ENABLE	0x0020
#define I3_PARTICLE_STYLE_FACETOCAM_ENABLE	0x0040
#define I3_PARTICLE_STYLE_IS_DECAL			0x0080
#define I3_PARTICLE_STYLE_ROTATE_ENABLE		0x0100
#define I3_PARTICLE_STYLE_START				0x0200
#define I3_PARTICLE_STYLE_STOP				0x0400


#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

class i3Particle;

struct i3ParticleTransformMatrix : i3::class_object_pool<i3ParticleTransformMatrix>		// 오브젝트 메모리풀 사용..
{
	MATRIX						m_WorldTx;
	MATRIX						m_invCamTx;
};

struct I3PARTICLEOBJECT
{
	i3Particle*		m_Owner = nullptr;

	UINT32			m_State = 0;
	REAL32			m_Time = 0.0f;
	REAL32			m_PosInterval = 0.0f;
	REAL32			m_PosArg[FUNC_COUNT] = { 0.0f, 0.0f };
	VEC3D			m_EmitDir;
	VEC3D			m_StartPos;
	VEC3D			m_TargetPos;
	VEC3D			m_Pos[2];
	REAL32			m_PosRandomInterval = 0.0f;
	REAL32			m_PosRandom = 0.0f;
	VEC3D			m_Gravity;
	
	REAL32			m_Mass = 0.0f;

	REAL32			m_ScaleInterval = 0.0f;
	REAL32			m_ScaleArg[FUNC_COUNT] = { 0.0f, 0.0f };
	VEC3D			m_StartScale;
	VEC3D			m_TargetScale;
	VEC3D			m_Scale;
	REAL32			m_ScaleRandom = 0.0f;

	REAL32			m_RotateAccm = 0.0f;
	VEC3D			m_RotateDir;
	REAL32			m_RotateRandom = 0.0f;

	REAL32			m_TexSeqInterval = 0.0f;

	VEC2D			m_UV1;
	VEC2D			m_UV2;

	UINT32			m_Color = 0;

	i3ParticleTransformMatrix*	m_pTransMtx = nullptr;	// 지연될 월드/뷰 매트릭스값 보관..(중도 변경 때문에 보관해야됨)..포인터만 가져다 쓰게 됨..
	VEC3D			m_PosFromView;				//
//	REAL32			m_ZDepth;					// 카메라 시점 기준 알파소팅용 중심 거리값

} ;

#if defined( I3_COMPILER_VC)
#if defined(PACKING_RIGHT)
	#pragma pack(pop)			// [test required] 패킹 팝 누락으로 추가합니다. 2017.02.23. soon9
#else
	#pragma pack(push,4)
#endif
#endif

/** \brief
	\desc leaf class
	*/

class I3_EXPORT_SCENE i3Particle : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3Particle, i3Node);

protected:
	i3ParticleInfo*		m_pInfo = nullptr;
	i3ParticleInfo*		m_pOrigInfo = nullptr;
	i3ParticleInfo*		m_pCustomInfo = nullptr;

	i3SceneGraphInfo *	m_pMeshInfo = nullptr;
	i3TextureSequence *	m_pTexSeq = nullptr;

	///////////////////////////
	// Volatile Data
	I3_PARTICLE_STYLE	m_uParticleStyle = 0;
	
	INT32				m_PosArgCount = 0;
	INT32				m_ScaleArgCount = 0;
	REAL32			m_timeLocal = 0.0f;

	VEC3D			m_EmitRightVector;
	VEC3D			m_EmitUpVector;

	REAL32			m_EmitInterval = 0.0f;

	REAL32			m_PosRInterval = 0.0f;
	REAL32			m_ScaleRInterval = 0.0f;
	REAL32			m_ColorRInterval = 0.0f;
	REAL32			m_RotateRInterval = 0.0f;

	VEC3D			m_UpRightVector;
	VEC3D			m_UpAtVector;

	I3PARTICLEOBJECT **	m_ppObj = nullptr;

	i3ParticleGroup *		m_pGroup = nullptr;

	i3Texture *				m_pTexture = nullptr;
	I3G_BLEND				m_BlendSrc = I3G_BLEND_SRCALPHA;
	I3G_BLEND				m_BlendDest = I3G_BLEND_INVSRCALPHA;
	I3G_BLEND_OP			m_BlendOP = I3G_BLEND_OP_ADD;
	i3Texture *				m_pNormalMap = nullptr;
	REAL32					m_ZBiasConst = 0.0f;
	REAL32					m_ZBiasSlope = 0.0f;
	
	i3AttrSet *				m_pScene;			// [initialize at constructor]
	i3BoundBox *			m_pBoundBox = nullptr;

	i3KeyframeTable::INTERPOLATION	m_PosInterp = i3KeyframeTable::NONE;
	i3KeyframeTable::INTERPOLATION	m_ScaleInterp = i3KeyframeTable::NONE;
	i3KeyframeTable::INTERPOLATION	m_ColorInterp = i3KeyframeTable::NONE;
	
	REAL32				m_RObjectLifeTime = 0.0f;
	INT32					m_Priority = 50;
	UINT32					m_lastFrameID = 0xFFFFFFFF;
	INT32					m_EmitWaitCount = 0;
	INT32					m_AllocatedCount = 0;
	INT32					m_MeshTriangleIndex = 0;
	
	bool					m_bCustomParticleInfoUse = false;


protected:
	void				_Emit( I3PARTICLEOBJECT * pObj);

public:
	i3Particle(void);
	virtual ~i3Particle(void);

	void				setParticleStyle( I3_PARTICLE_STYLE style)	{ m_uParticleStyle = style; }
	I3_PARTICLE_STYLE	getParticleStyle( void)						{ return m_uParticleStyle; }
	void				addParticleStyle( I3_PARTICLE_STYLE style)	{ m_uParticleStyle |= style; }
	void				removeParticleStyle( I3_PARTICLE_STYLE style) { m_uParticleStyle &= ~style; }
	bool				isParticleStyle( I3_PARTICLE_STYLE style)	{ return (m_uParticleStyle & style) == style; }

	INT32				getPriority(void)						{ return m_Priority; }
	void				setPriority( INT32 prio)				{ m_Priority = prio; }

	i3ParticleInfo *	GetParticleInfo(void) const				{ return m_pInfo; }
	void				SetParticleInfo( i3ParticleInfo * pInfo);

	void				SetCustomParticleInfo( i3ParticleInfo * pInfo);
	void				EnableCustomParticleInfo(bool bEnable);
	bool				IsEnabledCustomParticleInfo() const;

	i3ParticleInfo *	GetCustomParticleInfo(void) const		{ return m_pCustomInfo; }

	i3ParticleInfo *	GetOrigParticleInfo(void) const		{ return m_pOrigInfo; }


	i3ParticleGroup *	getParticleGroup(void)					{ return m_pGroup; }
	void				setParticleGroup( i3ParticleGroup * pGrp);

	bool				getDecalEnable(void)					{ return isParticleStyle( I3_PARTICLE_STYLE_IS_DECAL); }
	void				setDecalEnable( bool bFlag)				{ if(bFlag) addParticleStyle( I3_PARTICLE_STYLE_IS_DECAL);else removeParticleStyle(I3_PARTICLE_STYLE_IS_DECAL); }

	i3TextureSequence *	GetTextureSequence(void)				{ return m_pTexSeq; }
	void				SetTextureSequence( i3TextureSequence * pTex);

	bool				getBlendEnable(void)					{ return isParticleStyle( I3_PARTICLE_STYLE_BLEND_ENABLE); }
	void				setBlendEnable( bool bFlag)				{ if(bFlag) addParticleStyle( I3_PARTICLE_STYLE_BLEND_ENABLE);else removeParticleStyle(I3_PARTICLE_STYLE_BLEND_ENABLE); }

	I3G_BLEND			getSrcBlend(void)						{ return m_BlendSrc; }
	void				setSrcBlend( I3G_BLEND mode)			{ m_BlendSrc = mode; }

	I3G_BLEND			getDestBlend(void)						{ return m_BlendDest; }
	void				setDestBlend( I3G_BLEND mode)			{ m_BlendDest = mode; }

	I3G_BLEND_OP		getBlendOperation(void)					{ return m_BlendOP; }
	void				setBlendOperation( I3G_BLEND_OP op)		{ m_BlendOP = op; }

	bool				getZWriteEnable(void)					{ return isParticleStyle( I3_PARTICLE_STYLE_ZWRITE_ENABLE); }
	void				setZWriteEnable( bool bFlag)			{ if(bFlag) addParticleStyle( I3_PARTICLE_STYLE_ZWRITE_ENABLE); else removeParticleStyle( I3_PARTICLE_STYLE_ZWRITE_ENABLE); }

	///////////////////////////////////////////////////////////////////////////////
	// Texture
	i3Texture *			getTexture(void)						{ return m_pTexture; }
	void				setTexture( i3Texture * pTex);

	bool				getTextureEnable(void)					{ return isParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE); }
	void				setTextureEnable( bool bFlag)			{ if(bFlag) addParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE); else removeParticleStyle(I3_PARTICLE_STYLE_TEX_ENABLE); }


	///////////////////////////////////////////////////////////////////////////////
	// Normal Map
	bool				getNormalMapEnable(void)				{ return isParticleStyle( I3_PARTICLE_STYLE_NMMAP_ENABLE); }
	void				setNormalMapEnable( bool bFlag)			{ if(bFlag) addParticleStyle( I3_PARTICLE_STYLE_NMMAP_ENABLE); else removeParticleStyle( I3_PARTICLE_STYLE_NMMAP_ENABLE); }

	i3Texture *			getNormalMap(void)						{ return m_pNormalMap; }
	void				setNormalMap( i3Texture * pTex);

	//////////////////
	REAL32				getZBiasSlope(void)						{ return m_ZBiasSlope; }
	void				setZBiasSlope( REAL32 fVal)				{ m_ZBiasSlope = fVal; }

	REAL32				getZBias(void)							{ return m_ZBiasConst; }
	void				setZBias( REAL32 fVal)					{ m_ZBiasConst = fVal; }

	//
	bool				getZTestEnable(void)					{ return isParticleStyle( I3_PARTICLE_STYLE_ZTEST_ENABLE); }
	void				setZTestEnable( bool bFlag)				{ if(bFlag) addParticleStyle( I3_PARTICLE_STYLE_ZTEST_ENABLE); else removeParticleStyle( I3_PARTICLE_STYLE_ZTEST_ENABLE); }

	//////////////////
	bool				GetFaceToCameraEnable(void)				{ return isParticleStyle( I3_PARTICLE_STYLE_FACETOCAM_ENABLE); }
	void				SetFaceToCameraEnable( bool bFlag)		{ if(bFlag) addParticleStyle( I3_PARTICLE_STYLE_FACETOCAM_ENABLE); else removeParticleStyle( I3_PARTICLE_STYLE_FACETOCAM_ENABLE); }

	/////////////////////
	bool				getLightingEnable(void)					{ return isParticleStyle( I3_PARTICLE_STYLE_LIGHTING_ENABLE); }
	void				setLightingEnable( bool bFlag)			{ if( bFlag) addParticleStyle( I3_PARTICLE_STYLE_LIGHTING_ENABLE); else removeParticleStyle( I3_PARTICLE_STYLE_LIGHTING_ENABLE); }

	i3Node *			GetMesh(void)							
	{ 
		if( m_pMeshInfo == nullptr)
			return nullptr;

		return m_pMeshInfo->getInstanceSg(); 
	}

	void				SetMesh( i3SceneGraphInfo * Info);
	
	// Runtime Data
	bool				IsStarted(void)								{ return isParticleStyle( I3_PARTICLE_STYLE_START); }
	bool				IsStoped()									{ return isParticleStyle( I3_PARTICLE_STYLE_STOP ); }
	void				SetStop(bool bStop)							{ if(bStop) addParticleStyle(I3_PARTICLE_STYLE_STOP ); else removeParticleStyle(I3_PARTICLE_STYLE_STOP); }

	void				Create( INT32 maxInstance = 1);
	void				Start(void);
	void				Stop(void);

	void				Drive( i3SceneTracer * pTracer);
	void				CalcBoundBox(void);

	void				RefreshChanges(void);
	void				AllocateObject();
	void				FreeObject();

	static void			SetGravityDirection( VEC3D * pG);
	static void			SetGravityFactor( REAL32 val);

	virtual void		OnDeactivate(void) override;
	virtual	void		OnChangeTime( REAL32 tm) override;
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void		OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
	virtual bool		OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool		OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	i3DecalDrawInfo*	GetDecalDrawInfo() const { return m_pDrawDecalInfo; }

	//////////////////////////////////////////////////////////////////////
	//						Decal
private:
	i3Decal *				m_pDecal = nullptr;
	i3DecalDrawInfo *		m_pDrawDecalInfo = nullptr;

	UINT16				m_nTerrainType = 0;

protected:
	void				_FreeDecal( void);

public:
	UINT16				getTerrainType( void)				{ return m_nTerrainType; }
	void				setTerrainType( UINT16 type)		{ m_nTerrainType = type; }

};

#endif
