#if !defined( __I3_PARTICLE_INFO_H)
#define __I3_PARTICLE_INFO_H

#include "i3Node.h"
#include "i3MeshParticle.h"

#define	I3_PARTICLE_PARAM_STYLE_RANDOM				0x00000001
#define I3_PARTICLE_PARAM_STYLE_INTERPOLATE			0x00000002
#define I3_PARTICLE_PARAM_STYLE_CLAMP				0x00000004
#define I3_PARTICLE_PARAM_STYLE_GRAVITY				0x00000010
#define I3_PARTICLE_PARAM_STYLE_RANDOM_INTERPOLATE	0x00000020

#define	I3_PARTICLE_TYPE_MASK_ROTATE		0x80000000
#define	I3_PARTICLE_TYPE_MASK_TRACE_POS		0x40000000
#define	I3_PARTICLE_TYPE_MASK_TRACE_SCALE	0x20000000

#define FUNC_COUNT							2

class I3_EXPORT_SCENE i3ParticleInfo : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3ParticleInfo, i3PersistantElement);
public:
	enum PARTICLE_TYPE
	{
		PARTICLE_POINTSPRITE	=	0,
		PARTICLE_SPRITE			=	(1 | I3_PARTICLE_TYPE_MASK_ROTATE),
		PARTICLE_BILLBOARD		=	(2 | I3_PARTICLE_TYPE_MASK_TRACE_POS | I3_PARTICLE_TYPE_MASK_TRACE_SCALE),
		PARTICLE_SCENE			=	3,
		PARTICLE_ANCHOR_BILLBOARD = 4,
		PARTICLE_DECAL			= 5,
	};

	enum EMIT_TYPE
	{
		EMIT_CONE			= 0,
		EMIT_SPHERE			= 1,
		EMIT_DISK			= 2,
		EMIT_RECT_PLANE		= 3,
		EMIT_CIRCLE_PLANE	= 4,
		EMIT_MESH			= 5,
	};

	enum FUNC_TYPE
	{
		FUNC_NONE		= 0,
		FUNC_CONSTANT	= 1,
		FUNC_SIN		= 2,
		FUNC_COS		= 3,
		FUNC_RND		= 4,
		FUNC_TABLE		= 5,
	};

protected:
	// Persist Data
	PARTICLE_TYPE		m_Type = PARTICLE_POINTSPRITE;
	REAL32			m_LifeTime = 0.0f;
	REAL32			m_ObjectLifeTime = 0.0f;

	REAL32			m_MassMin = 0.0f;
	REAL32			m_MassMax = 0.0f;

	INT32				m_InitialObjectCount = 0;
	INT32				m_MaxObjectCount = 0;
	INT32				m_MaxObjectCapacity = 0;

	INT32				m_EmitCount = 0;
	bool				m_bVolatile = true;

	// Emitter
	EMIT_TYPE			m_EmitType = EMIT_CONE;
	VEC3D			m_EmitPos;
	VEC3D			m_EmitDir = { 0.0f, 0.0f, 1.0f };
	REAL32			m_EmitAngle = 0.0f;
	REAL32			m_EmitInterval = 0.0f;
	VEC3D			m_UpDir = { 0.0f, 1.0f, 0.0f };

	UINT32				m_PosStyle = 0;
	REAL32			m_PosUpdateInterval = 0.0f;
	FUNC_TYPE			m_PosFunc[FUNC_COUNT] = { FUNC_NONE, FUNC_NONE };
	REAL32			m_PosInit[FUNC_COUNT] = { 0.0f, 0.0f };
	REAL32			m_PosFactor[FUNC_COUNT] = { 1.0f, 1.0f };
	i3KeyframeTable *	m_PosTable[FUNC_COUNT] = { nullptr, nullptr };
	REAL32			m_PosMin = 0.0f;
	REAL32			m_PosMax = 0.0f;
	REAL32			m_PosInitialRandomFactor = 1.0f;
	VEC3D			m_PosRandomFactor = { 1.0f, 1.0f, 1.0f };
	REAL32			m_PosRandomInterval = 0.0f;

	UINT32				m_ScaleStyle = 0;
	REAL32			m_ScaleUpdateInterval = 0.0f;
	FUNC_TYPE			m_ScaleFunc[FUNC_COUNT] = { FUNC_NONE, FUNC_NONE };
	REAL32			m_ScaleInit[FUNC_COUNT] = { 0.0f, 0.0f };
	REAL32			m_ScaleFactor[FUNC_COUNT] = { 1.0f, 1.0f };
	i3KeyframeTable *	m_ScaleTable[FUNC_COUNT] = { nullptr, nullptr };
	REAL32			m_ScaleInitialRandomFactor = 1.0f;
	REAL32			m_ScaleMin = 0.0f;
	REAL32			m_ScaleMax = 0.0f;
	VEC3D			m_ScaleMask = { 1.0f, 1.0f, 1.0f };
	REAL32			m_ScaleBillboardFactor = 0.0f;

	UINT32				m_RotateStyle = 0;
	REAL32			m_RotateUpdateInterval = 0.0f;
	REAL32			m_RotateRandomFactor = 1.0f;
	FUNC_TYPE			m_RotateFunc = FUNC_NONE;				// Place Holder
	REAL32			m_RotateInit = 0.0f;				// Place Holder
	REAL32			m_RotateFactor = 1.0f;				// Place Holder
	i3KeyframeTable *	m_pRotateTable = nullptr;				// Place Holder
	REAL32			m_RotateLength = 0.0f;				// Place Holder
	REAL32			m_RotateMin = 0.0f;				// Place Holder
	REAL32			m_RotateMax = 0.0f;				// Place Holder

	UINT32				m_ColorStyle = 0;
	REAL32			m_ColorUpdateInterval = 0.0f;
	FUNC_TYPE			m_RGBFunc = FUNC_NONE;
	UINT32				m_RGBInit = 0;
	i3KeyframeTable *	m_pRTable = nullptr;
	i3KeyframeTable *	m_pGTable = nullptr;
	i3KeyframeTable *	m_pBTable = nullptr;
	i3KeyframeTable *	m_pATable = nullptr;

	FUNC_TYPE			m_AlphaFunc = FUNC_NONE;
	UINT32				m_AlphaInit = 255;

	i3MeshParticle *	m_pMeshParticle = nullptr;

