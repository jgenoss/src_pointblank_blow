#include "i3SceneDef.h"
#include "i3ParticleInfo.h"

I3_CLASS_INSTANCE( i3ParticleInfo);

i3ParticleInfo::~i3ParticleInfo(void)
{
	INT32 i;

	for( i = 0; i < FUNC_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_PosTable[i]);
		I3_SAFE_RELEASE( m_ScaleTable[i]);
	}

	I3_SAFE_RELEASE( m_pRotateTable);

	I3_SAFE_RELEASE( m_pRTable);
	I3_SAFE_RELEASE( m_pGTable);
	I3_SAFE_RELEASE( m_pBTable);
	I3_SAFE_RELEASE( m_pATable);

	I3_SAFE_RELEASE(m_pMeshParticle);
}

void i3ParticleInfo::SetPositionTable( INT32 idx, i3KeyframeTable * pTable)
{
	I3ASSERT( (idx >= 0) && (idx < FUNC_COUNT));

	I3_REF_CHANGE( m_PosTable[idx], pTable);
}

void i3ParticleInfo::SetScaleTable( INT32 idx, i3KeyframeTable * pTable)
{
	I3ASSERT( (idx >= 0) && (idx < FUNC_COUNT));

	I3_REF_CHANGE( m_ScaleTable[idx], pTable);
}

void i3ParticleInfo::SetRotationTable( i3KeyframeTable * pTable)
{
	I3_REF_CHANGE( m_pRotateTable, pTable);
}

void i3ParticleInfo::SetRTable( i3KeyframeTable * pTable)
{
	I3_REF_CHANGE( m_pRTable, pTable);
}

void i3ParticleInfo::SetGTable( i3KeyframeTable * pTable)
{
	I3_REF_CHANGE( m_pGTable, pTable);
}

void i3ParticleInfo::SetBTable( i3KeyframeTable * pTable)
{
	I3_REF_CHANGE( m_pBTable, pTable);
}

void i3ParticleInfo::SetAlphaTable( i3KeyframeTable * pTable)
{
	I3_REF_CHANGE( m_pATable, pTable);
}

void i3ParticleInfo::SetMeshParticle( i3MeshParticle * pMesh)
{
	I3_REF_CHANGE( m_pMeshParticle, pMesh);
}

void i3ParticleInfo::BuildMeshParticle(i3Node * pNode)
{
	if(pNode == nullptr)
	{
		I3ASSERT_0;
	}

	if(m_pMeshParticle == nullptr)
	{
		m_pMeshParticle = i3MeshParticle::new_object();
	}

	m_pMeshParticle->BuildMeshObject(pNode);
}

void i3ParticleInfo::DestroyMeshParticle()
{
	I3_SAFE_RELEASE(m_pMeshParticle);
}

void i3ParticleInfo::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	INT32 i;

	for( i = 0; i < FUNC_COUNT; i++)
	{
		if( m_PosTable[i] != nullptr)
			m_PosTable[i]->OnBuildObjectList( List);

		if( m_ScaleTable[i] != nullptr)
			m_ScaleTable[i]->OnBuildObjectList( List);

		if( m_pRTable != nullptr)
			m_pRTable->OnBuildObjectList( List);

		if( m_pGTable != nullptr)
			m_pGTable->OnBuildObjectList( List);

		if( m_pBTable != nullptr)
			m_pBTable->OnBuildObjectList( List);

		if( m_pATable != nullptr)
			m_pATable->OnBuildObjectList( List);
	}

	if( m_pRotateTable != nullptr)
	{
		m_pRotateTable->OnBuildObjectList( List);
	}

	if( m_pMeshParticle != nullptr)
	{
		m_pMeshParticle->OnBuildObjectList(List);
	}

	i3PersistantElement::OnBuildObjectList( List);
}

void i3ParticleInfo::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	UINT32 i;
	i3ParticleInfo * pDest = (i3ParticleInfo *) pObj;

	pDest->SetType( GetType());
	pDest->SetLifeTime( GetLifeTime());
	pDest->SetObjectLifeTime( GetObjectLifeTime());
	pDest->SetMassMin( GetMassMin());
	pDest->SetMassMax( GetMassMax());
	pDest->SetInitialObjectCount( GetInitialObjectCount());
	pDest->SetMaxObjectCount( GetMaxObjectCount());
	pDest->SetMaxObjectCapacity(GetMaxObjectCapacity());
	pDest->SetEmitCount( GetEmitCount());
	pDest->SetVolatile( IsVolatile());
	pDest->SetEmitType( GetEmitType());
	pDest->SetEmitPosition( GetEmitPosition());
	pDest->SetEmitDirection( GetEmitDirection());
	pDest->SetEmitAngle( GetEmitAngle());
	pDest->SetEmitInterval( GetEmitInterval());
	pDest->SetUpVector( GetUpVector());

	// Position
	pDest->SetPositionStyle( GetPositionStyle());
	pDest->SetPositionUpdateInterval( GetPositionUpdateInterval());
	pDest->SetPositionMin( GetPositionMin());
	pDest->SetPositionMax( GetPositionMax());
	pDest->SetPositionInitialRandomFactor( GetPositionInitialRandomFactor());
	pDest->SetPositionRandomFactor( GetPositionRandomFactor());
	pDest->SetPositionRandomInterval( GetPositionRandomInterval());

	// Scale
	pDest->SetScaleStyle( GetScaleStyle());
	pDest->SetScaleUpdateInterval( GetScaleUpdateInterval());
	pDest->SetScaleMask( GetScaleMask());
	pDest->SetScaleInitialRandomFactor( GetScaleInitialRandomFactor());
	pDest->SetScaleMin( GetScaleMin());
	pDest->SetScaleMax( GetScaleMax());
	pDest->SetScaleBillboardFactor( GetScaleBillboardFactor());

	// Rotation
	pDest->SetRotationStyle( GetRotationStyle());
	pDest->SetRotateUpdateInterval( GetRotateUpdateInterval());
	pDest->SetRotateRandomFactor( GetRotateRandomFactor());
	pDest->SetRotationFunc( GetRotationFunc());
	pDest->SetRotationInit( GetRotationInit());
	pDest->SetRotationFactor( GetRotationFactor());
	
	// Color
	pDest->SetColorStyle( GetColorStyle());
	pDest->SetColorUpdateInterval( GetColorUpdateInterval());
	pDest->SetRGBFunc( GetRGBFunc());
	pDest->SetRGBInit( GetRGBInit());
	pDest->SetAlphaFunc( GetAlphaFunc());
	pDest->SetAlphaInit( GetAlphaInit());

	if( method == I3_COPY_REF)
	{
		pDest->SetRotationTable( GetRotationTable());
		pDest->SetRTable( GetRTable());
		pDest->SetGTable( GetGTable());
		pDest->SetBTable( GetBTable());
		pDest->SetAlphaTable( GetAlphaTable());
	}
	else
	{
		i3KeyframeTable * pNew;

		if( GetRotationTable() != nullptr)
		{
			pNew = i3KeyframeTable::new_object_ref();
			GetRotationTable()->CopyTo( pNew, method);
			pDest->SetRotationTable( pNew);
		}

		if( GetRTable() != nullptr)
		{
			pNew = i3KeyframeTable::new_object_ref();
			GetRTable()->CopyTo( pNew, method);
			pDest->SetRTable( pNew);
		}

		if( GetGTable() != nullptr)
		{
			pNew = i3KeyframeTable::new_object_ref();
			GetGTable()->CopyTo( pNew, method);
			pDest->SetGTable( pNew);
		}

		if( GetBTable() != nullptr)
		{
			pNew = i3KeyframeTable::new_object_ref();
			GetBTable()->CopyTo( pNew, method);
			pDest->SetBTable( pNew);
		}

		if( GetAlphaTable() != nullptr)
		{
			pNew = i3KeyframeTable::new_object_ref();
			GetAlphaTable()->CopyTo( pNew, method);
			pDest->SetAlphaTable( pNew);
		}
	}

	for( i = 0; i < FUNC_COUNT; i++)
	{
		// Position
		pDest->SetPositionFunc( i, GetPositionFunc( i));
		pDest->SetPositionInit( i, GetPositionInit( i));
		pDest->SetPositionFactor( i, GetPositionFactor( i));

		// Scale
		pDest->SetScaleFunc( i, GetScaleFunc( i));
		pDest->SetScaleInit( i, GetScaleInit( i));
		pDest->SetScaleFactor( i, GetScaleFactor( i));

		if( method == I3_COPY_REF)
		{
			pDest->SetPositionTable( i, GetPositionTable(i));
			pDest->SetScaleTable( i, GetScaleTable(i));
		}
		else
		{
			i3KeyframeTable * pNew;

			if( GetPositionTable(i) != nullptr)
			{
				pNew = i3KeyframeTable::new_object_ref();

				GetPositionTable(i)->CopyTo( pNew, method);

				pDest->SetPositionTable( i, pNew);
			}

			if( GetScaleTable(i) != nullptr)
			{
				pNew = i3KeyframeTable::new_object_ref();

				GetScaleTable(i)->CopyTo( pNew, method);

				pDest->SetScaleTable( i, pNew);
			}
		}
	}

	pDest->SetMeshParticle(m_pMeshParticle);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 PARTICLE_INFO
	{
		char			m_ID[4] = { 'P', 'I', 'V', '1' };

		INT32			m_Type = 0;
		INT32			m_Dummy0 = 0;
		REAL32			m_LifeTime = 0.0f;
		REAL32			m_ObjectLifeTime = 0.0f;

		REAL32			m_MassMin = 0.0f;
		REAL32			m_MassMax = 0.0f;

		INT32			m_InitialObjectCount = 0;
		INT32			m_MaxObjectCount = 0;
		INT32			m_EmitCount = 0;
		BOOL			m_bVolatile = FALSE;


		INT32			m_EmitType = 0;
		i3::pack::VEC3D	m_EmitPos;
		i3::pack::VEC3D	m_EmitDir;
		REAL32			m_EmitAngle = 0.0f;
		REAL32			m_EmitInterval = 0.0f;
		i3::pack::VEC3D	m_UpDir;

		REAL32			m_PosRandomInterval = 0.0f;
		i3::pack::VEC3D	m_PosRandomFactor;

		UINT32			m_PosStyle = 0;
		INT32			m_PosFunc[4] = { 0 };
		REAL32			m_PosInit[4] = { 0.0f };
		REAL32			m_PosFactor[4] = { 0.0f };
		REAL32			m_Dummy1 = 0.0f;
		OBJREF			m_PosTable[4] = { 0 };
		REAL32			m_PosMin = 0.0f;
		REAL32			m_PosMax = 0.0f;

		UINT32			m_ScaleStyle = 0;
		INT32			m_ScaleFunc[4] = { 0 };
		REAL32			m_ScaleInit[4] = { 0.0f };
		REAL32			m_ScaleFactor[4] = { 0.0f };
		OBJREF			m_ScaleTable[4] = { 0 };
		REAL32			m_ScaleMin = 0.0f;
		REAL32			m_ScaleMax = 0.0f;

		UINT32			m_RotateStyle = 0;
		INT32			m_RotateFunc = 0;				// Place Holder
		REAL32			m_RotateInit = 0.0f;				// Place Holder
		REAL32			m_RotateFactor = 0.0f;				// Place Holder
		OBJREF			m_pRotateTable = 0;				// Place Holder
		REAL32			m_RotateLength = 0.0f;				// Place Holder
		REAL32			m_RotateMin = 0.0f;				// Place Holder
		REAL32			m_RotateMax = 0.0f;				// Place Holder

		UINT32			m_ColorStyle = 0;
		INT32			m_RGBFunc = 0;
		UINT32			m_RGBInit = 0;
		OBJREF			m_pRTable = 0;
		OBJREF			m_pGTable = 0;
		OBJREF			m_pBTable = 0;
		OBJREF			m_pATable = 0;

		INT32			m_AlphaFunc = 0;
		UINT32			m_AlphaInit = 0;

		REAL32			m_PosUpdateInterval = 0.0f;
		REAL32			m_ScaleUpdateInterval = 0.0f;
		REAL32			m_RotateUpdateInterval = 0.0f;
		REAL32			m_ColorUpdateInterval = 0.0f;

		i3::pack::VEC3D	m_ScaleMask;
		REAL32			m_RotateRandomFactor = 0.0f;

		REAL32			m_PosInitialRandomFactor = 0.0f;
		REAL32			m_ScaleInitialRandomFactor = 0.0f;

		REAL32			m_ScaleBillboardFactor = 0.0f;

		OBJREF			m_MeshParticleInfo = 0;

		UINT32			m_MaxObjectCapacity = 0;		// 최대 표현 파티클 갯수 따로 정해주도록 함..

		UINT8			m_Padding[76] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)					// [test required] 중복 push되었습니다. pop으로 수정. 2017.02.09 soon9
