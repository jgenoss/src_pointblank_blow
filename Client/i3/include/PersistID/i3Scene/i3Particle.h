#if !defined( __I3_PARTICLE_H)
#define __I3_PARTICLE_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Gfx.h"
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
#include "i3Decal.h"

class i3ParticleRender;
class i3ParticleGroup;

#define		I3_PARTICLE_STATE_ENABLE			0x00000001
#define		I3_PARTICLE_STATE_INITIALIZED		0x00000002
#define		I3_PARTICLE_STATE_FIRED				0x00000004
#define		I3_PARTICLE_STATE_FIRSTTIME			0x00000008
#define		I3_PARTICLE_STATE_FIRE_DECAL		0x00000010

typedef struct ALIGN16 _tagI3ParticleObject
{
	UINT32			m_State;
	REAL32			m_Time;

	VEC3D			m_EmitDir;
	REAL32			m_PosInterval;
	REAL32			m_PosArg[FUNC_COUNT];
	VEC3D			m_StartPos;
	VEC3D			m_TargetPos;
	VEC3D			m_Pos[2];
	REAL32			m_PosRandomInterval;
	REAL32			m_PosRandom;
	VEC3D			m_Gravity;
	
	REAL32			m_Mass;

	REAL32			m_ScaleInterval;
	REAL32			m_ScaleArg[FUNC_COUNT];
	VEC3D			m_StartScale;
	VEC3D			m_TargetScale;
	VEC3D			m_Scale;
	REAL32			m_ScaleRandom;

	REAL32			m_RotateAccm;
	VEC3D			m_RotateDir;
	REAL32			m_RotateRandom;

	REAL32			m_TexSeqInterval;

	VEC2D			m_UV1;
	VEC2D			m_UV2;

	UINT32			m_Color;
} I3PARTICLEOBJECT;

class I3_EXPORT_SCENE i3Particle : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3Particle);
public:

protected:

	i3ParticleInfo *	m_pInfo;
	i3SceneGraphInfo *	m_pMeshInfo;
	i3TextureSequence *	m_pTexSeq;

	///////////////////////////
	// Volatile Data
	BOOL				m_bStart;
	BOOL				m_bStop;

	INT32				m_PosArgCount;
	INT32				m_ScaleArgCount;
	RT_REAL32			m_timeLocal;

	RT_VEC3D			m_EmitRightVector;
	RT_VEC3D			m_EmitUpVector;

	RT_REAL32			m_EmitInterval;

	RT_REAL32			m_PosRInterval;
	RT_REAL32			m_ScaleRInterval;
	RT_REAL32			m_ColorRInterval;
	RT_REAL32			m_RotateRInterval;

	RT_VEC3D			m_UpRightVector;
	RT_VEC3D			m_UpAtVector;

	I3PARTICLEOBJECT *	m_pObj;

	i3ParticleGroup *		m_pGroup;

	i3Texture *				m_pTexture;
	bool					m_bTextureEnable;
	bool					m_bBlendEnable;
	I3G_BLEND				m_BlendSrc;
	I3G_BLEND				m_BlendDest;
	I3G_BLEND_OP			m_BlendOP;
	bool					m_bZWriteEnable;
	bool					m_bNormalMapEnable;
	i3Texture *				m_pNormalMap;
	REAL32					m_ZBiasConst;
	REAL32					m_ZBiasSlope;
	bool					m_bLightingEnable;

	bool					m_bFaceToCamera;
	bool					m_bDecal;

	i3AttrSet *				m_pScene;
	i3BoundBox *			m_pBoundBox;

	i3KeyframeTable::INTERPOLATION	m_PosInterp;
	i3KeyframeTable::INTERPOLATION	m_ScaleInterp;
	i3KeyframeTable::INTERPOLATION	m_ColorInterp;

	BOOL					m_bRotateEnable;
	RT_REAL32				m_RObjectLifeTime;

	i3Decal *				m_pDecal;

	INT32					m_Priority;

	UINT32					m_lastFrameID;

protected:
	void				_Emit( I3PARTICLEOBJECT * pObj);