public:
	virtual ~i3ParticleInfo(void);

	// Persist Data
	PARTICLE_TYPE		GetType(void)							{ return m_Type; }
	void				SetType( PARTICLE_TYPE type)			{ m_Type = type; }

	REAL32			GetLifeTime(void)						{ return m_LifeTime; }
	void				SetLifeTime( REAL32 tm)				{ m_LifeTime = tm; }

	REAL32			GetObjectLifeTime(void)					{ return m_ObjectLifeTime; }
	void				SetObjectLifeTime( REAL32 tm)		{ m_ObjectLifeTime = tm; }

	REAL32			GetMassMin(void)						{ return m_MassMin; }
	void				SetMassMin( REAL32 val)				{ m_MassMin = val; }

	REAL32			GetMassMax(void)						{ return m_MassMax; }
	void				SetMassMax( REAL32 val)				{ m_MassMax = val; }

	INT32				GetInitialObjectCount(void)				{ return m_InitialObjectCount; }
	void				SetInitialObjectCount( INT32 count)		{ m_InitialObjectCount = count; }

	INT32				GetMaxObjectCount(void)					{ return m_MaxObjectCount; }
	void				SetMaxObjectCount( INT32 count)			{ m_MaxObjectCount = count; }

	INT32				GetMaxObjectCapacity(void) { return m_MaxObjectCapacity; }
	void				SetMaxObjectCapacity(INT32 count) { m_MaxObjectCapacity = count; }

	INT32				GetEmitCount(void)						{ return m_EmitCount; }
	void				SetEmitCount( INT32 count)				{ m_EmitCount = count; }

	bool				IsVolatile(void)						{ return m_bVolatile; }
	void				SetVolatile( bool bFlag)				{ m_bVolatile = bFlag; }

	EMIT_TYPE			GetEmitType(void)						{ return m_EmitType; }
	void				SetEmitType( EMIT_TYPE type)			{ m_EmitType = type; }

	VEC3D *			GetEmitPosition(void)					{ return & m_EmitPos; }
	void				SetEmitPosition( VEC3D * pVec)		{ i3Vector::Copy( &m_EmitPos, pVec); }
	void				SetEmitPosition( REAL32 x, REAL32 y, REAL32 z)
	{
		i3Vector::Set( &m_EmitPos, x, y, z);
	}

	VEC3D *			GetEmitDirection(void)					{ return & m_EmitDir; }
	void				SetEmitDirection( VEC3D * pVec)			
	{ 
		i3Vector::Normalize( &m_EmitDir, pVec); 
	}

	void				SetEmitDirection( REAL32 x, REAL32 y, REAL32 z)
	{
		VEC3D		dir;
		i3Vector::Set( &dir, x, y, z);

		SetEmitDirection( &dir);
	}

	REAL32			GetEmitAngle(void)						{ return m_EmitAngle; }
	void				SetEmitAngle( REAL32 val)			{ m_EmitAngle = val; }

	REAL32			GetEmitInterval(void)					{ return m_EmitInterval; }
	void				SetEmitInterval( REAL32 tm)			{ m_EmitInterval = tm; }

	VEC3D *			GetUpVector(void)						{ return & m_UpDir;	}
	void				SetUpVector( VEC3D * pVec)			{ i3Vector::Copy( & m_UpDir, pVec);	}
	void				SetUpVector( REAL32 x, REAL32 y, REAL32 z)
	{
		i3Vector::Set( &m_UpDir, x, y, z);
	}

	///////////////////////////////////////////////////////////////////////////////
	// Position
	UINT32				GetPositionStyle(void)						{ return m_PosStyle; }
	void				SetPositionStyle( UINT32 style)				{ m_PosStyle = style; }

	REAL32			GetPositionUpdateInterval(void)				{ return m_PosUpdateInterval; }
	void				SetPositionUpdateInterval( REAL32 val)	{ m_PosUpdateInterval  = val; }

	FUNC_TYPE			GetPositionFunc( INT32 idx)					{ return m_PosFunc[idx]; }
	void				SetPositionFunc( INT32 idx, FUNC_TYPE func)	{ m_PosFunc[idx] = func;	}

	REAL32			GetPositionInit( INT32 idx)					{ return m_PosInit[idx]; }
	void				SetPositionInit( INT32 idx, REAL32 val)	{ m_PosInit[idx] = val; }

	REAL32			GetPositionFactor( INT32 idx)				{ return m_PosFactor[idx]; }
	void				SetPositionFactor( INT32 idx, REAL32 val)	{ m_PosFactor[idx] = val; }

	i3KeyframeTable *	GetPositionTable( INT32 idx)				{ return m_PosTable[idx]; }
	void				SetPositionTable( INT32 idx, i3KeyframeTable * pTable);

	REAL32			GetPositionMin(void)						{ return m_PosMin; }
	void				SetPositionMin( REAL32 val)				{ m_PosMin = val; }

	REAL32			GetPositionMax(void)						{ return m_PosMax; }
	void				SetPositionMax( REAL32 val)				{ m_PosMax = val; }

	REAL32			GetPositionInitialRandomFactor(void)		{ return m_PosInitialRandomFactor; }
	void				SetPositionInitialRandomFactor( REAL32 val)	{ m_PosInitialRandomFactor = val; }

	VEC3D *			GetPositionRandomFactor(void)				{ return &m_PosRandomFactor; }
	void				SetPositionRandomFactor( VEC3D * pVec)
	{
		i3Vector::Copy( &m_PosRandomFactor, pVec);
	}
	void				SetPositionRandomFactor( REAL32 x, REAL32 y, REAL32 z)
	{
		i3Vector::Set( &m_PosRandomFactor, x, y, z);
	}

	REAL32			GetPositionRandomInterval(void)				{ return m_PosRandomInterval; }
	void				SetPositionRandomInterval( REAL32 val)		{ m_PosRandomInterval = val; }

	///////////////////////////////////////////////////////////////////////////////
	// Scale
	UINT32				GetScaleStyle(void)							{ return m_ScaleStyle; }
	void				SetScaleStyle( UINT32 style)				{ m_ScaleStyle = style; }

	REAL32			GetScaleUpdateInterval(void)				{ return m_ScaleUpdateInterval; }
	void				SetScaleUpdateInterval( REAL32 val)			{ m_ScaleUpdateInterval = val; }

	FUNC_TYPE			GetScaleFunc( INT32 idx)					{ return m_ScaleFunc[ idx]; }
	void				SetScaleFunc( INT32 idx, FUNC_TYPE func)
	{
		m_ScaleFunc[idx] = func;
	}

	REAL32			GetScaleInit( INT32 idx)					{ return m_ScaleInit[idx]; }
	void				SetScaleInit( INT32 idx, REAL32 val)		{ m_ScaleInit[idx] = val; }

	REAL32			GetScaleFactor( INT32 idx)					{ return m_ScaleFactor[idx]; }
	void				SetScaleFactor( INT32 idx, REAL32 val)	{ m_ScaleFactor[idx] = val; }

	i3KeyframeTable *	GetScaleTable( INT32 idx)					{ return m_ScaleTable[idx]; }
	void				SetScaleTable( INT32 idx, i3KeyframeTable * pTable);

	VEC3D *			GetScaleMask(void)							{ return &m_ScaleMask; }
	void				SetScaleMask( VEC3D * pVec)				{ i3Vector::Copy( &m_ScaleMask, pVec); }
	void				SetScaleMask( REAL32 x, REAL32 y, REAL32 z)
	{
		i3Vector::Set( &m_ScaleMask, x, y, z);
	}

	REAL32			GetScaleInitialRandomFactor(void)			{ return m_ScaleInitialRandomFactor; }
	void				SetScaleInitialRandomFactor( REAL32 val)	{ m_ScaleInitialRandomFactor = val; }

	REAL32			GetScaleMin(void)							{ return m_ScaleMin; }
	void				SetScaleMin( REAL32 val)					{ m_ScaleMin = val; }
	
	REAL32			GetScaleMax(void)							{ return m_ScaleMax; }
	void				SetScaleMax( REAL32 val)					{ m_ScaleMax = val; }

	REAL32			GetScaleBillboardFactor(void)				{ return m_ScaleBillboardFactor; }
	void				SetScaleBillboardFactor( REAL32 val)		{ m_ScaleBillboardFactor = val; }

	///////////////////////////////////////////////////////////////////////////////
	// Rotation
	UINT32				GetRotationStyle(void)						{ return m_RotateStyle; }
	void				SetRotationStyle( UINT32 style)				{ m_RotateStyle = style; }

	REAL32			GetRotateUpdateInterval(void)				{ return m_RotateUpdateInterval; }
	void				SetRotateUpdateInterval( REAL32 val)		{ m_RotateUpdateInterval = val; }

	REAL32			GetRotateRandomFactor(void)					{ return m_RotateRandomFactor; }
	void				SetRotateRandomFactor( REAL32 val)		{ m_RotateRandomFactor = val; }

	FUNC_TYPE			GetRotationFunc(void)						{ return m_RotateFunc; }
	void				SetRotationFunc( FUNC_TYPE func)			{ m_RotateFunc = func; }

	REAL32			GetRotationInit(void)						{ return m_RotateInit; }
	void				SetRotationInit( REAL32 val)				{ m_RotateInit = val; }

	REAL32			GetRotationFactor(void)						{ return m_RotateFactor; }
	void				SetRotationFactor( REAL32 val)			{ m_RotateFactor = val; }

	i3KeyframeTable *	GetRotationTable(void)						{ return m_pRotateTable; }
	void				SetRotationTable( i3KeyframeTable * pTable);

	///////////////////////////////////////////////////////////////////////////////
	// Color (RGB)
	UINT32				GetColorStyle(void)							{ return m_ColorStyle; }
	void				SetColorStyle( UINT32 val)					{ m_ColorStyle = val; }

	REAL32			GetColorUpdateInterval(void)				{ return m_ColorUpdateInterval; }
	void				SetColorUpdateInterval( REAL32 val)		{ m_ColorUpdateInterval = val; }

	FUNC_TYPE			GetRGBFunc(void)							{ return m_RGBFunc; }
	void				SetRGBFunc( FUNC_TYPE type)					{ m_RGBFunc = type; }

	UINT32				GetRGBInit(void)						{ return m_RGBInit; }
	void				SetRGBInit( UINT32 val)					{ m_RGBInit = val; }

	i3KeyframeTable *	GetRTable(void)								{ return m_pRTable; }
	void				SetRTable( i3KeyframeTable * pTable);
	i3KeyframeTable *	GetGTable(void)								{ return m_pGTable; }
	void				SetGTable( i3KeyframeTable * pTable);
	i3KeyframeTable *	GetBTable(void)								{ return m_pBTable; }
	void				SetBTable( i3KeyframeTable * pTable);

	///////////////////////////////////////////////////////////////////////////////
	// Color (Alpha)
	FUNC_TYPE			GetAlphaFunc(void)							{ return m_AlphaFunc; }
	void				SetAlphaFunc( FUNC_TYPE func)				{ m_AlphaFunc = func; }

	UINT32				GetAlphaInit(void)							{ return m_AlphaInit; }
	void				SetAlphaInit( UINT32 val)					{ m_AlphaInit = val; }

	i3KeyframeTable *	GetAlphaTable(void)							{ return m_pATable; }
	void				SetAlphaTable( i3KeyframeTable * pTable);

	i3MeshParticle *	GetMeshParticle()							{ return m_pMeshParticle; }
	void				SetMeshParticle(i3MeshParticle * pMesh);
	void				DestroyMeshParticle();
	void				BuildMeshParticle(i3Node * pNode);

	virtual void		CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual void		OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual	UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
	virtual bool		OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool		OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif
