#if !defined( __I3_PARTICLE_INFO_H)
#define __I3_PARTICLE_INFO_H

#include "i3Base.h"
#include "i3MathType.h"
#include "i3KeyframeTable.h"
#include "i3Vector.h"
#include "i3FixedPointVector.h"

#define	I3_PARTICLE_PARAM_STYLE_RANDOM				0x00000001
#define I3_PARTICLE_PARAM_STYLE_INTERPOLATE			0x00000002
#define I3_PARTICLE_PARAM_STYLE_CLAMP				0x00000004
#define I3_PARTICLE_PARAM_STYLE_GRAVITY				0x00000010
#define I3_PARTICLE_PARAM_STYLE_RANDOM_INTERPOLATE	0x00000020

#define	I3_PARTICLE_TYPE_MASK_ROTATE		0x80000000
#define	I3_PARTICLE_TYPE_MASK_TRACE_POS		0x40000000
#define	I3_PARTICLE_TYPE_MASK_TRACE_SCALE	0x20000000

#define FUNC_COUNT							2

class I3_EXPORT_MATH i3ParticleInfo : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3ParticleInfo);
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
	PARTICLE_TYPE		m_Type;
	RT_REAL32			m_LifeTime;
	RT_REAL32			m_ObjectLifeTime;

	RT_REAL32			m_MassMin;
	RT_REAL32			m_MassMax;

	INT32				m_InitialObjectCount;
	INT32				m_MaxObjectCount;
	INT32				m_EmitCount;
	BOOL				m_bVolatile;

	// Emitter
	EMIT_TYPE			m_EmitType;
	RT_VEC3D			m_EmitPos;
	RT_VEC3D			m_EmitDir;
	RT_REAL32			m_EmitAngle;
	RT_REAL32			m_EmitInterval;
	RT_VEC3D			m_UpDir;

	UINT32				m_PosStyle;
	RT_REAL32			m_PosUpdateInterval;
	FUNC_TYPE			m_PosFunc[ FUNC_COUNT];
	RT_REAL32			m_PosInit[ FUNC_COUNT];
	RT_REAL32			m_PosFactor[ FUNC_COUNT];
	i3KeyframeTable *	m_PosTable[ FUNC_COUNT];
	RT_REAL32			m_PosMin;
	RT_REAL32			m_PosMax;
	RT_REAL32			m_PosInitialRandomFactor;
	RT_VEC3D			m_PosRandomFactor;
	RT_REAL32			m_PosRandomInterval;

	UINT32				m_ScaleStyle;
	RT_REAL32			m_ScaleUpdateInterval;
	FUNC_TYPE			m_ScaleFunc[ FUNC_COUNT];
	RT_REAL32			m_ScaleInit[ FUNC_COUNT];
	RT_REAL32			m_ScaleFactor[ FUNC_COUNT];
	i3KeyframeTable *	m_ScaleTable[ FUNC_COUNT];
	RT_REAL32			m_ScaleInitialRandomFactor;
	RT_REAL32			m_ScaleMin;
	RT_REAL32			m_ScaleMax;
	RT_VEC3D			m_ScaleMask;
	RT_REAL32			m_ScaleBillboardFactor;

	UINT32				m_RotateStyle;
	RT_REAL32			m_RotateUpdateInterval;
	RT_REAL32			m_RotateRandomFactor;
	FUNC_TYPE			m_RotateFunc;				// Place Holder
	RT_REAL32			m_RotateInit;				// Place Holder
	RT_REAL32			m_RotateFactor;				// Place Holder
	i3KeyframeTable *	m_pRotateTable;				// Place Holder
	RT_REAL32			m_RotateLength;				// Place Holder
	RT_REAL32			m_RotateMin;				// Place Holder
	RT_REAL32			m_RotateMax;				// Place Holder

	UINT32				m_ColorStyle;
	RT_REAL32			m_ColorUpdateInterval;
	FUNC_TYPE			m_RGBFunc;
	UINT32				m_RGBInit;
	i3KeyframeTable *	m_pRTable;
	i3KeyframeTable *	m_pGTable;
	i3KeyframeTable *	m_pBTable;
	i3KeyframeTable *	m_pATable;

	FUNC_TYPE			m_AlphaFunc;
	UINT32				m_AlphaInit;

public:
	i3ParticleInfo(void);
	virtual ~i3ParticleInfo(void);

	// Persist Data
	PARTICLE_TYPE		GetType(void)							{ return m_Type; }
	void				SetType( PARTICLE_TYPE type)			{ m_Type = type; }

	RT_REAL32			GetLifeTime(void)						{ return m_LifeTime; }
	void				SetLifeTime( RT_REAL32 tm)				{ m_LifeTime = tm; }

	RT_REAL32			GetObjectLifeTime(void)					{ return m_ObjectLifeTime; }
	void				SetObjectLifeTime( RT_REAL32 tm)		{ m_ObjectLifeTime = tm; }

	RT_REAL32			GetMassMin(void)						{ return m_MassMin; }
	void				SetMassMin( RT_REAL32 val)				{ m_MassMin = val; }

	RT_REAL32			GetMassMax(void)						{ return m_MassMax; }
	void				SetMassMax( RT_REAL32 val)				{ m_MassMax = val; }

	INT32				GetInitialObjectCount(void)				{ return m_InitialObjectCount; }
	void				SetInitialObjectCount( INT32 count)		{ m_InitialObjectCount = count; }

	INT32				GetMaxObjectCount(void)					{ return m_MaxObjectCount; }
	void				SetMaxObjectCount( INT32 count)			{ m_MaxObjectCount = count; }

	INT32				GetEmitCount(void)						{ return m_EmitCount; }
	void				SetEmitCount( INT32 count)				{ m_EmitCount = count; }

	BOOL				IsVolatile(void)						{ return m_bVolatile; }
	void				SetVolatile( BOOL bFlag)				{ m_bVolatile = bFlag; }

	EMIT_TYPE			GetEmitType(void)						{ return m_EmitType; }
	void				SetEmitType( EMIT_TYPE type)			{ m_EmitType = type; }

	RT_VEC3D *			GetEmitPosition(void)					{ return & m_EmitPos; }
	void				SetEmitPosition( RT_VEC3D * pVec)		{ i3Vector::Copy( &m_EmitPos, pVec); }
	void				SetEmitPosition( RT_REAL32 x, RT_REAL32 y, RT_REAL32 z)
	{
		i3Vector::Set( &m_EmitPos, x, y, z);
	}

	RT_VEC3D *			GetEmitDirection(void)					{ return & m_EmitDir; }
	void				SetEmitDirection( RT_VEC3D * pVec)			
	{ 
		i3Vector::Normalize( &m_EmitDir, pVec); 
	}

	void				SetEmitDirection( RT_REAL32 x, RT_REAL32 y, RT_REAL32 z)
	{
		RT_VEC3D		dir;
		i3Vector::Set( &dir, x, y, z);

		SetEmitDirection( &dir);
	}

	RT_REAL32			GetEmitAngle(void)						{ return m_EmitAngle; }
	void				SetEmitAngle( RT_REAL32 val)			{ m_EmitAngle = val; }

	RT_REAL32			GetEmitInterval(void)					{ return m_EmitInterval; }
	void				SetEmitInterval( RT_REAL32 tm)			{ m_EmitInterval = tm; }

	RT_VEC3D *			GetUpVector(void)						{ return & m_UpDir;	}
	void				SetUpVector( RT_VEC3D * pVec)			{ i3Vector::Copy( & m_UpDir, pVec);	}
	void				SetUpVector( RT_REAL32 x, RT_REAL32 y, RT_REAL32 z)
	{
		i3Vector::Set( &m_UpDir, x, y, z);
	}

	///////////////////////////////////////////////////////////////////////////////
	// Position
	UINT32				GetPositionStyle(void)						{ return m_PosStyle; }
	void				SetPositionStyle( UINT32 style)				{ m_PosStyle = style; }

	RT_REAL32			GetPositionUpdateInterval(void)				{ return m_PosUpdateInterval; }
	void				SetPositionUpdateInterval( RT_REAL32 val)	{ m_PosUpdateInterval  = val; }

	FUNC_TYPE			GetPositionFunc( INT32 idx)					{ return m_PosFunc[idx]; }
	void				SetPositionFunc( INT32 idx, FUNC_TYPE func)	{ m_PosFunc[idx] = func;	}

	RT_REAL32			GetPositionInit( INT32 idx)					{ return m_PosInit[idx]; }
	void				SetPositionInit( INT32 idx, RT_REAL32 val)	{ m_PosInit[idx] = val; }

	RT_REAL32			GetPositionFactor( INT32 idx)				{ return m_PosFactor[idx]; }
	void				SetPositionFactor( INT32 idx, RT_REAL32 val)	{ m_PosFactor[idx] = val; }

	i3KeyframeTable *	GetPositionTable( INT32 idx)				{ return m_PosTable[idx]; }
	void				SetPositionTable( INT32 idx, i3KeyframeTable * pTable);

	RT_REAL32			GetPositionMin(void)						{ return m_PosMin; }
	void				SetPositionMin( RT_REAL32 val)				{ m_PosMin = val; }

	RT_REAL32			GetPositionMax(void)						{ return m_PosMax; }
	void				SetPositionMax( RT_REAL32 val)				{ m_PosMax = val; }

	RT_REAL32			GetPositionInitialRandomFactor(void)		{ return m_PosInitialRandomFactor; }
	void				SetPositionInitialRandomFactor( RT_REAL32 val)	{ m_PosInitialRandomFactor = val; }

	RT_VEC3D *			GetPositionRandomFactor(void)				{ return &m_PosRandomFactor; }
	void				SetPositionRandomFactor( RT_VEC3D * pVec)
	{
		i3Vector::Copy( &m_PosRandomFactor, pVec);
	}
	void				SetPositionRandomFactor( RT_REAL32 x, RT_REAL32 y, RT_REAL32 z)
	{
		i3Vector::Set( &m_PosRandomFactor, x, y, z);
	}

	RT_REAL32			GetPositionRandomInterval(void)				{ return m_PosRandomInterval; }
	void				SetPositionRandomInterval( RT_REAL32 val)		{ m_PosRandomInterval = val; }

	///////////////////////////////////////////////////////////////////////////////
	// Scale
	UINT32				GetScaleStyle(void)							{ return m_ScaleStyle; }
	void				SetScaleStyle( UINT32 style)				{ m_ScaleStyle = style; }

	RT_REAL32			GetScaleUpdateInterval(void)				{ return m_ScaleUpdateInterval; }
	void				SetScaleUpdateInterval( RT_REAL32 val)			{ m_ScaleUpdateInterval = val; }

	FUNC_TYPE			GetScaleFunc( INT32 idx)					{ return m_ScaleFunc[ idx]; }
	void				SetScaleFunc( INT32 idx, FUNC_TYPE func)
	{
		m_ScaleFunc[idx] = func;
	}

	RT_REAL32			GetScaleInit( INT32 idx)					{ return m_ScaleInit[idx]; }
	void				SetScaleInit( INT32 idx, RT_REAL32 val)		{ m_ScaleInit[idx] = val; }

	RT_REAL32			GetScaleFactor( INT32 idx)					{ return m_ScaleFactor[idx]; }
	void				SetScaleFactor( INT32 idx, RT_REAL32 val)	{ m_ScaleFactor[idx] = val; }

	i3KeyframeTable *	GetScaleTable( INT32 idx)					{ return m_ScaleTable[idx]; }
	void				SetScaleTable( INT32 idx, i3KeyframeTable * pTable);

	RT_VEC3D *			GetScaleMask(void)							{ return &m_ScaleMask; }
	void				SetScaleMask( RT_VEC3D * pVec)				{ i3Vector::Copy( &m_ScaleMask, pVec); }
	void				SetScaleMask( RT_REAL32 x, RT_REAL32 y, RT_REAL32 z)
	{
		i3Vector::Set( &m_ScaleMask, x, y, z);
	}

	RT_REAL32			GetScaleInitialRandomFactor(void)			{ return m_ScaleInitialRandomFactor; }
	void				SetScaleInitialRandomFactor( RT_REAL32 val)	{ m_ScaleInitialRandomFactor = val; }

	RT_REAL32			GetScaleMin(void)							{ return m_ScaleMin; }
	void				SetScaleMin( RT_REAL32 val)					{ m_ScaleMin = val; }
	
	RT_REAL32			GetScaleMax(void)							{ return m_ScaleMax; }
	void				SetScaleMax( RT_REAL32 val)					{ m_ScaleMax = val; }

	RT_REAL32			GetScaleBillboardFactor(void)				{ return m_ScaleBillboardFactor; }
	void				SetScaleBillboardFactor( RT_REAL32 val)		{ m_ScaleBillboardFactor = val; }

	///////////////////////////////////////////////////////////////////////////////
	// Rotation
	UINT32				GetRotationStyle(void)						{ return m_RotateStyle; }
	void				SetRotationStyle( UINT32 style)				{ m_RotateStyle = style; }

	RT_REAL32			GetRotateUpdateInterval(void)				{ return m_RotateUpdateInterval; }
	void				SetRotateUpdateInterval( RT_REAL32 val)		{ m_RotateUpdateInterval = val; }

	RT_REAL32			GetRotateRandomFactor(void)					{ return m_RotateRandomFactor; }
	void				SetRotateRandomFactor( RT_REAL32 val)		{ m_RotateRandomFactor = val; }

	FUNC_TYPE			GetRotationFunc(void)						{ return m_RotateFunc; }
	void				SetRotationFunc( FUNC_TYPE func)			{ m_RotateFunc = func; }

	RT_REAL32			GetRotationInit(void)						{ return m_RotateInit; }
	void				SetRotationInit( RT_REAL32 val)				{ m_RotateInit = val; }

	RT_REAL32			GetRotationFactor(void)						{ return m_RotateFactor; }
	void				SetRotationFactor( RT_REAL32 val)			{ m_RotateFactor = val; }

	i3KeyframeTable *	GetRotationTable(void)						{ return m_pRotateTable; }
	void				SetRotationTable( i3KeyframeTable * pTable);

	///////////////////////////////////////////////////////////////////////////////
	// Color (RGB)
	UINT32				GetColorStyle(void)							{ return m_ColorStyle; }
	void				SetColorStyle( UINT32 val)					{ m_ColorStyle = val; }

	RT_REAL32			GetColorUpdateInterval(void)				{ return m_ColorUpdateInterval; }
	void				SetColorUpdateInterval( RT_REAL32 val)		{ m_ColorUpdateInterval = val; }

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

	virtual void		CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual void		OnBuildObjectList( i3List * pList);
	virtual	UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