public:
	i3Particle(void);
	virtual ~i3Particle(void);

	INT32						getPriority(void)						{ return m_Priority; }
	void						setPriority( INT32 prio)				{ m_Priority = prio; }

	i3ParticleInfo *			GetParticleInfo(void)					{ return m_pInfo; }
	void						SetParticleInfo( i3ParticleInfo * pInfo);

	i3ParticleGroup *			getParticleGroup(void)					{ return m_pGroup; }
	void						setParticleGroup( i3ParticleGroup * pGrp);

	I3SG_DECAL_STATE			getDecalState(void)						{ return m_pDecal->getDecalState(); }
	void						setDecalState( I3SG_DECAL_STATE state)	{ m_pDecal->setDecalState( state); }

	bool						getDecalEnable(void)					{ return m_bDecal; }
	void						setDecalEnable( bool bFlag)				{ m_bDecal = bFlag; }

	i3Decal *					getDecal(void)						{ return m_pDecal; }

	i3TextureSequence *			GetTextureSequence(void)				{ return m_pTexSeq; }
	void						SetTextureSequence( i3TextureSequence * pTex);

	bool						getBlendEnable(void)					{ return m_bBlendEnable; }
	void						setBlendEnable( bool bFlag)				{ m_bBlendEnable = bFlag; }

	I3G_BLEND					getSrcBlend(void)						{ return m_BlendSrc; }
	void						setSrcBlend( I3G_BLEND mode)			{ m_BlendSrc = mode; }

	I3G_BLEND					getDestBlend(void)						{ return m_BlendDest; }
	void						setDestBlend( I3G_BLEND mode)			{ m_BlendDest = mode; }

	I3G_BLEND_OP				getBlendOperation(void)					{ return m_BlendOP; }
	void						setBlendOperation( I3G_BLEND_OP op)		{ m_BlendOP = op; }

	bool						getZWriteEnable(void)					{ return m_bZWriteEnable; }
	void						setZWriteEnable( bool bFlag)			{ m_bZWriteEnable = bFlag; }

	///////////////////////////////////////////////////////////////////////////////
	// Texture
	i3Texture *					getTexture(void)						{ return m_pTexture; }
	void						setTexture( i3Texture * pTex);

	bool						getTextureEnable(void)					{ return m_bTextureEnable; }
	void						setTextureEnable( bool bFlag)			{ m_bTextureEnable = bFlag; }


	///////////////////////////////////////////////////////////////////////////////
	// Normal Map
	bool						getNormalMapEnable(void)				{ return m_bNormalMapEnable; }
	void						setNormalMapEnable( bool bFlag)			{ m_bNormalMapEnable = bFlag; }

	i3Texture *					getNormalMap(void)						{ return m_pNormalMap; }
	void						setNormalMap( i3Texture * pTex);

	//////////////////
	REAL32						getZBiasSlope(void)						{ return m_ZBiasSlope; }
	void						setZBiasSlope( REAL32 fVal)				{ m_ZBiasSlope = fVal; }

	REAL32						getZBias(void)							{ return m_ZBiasConst; }
	void						setZBias( REAL32 fVal)					{ m_ZBiasConst = fVal; }

	//////////////////
	bool						GetFaceToCameraEnable(void);
	void						SetFaceToCameraEnable( bool bFlag);

	/////////////////////
	bool						getLightingEnable(void)					{ return m_bLightingEnable; }
	void						setLightingEnable( bool bFlag)			{ m_bLightingEnable = bFlag; }

	i3Node *					GetMesh(void)							
	{ 
		if( m_pMeshInfo == NULL)
			return NULL;

		return m_pMeshInfo->getInstanceSg(); 
	}

	void						SetMesh( i3SceneGraphInfo * Info);
	
	// Runtime Data
	BOOL						IsStarted(void)								{ return m_bStart; }

	void						Create( INT32 maxInstance = 1);
	void						Start(void);
	void						Stop(void);

	void						Drive( i3SceneTracer * pTracer);
	void						CalcBoundBox(void);

	void						RefreshChanges(void);

	static void					SetGravityDirection( RT_VEC3D * pG);
	static void					SetGravityFactor( RT_REAL32 val);

	virtual	void				OnChangeTime( REAL32 tm);
	virtual void				CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void				OnBuildObjectList( i3List *pList);
	virtual UINT32				OnSave( i3ResourceFile * pResFile);
	virtual UINT32				OnLoad( i3ResourceFile * pResFile);
};

#endif