#endif

UINT32 i3ParticleInfo::OnSave( i3ResourceFile * pResFile)
{
	INT32 i;
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::PARTICLE_INFO Info;
	
	Info.m_Type					= (INT32) m_Type;
	Info.m_LifeTime				= m_LifeTime;
	Info.m_ObjectLifeTime		= m_ObjectLifeTime;

	Info.m_MassMin				= m_MassMin;
	Info.m_MassMax				= m_MassMax;

	Info.m_InitialObjectCount	= m_InitialObjectCount;
	Info.m_MaxObjectCount		= m_MaxObjectCount;
	Info.m_EmitCount			= m_EmitCount;
	Info.m_bVolatile			= m_bVolatile;

	Info.m_EmitType				= m_EmitType;

	Info.m_EmitPos.x			= i3Vector::GetX( &m_EmitPos);
	Info.m_EmitPos.y			= i3Vector::GetY( &m_EmitPos);
	Info.m_EmitPos.z			= i3Vector::GetZ( &m_EmitPos);

	Info.m_EmitDir.x			= i3Vector::GetX( &m_EmitDir);
	Info.m_EmitDir.y			= i3Vector::GetY( &m_EmitDir);
	Info.m_EmitDir.z			= i3Vector::GetZ( &m_EmitDir);

	Info.m_EmitAngle			= m_EmitAngle;

	Info.m_EmitInterval			= m_EmitInterval;

	Info.m_UpDir.x				= i3Vector::GetX( &m_UpDir);
	Info.m_UpDir.y				= i3Vector::GetY( &m_UpDir);
	Info.m_UpDir.z				= i3Vector::GetZ( &m_UpDir);

	Info.m_PosStyle				= m_PosStyle;
	Info.m_PosUpdateInterval	= m_PosUpdateInterval;
	Info.m_PosRandomInterval	= m_PosRandomInterval;
	Info.m_PosRandomFactor.x	= i3Vector::GetX( &m_PosRandomFactor);
	Info.m_PosRandomFactor.y	= i3Vector::GetY( &m_PosRandomFactor);
	Info.m_PosRandomFactor.z	= i3Vector::GetZ( &m_PosRandomFactor);
	Info.m_PosMin				= m_PosMin;
	Info.m_PosInitialRandomFactor = m_PosInitialRandomFactor;
	Info.m_PosMax				= m_PosMax;

	Info.m_ScaleStyle			= m_ScaleStyle;
	Info.m_ScaleUpdateInterval	= m_ScaleUpdateInterval;
	Info.m_ScaleMin				= m_ScaleMin;
	Info.m_ScaleMax				= m_ScaleMax;
	Info.m_ScaleInitialRandomFactor = m_ScaleInitialRandomFactor;
	Info.m_ScaleMask.x			= i3Vector::GetX( &m_ScaleMask);
	Info.m_ScaleMask.y			= i3Vector::GetY( &m_ScaleMask);
	Info.m_ScaleMask.z			= i3Vector::GetZ( &m_ScaleMask);
	Info.m_ScaleBillboardFactor	= m_ScaleBillboardFactor;

	Info.m_RotateStyle			= m_RotateStyle;
	Info.m_RotateUpdateInterval	= m_RotateUpdateInterval;
	Info.m_RotateFunc			= m_RotateFunc;
	Info.m_RotateInit			= m_RotateInit;
	Info.m_RotateFactor			= m_RotateFactor;
	Info.m_pRotateTable			= (OBJREF) pResFile->GetObjectPersistID( m_pRotateTable);
	Info.m_RotateLength			= m_RotateLength;
	Info.m_RotateMin			= m_RotateMin;
	Info.m_RotateMax			= m_RotateMax;
	Info.m_RotateRandomFactor	= m_RotateRandomFactor;

	Info.m_ColorStyle			= m_ColorStyle;
	Info.m_ColorUpdateInterval	= m_ColorUpdateInterval;
	Info.m_RGBFunc				= m_RGBFunc;
	Info.m_RGBInit				= m_RGBInit;

	Info.m_pRTable				= (OBJREF) pResFile->GetObjectPersistID( m_pRTable);
	Info.m_pGTable				= (OBJREF) pResFile->GetObjectPersistID( m_pGTable);
	Info.m_pBTable				= (OBJREF) pResFile->GetObjectPersistID( m_pBTable);
	Info.m_pATable				= (OBJREF) pResFile->GetObjectPersistID( m_pATable);
	Info.m_AlphaFunc			= m_AlphaFunc;
	Info.m_AlphaInit			= m_AlphaInit;

	for( i = 0; i < FUNC_COUNT; i++)
	{
		Info.m_PosFunc[i]		= m_PosFunc[i];
		Info.m_PosInit[i]		= m_PosInit[i];
		Info.m_PosFactor[i]		= m_PosFactor[i];
		Info.m_PosTable[i]		= (OBJREF)  pResFile->GetObjectPersistID( m_PosTable[i]);
		Info.m_ScaleFunc[i]		= m_ScaleFunc[i];
		Info.m_ScaleInit[i]		= m_ScaleInit[i];
		Info.m_ScaleFactor[i]	= m_ScaleFactor[i];
		Info.m_ScaleTable[ i]	= (OBJREF)  pResFile->GetObjectPersistID( m_ScaleTable[ i]);
	}

	Info.m_MeshParticleInfo = (OBJREF) pResFile->GetObjectPersistID(m_pMeshParticle);

	if (m_MaxObjectCapacity == 0)
		Info.m_MaxObjectCapacity = Info.m_MaxObjectCount;
	else
		Info.m_MaxObjectCapacity = m_MaxObjectCapacity;

	Rc = pStream->Write( &Info, sizeof(Info));

	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ParticleInfo::OnSave()", "Could not write particle information.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3ParticleInfo::OnLoad( i3ResourceFile * pResFile)
{
	INT32 i;
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::PARTICLE_INFO Info;
	i3KeyframeTable * pTable;
	i3MeshParticle * pMesh;

	Rc = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ParticleInfo::OnLoad()", "Could not read particle information.");
		return STREAM_ERR;
	}
	Result += Rc;

	m_Type							= (PARTICLE_TYPE) Info.m_Type;
	m_LifeTime						= Info.m_LifeTime;
	m_ObjectLifeTime				= Info.m_ObjectLifeTime;

	m_MassMin						= Info.m_MassMin;
	m_MassMax						= Info.m_MassMax;

	m_InitialObjectCount			= Info.m_InitialObjectCount;
	m_MaxObjectCount				= Info.m_MaxObjectCount;

	if (Info.m_MaxObjectCapacity == 0)
	{
		m_MaxObjectCapacity = m_MaxObjectCount;
	}
	else
	{
		m_MaxObjectCapacity = Info.m_MaxObjectCapacity;
	}

	m_EmitCount						= Info.m_EmitCount;
	m_bVolatile						= Info.m_bVolatile != 0;

	m_EmitType						= (EMIT_TYPE) Info.m_EmitType;

	i3Vector::Copy(&m_EmitPos, &Info.m_EmitPos);
	i3Vector::Copy(&m_EmitDir, &Info.m_EmitDir);

	m_EmitAngle						= Info.m_EmitAngle;
	m_EmitInterval					= Info.m_EmitInterval;

	i3Vector::Copy(&m_UpDir, &Info.m_UpDir);

	m_PosRandomInterval				= Info.m_PosRandomInterval;

	i3Vector::Copy(&m_PosRandomFactor, &Info.m_PosRandomFactor);

	m_PosStyle						= Info.m_PosStyle;
	m_PosUpdateInterval				= Info.m_PosUpdateInterval;
	m_PosMin						= Info.m_PosMin;
	m_PosMax						= Info.m_PosMax;
	m_PosInitialRandomFactor		= Info.m_PosInitialRandomFactor;

	m_ScaleStyle					= Info.m_ScaleStyle;
	m_ScaleUpdateInterval			= Info.m_ScaleUpdateInterval;
	m_ScaleMin						= Info.m_ScaleMin;
	m_ScaleMax						= Info.m_ScaleMax;
	m_ScaleInitialRandomFactor		= Info.m_ScaleInitialRandomFactor;
	m_ScaleBillboardFactor			= Info.m_ScaleBillboardFactor;

	i3Vector::Copy(&m_ScaleMask, &Info.m_ScaleMask);

	m_RotateStyle					= Info.m_RotateStyle;
	m_RotateUpdateInterval			= Info.m_RotateUpdateInterval;
	m_RotateFunc					= (FUNC_TYPE) Info.m_RotateFunc;
	m_RotateInit					= Info.m_RotateInit;
	m_RotateFactor					= Info.m_RotateFactor;
	m_RotateLength					= Info.m_RotateLength;
	m_RotateMin						= Info.m_RotateMin;
	m_RotateMax						= Info.m_RotateMax;
	m_RotateRandomFactor			= Info.m_RotateRandomFactor;

	if( Info.m_pRotateTable != 0)
	{
		pTable = (i3KeyframeTable *) pResFile->FindObjectByID( Info.m_pRotateTable);
		I3ASSERT( pTable != nullptr);

		SetRotationTable( pTable);
	}

	m_ColorStyle					= Info.m_ColorStyle;
	m_ColorUpdateInterval			= Info.m_ColorUpdateInterval;
	m_RGBFunc						= (FUNC_TYPE) Info.m_RGBFunc;
	m_RGBInit						= Info.m_RGBInit;

	if( m_RGBInit & 0xFF000000)
	{
		// 이전 버전과 Color Format이 달라졌다.
		m_RGBInit = m_RGBInit >> 8;
	}

	if( Info.m_pRTable != (OBJREF)nullptr)
	{
		pTable = (i3KeyframeTable *) pResFile->FindObjectByID( Info.m_pRTable);
		I3ASSERT( pTable != nullptr);

		SetRTable( pTable);
	}

	if( Info.m_pGTable != (OBJREF)nullptr)
	{
		pTable = (i3KeyframeTable *) pResFile->FindObjectByID( Info.m_pGTable);
		I3ASSERT( pTable != nullptr);

		SetGTable( pTable);
	}

	if( Info.m_pBTable != (OBJREF)nullptr)
	{
		pTable = (i3KeyframeTable *) pResFile->FindObjectByID( Info.m_pBTable);
		I3ASSERT( pTable != nullptr);

		SetBTable( pTable);
	}

	if( Info.m_pATable != (OBJREF)nullptr)
	{
		pTable = (i3KeyframeTable *) pResFile->FindObjectByID( Info.m_pATable);
		I3ASSERT( pTable != nullptr);

		SetAlphaTable( pTable);
	}

	m_AlphaFunc					= (FUNC_TYPE) Info.m_AlphaFunc;
	m_AlphaInit					= Info.m_AlphaInit;

	for( i = 0; i < FUNC_COUNT; i++)
	{
		m_PosFunc[i]			= (FUNC_TYPE) Info.m_PosFunc[i];
		m_PosInit[i]			= Info.m_PosInit[i];
		m_PosFactor[i]			= Info.m_PosFactor[i];
		if( Info.m_PosTable[i] != (OBJREF)nullptr)
		{
			pTable = (i3KeyframeTable *) pResFile->FindObjectByID( Info.m_PosTable[i]);
			I3ASSERT( pTable != nullptr);

			SetPositionTable( i, pTable);
		}

		m_ScaleFunc[i]			= (FUNC_TYPE) Info.m_ScaleFunc[i];
		m_ScaleInit[i]			= Info.m_ScaleInit[i];
		m_ScaleFactor[i]		= Info.m_ScaleFactor[i];

		if( Info.m_ScaleTable[i] != (OBJREF)nullptr)
		{
			pTable = (i3KeyframeTable *) pResFile->FindObjectByID( Info.m_ScaleTable[i]);
			I3ASSERT( pTable != nullptr);

			SetScaleTable( i, pTable);
		}
	}

	if( Info.m_MeshParticleInfo != (OBJREF)nullptr)
	{
		pMesh = (i3MeshParticle *) pResFile->FindObjectByID( Info.m_MeshParticleInfo);
		I3ASSERT( pMesh != nullptr);

		SetMeshParticle(pMesh);
	}

	return Result;
}

bool i3ParticleInfo::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr("Type", UINT32(m_Type));
	pXML->addAttr("LifeTime", m_LifeTime);
	pXML->addAttr("ObjectLifeTime", m_ObjectLifeTime);
	pXML->addAttr("MassMin", m_MassMin);
	pXML->addAttr("MassMax", m_MassMax);
	pXML->addAttr("InitialObjectCount", m_InitialObjectCount);
	pXML->addAttr("MaxObjectCount", m_MaxObjectCount);
	pXML->addAttr("EmitCount", m_EmitCount);
	pXML->addAttr("Volatile", UINT32(m_bVolatile));
	pXML->addAttr("EmitType", UINT32(m_EmitType));

	i3Vector::SetToXML(pXML, "EmitPos", &m_EmitPos);
	i3Vector::SetToXML(pXML, "EmitDir", &m_EmitDir);
	
	pXML->addAttr("EmitAngle", m_EmitAngle);
	pXML->addAttr("EmitInterval", m_EmitInterval);

	i3Vector::SetToXML(pXML, "UpDir", &m_UpDir);

	pXML->addAttr("PosStyle", m_PosStyle);
	pXML->addAttr("PosUpdateInterval", m_PosUpdateInterval);
	pXML->addAttr("PosRandomInterval", m_PosRandomInterval);
	i3Vector::SetToXML(pXML, "PosRandomFactor", &m_PosRandomFactor);
	pXML->addAttr("PosMin", m_PosMin);
	pXML->addAttr("PosMax", m_PosMax);
	pXML->addAttr("PosInitialRandomFactor", m_PosInitialRandomFactor);

	pXML->addAttr("ScaleStyle", m_ScaleStyle);
	pXML->addAttr("ScaleUpdateInterval", m_ScaleUpdateInterval);
	pXML->addAttr("ScaleMin", m_ScaleMin);
	pXML->addAttr("ScaleMax", m_ScaleMax);
	pXML->addAttr("ScaleInitialRandomFactor", m_ScaleInitialRandomFactor);
	i3Vector::SetToXML(pXML, "ScaleMask", &m_ScaleMask);
	pXML->addAttr("ScaleBillboardFactor", m_ScaleBillboardFactor);
	pXML->addAttr("RotateStyle", m_RotateStyle);
	pXML->addAttr("RotateUpdateInterval", m_RotateUpdateInterval);
	pXML->addAttr("RotateFunc", UINT32(m_RotateFunc));
	pXML->addAttr("RotateInit", m_RotateInit);
	pXML->addAttr("RotateFactor", m_RotateFactor);
	pXML->addAttr("RotateTable", OBJREF(pFile->GetObjectPersistID(m_pRotateTable)));
	pXML->addAttr("RotateLength", m_RotateLength);
	pXML->addAttr("RotateMin", m_RotateMin);
	pXML->addAttr("RotateMax", m_RotateMax);
	pXML->addAttr("RotateRandomFactor", m_RotateRandomFactor);
	pXML->addAttr("ColorStyle", m_ColorStyle);
	pXML->addAttr("ColorUpdateInterval", m_ColorUpdateInterval);
	pXML->addAttr("RGBFunc", UINT32(m_RGBFunc));
	pXML->addAttr("RGBInit", m_RGBInit);
	pXML->addAttr("RTable", OBJREF(pFile->GetObjectPersistID(m_pRTable)));
	pXML->addAttr("GTable", OBJREF(pFile->GetObjectPersistID(m_pGTable)));
	pXML->addAttr("BTable", OBJREF(pFile->GetObjectPersistID(m_pBTable)));
	pXML->addAttr("ATable", OBJREF(pFile->GetObjectPersistID(m_pATable)));
	pXML->addAttr("AlphaFunc", UINT32(m_AlphaFunc));
	pXML->addAttr("AlphaInit", UINT32(m_AlphaInit));

	char szTemp[128];
	for (INT32 i = 0; i < FUNC_COUNT; i++)
	{
		i3::sprintf(szTemp, "PosFunc%d", i);
		pXML->addAttr(szTemp, UINT32(m_PosFunc[i]));

		i3::sprintf(szTemp, "PosInit%d", i);
		pXML->addAttr(szTemp, m_PosInit[i]);

		i3::sprintf(szTemp, "PosFactor%d", i);
		pXML->addAttr(szTemp, m_PosFactor[i]);

		i3::sprintf(szTemp, "PosTable%d", i);
		pXML->addAttr(szTemp, OBJREF(pFile->GetObjectPersistID(m_PosTable[i])));

		i3::sprintf(szTemp, "ScaleFunc%d", i);
		pXML->addAttr(szTemp, UINT32(m_ScaleFunc[i]));

		i3::sprintf(szTemp, "ScaleInit%d", i);
		pXML->addAttr(szTemp, m_ScaleInit[i]);

		i3::sprintf(szTemp, "ScaleFactor%d", i);
		pXML->addAttr(szTemp, m_ScaleFactor[i]);

		i3::sprintf(szTemp, "ScaleTable%d", i);
		pXML->addAttr(szTemp, OBJREF(pFile->GetObjectPersistID(m_ScaleTable[i])));
	}

	pXML->addAttr("MeshParticle", OBJREF(pFile->GetObjectPersistID(m_pMeshParticle)));

	if (m_MaxObjectCapacity == 0)
		pXML->addAttr("MaxObjectCapacity", m_MaxObjectCount);
	else
		pXML->addAttr("MaxObjectCapacity", m_MaxObjectCapacity);

	return true;
}

bool i3ParticleInfo::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	UINT32 uTemp;
	pXML->getAttr("Type", &uTemp);
	m_Type = PARTICLE_TYPE(uTemp);

	pXML->getAttr("LifeTime", &m_LifeTime);
	pXML->getAttr("ObjectLifeTime", &m_ObjectLifeTime);
	pXML->getAttr("MassMin", &m_MassMin);
	pXML->getAttr("MassMax", &m_MassMax);

	pXML->getAttr("InitialObjectCount", &m_InitialObjectCount);
	pXML->getAttr("MaxObjectCount", &m_MaxObjectCount);
	pXML->getAttr("MaxObjectCapacity", &m_MaxObjectCapacity);

	if (m_MaxObjectCapacity == 0)
	{
		m_MaxObjectCapacity = m_MaxObjectCount;
	}

	pXML->getAttr("EmitCount", &m_EmitCount);
	pXML->getAttr("Volatile", &uTemp);
	m_bVolatile = uTemp != 0;
	pXML->getAttr("EmitType", &uTemp);
	m_EmitType = EMIT_TYPE(uTemp);
	i3Vector::GetFromXML(pXML, "EmitPos", &m_EmitPos);
	i3Vector::GetFromXML(pXML, "EmitDir", &m_EmitDir);
	pXML->getAttr("EmitAngle", &m_EmitAngle);
	pXML->getAttr("EmitInterval", &m_EmitInterval);
	i3Vector::GetFromXML(pXML, "UpDir", &m_UpDir);
	pXML->getAttr("PosRandomInterval", &m_PosRandomInterval);
	i3Vector::GetFromXML(pXML, "PosRandomFactor", &m_PosRandomFactor);
	pXML->getAttr("PosStyle", &m_PosStyle);
	pXML->getAttr("PosUpdateInterval", &m_PosUpdateInterval);
	pXML->getAttr("PosMin", &m_PosMin);
	pXML->getAttr("PosMax", &m_PosMax);
	pXML->getAttr("PosInitialRandomFactor", &m_PosInitialRandomFactor);
	pXML->getAttr("ScaleStyle", &m_ScaleStyle);
	pXML->getAttr("ScaleUpdateInterval", &m_ScaleUpdateInterval);
	pXML->getAttr("ScaleMin", &m_ScaleMin);
	pXML->getAttr("ScaleMax", &m_ScaleMax);
	pXML->getAttr("ScaleInitialRandomFactor", &m_ScaleInitialRandomFactor);
	pXML->getAttr("ScaleBillboardFactor", &m_ScaleBillboardFactor);
	i3Vector::GetFromXML(pXML, "ScaleMask", &m_ScaleMask);

	pXML->getAttr("RotateStyle", &m_RotateStyle);
	pXML->getAttr("RotateUpdateInterval", &m_RotateUpdateInterval);
	pXML->getAttr("RotateFunc", &uTemp);
	m_RotateFunc = FUNC_TYPE(uTemp);
	pXML->getAttr("RotateInit", &m_RotateInit);
	pXML->getAttr("RotateFactor", &m_RotateFactor);
	pXML->getAttr("RotateLength", &m_RotateLength);
	pXML->getAttr("RotateMin", &m_RotateMin);
	pXML->getAttr("RotateMax", &m_RotateMax);
	pXML->getAttr("RotateRandomFactor", &m_RotateRandomFactor);

	OBJREF objref;
	pXML->getAttr("RotateTable", &objref);
	if (objref != 0)
	{
		i3KeyframeTable *pTable = (i3KeyframeTable *)pFile->FindObjectByID(objref);
		I3ASSERT(pTable != nullptr);

		SetRotationTable(pTable);
	}

	pXML->getAttr("ColorStyle", &m_ColorStyle);
	pXML->getAttr("ColorUpdateInterval", &m_ColorUpdateInterval);
	pXML->getAttr("RGBFunc", &uTemp);
	m_RGBFunc = FUNC_TYPE(uTemp);
	pXML->getAttr("RGBInit", &m_RGBInit);

	if (m_RGBInit & 0xFF000000)
	{
		// 이전 버전과 Color Format이 달라졌다.
		m_RGBInit = m_RGBInit >> 8;
	}

	pXML->getAttr("RTable", &objref);
	if (objref != 0)
	{
		i3KeyframeTable *pTable = (i3KeyframeTable *)pFile->FindObjectByID(objref);
		I3ASSERT(pTable != nullptr);

		SetRTable(pTable);
	}

	pXML->getAttr("GTable", &objref);
	if (objref != 0)
	{
		i3KeyframeTable *pTable = (i3KeyframeTable *)pFile->FindObjectByID(objref);
		I3ASSERT(pTable != nullptr);

		SetGTable(pTable);
	}

	pXML->getAttr("BTable", &objref);
	if (objref != 0)
	{
		i3KeyframeTable *pTable = (i3KeyframeTable *)pFile->FindObjectByID(objref);
		I3ASSERT(pTable != nullptr);

		SetBTable(pTable);
	}

	pXML->getAttr("ATable", &objref);
	if (objref != 0)
	{
		i3KeyframeTable *pTable = (i3KeyframeTable *)pFile->FindObjectByID(objref);
		I3ASSERT(pTable != nullptr);

		SetAlphaTable(pTable);
	}

	pXML->getAttr("AlphaFunc", &uTemp);
	m_AlphaFunc = FUNC_TYPE(uTemp);
	pXML->getAttr("AlphaInit", &m_AlphaInit);

	char szTemp[128];
	i3KeyframeTable* pTable;
	for (INT32 i = 0; i < FUNC_COUNT; i++)
	{
		i3::sprintf(szTemp, "PosFunc%d", i);
		pXML->getAttr(szTemp, &uTemp);
		m_PosFunc[i] = FUNC_TYPE(uTemp);

		i3::sprintf(szTemp, "PosInit%d", i);
		pXML->getAttr(szTemp, &m_PosInit[i]);

		i3::sprintf(szTemp, "PosFactor%d", i);
		pXML->getAttr(szTemp, &m_PosFactor[i]);

		i3::sprintf(szTemp, "PosTable%d", i);
		pXML->getAttr(szTemp, &objref);
		if (objref > 0)
		{
			pTable = (i3KeyframeTable *)pFile->FindObjectByID(objref);
			I3ASSERT(pTable != nullptr);

			SetPositionTable(i, pTable);
		}

		i3::sprintf(szTemp, "ScaleFunc%d", i);
		pXML->getAttr(szTemp, &uTemp);
		m_ScaleFunc[i] = FUNC_TYPE(uTemp);

		i3::sprintf(szTemp, "ScaleInit%d", i);
		pXML->getAttr(szTemp, &m_ScaleInit[i]);

		i3::sprintf(szTemp, "ScaleFactor%d", i);
		pXML->getAttr(szTemp, &m_ScaleFactor[i]);

		i3::sprintf(szTemp, "ScaleTable%d", i);
		pXML->getAttr(szTemp, &objref);
		if (objref > 0)
		{
			pTable = (i3KeyframeTable *)pFile->FindObjectByID(objref);
			I3ASSERT(pTable != nullptr);

			SetScaleTable(i, pTable);
		}
	}

	pXML->getAttr("MeshParticleInfo", &objref);
	if (objref>0)
	{
		i3MeshParticle* pMesh = (i3MeshParticle *)pFile->FindObjectByID(objref);
		I3ASSERT(pMesh != nullptr);

		SetMeshParticle(pMesh);
	}
	return true;
}